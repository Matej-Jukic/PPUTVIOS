#ifndef _TABLES_PARSER_H_
#define _TABLES_PARSER_H_

#include <stdint.h>

#define SUBTITLING_DESCRIPTOR_TAG 0x59
#define SHORT_EVENT_DESCRIPTOR_TAG 0x4D
#define SUBTITLE_CHARACTERS_COUNT 3

typedef enum _tablesParserStatus
{
    TABLES_PARSER_NOT_INITIALIZED = -1,
    TABLES_PARSER_NO_ERROR,
    TABLES_PARSER_ERROR
} tablesParserStatus;

/* ---- PAT table ---- */
typedef struct _patTableHeader
{
    uint8_t tableId;
    uint8_t sectionSyntaxIndicator;
    uint16_t sectionLength;
    uint16_t transportStreamId;
    uint8_t versionNumber;
    uint8_t currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
} patTableHeader;

typedef struct _patTableProgramInformation
{
    uint16_t programNumber;
    uint16_t programMapPid;
} patTableProgramInformation;

typedef struct _patTable
{
    patTableHeader patHeader;
    patTableProgramInformation *programInformation;
    uint8_t sectionCount;
    uint8_t programCount;
} patTable;
/* ---- PAT table ---- */

/* ---- PMT table ---- */
typedef struct _pmtTableHeader
{
    uint8_t tableId;
    uint8_t sectionSyntaxIndicator;
    uint16_t sectionLength;
    uint16_t programNumber;
    uint8_t versionNumber;
    uint8_t currentNextIndicator;
    uint8_t sectionNumber;
    uint8_t lastSectionNumber;
    uint16_t pcrPid;
    uint16_t programInfoLength;
} pmtTableHeader;

typedef struct _pmtTableElementaryInformation
{
    uint8_t streamType;
    uint16_t elementaryPid;
    uint16_t esInfoLength;
} pmtTableElementaryInformation;

typedef struct _pmtTable
{
    pmtTableHeader pmtHeader;
    pmtTableElementaryInformation *elementaryInformation;
    uint8_t elementaryInformationCount;
    uint8_t subtitleCount;
    char *subtitles;
} pmtTable;
/* ---- PMT table ---- */


/*Function for parsing PAT table from transport stream.*/
tablesParserStatus parsePAT(uint8_t *buffer, patTable *pat);

/*Function for parsing PMT table from transport stream.*/
tablesParserStatus parsePMT(uint8_t *buffer, pmtTable *pmt);

/*Function for printing PAT table variables values.*/
tablesParserStatus printPAT(patTable *pat);

/*Function for printing PMT table variables values.*/
tablesParserStatus printPMT(pmtTable *pmt);


#endif // _TABLES_PARSER_H_
