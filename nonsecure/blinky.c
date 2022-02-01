/* Trying out the peripherals on the nRF9160 starting out with the LEDs on the nRF9160DK */

#include <stdio.h>
#include <nrf.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "log.h"
#include "blinky.h"

/**
 * @brief Implements the task which will blink the LEDs.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void *pvParameters;
static void prvBlinkyTask( void * pvParameters );

/* Configure GPIO */

void nrf_gpio_dir_set( uint16_t pin );
void nrf_gpio_dir_clear( uint16_t pin );
static void nrf_gpio_out_set( uint16_t pin );
void nrf_gpio_out_clear( uint16_t pin );
void nrf_gpio_out_toggle( uint16_t pin );
uint32_t powerof2( uint32_t number );
uint32_t base2( uint32_t number, uint32_t base );

/* Custom task delay (FreeRTOS not tolerating more than one tick) */
void vDelay( uint16_t milliseconds );

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

    /* Set up LEDs */
    for ( i = 2; i < 10; i++ )
    {
        nrf_gpio_dir_set(i);
    }

    /* Start LEDs */
    for ( i = 0; ; i = ( i + 1 ) % 4 )
    {
        /* This task will blink the LEDs 
         */
         sprintf( ucBuf, "LED #%d\n", i );
         vLogPrint( ucBuf );
        nrf_gpio_out_toggle( i + 2 );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        nrf_gpio_out_toggle( i + 2 );
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

void nrf_gpio_dir_set( uint16_t pin )
{
    NRF_P0_NS->DIRSET = ( 1 << pin );
}

void nrf_gpio_dir_clear( uint16_t pin )
{
    NRF_P0_NS->DIRCLR = ( 1 << pin );
}

static void nrf_gpio_out_set( uint16_t pin )
{
    NRF_P0_NS->OUTSET = ( 1 << pin );
}

void nrf_gpio_out_clear( uint16_t pin )
{
    NRF_P0_NS->OUTCLR = ( 1 << pin );
}

void nrf_gpio_out_toggle( uint16_t pin )
{
    uint32_t gpio;

    gpio = NRF_P0_NS->OUT;
    gpio ^= ( 1 << pin );
    NRF_P0_NS->OUT = gpio;
}

uint32_t powerof2( uint32_t number )
{
    uint32_t n = number;
    uint32_t p = 0;

    while ( n > 0 )
    {
        p++;
        n >>= 1;
    }

    if ( number > ( 1 << p ) )
    {
        p++;
    }

    return 1 << p;
}

uint32_t base2( uint32_t number, uint32_t base )
{
    uint32_t p = number / base;

    return p * base;
}
