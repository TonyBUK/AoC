#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, sizeof(char), nFileSize, *pFile);
    assert(nReadCount > 0);
    assert(nReadCount <= nFileSize);
 
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        bProcessUnix = 0;
    }
 
    if ((*pkFileBuffer)[nReadCount] != '\n')
    {
        (*pkFileBuffer)[nReadCount]   = '\n';
        (*pkFileBuffer)[nReadCount+1] = '\0';
 
        if (0 == bProcessUnix)
        {
            if (nReadCount >= 1)
            {
                if ((*pkFileBuffer)[nReadCount-1] != '\r')
                {
                    (*pkFileBuffer)[nReadCount-1]   = '\0';
                }
            }
        }
    }
 
    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, (bProcessUnix ? "\n" : "\r\n"));
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
        char* pBufferedLine;
 
        assert(pLine < &(*pkFileBuffer)[nReadCount]);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;
 
        pBufferedLine = pLine;

        pLine = strtok(NULL, (bProcessUnix ? "\n" : "\r\n"));

        /* Fix long standing bug where duplicate empty lines are skipped */
        if (pLine)
        {
            if((pLine - pBufferedLine) != nLineLength + 1)
            {
                if (bProcessUnix)
                {
                    char* pPreviousLine = pLine - 1;
                    while (*pPreviousLine == '\n')
                    {
                        (*pkLines)[nLineCount] = pPreviousLine;
                        ++nLineCount;

                        *pPreviousLine = '\0';
                        --pPreviousLine;
                    }
                }
                else
                {
                    char* pPreviousLine = pLine - 2;
                    while (*pPreviousLine == '\r')
                    {
                        (*pkLines)[nLineCount] = pPreviousLine;
                        ++nLineCount;

                        pPreviousLine[0] = '\0';
                        pPreviousLine[1] = '\0';
                        --pPreviousLine;
                        --pPreviousLine;
                    }
                }
            }
        }
  
        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nReadCount+1];
        ++nLineCount;
    }

    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nReadCount;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

uint64_t findReflection(const char** kGrid, const size_t nGridWidth, const size_t nGridHeight, const unsigned bHorizontal, const size_t nIgnore)
{
    size_t X;
    size_t Y;

    if (AOC_FALSE == bHorizontal)
    {
        for (Y = 0; Y < (nGridHeight - 1); ++Y)
        {
            unsigned bMatch = AOC_TRUE;
            size_t   nTop;
            size_t   nBottom;

            if ((Y + 1) == nIgnore) continue;

            nTop    = Y;
            nBottom = Y + 1;

            while (1)
            {
                for (X = 0; X < nGridWidth; ++X)
                {
                    if (kGrid[nTop][X] != kGrid[nBottom][X])
                    {
                        bMatch = AOC_FALSE;
                        break;
                    }
                }

                if (AOC_FALSE == bMatch)
                {
                    break;
                }

                if (nTop == 0)
                {
                    break;
                }
                else if (nBottom == (nGridHeight - 1))
                {
                    break;
                }

                --nTop;
                ++nBottom;
            }

            if (AOC_TRUE == bMatch)
            {
                return (uint64_t)(Y + 1);
            }
        }
    }
    else
    {
        for (X = 0; X < (nGridWidth - 1); ++X)
        {
            unsigned bMatch = AOC_TRUE;
            size_t   nLeft;
            size_t   nRight;

            if ((X + 1) == nIgnore) continue;

            nLeft  = X;
            nRight = X + 1;

            while (1)
            {
                for (Y = 0; Y < nGridHeight; ++Y)
                {
                    if (kGrid[Y][nLeft] != kGrid[Y][nRight])
                    {
                        bMatch = AOC_FALSE;
                        break;
                    }
                }

                if (AOC_FALSE == bMatch)
                {
                    break;
                }

                if (nLeft == 0)
                {
                    break;
                }
                else if (nRight == (nGridWidth - 1))
                {
                    break;
                }

                --nLeft;
                ++nRight;
            }

            if (AOC_TRUE == bMatch)
            {
                return (uint64_t)(X + 1);
            }
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;

        unsigned                bGridProcessing = AOC_FALSE;
        size_t                  nGridStartPos;
        size_t                  nGridHeight;
        size_t                  nGridWidth;

        uint64_t                nPatternSummaryPartOne = 0;
        uint64_t                nPatternSummaryPartTwo = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, NULL, 1);
        fclose(pData);

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            if (AOC_FALSE == bGridProcessing)
            {
                nGridStartPos   = nLine;
                nGridHeight     = 1;
                nGridWidth      = strlen(kLines[nLine]);
                bGridProcessing = AOC_TRUE;

                assert(nGridWidth);
            }
            else
            {
                const size_t nLocalWidth = strlen(kLines[nLine]);
                if (nLocalWidth != nGridWidth)
                {
                    /* Part 1 : Normal Reflection */
                    const uint64_t nHorizontalReflection = findReflection((const char**)&kLines[nGridStartPos], nGridWidth, nGridHeight, AOC_TRUE, (size_t)-1);
                    const uint64_t nVerticalReflection   = findReflection((const char**)&kLines[nGridStartPos], nGridWidth, nGridHeight, AOC_FALSE, (size_t)-1);

                    size_t X, Y;
                    unsigned bFoundSmudge = AOC_FALSE;

                    nPatternSummaryPartOne += nHorizontalReflection;
                    nPatternSummaryPartOne += nVerticalReflection * 100;

                    /* Part 2 : Fix Smudges */
                    for (Y = 0; (AOC_FALSE == bFoundSmudge) && (Y < nGridHeight); ++Y)
                    {
                        for (X = 0; (AOC_FALSE == bFoundSmudge) && (X < nGridWidth); ++X)
                        {
                            uint64_t nFixedSmudgeHorizontalReflection;
                            uint64_t nFixedSmudgeVerticalReflection;

                            kLines[nGridStartPos + Y][X] = (kLines[nGridStartPos + Y][X] == '#') ? '.' : '#';

                            nFixedSmudgeHorizontalReflection = findReflection((const char**)&kLines[nGridStartPos], nGridWidth, nGridHeight, AOC_TRUE, nHorizontalReflection);
                            nFixedSmudgeVerticalReflection   = findReflection((const char**)&kLines[nGridStartPos], nGridWidth, nGridHeight, AOC_FALSE, nVerticalReflection);

                            if ((nFixedSmudgeHorizontalReflection > 0) || (nFixedSmudgeVerticalReflection > 0))
                            {
                                nPatternSummaryPartTwo += nFixedSmudgeHorizontalReflection;
                                nPatternSummaryPartTwo += nFixedSmudgeVerticalReflection * 100;
                                bFoundSmudge = AOC_TRUE;
                            }

                            kLines[nGridStartPos + Y][X] = (kLines[nGridStartPos + Y][X] == '#') ? '.' : '#';
                        }
                    }


                    bGridProcessing = AOC_FALSE;

                    assert(nLocalWidth == 0);
                }
                else
                {
                    ++nGridHeight;
                }
            }
        }

        printf("Part 1: %llu\n", nPatternSummaryPartOne);
        printf("Part 2: %llu\n", nPatternSummaryPartTwo);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
    }
 
    return 0;
}