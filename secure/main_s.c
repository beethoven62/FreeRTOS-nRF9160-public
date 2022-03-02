/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://aws.amazon.com/freertos
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include <secure_port_macros.h>

/* Device includes */
#include <nrf.h>
#include <arm_cmse.h>

/* typedef for non-secure Reset Handler. */
typedef void ( *NonSecureResetHandler_t ) ( void ) __attribute__( ( cmse_nonsecure_call ) );
/*-----------------------------------------------------------*/

/**
 * @brief Boots into the non-secure code.
 *
 * @param[in] ulNonSecureStartAddress Start address of the non-secure application.
 */
static void prvBootNonSecure( uint32_t ulNonSecureStartAddress );

/**
 * @brief Sets up System Protection Unit (SPU) for non-secure and NSC sections.
 */
/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Address regions and SPU_S register configuration */
#define mainNONSECURE_APP_START_ADDRESS         ( 0x00010000U )
#define mainNONSECURE_RAM_START_ADDRESS         ( 0x20000000U )
#define NSC_FUNCTION_ADDRESS                    ( 0x0000f000U )
#define RAM_ADDRESS_MASK                        ( 0x0fffffffU )
#define SPU_FLASHREGION_SIZE                    ( 0x8000U )
#define SPU_RAMREGION_SIZE                      ( 0x2000U )
#define SPU_NSCREGION_SIZE                      ( 8 )
#define SPU_NUM_REGIONS                         ( 0x20U )

/* Configure non-secure regions. */
static void prvSetupSPU( void );
void config_nonsecure_peripheral( void *peripheral );
void nrf_spu_flashregion_set( uint16_t region, uint32_t flags );
void nrf_spu_flashregion_clear( uint16_t region, uint32_t flags );
void nrf_spu_ramregion_set( uint16_t region, uint32_t flags );
void nrf_spu_ramregion_clear( uint16_t region, uint32_t flags );
void nrf_spu_periph_set( uint16_t id, uint32_t flags );
void nrf_spu_periph_clear( uint16_t id, uint32_t flags );
void nrf_spu_gpio_set( uint16_t id );
void nrf_spu_gpio_clear( uint16_t id );
void nrf_spu_nsc( uint32_t address );
/*-----------------------------------------------------------*/

/* Secure main(). */
int main(void)
{
    printf( "Booting Secure World.\r\n" );

    /* Set floating point coprosessor access mode.
     * Set CP10 and CP11 full access from Non-Secure code. */
    SCB_NS->CPACR |= ( ( 3UL << 10 * 2 ) | ( 3UL << 11 * 2 ) );

    prvSetupSPU();

    /* Boot the non-secure code. */
    printf( "Booting Non-Secure World.\r\n" );
    prvBootNonSecure( mainNONSECURE_APP_START_ADDRESS );

    /* Non-secure software does not return, this code is not executed. */
    for( ; ; )
    {
        /* Should not reach here. */
    }
}
/*-----------------------------------------------------------*/

static void prvBootNonSecure( uint32_t ulNonSecureStartAddress )
{
    NonSecureResetHandler_t pxNonSecureResetHandler;

    /* Setup the non-secure vector table. */
    SCB_NS->VTOR = ulNonSecureStartAddress;
        
    /* Main Stack Pointer value for the non-secure side is the first entry in
     * the non-secure vector table. Read the first entry and assign the same to
     * the non-secure main stack pointer(MSP_NS). */
    secureportSET_MSP_NS( *( ( uint32_t * )( ulNonSecureStartAddress ) ) );

    /* Non secure Reset Handler is the second entry in the non-secure vector
     * table. Read the non-secure reset handler.
     */
    pxNonSecureResetHandler = ( NonSecureResetHandler_t )( * ( ( uint32_t * ) ( ( ulNonSecureStartAddress ) + 4U ) ) );

    /* Start non-secure software application by jumping to the non-secure Reset
     * Handler. */
    pxNonSecureResetHandler();
}
/*-----------------------------------------------------------*/

static void prvSetupSPU( void )
{
    uint16_t rgn;

    /* Configure non-secure flash memory */
    for ( rgn = mainNONSECURE_APP_START_ADDRESS / SPU_FLASHREGION_SIZE; rgn < SPU_NUM_REGIONS; rgn++ )
    {
        nrf_spu_flashregion_clear(rgn, SPU_FLASHREGION_PERM_SECATTR_Msk);
    }

    /* Configure non-secure RAM memory */
    for ( rgn = ( mainNONSECURE_RAM_START_ADDRESS & RAM_ADDRESS_MASK ) / SPU_RAMREGION_SIZE; rgn < SPU_NUM_REGIONS; rgn++ )
    {
        nrf_spu_ramregion_clear( rgn, SPU_RAMREGION_PERM_SECATTR_Msk );
    }

    /* Configure pins */
    for ( rgn = 0; rgn < 32; rgn++ )
    {
        nrf_spu_gpio_clear( rgn );
    }

    /* Configure non-secure peripherals */
    config_nonsecure_peripheral( NRF_CLOCK_NS );
    config_nonsecure_peripheral( NRF_RTC1_NS );
    config_nonsecure_peripheral( NRF_POWER_NS );
    config_nonsecure_peripheral( NRF_IPC_NS );
    config_nonsecure_peripheral( NRF_NVMC_NS );
    config_nonsecure_peripheral( NRF_VMC_NS );
    config_nonsecure_peripheral( NRF_P0_NS );
    config_nonsecure_peripheral( NRF_GPIOTE1_NS );
    config_nonsecure_peripheral( NRF_UARTE1_NS );
    config_nonsecure_peripheral( NRF_UARTE2_NS );
    config_nonsecure_peripheral( NRF_EGU1_NS );
    config_nonsecure_peripheral( NRF_EGU2_NS );
    config_nonsecure_peripheral( NRF_FPU_NS );
    config_nonsecure_peripheral( NRF_TWIM2_NS );
    config_nonsecure_peripheral( NRF_SPIM3_NS );
    config_nonsecure_peripheral( NRF_TIMER0_NS );

    /* Configure non-secure callable functions */
    nrf_spu_nsc( NSC_FUNCTION_ADDRESS );
}

void nrf_spu_flashregion_set( uint16_t region, uint32_t flags )
{
    NRF_SPU_S->FLASHREGION[region].PERM |= flags;
}

void nrf_spu_flashregion_clear( uint16_t region, uint32_t flags )
{
    NRF_SPU_S->FLASHREGION[region].PERM &= ~flags;
}

void nrf_spu_ramregion_set( uint16_t region, uint32_t flags )
{
    NRF_SPU_S->RAMREGION[region].PERM |= flags;
}

void nrf_spu_ramregion_clear( uint16_t region, uint32_t flags )
{
    NRF_SPU_S->RAMREGION[region].PERM &= ~flags;
}

void nrf_spu_gpio_set( uint16_t id )
{
    NRF_SPU_S->GPIOPORT[0].PERM |= ( 1 << id );
}

void nrf_spu_gpio_clear( uint16_t id )
{
    NRF_SPU_S->GPIOPORT[0].PERM &= ~( 1 << id );
}

void nrf_spu_periph_set( uint16_t id, uint32_t flags )
{
    NRF_SPU_S->PERIPHID[id].PERM |= flags;
}

void nrf_spu_periph_clear( uint16_t id, uint32_t flags )
{
    NRF_SPU_S->PERIPHID[id].PERM &= ~flags;
}

void nrf_spu_nsc( uint32_t address )
{
    NRF_SPU_S->FLASHNSC[0].REGION = address / SPU_FLASHREGION_SIZE;
    NRF_SPU_S->FLASHNSC[0].SIZE = SPU_NSCREGION_SIZE;
}

void config_nonsecure_peripheral( void *peripheral )
{
    uint32_t id = ( ( uint32_t )peripheral >> 12 ) & 0x7f;

    nrf_spu_periph_clear( id, SPU_PERIPHID_PERM_SECATTR_Msk );
    NVIC->ITNS[ id >> 5 ] |= 1 << ( id % 32 );
    NVIC->ICER[ id >> 5 ] = 1 << ( id % 32 );
}
/*-----------------------------------------------------------*/
