#include <timeslicetimerhandler.h>
#include <scheduler.h>
#include <config.h>

void TimeSliceTimerHandler(unsigned hTimer, void *pParam, void *pContext)
{
    UNUSED(hTimer);
    UNUSED(pParam);
    UNUSED(pContext);

	SchedulerYieldInterrupt();
}
