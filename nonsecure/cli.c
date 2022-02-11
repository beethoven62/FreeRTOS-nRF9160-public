/* Standard includes. */
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
#include "board.h"
#include "log.h"
#include "cli.h"

static void prvCLITask( void *prvParameters );

void vStartCLITask( void )
{
    uint16_t i;

    /* Set up CLI task */
    static StackType_t xCLITaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    TaskParameters_t xCLITaskParameters =
    {
        .pvTaskCode     = prvCLITask,
        .pcName         = "CLITask",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = tskIDLE_PRIORITY | portPRIVILEGE_BIT,
        .puxStackBuffer = xCLITaskStack,
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
    xCLITaskParameters.pvParameters = ( void* )xGetLogHandle();

    /* Create a privileged task. */
    xTaskCreateRestricted(  &( xCLITaskParameters ), NULL );
}

void prvCLITask( void *pvParameters )
{
    uint32_t nbytes;
    char cByte, cBuf[ LOG_MSG_MAX ];
    QueueHandle_t xQueue = ( QueueHandle_t )pvParameters;

    vLogPrint( xQueue, "CLI task started" );

    for( ; ; ) 
    {
        if ( ( nbytes = nrf_uart_rx( NRF_UARTE1_NS, &cByte, 1 ) ) > 0 )
        {
            sprintf( cBuf, "Received: %s", &cByte );
            vLogPrint( xQueue, cBuf );
            if ( cByte == 'x' || cByte == 'X' )
            {
                vSetFlag( false );
            }
            else
            {
                vSetFlag( true );
            }
        }
    }
}
