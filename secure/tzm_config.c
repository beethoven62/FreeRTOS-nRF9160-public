/*
 * SPU regions (SPU register in nRF9160 replaces SAU register for ARM Cortex-M33 core)
 */
/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
/* FreeRTOS includes. */
#include <secure_port_macros.h>

#include <nrf.h>
#include "tzm_config.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Address regions and SPU_S register configuration */
#define mainNONSECURE_RAM_START_ADDRESS         ( 0x20008000U )
#define NSC_FUNCTION_ADDRESS                    ( 0x0000f000U )
#define RAM_ADDRESS_MASK                        ( 0x0fffffffU )
#define SPU_FLASHREGION_SIZE                    ( 0x8000U )
#define SPU_RAMREGION_SIZE                      ( 0x2000U )
#define SPU_NSCREGION_SIZE                      ( 8 )
#define SPU_NUM_REGIONS                         ( 0x20U )

/* Configure non-secure regions. */
void BOARD_InitTrustZone( void );
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

/***********************************************************************************************************************
 * BOARD_InitTrustZone function
 **********************************************************************************************************************/
void BOARD_InitTrustZone( void )
{
  uint16_t rgn;

  /* Configure non-secure memory */
  for ( rgn = mainNONSECURE_APP_START_ADDRESS / SPU_FLASHREGION_SIZE; rgn < SPU_NUM_REGIONS; rgn++ )
  {
    nrf_spu_flashregion_clear(rgn, SPU_FLASHREGION_PERM_SECATTR_Msk);
   }
  for ( rgn = ( mainNONSECURE_RAM_START_ADDRESS & RAM_ADDRESS_MASK ) / SPU_RAMREGION_SIZE; rgn < SPU_NUM_REGIONS; rgn++ )
  {
    nrf_spu_ramregion_clear( rgn, SPU_RAMREGION_PERM_SECATTR_Msk );
  }

  /* Configure non-secure peripherals */
  config_nonsecure_peripheral( NRF_P0_NS );

  for ( rgn = 2; rgn < 10; rgn++ )
  {
    nrf_spu_gpio_clear( rgn );
  }

  config_nonsecure_peripheral( NRF_UARTE0_NS );
  config_nonsecure_peripheral( NRF_UARTE1_NS );

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
}