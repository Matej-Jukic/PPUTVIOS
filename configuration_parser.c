#include "configuration_parser.h"

/* helper keywords needed only for configuration parser module */
#define CONFIG_FILE_EXTENSION ".xml"
#define INITIAL_CONFIG "initial_config"
#define TRANSPONDER "transponder"
#define STARTING_CHANNEL "starting_channel"
#define FREQUENCY "frequency"
#define BANDWIDTH "bandwidth"
#define MODULE "module"
#define AUDIO_PID "audio_pid"
#define VIDEO_PID "video_pid"
#define AUDIO_TYPE "audio_type"
#define VIDEO_TYPE "video_type"
#define MODULE_DVBT "DVB-T"
#define MODULE_DVBT2 "DVB-T2"

#define BUFFER_MAX 50     // maximum characters per line
#define KEY_BUFFER_MAX 20 // maximum keyword characters

/* macro function needed only for configuration parser module */
#define ASSERT_PARSING_RESULT(x, y)            \
    {                                          \
        if (CONFIGURATION_PARSER_NOT_SET == x) \
        {                                      \
            textColor(1, 1, 0);                \
            printf("%s not set.\n", y);        \
            textColor(0, 7, 0);                \
            return CONFIGURATION_PARSER_ERROR; \
        }                                      \
    }

/* helper functions needed only for configuration parser module */
static void initValues(initialConfig *config);
static configurationParserStatus checkValues(initialConfig *config);
static void printValues(initialConfig *config);

configurationParserStatus parseConfigurationFile(char *fileName, initialConfig *config)
{
    // https://stackoverflow.com/questions/5309471/getting-file-extension-in-c
    const char *dot = strrchr(fileName, '.');
    if (!dot || dot == fileName)
    {
        printf("Error opening configuration file. Wrong file format.\n");
        return CONFIGURATION_PARSER_ERROR;
    }

    if (strcmp(dot, CONFIG_FILE_EXTENSION) != 0)
    {
        printf("Error opening configuration file. Wrong file extension.\n");
        printf("Expected file extension is %s\n", CONFIG_FILE_EXTENSION);
        printf("Loaded file extension is %s\n", dot);
        return CONFIGURATION_PARSER_ERROR;
    }

    FILE *filePointer;
    if ((filePointer = fopen(fileName, "r")) == NULL)
    {
        printf("Error opening configuration file.\n");
        return CONFIGURATION_PARSER_ERROR;
    }

    initValues(config);

    // https://stackoverflow.com/questions/22330969/using-fscanf-vs-fgets-and-sscanf
    char buffer[BUFFER_MAX] = {0};  // buffer for storing read lines
    char key[KEY_BUFFER_MAX] = {0}; // buffer for storing read keywords

    /* helper flags */
    int initialConfigFlag = 0;
    int transponderFlag = 0;
    int startingChannelFlag = 0;
    int isStartingChannelSet = 0;
    int isTranspoderSet = 0;

    long initialConfigFilePointerLocation = 0; // stores filePointer location after initial_config keyword is read

    while (fgets(buffer, BUFFER_MAX, filePointer) != NULL)
    {
        if (sscanf(buffer, "<%[^>]", key) == 1)
        {
            if (!strcmp(key, INITIAL_CONFIG))
            {
                initialConfigFlag = 1;
                initialConfigFilePointerLocation = ftell(filePointer);
            }
        }

        while (initialConfigFlag)
        {
            /* transponder node */
            if (sscanf(buffer, " <%[^>]", key) == 1)
            {
                if (!strcmp(key, TRANSPONDER) && !isTranspoderSet)
                {
                    transponderFlag = 1;
                }

                while (transponderFlag)
                {
                    if (sscanf(buffer, " <frequency>%[^<]", key) == 1)
                    {
                        config->transponder.frequency = atoi(key);
                    }

                    if (sscanf(buffer, " <bandwidth>%[^<]", key) == 1)
                    {
                        config->transponder.bandwidth = atoi(key);
                    }

                    if (sscanf(buffer, " <module>%[^<]", key) == 1)
                    {
                        if (!strcmp(key, MODULE_DVBT))
                        {
                            config->transponder.module = DVB_T;
                        }
                        if (!strcmp(key, MODULE_DVBT2))
                        {
                            config->transponder.module = DVB_T2;
                        }
                    }

                    if (sscanf(buffer, " </%[^>]", key) == 1)
                    {
                        if (!strcmp(key, TRANSPONDER))
                        {
                            transponderFlag = 0;
                            isTranspoderSet = 1;
                            if (isTranspoderSet && !isStartingChannelSet)
                            {
                                fseek(filePointer, initialConfigFilePointerLocation, SEEK_SET);
                            }
                        }
                    }
                    fgets(buffer, BUFFER_MAX, filePointer);
                } // transponder flag while
            }     // transponder node if

            /* starting_channel node */
            if (sscanf(buffer, " <%[^>]", key) == 1)
            {
                if (!strcmp(key, STARTING_CHANNEL) && !isStartingChannelSet)
                {
                    startingChannelFlag = 1;
                }

                while (startingChannelFlag)
                {
                    if (sscanf(buffer, " <audio_pid>%[^<]", key) == 1)
                    {
                        config->startingChannel.audioPID = atoi(key);
                    }

                    if (sscanf(buffer, " <video_pid>%[^<]", key) == 1)
                    {
                        config->startingChannel.videoPID = atoi(key);
                    }

                    if (sscanf(buffer, " <audio_type>%[^<]", key) == 1)
                    {
                        if (!strcmp(key, "ac3"))
                        {
                            config->startingChannel.audioType = AUDIO_TYPE_DOLBY_AC3;
                        }
                        if (!strcmp(key, "mpeg"))
                        {
                            config->startingChannel.audioType = AUDIO_TYPE_MPEG_AUDIO;
                        }
                    }

                    if (sscanf(buffer, " <video_type>%[^<]", key) == 1)
                    {
                        if (!strcmp(key, "mpeg2"))
                        {
                            config->startingChannel.videoType = VIDEO_TYPE_MPEG2;
                        }
                    }

                    if (sscanf(buffer, " </%[^>]", key) == 1)
                    {
                        if (!strcmp(key, STARTING_CHANNEL))
                        {
                            startingChannelFlag = 0;
                            isStartingChannelSet = 1;
                            if (isStartingChannelSet && !isTranspoderSet)
                            {
                                fseek(filePointer, initialConfigFilePointerLocation, SEEK_SET); // returns filePointer back to set location
                            }
                        }
                    }
                    fgets(buffer, BUFFER_MAX, filePointer);
                } // starting_channel flag while
            }     // starting_channel node if

            if (sscanf(buffer, " </%[^>]", key) == 1)
            {
                if (!strcmp(key, INITIAL_CONFIG))
                {
                    initialConfigFlag = 0;
                }
            }
            fgets(buffer, BUFFER_MAX, filePointer);
        } // while initialConfigFlag
    }     // main while exit

    fclose(filePointer);

    if (checkValues(config) == CONFIGURATION_PARSER_NO_ERROR)
    {
        printf("Configuration file parsed!\n");
        printValues(config);
        return CONFIGURATION_PARSER_NO_ERROR;
    }

    printf("Configuration file parsing failed!\n");
    return CONFIGURATION_PARSER_ERROR;
}

/****************************************************************************
 * @brief    Function for setting variables to initial value. Values are used for later validation.
 *
 * @param    config - [in] Pointer to structure variable in which loaded parameters are stored.
****************************************************************************/
static void initValues(initialConfig *config)
{
    config->transponder.frequency = CONFIGURATION_PARSER_NOT_SET;
    config->transponder.bandwidth = CONFIGURATION_PARSER_NOT_SET;
    config->transponder.module = CONFIGURATION_PARSER_NOT_SET;
    config->startingChannel.audioPID = CONFIGURATION_PARSER_NOT_SET;
    config->startingChannel.videoPID = CONFIGURATION_PARSER_NOT_SET;
    config->startingChannel.audioType = CONFIGURATION_PARSER_NOT_SET;
    config->startingChannel.videoType = CONFIGURATION_PARSER_NOT_SET;
}

/****************************************************************************
 * @brief    Function for checking if values are read from file and saved to corresponding variable.
 *
 * @param    config - [in] Pointer to structure variable in which loaded parameters are stored.
 *
 * @return   CONFIGURATION_PARSER_NO_ERROR, if there are no errors.
 *           CONFIGURATION_PARSER_ERROR, in case of an error.
****************************************************************************/
static configurationParserStatus checkValues(initialConfig *config)
{
    ASSERT_PARSING_RESULT(config->transponder.frequency, "Frequency");

    ASSERT_PARSING_RESULT(config->transponder.bandwidth, "Bandwidth");

    ASSERT_PARSING_RESULT(config->transponder.module, "Module");

    ASSERT_PARSING_RESULT(config->startingChannel.audioPID, "Initial channel audio PID");

    ASSERT_PARSING_RESULT(config->startingChannel.videoPID, "Initial channel video PID");

    ASSERT_PARSING_RESULT(config->startingChannel.audioType, "Initial channel audio type");

    ASSERT_PARSING_RESULT(config->startingChannel.videoType, "Initial channel video type");

    return CONFIGURATION_PARSER_NO_ERROR;
}

/****************************************************************************
 * @brief    Function for printing configuration variables values.
 *
 * @param    config - [in] Pointer to structure variable in which loaded parameters are stored.
****************************************************************************/
static void printValues(initialConfig *config)
{
    printf("Configuration file values:\n");
    printf("\tfrequency: %d\n", config->transponder.frequency);
    printf("\tbandwidth: %d\n", config->transponder.bandwidth);
    printf("\tmodule: %d\n", config->transponder.module);
    printf("\taudioPID: %d\n", config->startingChannel.audioPID);
    printf("\tvideoPID: %d\n", config->startingChannel.videoPID);
    printf("\taudioType: %d\n", config->startingChannel.audioType);
    printf("\tvideoType: %d\n", config->startingChannel.videoType);
}

