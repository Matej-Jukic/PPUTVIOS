#include "graphics_controller.h"

#include <stdio.h>
#include <directfb.h>
#include "math.h"

#include "timer_controller.h"


// https://stackoverflow.com/questions/2570934/how-to-round-floating-point-numbers-to-the-nearest-integer-in-c
#define roundNumber(x) ((int)((x) < 0.0 ? (x)-0.5 : (x) + 0.5))

#define DFBCHECK(x...)                                           \
    {                                                            \
        DFBResult err = x;                                       \
        if (err != DFB_OK)                                       \
        {                                                        \
            fprintf(stderr, "%s <%d>:\n\t", __FILE__, __LINE__); \
            DirectFBErrorFatal(#x, err);                         \
            return GRAPHICS_CONTROLLER_ERROR;                    \
        }                                                        \
    }

/* helper variables needed only for graphics controller module */
static IDirectFBSurface *primary = NULL;
static IDirectFB *dfbInterface = NULL;
static int screenWidth = 0;
static int screenHeight = 0;
static DFBSurfaceDescription surfaceDesc;

static IDirectFBFont *fontInterface = NULL;
static DFBFontDescription fontDesc;

static timer_t timerChannelInfo;
static timer_t timerChannelNumberMessage;
static timer_t timerVolumeInfo;

static uint8_t showingChannelInfo;
static uint8_t showingVolumeInfo;

/* helper functions needed only for graphics controller module */
static void removeChannelInfo();
static void removeVolumeInfo();
static void removeMenuInfo();
static void removeChannelNumberMessage();

graphicsControllerStatus graphicsControllerInit()
{
    /* initialize DirectFB */
    DFBCHECK(DirectFBInit(NULL, NULL));

    /* fetch the DirectFB interface */
    DFBCHECK(DirectFBCreate(&dfbInterface));

    /* tell the DirectFB to take the full screen for this application */
    DFBCHECK(dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN));

    /* create primary surface with double buffering enabled */
    surfaceDesc.flags = DSDESC_CAPS;
    surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
    DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary));

    /* fetch the screen size */
    DFBCHECK(primary->GetSize(primary, &screenWidth, &screenHeight));

    return GRAPHICS_CONTROLLER_NO_ERROR;
}

graphicsControllerStatus graphicsControllerDeinit()
{
    DFBCHECK(primary->Release(primary));
    DFBCHECK(dfbInterface->Release(dfbInterface));

    return GRAPHICS_CONTROLLER_NO_ERROR;
}

graphicsControllerStatus drawChannelNumber(uint16_t channelNumberValue)
{
    if (timerChannelInfo)
        timerStopAndDelete(&timerChannelInfo);
    if (timerVolumeInfo)
        timerStopAndDelete(&timerVolumeInfo);
    if (timerChannelNumberMessage)
        timerStopAndDelete(&timerChannelNumberMessage);

    char channelNumberString[4];
    sprintf(channelNumberString, "%d", channelNumberValue);

    clearScreen(COLOUR_BLACK);

    /* specify the height of the font by raising the appropriate flag and setting the height value */
    fontDesc.flags = DFDESC_HEIGHT;
    fontDesc.height = 100;

    /* create the font and set the created font for primary surface text drawing */
    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
    DFBCHECK(primary->SetFont(primary, fontInterface));

    /* draw  channel number */
    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->DrawString(primary, channelNumberString, -1, screenHeight / 7, screenHeight / 7, DSTF_LEFT));

    return GRAPHICS_CONTROLLER_NO_ERROR;
}

graphicsControllerStatus drawChannelNumberMessage(uint16_t channelNumberValue)
{
    if (timerChannelInfo)
        timerStopAndDelete(&timerChannelInfo);
    if (timerVolumeInfo)
        timerStopAndDelete(&timerVolumeInfo);
    if (timerChannelNumberMessage)
        timerStopAndDelete(&timerChannelNumberMessage);

    char message[27];
    sprintf(message, "Channel %d doesn't exist ", channelNumberValue);

    clearScreen(COLOUR_BLACK);

    /* specify the height of the font by raising the appropriate flag and setting the height value */
    fontDesc.flags = DFDESC_HEIGHT;
    fontDesc.height = 70;

    /* create the font and set the created font for primary surface text drawing */
    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
    DFBCHECK(primary->SetFont(primary, fontInterface));

    /* draw yellow #FFA500 channel number */ ///CHANGED TO WHITE
    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->DrawString(primary, message, -1, screenHeight / 7, screenHeight / 7, DSTF_LEFT));

    /* timer setup */ ///CHANGED from 2 to 4
    timerSetAndStart(&timerChannelNumberMessage, 4, removeChannelNumberMessage);

    return GRAPHICS_CONTROLLER_NO_ERROR;
}

graphicsControllerStatus drawChannelInfo(uint16_t channelNumberValue, uint8_t subtitleCount, char *subtitles)
{
    if (timerChannelInfo)
        timerStopAndDelete(&timerChannelInfo);
    if (timerVolumeInfo)
        timerStopAndDelete(&timerVolumeInfo);
    if (timerChannelNumberMessage)
        timerStopAndDelete(&timerChannelNumberMessage);

    char channelNumber[12];

    if (channelNumberValue)
    {
        sprintf(channelNumber, "Channel %d", channelNumberValue);
    }

    int subtitlesArraySize = 5 * subtitleCount - 2; 
    char channelSubtitles[subtitlesArraySize];

    if (subtitleCount)
    {
        int i = 0;
        int j = 0;
        int character_count = 0;
        while (subtitles[i] != '\0')
        {
            channelSubtitles[j] = subtitles[i];
            character_count++;
            j++;

            if (character_count == 3)
            {
                if (subtitles[i + 1] == '\0')
                {
                    character_count = 0;
                }
                else
                {
                    channelSubtitles[j++] = ',';
                    channelSubtitles[j++] = ' ';
                    character_count = 0;
                }
            }
            i++;
        }
        channelSubtitles[subtitlesArraySize] = '\0';
    }

    clearScreen(COLOUR_BLACK);

    /* draw purple info rectangle */ ///CHANGED INFO FRAME COLOR TO BLUE
    DFBCHECK(primary->SetColor(primary, 0x80, 0x00, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->FillRectangle(primary, screenWidth / 4, (5.3 * screenHeight) / 6.5, screenWidth / 2, screenHeight / 6));

    /* draw darkPurple info rectangle */ ///SECOND CHANGE TO PURPLE INFO FILL
    DFBCHECK(primary->SetColor(primary, 0x5a, 0x00, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->FillRectangle(primary, screenWidth / 4 + 5, (5.3 * screenHeight) / 6.5 + 5, screenWidth / 2 - 10, screenHeight / 6 - 10));

    /* specify the height of the font by raising the appropriate flag and setting the height value */
    fontDesc.flags = DFDESC_HEIGHT;
    fontDesc.height = 68;

    /* create the font and set the created font for primary surface text drawing */
    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
    DFBCHECK(primary->SetFont(primary, fontInterface));

    /* draw yellow #FFA500 channel string information */ ///CHANGED - LETTERS AND POSITION OF CHANNEL NUMBER
    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->DrawString(primary, channelNumber, -1, screenWidth / 10 * 4 , (5.3 * screenHeight) / 6.5 + 80, DSTF_LEFT));

    /* specify the height of the font by raising the appropriate flag and setting the height value */
    fontDesc.flags = DFDESC_HEIGHT;
    fontDesc.height = 48;

    /* create the font and set the created font for primary surface text drawing */
    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
    DFBCHECK(primary->SetFont(primary, fontInterface));
	char broj_subtitle_kanala[4];
    char subs[10]="Subs: ";
    int iterator = 0;
    if (subtitleCount)
    {   

        sprintf(broj_subtitle_kanala, "%d", subtitleCount);
        for(iterator=0; iterator<4; iterator++){
        subs[6+iterator]=broj_subtitle_kanala[iterator];
        }
        DFBCHECK(primary->DrawString(primary, subs, -1, screenWidth / 20 * 9, (5.3 * screenHeight) / 6.5 + 140, DSTF_LEFT));
       
    }
    else
    {
        DFBCHECK(primary->DrawString(primary, "No available subtitles", -1, screenWidth / 80 * 29, (5.3 * screenHeight) / 6.5 + 140, DSTF_LEFT));
    }

    /* timer setup */
    if (showingChannelInfo)
    {
        timerStopAndDelete(&timerChannelInfo);
    }
    timerSetAndStart(&timerChannelInfo, 4, removeChannelInfo);
    showingChannelInfo = 1;

    return GRAPHICS_CONTROLLER_NO_ERROR;
}


graphicsControllerStatus drawVolumeInfo(float volumePercent)
{
    if (timerChannelInfo)
        timerStopAndDelete(&timerChannelInfo);
    if (timerVolumeInfo)
        timerStopAndDelete(&timerVolumeInfo);
    if (timerChannelNumberMessage)
        timerStopAndDelete(&timerChannelNumberMessage);

    float volumePercent_1 = volumePercent + 0.01;
    char volume[4]; // 2 digits +  % sign + '\0' 
    uint8_t volumePercentInt = roundNumber(volumePercent * 100);
    

    clearScreen(COLOUR_BLACK);

    sprintf(volume, "%d%%", volumePercentInt);
///ADDED


    DFBCHECK(primary->SetColor(primary, 0x5a, 0x00, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->FillRectangle(primary, screenWidth * 0.9 , screenHeight * 0.08, screenWidth / 14, screenHeight * 0.62));
    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->FillRectangle(primary, screenWidth * 0.91 , (screenHeight * 0.10) + ((1-volumePercent_1) * screenHeight * 0.5 ), screenWidth / 19, volumePercent_1 * screenHeight * 0.5 ));
    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->DrawRectangle(primary, screenWidth * 0.91 , screenHeight * 0.095 , screenWidth / 19, screenHeight * 0.5));

    fontDesc.flags = DFDESC_HEIGHT;
    fontDesc.height = 38;

    /* create the font and set the created font for primary surface text drawing */
    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
    DFBCHECK(primary->SetFont(primary, fontInterface));

    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, COLOUR_WHITE));
    DFBCHECK(primary->DrawString(primary, volume, -1, screenWidth * 0.96, screenHeight * 0.68 , DSTF_RIGHT));

    /* timer setup */
    if (showingVolumeInfo)
    {
        timerStopAndDelete(&timerVolumeInfo);
    }
    timerSetAndStart(&timerVolumeInfo, 2, removeVolumeInfo);
    showingVolumeInfo = 1;

    return GRAPHICS_CONTROLLER_NO_ERROR;
}

graphicsControllerStatus drawOnScreen()
{
    /* switch between the displayed and the work buffer (update the display) */
    DFBCHECK(primary->Flip(primary, NULL, 0));
    return GRAPHICS_CONTROLLER_NO_ERROR;
}

graphicsControllerStatus clearScreen(uint8_t alpha)
{
    DFBCHECK(primary->SetColor(primary, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, alpha));
    DFBCHECK(primary->FillRectangle(primary, 0, 0, screenWidth, screenHeight));

    return GRAPHICS_CONTROLLER_NO_ERROR;
}

/* -------------------- HELPER FUNCTIONS -------------------- */
/****************************************************************************
 * @brief    Function for removing channel information banner from screen at timer trigger.
****************************************************************************/
static void removeChannelInfo()
{
    showingChannelInfo = 0;
    clearScreen(COLOUR_BLACK);
    drawOnScreen();
    clearScreen(COLOUR_BLACK);
}

/****************************************************************************
 * @brief    Function for removing volume information banner from screen at timer trigger.
****************************************************************************/
static void removeVolumeInfo()
{
    if (!showingChannelInfo)
    {
        showingVolumeInfo = 0;
        clearScreen(COLOUR_BLACK);
        drawOnScreen();
        clearScreen(COLOUR_BLACK);
    }
}


/****************************************************************************
 * @brief    Function for removing channel number message from screen at timer trigger.
****************************************************************************/
static void removeChannelNumberMessage()
{
    clearScreen(COLOUR_BLACK);
    drawOnScreen();
    clearScreen(COLOUR_BLACK);
}

