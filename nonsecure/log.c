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
#include <nrfx_uarte.h>

/* Other includes */
#include "defs.h"
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
    xLogTaskParameters.pvParameters = ( void *)xGetLogHandle();

    /* Create a privileged task. */
    xTaskCreateRestricted(  &( xLogTaskParameters ), NULL );
}

void prvLogTask( void *prvParameters )
{
    LogMessage_t xLogMessageRx;
    QueueHandle_t xQueue = ( QueueHandle_t )prvParameters;
    char cBuf[ 256 ];
    TickType_t uiMTime;
    nrfx_uarte_t uarte = NRFX_UARTE_INSTANCE( 1 );

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
            /* xLogMessage now contains the received data. */
            if ( bLogFlag )
            {
                uiMTime = pdTICKS_TO_MS( xLogMessageRx.uiTicks );
                sprintf( cBuf, "Time:\t\t%d.%d%d%d s, Message:\t%s\r\n", uiMTime / 1000, ( uiMTime % 1000 ) / 100, ( uiMTime % 100 ) / 10, uiMTime % 10, xLogMessageRx.cData );
                nrfx_uarte_tx( &uarte, ( uint8_t* )cBuf, strlen( cBuf ) );
            }
            printf( "%s", cBuf );
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

    xLogMessageTx.uiTicks = xTaskGetTickCount();
    strcpy( xLogMessageTx.cData, pcLogMessage );
    if ( xQueueSend( xQueue, &xLogMessageTx, pdMS_TO_TICKS( 100 ) ) != pdPASS )
    {
        /* Message failed to send */
    }
}

void vSetFlag( bool bFlag )
{
    bLogFlag = bFlag;
}