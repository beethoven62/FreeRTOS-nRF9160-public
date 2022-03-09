/* Modem includes */
#include <nrf_modem.h>
#include <nrf_modem_os.h>
#include <nrf_errno.h>
#include <nrf_modem_platform.h>
#include <nrf_modem_limits.h>

/* Device includes */
#include <nrf.h>
#include "errno.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Other includes */
#include "defs.h"

#define NRF_MODEM_APPLICATION_IRQ           EGU1_IRQn
#define NRF_MODEM_APPLICATION_IRQ_PRIORITY  6
#define NRF_MODEM_APPLICATION_IRQ_HANDLER   EGU1_IRQHandler

#define TRACE_IRQ                           EGU2_IRQn
#define TRACE_IRQ_PRIORITY                  6
#define TRACE_IRQ_HANDLER                   EGU2_IRQHandler

static TimerHandle_t xModemTimer;
void vModemTimerCallback( TimerHandle_t xTimer );

void read_task_create( void )
{
    // The read task is achieved using SW interrupt.
    NVIC_SetPriority( NRF_MODEM_APPLICATION_IRQ, NRF_MODEM_APPLICATION_IRQ_PRIORITY );
    NVIC_ClearPendingIRQ( NRF_MODEM_APPLICATION_IRQ );
    NVIC_EnableIRQ( NRF_MODEM_APPLICATION_IRQ );
}

void trace_task_create( void )
{
    NVIC_SetPriority( TRACE_IRQ, TRACE_IRQ_PRIORITY );
    NVIC_ClearPendingIRQ( TRACE_IRQ );
    NVIC_EnableIRQ( TRACE_IRQ );
}

void nrf_modem_os_init( void )
{
    read_task_create();
    trace_task_create();
    // Initialize timers / sleeping threads used in the nrf_modem_os_timedwait function.
    xModemTimer = xTimerCreate(
                             /* Time name */
                             "ModemTimer",
                             /* The timer period in ticks, must be greater than 0. */
                             pdMS_TO_TICKS( 10000 ),
                             /* The timer is a one-shot. */
                             pdFALSE,
                             /* The timer ID. */
                             ( void * ) 0,
                             /* The callback when the timerexpires. */
                             vModemTimerCallback
                             );

    // Initialize trace medium used in the nrf_modem_os_trace_put function.
}

void *nrf_modem_os_shm_tx_alloc( size_t bytes )
{
    return nrf_modem_os_alloc( bytes );
}

void nrf_modem_os_shm_tx_free( void *mem )
{
    nrf_modem_os_free( mem );
}

void *nrf_modem_os_alloc( size_t bytes )
{
    void* ptr = NULL;

    if( bytes > 0 )
    {
        // We simply wrap the FreeRTOS call into a standard form
        ptr = pvPortMalloc( bytes );
    } // else NULL if there was an error

    return ptr;
}

void nrf_modem_os_free( void *mem )
{
    if( mem )
    {
        // We simply wrap the FreeRTOS call into a standard form
        vPortFree( mem );
    }
}

int32_t nrf_modem_os_timedwait( uint32_t context, int32_t * timeout )
{
    // Return remaining time by reference in timeout parameter,
    // if not yet timed out.
    // Else return NRF_ETIMEDOUT if timeout has triggered.
    // A blind return value of 0 will make all Modem library operations
    // always block.
    //return 0;

    if ( *timeout == 0 )
    {
        vPortYield();
        return NRF_ETIMEDOUT;
    }

    if ( *timeout < 0 )
    {
        return 0;
    }

    if( xTimerIsTimerActive( xModemTimer ) == pdFALSE )
    {
        if ( ( uint32_t )pvTimerGetTimerID( xModemTimer ) == 0 )
        {
            xTimerChangePeriod( xModemTimer, pdMS_TO_TICKS( *timeout * 1000 ), 100 );
            xTimerStart( xModemTimer, 0 );
            vTimerSetTimerID( xModemTimer, ( void* )1 );
        }
        else
        {
            *timeout = 0;
            vTimerSetTimerID( xModemTimer, ( void* )0 );
        }
    }
    else
    {
        *timeout = ( int32_t )( pdTICKS_TO_MS( xTimerGetExpiryTime( xModemTimer ) ) - pdTICKS_TO_MS( xTaskGetTickCount() ) + 500 ) / 1000;
        *timeout = *timeout > 0 ? *timeout : 0;
    }

    if ( *timeout == 0 )
    {
        return NRF_ETIMEDOUT;
    }

    vPortYield();
    return 0;
}

void nrf_modem_os_errno_set( int errno_val )
{
    // Translate nrf_errno.h errno to the OS specific value.
}

void nrf_modem_os_application_irq_set( void )
{
    NVIC_SetPendingIRQ( NRF_MODEM_APPLICATION_IRQ );
}

void nrf_modem_os_application_irq_clear( void )
{
    NVIC_ClearPendingIRQ( NRF_MODEM_APPLICATION_IRQ );
}

void NRF_MODEM_APPLICATION_IRQ_HANDLER( void )
{
    nrf_modem_application_irq_handler();
}

void nrf_modem_os_trace_irq_set( void )
{
    NVIC_SetPendingIRQ( TRACE_IRQ );
}

void nrf_modem_os_trace_irq_clear( void )
{
    NVIC_ClearPendingIRQ( TRACE_IRQ );
}

void TRACE_IRQ_HANDLER( void )
{
    nrf_modem_trace_irq_handler();
}

int32_t nrf_modem_os_trace_put( const uint8_t * const p_buffer, uint32_t buf_len )
{
    // Store buffer to chosen medium.
    // Traces can be dropped if not needed.
    return 0;
}

void vModemTimerCallback( TimerHandle_t xTimer )
{
    vTimerSetTimerID( xTimer, ( void* )2 );
}

void nrf_modem_os_busywait( int32_t usec )
{
    vTaskDelay( pdMS_TO_TICKS( usec / 1000 ) );
}

bool nrf_modem_os_is_in_isr( void )
{
    /*
     * TODO: need to read interrupt flag.
     * So we can use it here.
     */

    return false;
}

int nrf_modem_os_sem_init( void **arg, unsigned int initial_count, unsigned int limit )
{
    return 0;
}
