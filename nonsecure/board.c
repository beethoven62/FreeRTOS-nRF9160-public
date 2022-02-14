/* Set up peripherals for board */

/* Standard includes. */
#include <stddef.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Device includes. */
#include <nrf.h>
#include <nrfx_uarte.h>

/* Other includes */
#include "board.h"

#define UART1_TX_PIN    ( 29 )
#define UART1_RX_PIN    ( 28 )

/* Configure GPIO */

void vBoardInit( void )
{
    uint16_t i;
    nrfx_uarte_t uarte = NRFX_UARTE_INSTANCE( 1 );
    nrfx_uarte_config_t uarte1_config = NRFX_UARTE_DEFAULT_CONFIG( UART1_TX_PIN, UART1_RX_PIN );

    /* Set up LEDs */
    for ( i = 2; i < 10; i++ )
    {
        nrf_gpio_dir_set(i);
    }

    /* Set up UART1 */
    nrfx_uarte_init( &uarte, &uarte1_config, NULL );

    /* Let app setup buffers */
}

void nrf_gpio_dir_set( uint16_t pin )
{
    NRF_P0_NS->DIRSET = ( 1 << pin );
}

void nrf_gpio_dir_clear( uint16_t pin )
{
    NRF_P0_NS->DIRCLR = ( 1 << pin );
}

void nrf_gpio_out_set( uint16_t pin )
{
    NRF_P0_NS->OUTSET = ( 1 << pin );
}

void nrf_gpio_out_clear( uint16_t pin )
{
    NRF_P0_NS->OUTCLR = ( 1 << pin );
}

void nrf_gpio_out_toggle( uint16_t pin )
{
    uint32_t gpio;

    gpio = NRF_P0_NS->OUT;
    gpio ^= ( 1 << pin );
    NRF_P0_NS->OUT = gpio;
}
