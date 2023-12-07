#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength)
{
    const unsigned long     nStartPos   = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    char*                   pLine;
    size_t                  nLineCount  = 0;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';
    }
    else
    {
        (*pkFileBuffer)[nFileSize]   = '\0';
    }
 
    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, "\n");
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;

        pLine = strtok(NULL, "\n");
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

uint64_t getNumberBadKerning(char* pString)
{
    uint64_t nValue = 0;

    while (*(++pString) != '\0')
    {
        if (*pString == ' ')
        {
            continue;
        }

        else
        {
            nValue = (nValue * (uint64_t)10u) + (uint64_t)(*pString - '0');
        }
    }

    return nValue;
}

uint64_t getNumber(char** pString)
{
    uint64_t nValue;

    while (*(++(*pString)) == ' ')
    {
    }

    nValue   = strtoull(*pString, NULL, 10);
    *pString = strstr(*pString, " ");

    return nValue;
}

unsigned isWinner(const uint64_t nValue, const uint64_t nRaceLength, const uint64_t nRaceRecord)
{
    return ((nValue * (nRaceLength - nValue)) > nRaceRecord) ? AOC_TRUE : AOC_FALSE;
}

uint64_t winningRaces(const uint64_t nTime, const uint64_t nRecord)
{
    /* Use a Binary Search to find the "tipping" point where we start winning */
    uint64_t nRaceMidPoint = nTime / 2;
    uint64_t nRaceStart    = 1;
    uint64_t nRaceEnd      = nRaceMidPoint;
 
    assert(isWinner(nRaceMidPoint, nTime, nRecord));

    while (1)
    {
        unsigned bCurrentWinner;

        nRaceMidPoint = nRaceStart + ((nRaceEnd - nRaceStart) / 2);
        bCurrentWinner = isWinner(nRaceMidPoint, nTime, nRecord);

        if (bCurrentWinner)
        {
            const unsigned bPreviousWinner = isWinner(nRaceMidPoint - 1, nTime, nRecord);

            if (AOC_FALSE == bPreviousWinner)
            {
                break;
            }

            nRaceEnd   -= (nRaceEnd - nRaceStart) / 2;
        }
        else
        {
            nRaceStart  += (nRaceEnd - nRaceStart) / 2;
        }

    }

    return nTime - (nRaceMidPoint * 2) + 1;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;

        uint64_t*               kTimesPartOne;
        uint64_t                nTimePartTwo;
        uint64_t*               kRecordsPartOne;
        uint64_t                nRecordPartTwo;

        size_t                  nTimeCountPartOne   = 0;
        size_t                  nRecordCountPartOne = 0;

        uint64_t                nPartOneProduct     = 1;

        size_t                  i;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pData);

        assert(nLineCount == 2);

        kTimesPartOne   = (uint64_t*)calloc(strlen(kLines[0]), sizeof(uint64_t));
        kRecordsPartOne = (uint64_t*)calloc(strlen(kLines[0]), sizeof(uint64_t));

        /* Parse the Times */
        {
            char* pTimePartTwo   = strstr(kLines[0], " ");
            char* pRecordPartTwo = strstr(kLines[1], " ");

            char* pTimePartOne   = pTimePartTwo;
            char* pRecordPartOne = pRecordPartTwo;

            assert(pTimePartOne);
            assert(pRecordPartOne);

            while (pTimePartOne)
            {
                kTimesPartOne[nTimeCountPartOne++]     = getNumber(&pTimePartOne);
            }

            while (pRecordPartOne)
            {
                kRecordsPartOne[nRecordCountPartOne++] = getNumber(&pRecordPartOne);
            }

            assert(nTimeCountPartOne == nRecordCountPartOne);

            nTimePartTwo   = getNumberBadKerning(pTimePartTwo);
            nRecordPartTwo = getNumberBadKerning(pRecordPartTwo);
        }

        for (i = 0; i < nTimeCountPartOne; ++i)
        {
            nPartOneProduct *= winningRaces(kTimesPartOne[i], kRecordsPartOne[i]);
        }
        printf("Part 1: %llu\n", nPartOneProduct);
        printf("Part 2: %llu\n", winningRaces(nTimePartTwo, nRecordPartTwo));

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kTimesPartOne);
        free(kRecordsPartOne);
    }
 
    return 0;
}