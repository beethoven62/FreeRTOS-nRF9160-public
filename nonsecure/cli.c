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
        .uxPriority     = tskIDLE_PRIORITY,
        .puxStackBuffer = xCLITaskStack,
        .xRegions       =
        {
            { 0,    0,    0 },
            { 0,    0,    0 },
            { 0,    0,    0 },
        }
    };

    /* Create a privileged task. */
#if configENABLE_MPU == 1
    xTaskCreateRestricted(  &( xCLITaskParameters ), NULL );
#else
    xTaskCreate( xCLITaskParameters.pvTaskCode, 
                 xCLITaskParameters.pcName, 
                 xCLITaskParameters.usStackDepth, 
                 xCLITaskParameters.pvParameters, 
                 xCLITaskParameters.uxPriority, 
                 NULL );
#endif
}

void prvCLITask( void *prvParameters )
{
    uint32_t nbytes;
    char cBuf[ LOG_MSG_MAX ];

    vLogPrint( "CLI Task\r\n" );

    for( ; ; ) 
    {
        if ( ( nbytes = nrf_uart_rx( NRF_UARTE1_NS, cBuf, 1 ) ) > 0 )
        {
            nbytes = nbytes < LOG_MSG_MAX ? nbytes : LOG_MSG_MAX - 1;
            cBuf[ nbytes ] = 0;
            if ( strlen( cBuf ) > 0 )
            {
                printf( "Received: %s\n", cBuf );
                if ( cBuf[ 0 ] == 'x' || cBuf == 'X' )
                {
                    vSetFlag( false );
                }
                else
                {
                    vSetFlag( true );
                }
            }
            cBuf[ 0 ] = 0;
        }
    }
}
