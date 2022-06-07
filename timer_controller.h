#ifndef _TIMER_CONTROLLER_H_
#define _TIMER_CONTROLLER_H_

#include <time.h>

/*Function for timer initialization.*/
void timerSetAndStart(timer_t *timerId, time_t triggerSec, void *callback);

/*Function for timer deinitialization.*/
void timerStopAndDelete(timer_t *timerId);

#endif
