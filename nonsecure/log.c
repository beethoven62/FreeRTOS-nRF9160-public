#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS include. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Device includes. */
#include <nrf.h>

/* Other includes */
#include "log.h"

static uint32_t uiMessageID;
static QueueHandle_t xLogQueue;
static void *pvParameters;
static void prvLogTask( void *prvParameters );

void vStartLogTask( void )
{
    uint16_t i;

    /* Set up log task */
    uiMessageID = 0;
    xLogQueue = xQueueCreate( LOG_QUEUE_LEN, LOG_MSG_MAX );
    vQueueAddToRegistry( xLogQueue, "Log Queue" );
    static StackType_t xLogTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    TaskParameters_t xLogTaskParameters =
    {
        .pvTaskCode     = prvLogTask,
        .pcName         = "LogTask",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = ( void *) xLogQueue,
        .uxPriority     = tskIDLE_PRIORITY | portPRIVILEGE_BIT,
        .puxStackBuffer = xLogTaskStack,
        .xRegions       =
        {
            { 0,    0,    0 },
            { 0,    0,    0 },
            { 0,    0,    0 },
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
    QueueHandle_t xLogQueue;
    LogMessage_t xLogMessage;

    /* The queue handle is passed into this task as the task parameter */
    xLogQueue = ( QueueHandle_t ) prvParameters;

    for( ; ; ) 
    {
        /* Wait for the maximum period for data to become available on the queue. 
         * The period will be indefinite if INCLUDE_vTaskSuspend is set to 1 in 
         * FreeRTOSConfig.h. 
         */
        if( xQueueReceive( xLogQueue, &xLogMessage, portMAX_DELAY ) != pdPASS )
        {
            /* Nothing was received from the queue even after blocking to wait
             * for data to arrive. */
        } 
        else 
        {
            /* xLogMessage now contains the received data. */
            printf( "%s", xLogMessage.ucData );
            uiMessageID++;
        } 
    }
}

QueueHandle_t xGetLogHandle( void )
{
    return xLogQueue;
}

uint32_t uiGetMessageID( void )
{
    return uiMessageID;
}

void vLogPrint( char * pcLogMessage )
{
    LogMessage_t xLogMessage;

    strcpy(xLogMessage.ucData, pcLogMessage);
    xLogMessage.ucLogMessageID = uiMessageID;
    if ( xQueueSend(xGetLogHandle(),&xLogMessage, pdMS_TO_TICKS( 100 ) ) != pdPASS )
    {
        /* Message failed to send */
    }

    uiMessageID++;
}
