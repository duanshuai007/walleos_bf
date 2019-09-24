#ifndef _ENV_INCLUDE_USPIOS_H_
#define _ENV_INCLUDE_USPIOS_H_

#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

//typedef void TInterruptHandler (void *pParam);
typedef void TKernelTimerHandler (unsigned hTimer, void *pParam, void *pContext);

// returns the timer handle (hTimer)
unsigned StartKernelTimer (unsigned nHzDelay,   // in HZ units (see "system configuration" above)
               TKernelTimerHandler *pHandler,
               void *pParam, void *pContext);   // handed over to the timer handler

void CancelKernelTimer (unsigned hTimer);
void UpdateKernelTimer(unsigned hTimer, unsigned nDelay);
typedef void TInterruptHandler (void *pParam);

boolean USPiTouchScreenUpdate(void);

#ifdef __cplusplus
}
#endif

#endif
