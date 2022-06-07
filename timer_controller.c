#include "timer_controller.h"

#include <signal.h>
#include <string.h>

void timerSetAndStart(timer_t *timerId, time_t triggerSec, void *callback)
{
    struct sigevent signalEvent;
    struct itimerspec timerSpec;

    /* tell OS to send notification by calling specific function from specific thread*/
    signalEvent.sigev_notify = SIGEV_THREAD;

    /* function which OS will call when interval expires */
    signalEvent.sigev_notify_function = callback;

    /* function arguments */
    signalEvent.sigev_value.sival_ptr = NULL;

    /* thread attributes - if NULL default attributes are applied */
    signalEvent.sigev_notify_attributes = NULL;

    /*parameters: system clock for time measurement, timer setup, where to put new timer ID*/
    timer_create(CLOCK_REALTIME, &signalEvent, timerId);

    memset(&timerSpec, 0, sizeof(timerSpec));
    /* timer initial value */
    timerSpec.it_value.tv_sec = triggerSec;
    timerSpec.it_value.tv_nsec = 0;

    /* arms or disarms the timer identified by timerId */
    timer_settime(*timerId, 0, &timerSpec, NULL);
}

void timerStopAndDelete(timer_t *timerId)
{
    struct itimerspec timerSpec;
    memset(&timerSpec, 0, sizeof(timerSpec));
    timer_settime(timerId, 0, &timerSpec, NULL);
    timer_delete(*timerId);
}
