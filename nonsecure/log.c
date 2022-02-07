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
static StaticQueue_t xLogQueue;
static void prvLogTask( void *prvParameters );
static uint8_t bLogFlag;

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
        .uxPriority     = tskIDLE_PRIORITY | portPRIVILEGE_BIT,
        .puxStackBuffer = xLogTaskStack,
        .xRegions       =
        {
            { ( void *)xLogQueueHandle, sizeof( xLogQueueHandle ), portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER },
            { 0, 0, 0 },
            { 0, 0, 0 },
        }
    };

    /* Create a privileged task. */
#if configENABLE_MPU == 1
    xTaskCreateRestricted(  &( xLogTaskParameters ), NULL );
#else
    xTaskCreate( xLogTaskParameters.pvTaskCode, 
                 xLogTaskParameters.pcName, 
                 xLogTaskParameters.usStackDepth, 
                 xLogTaskParameters.pvParameters, 
                 xLogTaskParameters.uxPriority, 
                 NULL );
#endif
}

void prvLogTask( void *prvParameters )
{
    LogMessage_t xLogMessageRx;
    uint32_t uiMessageID = 0;
    bLogFlag = true;

    for( ; ; ) 
    {
        /* Wait for the maximum period for data to become available on the queue. 
         * The period will be indefinite if INCLUDE_vTaskSuspend is set to 1 in 
         * FreeRTOSConfig.h. 
         */
        //if( xQueueReceive( xLogQueue, &xLogMessage, portMAX_DELAY ) != pdPASS )
        if( xQueueReceive( xLogQueueHandle, &xLogMessageRx, portMAX_DELAY ) != pdPASS )
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

QueueHandle_t xGetLogHandle( void )
{
    return xLogQueueHandle;
}

void vLogPrint( char *pcLogMessage )
{
    LogMessage_t xLogMessageTx;

    strcpy( xLogMessageTx.ucData, pcLogMessage );
    xLogMessageTx.uiLogMessageID = 0;
    if ( xQueueSend( xLogQueueHandle, &xLogMessageTx, pdMS_TO_TICKS( 100 ) ) != pdPASS )
    {
        /* Message failed to send */
    }
}

void vSetFlag( bool bFlag )
{
    bLogFlag = bFlag;
}