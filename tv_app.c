#include "remote_controller.h"

#include <pthread.h>

int main(int argc, char **argv)
{
    initialConfig config;
    pthread_t remoteThreadHandle;
    pthread_t channelsSetupHandle;

    if (argc != 2)
    {
        printf("Invalid number of command line arguments!\n");
        return 1;
    }

    /* parse initial configuration file  */
    ASSERT_TDP_RESULT(parseConfigurationFile(argv[1], &config), "parseConfigurationFile");

    /* remote controller initialization */
    ASSERT_TDP_RESULT(remoteControllerInit(), "remoteControllerInit");
    ASSERT_TDP_RESULT(pthread_create(&remoteThreadHandle, NULL, &remoteControllerEvent, NULL), "remote controller thread create");

    /* graphics controller initialization */
    ASSERT_TDP_RESULT(graphicsControllerInit(), "graphicsControllerInit");

    /* stream controller initialization */
    ASSERT_TDP_RESULT(streamControllerInit(&config), "streamControllerInit");
    ASSERT_TDP_RESULT(startPlayerStream(&config.startingChannel), "startPlayerStream");

    /* channel configuration thread initialization */
    ASSERT_TDP_RESULT(pthread_create(&channelsSetupHandle, NULL, &channelsSetup, NULL), "channel setup thread create");

    /* wait for exit key press */
    ASSERT_TDP_RESULT(pthread_join(remoteThreadHandle, NULL), "remote controller thread handle join");

    /* deinitialization and deallocation */
    ASSERT_TDP_RESULT(streamControllerDeinit(), "streamControllerDeinit");
    ASSERT_TDP_RESULT(graphicsControllerDeinit(), "graphicsControllerDeinit");

    return 0;
}
