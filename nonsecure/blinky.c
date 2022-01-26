/* Trying out the peripherals on the nRF9160 starting out with the LEDs on the nRF9160DK */

#include <stdio.h>
#include <stdlib.h>
#include <nrf.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "blinky.h"

/**
 * @brief Size of the shared memory region.
 */
#define SHARED_MEMORY_SIZE    32

/**
 * @brief Implements the task which will blink the LEDs.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void* pvParameters;
static void prvBlinkyTask( void * pvParameters );

/* Configure GPIO */

void nrf_gpio_dir_set(uint16_t pin);
void nrf_gpio_dir_clear(uint16_t pin);
static void nrf_gpio_out_set(uint16_t pin);
void nrf_gpio_out_clear(uint16_t pin);
void nrf_gpio_out_toggle(uint16_t pin);

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
            { NRF_P0_NS, 640, tskMPU_REGION_READ_WRITE | tskMPU_REGION_EXECUTE_NEVER },
            { 0,           0, 0                                                      },
            { 0,           0, 0                                                      },
        }
    };

    /* Set up LEDs */
    for ( i = 2; i < 10; i++ )
    {
        nrf_gpio_dir_set(i);
    }

    /* Create an unprivileged task. */
    xTaskCreateRestricted(  &( xBlinkyTaskParameters ), NULL  );
}

/*-----------------------------------------------------------*/

static void prvBlinkyTask( void * pvParameters )
{
    uint16_t i = 0;
    uint32_t j;

    do {
        /* This task will blink the LEDs 
         */
        nrf_gpio_out_toggle( i + 2 );
        vDelay( 1000 );
        nrf_gpio_out_toggle( i + 2 );
        vDelay( 1000 );
        i = ( i + 1 ) % 4;
    } while ( pdTRUE );
}

void nrf_gpio_dir_set(uint16_t pin)
{
    NRF_P0_NS->DIRSET = (1 << pin);
}

void nrf_gpio_dir_clear(uint16_t pin)
{
    NRF_P0_NS->DIRCLR = (1 << pin);
}

static void nrf_gpio_out_set(uint16_t pin)
{
    NRF_P0_NS->OUTSET = (1 << pin);
}

void nrf_gpio_out_clear(uint16_t pin)
{
    NRF_P0_NS->OUTCLR = (1 << pin);
}

void nrf_gpio_out_toggle(uint16_t pin)
{
    uint32_t gpio;

    gpio = NRF_P0_NS->OUT;
    gpio ^= (1 << pin);
    NRF_P0_NS->OUT = gpio;
}

void vDelay( uint16_t milliseconds )
{
    TickType_t i;
    TickType_t ticks = pdMS_TO_TICKS( milliseconds );

    for ( i = 0; i < ticks; i++ )
    {
        vTaskDelay( 1 );
    }
}