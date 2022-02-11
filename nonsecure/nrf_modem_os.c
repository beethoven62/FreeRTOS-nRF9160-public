   #include <nrf_modem_os.h>
   #include <nrf_errno.h>
   #include <nrf_modem_platform.h>
   #include <nrf_modem_limits.h>

   #include <nrf.h>
   #include "errno.h"

   #define TRACE_IRQ          EGU2_IRQn
   #define TRACE_IRQ_PRIORITY 6
   #define TRACE_IRQ_HANDLER  EGU2_IRQHandler

   void read_task_create(void)
   {
       // The read task is achieved using SW interrupt.
       NVIC_SetPriority(NRF_MODEM_APPLICATION_IRQ, NRF_MODEM_APPLICATION_IRQ_PRIORITY);
       NVIC_ClearPendingIRQ(NRF_MODEM_APPLICATION_IRQ);
       NVIC_EnableIRQ(NRF_MODEM_APPLICATION_IRQ);
   }

   void trace_task_create(void) {
       NVIC_SetPriority(TRACE_IRQ, TRACE_IRQ_PRIORITY);
       NVIC_ClearPendingIRQ(TRACE_IRQ);
       NVIC_EnableIRQ(TRACE_IRQ);
   }

   void nrf_modem_os_init(void) {
       read_task_create();
       trace_task_create();
       // Initialize timers / sleeping threads used in the nrf_modem_os_timedwait function.
       // Initialize trace medium used in the nrf_modem_os_trace_put function.
   }

   int32_t nrf_modem_os_timedwait(uint32_t context, int32_t * timeout)
   {
       // Return remaining time by reference in timeout parameter,
       // if not yet timed out.
       // Else return NRF_ETIMEDOUT if timeout has triggered.
       // A blind return value of 0 will make all Modem library operations
       // always block.
       return 0;
   }

   void nrf_modem_os_errno_set(int errno_val) {
       // Translate nrf_errno.h errno to the OS specific value.
   }

   void nrf_modem_os_application_irq_set(void) {
       NVIC_SetPendingIRQ(NRF_MODEM_APPLICATION_IRQ);
   }


   void nrf_modem_os_application_irq_clear(void) {
       NVIC_ClearPendingIRQ(NRF_MODEM_APPLICATION_IRQ);
   }

   void NRF_MODEM_APPLCAITON_IRQ_HANDLER(void) {
       nrf_modem_os_application_irq_handler();
   }

   void nrf_modem_os_trace_irq_set(void) {
       NVIC_SetPendingIRQ(TRACE_IRQ);
   }

   void nrf_modem_os_trace_irq_clear(void) {
       NVIC_ClearPendingIRQ(TRACE_IRQ);
   }

   void TRACE_IRQ_HANDLER(void) {
       nrf_modem_os_trace_irq_handler();
   }

   int32_t nrf_modem_os_trace_put(const uint8_t * const p_buffer, uint32_t buf_len) {
       // Store buffer to chosen medium.
       // Traces can be dropped if not needed.
       return 0;
   }
