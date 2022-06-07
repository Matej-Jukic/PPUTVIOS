#ifndef _REMOTE_CONTROLLER_H_
#define _REMOTE_CONTROLLER_H_

#include "stream_controller.h"

typedef enum _remoteControllerStatus
{
    REMOTE_CONTROLLER_NO_ERROR = 0,
    REMOTE_CONTROLLER_ERROR
} remoteControllerStatus;

/*Function for remote controller initialization.*/
remoteControllerStatus remoteControllerInit();

/*Function for executing functions on corresponding key press event.*/
void *remoteControllerEvent();

#endif
