#ifndef _TZM_CONFIG_H_
#define _TZM_CONFIG_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* Start address of non-secure application. */
#define mainNONSECURE_APP_START_ADDRESS         ( 0x00010000U )

/***********************************************************************************************************************
 * Initialize TrustZone
 **********************************************************************************************************************/
void BOARD_InitTrustZone(void);

#if defined(__cplusplus)
}
#endif

#endif /* _TZM_CONFIG_H_ */
