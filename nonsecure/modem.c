/* Standard includes. */
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Device includes. */
#include <nrf.h>
#include <nrf_modem.h>
#include <nrf_modem_platform.h>
#include <nrfx_ipc.h>

/* Other includes */
#include "board.h"
#include "log.h"
#include "modem.h"

/* Allocate modem RAM spaces */
nrf_modem_bufs_t nrf_modem_bufs __attribute__( ( aligned( 32 ) ) );

/**
 * @brief Implements the task which interfaces with the LTE modem.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void *pvParameters;
static void prvModemTask( void * pvParameters );

void vStartModemTask( void )
{
    static StackType_t xModemTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    TaskParameters_t xModemTaskParameters =
    {
        .pvTaskCode     = prvModemTask,
        .pcName         = "ModemTask",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = tskIDLE_PRIORITY | portPRIVILEGE_BIT,
        .puxStackBuffer = xModemTaskStack,
        .xRegions       =
        {
            { ( void *)&nrf_modem_bufs, sizeof( nrf_modem_bufs_t ), tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
            { 0,                        0,                           0                                                     },
        }
    };
    xModemTaskParameters.pvParameters = ( void* )xGetLogHandle();

    /* Create an unprivileged task. */
    xTaskCreateRestricted(  &( xModemTaskParameters ), NULL );
}

void prvModemTask( void* pvParameters )
{
    int32_t error;
    char cBuf[ LOG_MSG_MAX ];
    QueueHandle_t xQueue = ( QueueHandle_t )pvParameters;
    nrf_modem_init_params_t modem =
    {
        .shmem.ctrl     = { ( uint32_t )nrf_modem_bufs.nrf_modem_ctrl, sizeof( nrf_modem_bufs.nrf_modem_ctrl ) },
        .shmem.tx       = { ( uint32_t )nrf_modem_bufs.nrf_modem_tx, sizeof( nrf_modem_bufs.nrf_modem_tx ) },
        .shmem.rx       = { ( uint32_t )nrf_modem_bufs.nrf_modem_rx, sizeof( nrf_modem_bufs.nrf_modem_rx ) },
#if CONFIG_NRF_MODEM_LIB_TRACE_ENABLED
        .shmem.trace    = { 0x0, 0 },
#endif
        .ipc_irq_prio   = NRF_MODEM_NETWORK_IRQ_PRIORITY
    };

    vLogPrint( xQueue, "Modem task started" );

    NVIC_SetPriority( NRF_MODEM_NETWORK_IRQ, NRF_MODEM_NETWORK_IRQ_PRIORITY );
    NVIC_ClearPendingIRQ( NRF_MODEM_NETWORK_IRQ );

    if ( ( error = nrf_modem_init( &modem, NORMAL_MODE) ) < 0 )
    {
        sprintf( cBuf, "Modem initialization failed: %d", error );
        vLogPrint( xQueue, cBuf );
    }
    else
    {
        vLogPrint( xQueue, "Modem initialization complete." );
    }

    for( ; ; )
    {
    }
}

void IPC_IRQHandler( void )
{
    nrfx_ipc_irq_handler();
}