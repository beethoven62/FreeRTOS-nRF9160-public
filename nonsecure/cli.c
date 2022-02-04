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
    char ucBuf[ LOG_MSG_MAX ];

    vLogPrint( "CLI Task\r\n" );

    for( ; ; ) 
    {
        if ( ( nbytes = nrf_uart_rx( NRF_UARTE1_NS, ucBuf, 1 ) ) > 0 )
        {
            nbytes = nbytes < LOG_MSG_MAX ? nbytes : LOG_MSG_MAX - 1;
            ucBuf[ nbytes ] = 0;
            printf( "%s", ucBuf );
            ucBuf[ 0 ] = 0;
        }
    }
}
