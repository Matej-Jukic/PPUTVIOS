#ifndef _CONFIGURATION_PARSER_H_
#define _CONFIGURATION_PARSER_H_

#ifndef _TDP_API_H_
#define _TDP_API_H_

#include "tdp_api.h"

#endif // _TDP_API_H_

#include <stdio.h>
#include <string.h>

/* macro for changing console text colour in case of an error */
static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
    char command[13];
    /* Command is the control command to the terminal */
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    printf("%s", command);
}

// https://stackoverflow.com/questions/742699/returning-an-enum-from-a-function-in-c
typedef enum _configurationParserStatus
{
    CONFIGURATION_PARSER_NOT_SET = -1,
    CONFIGURATION_PARSER_NO_ERROR,
    CONFIGURATION_PARSER_ERROR
} configurationParserStatus;

typedef struct _transponderInit
{
    uint32_t frequency;
    uint32_t bandwidth;
    t_Module module;
} transponderInit;

typedef struct _startingChannelInit
{
    uint32_t audioPID;
    uint32_t videoPID;
    tStreamType audioType;
    tStreamType videoType;
} startingChannelInit;

typedef struct _initialConfig
{
    transponderInit transponder;
    startingChannelInit startingChannel;
} initialConfig;

/****************************************************************************
 * @brief    Function for loading and parsing initial .xml configuration file.
 *
 * @param    fileName - [in] Full path to configuration file.
 *           config - [in] Pointer to structure variable in which loaded parameters are stored.
 *
 * @return   CONFIGURATION_PARSER_NO_ERROR, if there are no errors.
 *           CONFIGURATION_PARSER_ERROR, in case of an error.
****************************************************************************/
configurationParserStatus parseConfigurationFile(char *fileName, initialConfig *config);

#endif // _CONFIGURATION_PARSER_H_
