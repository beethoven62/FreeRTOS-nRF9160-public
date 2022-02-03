#ifndef __LOG_DEMO_H__
#define __LOG_DEMO_H__

#include "queue.h"

#define LOG_QUEUE_LEN   ( 5 )
#define LOG_MSG_MAX     ( 32 )

/* Define the stucture of the log message that will be queued */
typedef struct LogMessage
{
    uint32_t ucLogMessageID;
    char ucData[ LOG_MSG_MAX ];
} LogMessage_t;

/**
 * @brief Creates all the tasks for MPU demo.
 *
 * The log task is a privileged task that prints out logs 
 */
void vStartLogTask( void );
void vLogPrint( char * pcLogMessage );
QueueHandle_t xGetLogHandle( void );
uint32_t uiGetMessageID( void );

#endif /* __LOG_DEMO_H__ */