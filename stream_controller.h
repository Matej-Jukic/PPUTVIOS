#ifndef _STREAM_CONTROLLER_H_
#define _STREAM_CONTROLLER_H_

#include "configuration_parser.h"

typedef enum _streamControllerStatus
{
    STREAM_CONTROLLER_NO_ERROR = 0,
    STREAM_CONTROLLER_ERROR
} streamControllerStatus;

#define ASSERT_TDP_RESULT(x, y)              \
    {                                        \
        if (STREAM_CONTROLLER_NO_ERROR == x) \
            printf("%s success\n", y);       \
        else                                 \
        {                                    \
            textColor(1, 1, 0);              \
            printf("%s fail\n", y);          \
            textColor(0, 7, 0);              \
            return STREAM_CONTROLLER_ERROR;  \
        }                                    \
    }

typedef struct _channelData
{
    uint16_t pmtProgramNumber;

    startingChannelInit channelInit;

    uint32_t presentShowStartTime;
    uint32_t presentShowDuration;
    char *presentShowName;
    char *presentShowDescription;

    uint32_t followingShowStartTime;
    uint32_t followingShowDuration;
    char *followingShowName;
    char *followingShowDescription;

    uint8_t subtitleCount;
    char *subtitles;
} channelData;

typedef struct _channels
{
    channelData *channel;
    uint32_t channelCount;
} Channels;

typedef enum _dvbStreamType
{
    dvbVideo = 0x01,
    dvbVideoMPEG2 = 0x02,
    dvbAudioMPEG = 0x03
} dvbStreamType;

/*Function for tuner and player initialization.*/
streamControllerStatus streamControllerInit(initialConfig *config);

/*Function for tuner and player deinitialization.*/
streamControllerStatus streamControllerDeinit();

/*Function for creating player stream and setting volume.*/
streamControllerStatus startPlayerStream(startingChannelInit *channel);

/*Function for removing player stream.*/
streamControllerStatus stopPlayerStream();

/*Function for setting up channels based on information from PAT, PMT and EIT tables.*/
void *channelsSetup();

/*Function for starting player stream.*/
streamControllerStatus playChannel(uint16_t channelNumber);

/*Function for starting player stream for next channel.*/
streamControllerStatus playNextChannel();

/*Function for starting player stream for previous channel.*/
streamControllerStatus playPreviousChannel();

/*Function for muting or unmuting volume.*/
streamControllerStatus volumeMute();

/*Function for increasing volume.*/
streamControllerStatus volumeUp();

/*Function for decreasing volume.*/
streamControllerStatus volumeDown();

/*Function for calling functions for drawing and showing channel information banner.*/
streamControllerStatus showChannelInfo();

/*Function for calling functions for drawing and showing channel volume banner.*/
streamControllerStatus showVolumeInfo();

/*Function for calling functions for drawing and showing input channel number value.*/
streamControllerStatus showChannelNumber(uint16_t channelNumberValue);

/*Function for calling functions for drawing and showing input channel number message.*/
streamControllerStatus showChannelNumberMessage(uint16_t channelNumberValue);

#endif
