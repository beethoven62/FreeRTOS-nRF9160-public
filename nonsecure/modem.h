#ifndef __MODEM_H__
#define __MODEM_H__

#define NRF_MODEM_SHMEM_TX_SIZE   0x4000
#define NRF_MODEM_SHMEM_RX_SIZE   0x4000

/* Create one contiguous memory space for the three buffers required by the modem driver */
typedef struct {
    uint8_t nrf_modem_ctrl[ NRF_MODEM_SHMEM_CTRL_SIZE ];
    uint8_t nrf_modem_tx[ NRF_MODEM_SHMEM_TX_SIZE ];
    uint8_t nrf_modem_rx[ NRF_MODEM_SHMEM_RX_SIZE ];
} nrf_modem_bufs_t;

extern nrf_modem_bufs_t nrf_modem_bufs;

/**
 * @brief Creates all the tasks for modem interface.
 *
 * The Modem task creates 1 unprivileged task that interacts with the LTE Modem
 */
void vStartModemTask( void );

#endif /* __MODEM_H__ */
