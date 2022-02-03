/* Trying out the peripherals on the nRF9160 starting out with the LEDs on the nRF9160DK */

#include <stdio.h>
#include <nrf.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "board.h"
#include "log.h"
#include "blinky.h"

/**
 * @brief Implements the task which will blink the LEDs.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void *pvParameters;
static void prvBlinkyTask( void * pvParameters );

void vStartBlinkyDemo( void )
{
    uint16_t i;
    static StackType_t xBlinkyTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );
    TaskParameters_t xBlinkyTaskParameters =
    {
        .pvTaskCode     = prvBlinkyTask,
        .pcName         = "Blinky",
        .usStackDepth   = configMINIMAL_STACK_SIZE,
        .pvParameters   = NULL,
        .uxPriority     = tskIDLE_PRIORITY,
        .puxStackBuffer = xBlinkyTaskStack,
        .xRegions       =
        {
            { ( void *)NRF_P0_NS, sizeof( NRF_GPIO_Type ), portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER },
            { 0,                                                       0,                                   0                                                      },
            { 0,                                                       0,                                   0                                                      },
        }
    };

    /* Create an unprivileged task. */
#if configENABLE_MPU == 1
    xTaskCreateRestricted(  &( xBlinkyTaskParameters ), NULL );
#else
    xTaskCreate( xBlinkyTaskParameters.pvTaskCode, 
                 xBlinkyTaskParameters.pcName, 
                 xBlinkyTaskParameters.usStackDepth, 
                 xBlinkyTaskParameters.pvParameters, 
                 xBlinkyTaskParameters.uxPriority, 
                 NULL );
#endif

}

/*-----------------------------------------------------------*/

static void prvBlinkyTask( void * pvParameters )
{
    uint16_t i;
    uint32_t j;
    char ucBuf[ LOG_MSG_MAX ];

    vLogPrint( "Blinky\n" );

    /* Start LEDs */
    for ( i = 0; ; i = ( i + 1 ) % 4 )
    {
        /* This task will blink the LEDs */
        sprintf( ucBuf, "LED #%d\n", i );
        vLogPrint( ucBuf );
        nrf_gpio_out_toggle( i + 2 );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        nrf_gpio_out_toggle( i + 2 );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
