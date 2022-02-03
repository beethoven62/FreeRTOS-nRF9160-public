/* Set up peripherals for board */

#include <stddef.h>
#include <stdbool.h>
#include <nrf.h>
#include "board.h"

#define UART1_TX_PIN    ( 29 )
#define UART1_RX_PIN    ( 28 )

/* Configure GPIO */

void vBoardInit( void )
{
    uint16_t i;

    /* Set up LEDs */
    for ( i = 2; i < 10; i++ )
    {
        nrf_gpio_dir_set(i);
    }

    /* Set up UART1 */
    nrf_uart_configure( NRF_UARTE1_NS, UARTE_BAUDRATE_BAUDRATE_Baud115200, UART1_TX_PIN, UART1_RX_PIN );
    nrf_uart_enable( NRF_UARTE1_NS );

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

uint8_t nrf_uart_configure( NRF_UARTE_Type* uart, uint32_t baudrate, uint32_t txpin, uint32_t rxpin )
{
    uint8_t flag = false;

    switch ( baudrate )
    {
        case UARTE_BAUDRATE_BAUDRATE_Baud1200:
        case UARTE_BAUDRATE_BAUDRATE_Baud2400:
        case UARTE_BAUDRATE_BAUDRATE_Baud4800:
        case UARTE_BAUDRATE_BAUDRATE_Baud9600:
        case UARTE_BAUDRATE_BAUDRATE_Baud14400:
        case UARTE_BAUDRATE_BAUDRATE_Baud19200:
        case UARTE_BAUDRATE_BAUDRATE_Baud28800:
        case UARTE_BAUDRATE_BAUDRATE_Baud31250:
        case UARTE_BAUDRATE_BAUDRATE_Baud38400:
        case UARTE_BAUDRATE_BAUDRATE_Baud56000:
        case UARTE_BAUDRATE_BAUDRATE_Baud57600:
        case UARTE_BAUDRATE_BAUDRATE_Baud76800:
        case UARTE_BAUDRATE_BAUDRATE_Baud115200:
        case UARTE_BAUDRATE_BAUDRATE_Baud230400:
        case UARTE_BAUDRATE_BAUDRATE_Baud250000:
        case UARTE_BAUDRATE_BAUDRATE_Baud460800:
        case UARTE_BAUDRATE_BAUDRATE_Baud921600:
        case UARTE_BAUDRATE_BAUDRATE_Baud1M:
            uart->BAUDRATE = baudrate;
            flag = true;
            break;
        default:
            flag = false;
            break;
    }
    if ( flag && ( txpin != rxpin ) )
    {
        uart->PSEL.TXD = ( UARTE_PSEL_TXD_CONNECT_Connected << UARTE_PSEL_TXD_CONNECT_Pos ) | ( txpin & UARTE_PSEL_TXD_PIN_Msk );
        uart->PSEL.RXD = ( UARTE_PSEL_RXD_CONNECT_Connected << UARTE_PSEL_RXD_CONNECT_Pos ) | ( rxpin & UARTE_PSEL_RXD_PIN_Msk );
    }
    else
    {
        flag = false;
    }

    if ( flag )
    {
        uart->CONFIG = UARTE_CONFIG_STOP_One | UARTE_CONFIG_PARITY_Excluded | UARTE_CONFIG_HWFC_Disabled;
    }

    return flag;
}

uint32_t nrf_uart_tx( NRF_UARTE_Type* uart, void* txbuf, uint32_t txlen )
{
    uint32_t nbytes = 0;

    if ( txbuf != NULL && txlen > 0 )
    {
        uart->TXD.PTR = ( uint32_t ) txbuf;
        uart->TXD.MAXCNT = txlen;
        uart->TASKS_STARTTX = UARTE_TASKS_STARTTX_TASKS_STARTTX_Trigger;

        while( uart->EVENTS_ENDTX == UARTE_EVENTS_ENDTX_EVENTS_ENDTX_NotGenerated );
        nbytes = uart->TXD.AMOUNT;
    }

    return nbytes;
}

uint32_t nrf_uart_rx( NRF_UARTE_Type* uart, void* rxbuf, uint32_t rxlen )
{
    uint32_t nbytes = 0;

    if ( rxbuf != NULL && rxlen > 0 )
    {
        uart->RXD.PTR = ( uint32_t ) rxbuf;
        uart->RXD.MAXCNT = rxlen;
        uart->TASKS_STARTRX = UARTE_TASKS_STARTRX_TASKS_STARTRX_Trigger;

        while( uart->EVENTS_ENDRX == UARTE_EVENTS_ENDRX_EVENTS_ENDRX_NotGenerated ); // This needs improvement
        nbytes = uart->RXD.AMOUNT;
    }

    return nbytes;
}

void nrf_uart_enable( NRF_UARTE_Type* uart )
{
    uart->ENABLE = UARTE_ENABLE_ENABLE_Enabled;
}

void nrf_uart_disable( NRF_UARTE_Type* uart )
{
    uart->ENABLE = UARTE_ENABLE_ENABLE_Disabled;
}
