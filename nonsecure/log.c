/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* FreeRTOS include. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Device includes. */
#include <nrf.h>

/* Other includes */
#include "board.h"
#include "log.h"

QueueHandle_t xLogQueueHandle = NULL;
static bool bLogFlag;
static StaticQueue_t xLogQueue;
static void prvLogTask( void *prvParameters );

void vStartLogTask( void )
{
    uint16_t i;

    /* Set up log task */
    xLogQueueHandle = xQueueCreate( LOG_QUEUE_LEN, sizeof( LogMessage_t ) );
    static StackType_t xLogTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    TaskParameters_t xLogTaskParameters =
    {
        .pvTaskCode     = prvLogTask,
        .pcName         = "LogTask",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = ( tskIDLE_PRIORITY + 1 ) | portPRIVILEGE_BIT,
        .puxStackBuffer = xLogTaskStack,
        .xRegions       =
        {
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
        }
    };
    xLogTaskParameters.pvParameters = ( void * )xGetLogHandle();

    /* Create a privileged task. */
    xTaskCreateRestricted(  &( xLogTaskParameters ), NULL );
}

void prvLogTask( void *prvParameters )
{
    LogMessage_t xLogMessageRx;
    uint32_t uiMessageID = 0;
    QueueHandle_t xQueue = ( QueueHandle_t )prvParameters;

    bLogFlag = true;
    for( ; ; ) 
    {
        /* Wait for the maximum period for data to become available on the queue. 
         * The period will be indefinite if INCLUDE_vTaskSuspend is set to 1 in 
         * FreeRTOSConfig.h. 
         */
        if( xQueueReceive( xQueue, &xLogMessageRx, portMAX_DELAY ) != pdPASS )
        {
            /* Nothing was received from the queue even after blocking to wait
             * for data to arrive. */
        } 
        else 
        {
            /* Message received, increment message ID counter */
            uiMessageID++;

            /* xLogMessage now contains the received data. */
            if ( bLogFlag )
            {
                printf( "UART1: %d bytes transmitted.\n", nrf_uart_tx( NRF_UARTE1_NS, xLogMessageRx.ucData, strlen( xLogMessageRx.ucData ) ) );
            }
            printf( "ID: %d, Message: %s", xLogMessageRx.uiLogMessageID, xLogMessageRx.ucData );
        } 
    }
}

/**
 * @brief Return log queue handle. Warning: for use in setup only.
 */
QueueHandle_t xGetLogHandle( void )
{
    return xLogQueueHandle;
}

void vLogPrint( QueueHandle_t xQueue, char *pcLogMessage )
{
    LogMessage_t xLogMessageTx;

    strcpy( xLogMessageTx.ucData, pcLogMessage );
    if ( xQueueSend( xQueue, &xLogMessageTx, pdMS_TO_TICKS( 100 ) ) != pdPASS )
    {
        /* Message failed to send */
    }
}

void vSetFlag( bool bFlag )
{
    bLogFlag = bFlag;
}