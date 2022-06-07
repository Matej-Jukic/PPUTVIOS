#include "remote_controller.h"

#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

/* helper keywords needed only for graphics controller module */
#define DEV_PATH "/dev/input/event0"
#define NUM_EVENTS 5

#define REMOTE_KEY_1 2
#define REMOTE_KEY_2 3
#define REMOTE_KEY_3 4
#define REMOTE_KEY_4 5
#define REMOTE_KEY_5 6
#define REMOTE_KEY_6 7
#define REMOTE_KEY_7 8
#define REMOTE_KEY_8 9
#define REMOTE_KEY_9 10
#define REMOTE_KEY_0 11
#define REMOTE_KEY_MUTE 60
#define REMOTE_KEY_PROGRAM_UP 62
#define REMOTE_KEY_PROGRAM_DOWN 61
#define REMOTE_KEY_VOLUME_UP 63
#define REMOTE_KEY_VOLUME_DOWN 64
#define REMOTE_KEY_INFO 358
#define REMOTE_KEY_EXIT 102

/* helper variables needed only for remote controller module */
static int32_t inputFileDesc;
static struct input_event *eventBuf;

static uint16_t channelNumber;
static timer_t timerChannelNumber;
static uint8_t channelKeysPressed;
static uint8_t channelKeys[3];

static uint8_t showingMenuInfo;

/* helper functions needed only for remote controller module */
remoteControllerStatus getKeys(int32_t count, uint8_t *buf, int32_t *eventRead);
static void generateChannelNumber(uint8_t remoteKey);
static void changeChannel();

remoteControllerStatus remoteControllerInit()
{
    char deviceName[20];

    inputFileDesc = open(DEV_PATH, O_RDWR);
    if (inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !\n", strerror(errno));
        return REMOTE_CONTROLLER_ERROR;
    }

    ioctl(inputFileDesc, EVIOCGNAME(sizeof(deviceName)), deviceName);
    printf("RC device opened succesfully [%s]\n", deviceName);

    eventBuf = malloc(NUM_EVENTS * sizeof(struct input_event));
    if (!eventBuf)
    {
        printf("Error allocating memory !\n");
        return REMOTE_CONTROLLER_ERROR;
    }

    return REMOTE_CONTROLLER_NO_ERROR;
}

void *remoteControllerEvent()
{
    uint32_t eventCnt;
    uint32_t i;
    uint32_t exit = 0;

    while (exit == 0)
    {
        /* read input events */
        if (getKeys(NUM_EVENTS, (uint8_t *)eventBuf, &eventCnt))
        {
            printf("Error while reading input events!");
            exit = 1;
        }

        for (i = 0; i < eventCnt; i++)
        {
            if (eventBuf[i].value == 1)
            {
                switch (eventBuf[i].code)
                {
                case REMOTE_KEY_PROGRAM_UP:
                    playNextChannel();
                    break;

                case REMOTE_KEY_PROGRAM_DOWN:
                    playPreviousChannel();
                    break;

                case REMOTE_KEY_VOLUME_UP:
                    volumeUp();
                    break;

                case REMOTE_KEY_VOLUME_DOWN:
                    volumeDown();
                    break;

                case REMOTE_KEY_MUTE:
                    volumeMute();
                    break;

                case REMOTE_KEY_INFO:
                    showChannelInfo();
                    break;
            
                case REMOTE_KEY_EXIT:
                    exit = 1;
                    break;

                default:
                    if (eventBuf[i].code >= 2 && eventBuf[i].code <= 11)
                    {
                        /* remote number buttor pressed */
                        if (timerChannelNumber)
                            timerStopAndDelete(&timerChannelNumber);
                        if (eventBuf[i].code != 11)
                            generateChannelNumber(eventBuf[i].code - 1);
                        else
                            generateChannelNumber(0);
                        showChannelNumber(channelNumber);
                        timerSetAndStart(&timerChannelNumber, 3, changeChannel);
                    }
                    else
                    {
                        printf("%d key not assigned!\n", eventBuf[i].code);
                    }
                    break;
                } // switch exit
            }     // if exit
            else if (eventBuf[i].value == 2)
            {
                switch (eventBuf[i].code)
                {
                case REMOTE_KEY_VOLUME_UP:
                    volumeUp();
                    break;

                case REMOTE_KEY_VOLUME_DOWN:
                    volumeDown();
                    break;
                } 
            }     
        }         
    }             

    free(eventBuf);
    return (void *)REMOTE_CONTROLLER_NO_ERROR;
}

/*Function for getting values from remote key press.*/
remoteControllerStatus getKeys(int32_t count, uint8_t *buf, int32_t *eventsRead)
{
    int32_t ret = 0;

    /* read input events and put them in buffer */
    ret = read(inputFileDesc, buf, (size_t)(count * (int)sizeof(struct input_event)));
    if (ret <= 0)
    {
        printf("Error code %d", ret);
        return REMOTE_CONTROLLER_ERROR;
    }
    /* calculate number of read events */
    *eventsRead = ret / (int)sizeof(struct input_event);

    return REMOTE_CONTROLLER_NO_ERROR;
}

/*Function for generating channel number based on remote key input values.*/
static void generateChannelNumber(uint8_t remoteKey)
{
    switch (channelKeysPressed)
    {
    case 0:
        channelKeys[0] = remoteKey;
        channelNumber = channelKeys[0];
        channelKeysPressed++;
        break;

    case 1:
        channelKeys[1] = remoteKey;
        channelNumber = 10 * channelKeys[0] + channelKeys[1];
        channelKeysPressed++;
        break;

    case 2:
        channelKeys[2] = remoteKey;
        channelNumber = 100 * channelKeys[0] + 10 * channelKeys[1] + channelKeys[2];
        channelKeysPressed++;
        break;

    case 3:
        channelKeysPressed = 1;
        channelKeys[0] = remoteKey;
        channelKeys[1] = 0;
        channelKeys[2] = 0;
        channelNumber = channelKeys[0];
        break;
    }
}

/*Function for executing channel change at timer trigger.*/
static void changeChannel()
{
    playChannel(channelNumber);
    channelKeysPressed = 0;
    int i;
    for (i = 0; i < 3; i++)
    {
        channelKeys[i] = 0;
    }
}
