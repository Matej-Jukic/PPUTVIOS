#include "tables_parser.h"

#include <stdio.h>
#include <stdlib.h>

tablesParserStatus parsePAT(uint8_t *buffer, patTable *pat)
{
    pat->patHeader.tableId = (uint8_t)*buffer;

    pat->patHeader.sectionSyntaxIndicator = (uint8_t)(*(buffer + 1) >> 7) & 0x01;

    pat->patHeader.sectionLength = (uint16_t)((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF;

    pat->patHeader.transportStreamId = (uint16_t)(*(buffer + 3) << 8) + *(buffer + 4);

    pat->patHeader.versionNumber = (uint8_t)(*(buffer + 5) >> 1) & 0x001F;

    pat->patHeader.currentNextIndicator = (uint8_t)(*(buffer + 5) >> 7) & 0x01;

    pat->patHeader.sectionNumber = (uint8_t) * (buffer + 6);

    pat->patHeader.lastSectionNumber = (uint8_t) * (buffer + 7);

    pat->programCount = 0;
    pat->sectionCount = (uint8_t)(pat->patHeader.sectionLength - 9) / 4;

    pat->programInformation = (patTableProgramInformation *)malloc(pat->sectionCount * sizeof(patTableProgramInformation));

    int i;

    for (i = 0; i < pat->sectionCount; i++)
    {
        pat->programInformation[i].programNumber = (uint16_t)(*(buffer + 8 + i * 4) << 8) + *(buffer + 9 + i * 4);
        pat->programInformation[i].programMapPid = (uint16_t)((*(buffer + 10 + i * 4) << 8) + *(buffer + 11 + i * 4)) & 0x1FFF;

        if (pat->programInformation[i].programNumber)
        {
            pat->programCount++;
        }
    }

    

    return TABLES_PARSER_NO_ERROR;
}

tablesParserStatus parsePMT(uint8_t *buffer, pmtTable *pmt)
{
    pmt->pmtHeader.tableId = (uint8_t)*buffer;

    pmt->pmtHeader.sectionSyntaxIndicator = (uint8_t)(*(buffer + 1) >> 7) & 0x01;

    pmt->pmtHeader.sectionLength = (uint16_t)(((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);

    pmt->pmtHeader.programNumber = (uint16_t)(*(buffer + 3) << 8) + *(buffer + 4);

    pmt->pmtHeader.versionNumber = (uint8_t)(*(buffer + 5) >> 1) & 0x001F;

    pmt->pmtHeader.currentNextIndicator = (uint8_t)(*(buffer + 5) >> 7) & 0x01;

    pmt->pmtHeader.sectionNumber = (uint8_t) * (buffer + 6);

    pmt->pmtHeader.lastSectionNumber = (uint8_t) * (buffer + 7);

    pmt->pmtHeader.pcrPid = (uint16_t)((*(buffer + 8) << 8) + *(buffer + 9)) & 0x1FFF;

    pmt->pmtHeader.programInfoLength = (uint16_t)((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF;

    pmt->elementaryInformationCount = (uint8_t)(pmt->pmtHeader.sectionLength - 13) / 5;
    pmt->elementaryInformation = (pmtTableElementaryInformation *)malloc(pmt->elementaryInformationCount * sizeof(pmtTableElementaryInformation));

    pmt->subtitleCount = 0;
    pmt->subtitles = NULL;

    int offset = 0;
    int i;
    int j;

    for (i = 0; i < pmt->elementaryInformationCount; i++)
    {
        pmt->elementaryInformation[i].streamType = (uint8_t) * (buffer + 12 + i * 5 + offset);
        pmt->elementaryInformation[i].elementaryPid = (uint16_t)((*(buffer + 13 + i * 5 + offset) << 8) + *(buffer + 14 + i * 5 + offset)) & 0x1FFF;
        pmt->elementaryInformation[i].esInfoLength = (uint16_t)((*(buffer + 15 + i * 5 + offset) << 8) + *(buffer + 16 + i * 5 + offset)) & 0x0FFF;

        if (((uint8_t) * (buffer + 17 + i * 5 + offset)) == SUBTITLING_DESCRIPTOR_TAG)
        {

            pmt->subtitleCount = ((uint8_t) * (buffer + 18 + i * 5 + offset)) / 8;

            pmt->subtitles = (char *)malloc(sizeof(char) * pmt->subtitleCount * SUBTITLE_CHARACTERS_COUNT + 1);

            for (j = 0; j < pmt->subtitleCount; j++)
            {
                pmt->subtitles[j * SUBTITLE_CHARACTERS_COUNT] = (uint8_t) * (buffer + 19 + i * 5 + offset);
                pmt->subtitles[j * SUBTITLE_CHARACTERS_COUNT + 1] = (uint8_t) * (buffer + 20 + i * 5 + offset);
                pmt->subtitles[j * SUBTITLE_CHARACTERS_COUNT + 2] = (uint8_t) * (buffer + 21 + i * 5 + offset);
                pmt->subtitles[j * SUBTITLE_CHARACTERS_COUNT + 3] = '\0';
            }
        }

        offset += pmt->elementaryInformation[i].esInfoLength;
    }

    //printPMT(pmt);

    return TABLES_PARSER_NO_ERROR;
}

tablesParserStatus printPAT(patTable *pat)
{
    printf("\nPAT TABLE\n");
    printf("\tTable ID (hex): %#04x\n", pat->patHeader.tableId);
    printf("\tSection syntax indicator: %d\n", pat->patHeader.sectionSyntaxIndicator);
    printf("\tSection length: %d\n", pat->patHeader.sectionLength);
    printf("\tTransport stream id: %d\n", pat->patHeader.transportStreamId);
    printf("\tVersion number: %d\n", pat->patHeader.versionNumber);
    printf("\tSection number: %d\n", pat->patHeader.sectionNumber);
    printf("\tCurrent Next indicator: %d\n", pat->patHeader.currentNextIndicator);
    printf("\tLast section number: %d\n", pat->patHeader.lastSectionNumber);
    int i;
    for (i = 0; i < pat->sectionCount; i++)
    {
        if (pat->programInformation[i].programNumber)
        {
            printf("\tSection %d:\n", i);
            printf("\t\tProgram number: %d\n", pat->programInformation[i].programNumber);
            printf("\t\tPID: %d\n", pat->programInformation[i].programMapPid);
        }
    }

    return TABLES_PARSER_NO_ERROR;
}

tablesParserStatus printPMT(pmtTable *pmt)
{
    printf("\nPMT TABLE\n");
    printf("\tTable ID (hex): %#04x\n", pmt->pmtHeader.tableId);
    printf("\tSection syntax indicator: %d\n", pmt->pmtHeader.sectionSyntaxIndicator);
    printf("\tSection length: %d\n", pmt->pmtHeader.sectionLength);
    printf("\tProgram number id: %d\n", pmt->pmtHeader.programNumber);
    printf("\tVersion number: %d\n", pmt->pmtHeader.versionNumber);
    printf("\tSection number: %d\n", pmt->pmtHeader.sectionNumber);
    printf("\tCurrent Next indicator: %d\n", pmt->pmtHeader.currentNextIndicator);
    printf("\tLast section number: %d\n", pmt->pmtHeader.lastSectionNumber);
    printf("\tpcrPID: %d\n", pmt->pmtHeader.pcrPid);
    printf("\tProgram info length: %d\n", pmt->pmtHeader.programInfoLength);
    printf("\tSubtitle count: %d\n", pmt->subtitleCount);
    int i = 0;
    int characterCount = 0;
    if (pmt->subtitleCount)
    {
        while (pmt->subtitles[i] != '\0')
        {
            printf("%c", pmt->subtitles[i]);
            characterCount++;
            i++;

            if (characterCount == 3)
            {
                if (pmt->subtitles[i] == '\0')
                {
                    characterCount = 0;
                }
                else
                {
                    printf(", ");
                    characterCount = 0;
                }
            }
        }
        printf("\n");
    }
    for (i = 0; i < pmt->elementaryInformationCount; i++)
    {
        if (pmt->elementaryInformation[i].streamType)
        {
            printf("\tSection %d:\n", i);
            printf("\t\tStream type (hex): %#04x\n", pmt->elementaryInformation[i].streamType);
            printf("\t\tElementary PID: %d\n", pmt->elementaryInformation[i].elementaryPid);
        }
    }

    return TABLES_PARSER_NO_ERROR;
}
