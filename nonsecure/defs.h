/*
 * Common definitions
 */

#ifndef __DEFS_H__
#define __DEFS_H__

#define pdTICKS_TO_MS( xTimeInTicks )    ( uint32_t )( ( uint64_t )( ( TickType_t ) xTimeInTicks * ( TickType_t ) 1000U ) / ( TickType_t ) configTICK_RATE_HZ )

#endif /* __DEFS_H__ */
