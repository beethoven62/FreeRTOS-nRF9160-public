#ifndef __BOARD_H__
#define __BOARD_H__

/**
 * @brief nRF9160DK board setup.
 *
 */

/* Board initialization */
void vBoardInit( void );

/* GPIO set up */
void nrf_gpio_dir_set( uint16_t pin );
void nrf_gpio_dir_clear( uint16_t pin );
void nrf_gpio_out_set( uint16_t pin );
void nrf_gpio_out_clear( uint16_t pin );
void nrf_gpio_out_toggle( uint16_t pin );

/* UART setup */
uint8_t nrf_uart_configure( NRF_UARTE_Type* uart, uint32_t baudrate, uint32_t txpin, uint32_t rxpin );
uint32_t nrf_uart_tx( NRF_UARTE_Type* uart, void* txbuf, uint32_t txlen );
uint32_t nrf_uart_rx( NRF_UARTE_Type* uart, void* rxbuf, uint32_t rxlen );
void nrf_uart_enable( NRF_UARTE_Type* uart );
void nrf_uart_disable( NRF_UARTE_Type* uart );

#endif /* __BOARD_H__ */
