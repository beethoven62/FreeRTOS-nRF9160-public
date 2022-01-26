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

/* FreeRTOS includes. */
#include <secure_port_macros.h>

#include <stdio.h>
#include <stdlib.h>
#include <arm_cmse.h>
#include <nrf.h>

//#include "board.h"
//#include "pin_mux.h"
#include "tzm_config.h"

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
 * @brief Application-specific implementation of the SystemInitHook() weak
 * function.
 */
void SystemInitHook( void );
/*-----------------------------------------------------------*/
/* Configure non-secure regions. */
void BOARD_InitPins( void );
void nrf_spu_periph_set( uint16_t id, uint32_t flags );
void nrf_spu_periph_clear( uint16_t id, uint32_t flags );
void nrf_spu_gpio_set( uint16_t id );
void nrf_spu_gpio_clear( uint16_t id );

/* Secure main(). */
int main(void)
{
    /* Remove this function once FreeRTOS is deployed */
    SystemInitHook();
    printf( "Booting Secure World.\n" );

    /* Init board hardware. */
    BOARD_InitPins();

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

void SystemInitHook( void )
{
    /* The TrustZone should be configured as early as possible after RESET.
     * Therefore it is called from SystemInit() during startup. The
     * SystemInitHook() weak function overloading is used for this purpose.
     */
    BOARD_InitTrustZone();
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void BOARD_InitPins( void )
{
  uint16_t rgn;

  /* Configure non-secure peripherals */
  rgn = (( uint32_t )NRF_P0_NS >> 12) & 0x7f;
  nrf_spu_periph_clear( rgn, SPU_PERIPHID_PERM_SECATTR_Msk );

  for ( rgn = 2; rgn < 10; rgn++ )
  {
    nrf_spu_gpio_clear( rgn );
  }
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
