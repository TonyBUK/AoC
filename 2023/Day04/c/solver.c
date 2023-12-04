#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

typedef struct tWinningCount
{
    size_t   nMatchingCount;
    size_t   nQuantity;
} tWinningCount;

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

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nFileLength;
        size_t                  nLine;

        tWinningCount*          kWinningCount;
        char*                   kCurrentValue;
        size_t                  nCurrentValuePos;

        uint64_t                nPartOneAnswer = 0;
        uint64_t                nPartTwoAnswer = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, &nFileLength);

        /* Allocate the Card Buffer */
        kWinningCount = (tWinningCount*)malloc(nLineCount * sizeof(tWinningCount));
        kCurrentValue = (char*)malloc(nFileLength * sizeof(char));
        kCurrentValue[0] = ' ';

        /* We need this to be initialised... */
        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            kWinningCount[nLine].nQuantity      = 1;
        }

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            size_t   nPos, nSize;
            char*    pLine;
            char*    pWinningStart;
            char*    pMatchingStart;
            unsigned bNumber = AOC_FALSE;

            /* Buffer the location of the Winning Numbers */
            pLine = strstr(kLines[nLine], ":");
            assert(pLine);
            ++pLine;
            pWinningStart = pLine;

            /* Buffer the location of the Matching Numbers */
            pLine = strstr(pLine, "|");
            assert(pLine);
            ++pLine;
            pMatchingStart = pLine;

            /* Deliberately processes the NUL terminator as well... */
            nSize = strlen(pLine);
            for (nPos = 0; nPos <= nSize; ++nPos)
            {
                /* This just treats the data as things between spaces. */
                if ((' ' != pLine[nPos]) && ('\0' != pLine[nPos]))
                {
                    /* Copy the data into the buffer */
                    if (AOC_FALSE == bNumber)
                    {
                        nCurrentValuePos = 1;
                        bNumber = AOC_TRUE;
                    }
                    kCurrentValue[nCurrentValuePos++] = pLine[nPos];
                }
                else if (AOC_TRUE == bNumber)
                {
                    /* Search for the data (with spaces either side)
                     * We either:
                     * - Don't find the data
                     * - Find the data but it's not in the Winning List
                     * - Find the data and it's in the Winning List
                     * 
                     * Only if it's the last case do we treat it as a match.
                     */
                    char* pMatch;

                    kCurrentValue[nCurrentValuePos++] = ' ';
                    kCurrentValue[nCurrentValuePos] = '\0';
                    bNumber = AOC_FALSE;

                    pMatch = strstr(pWinningStart, kCurrentValue);
                    if (pMatch)
                    {
                        if (pMatch < pMatchingStart)
                        {
                            ++kWinningCount[nLine].nMatchingCount;
                        }
                    }
                }
            }

            /* Part 1 */
            if (kWinningCount[nLine].nMatchingCount > 0)
            {
                nPartOneAnswer += (uint64_t)1 << (uint64_t)(kWinningCount[nLine].nMatchingCount - 1);
            }

            /* Part 2 */
            for (nPos = 1; nPos <= kWinningCount[nLine].nMatchingCount; ++nPos)
            {
                if ((nLine + nPos) >= nLineCount) break;
                {
                    kWinningCount[nLine + nPos].nQuantity += kWinningCount[nLine].nQuantity;
                }
            }

            nPartTwoAnswer += (uint64_t)kWinningCount[nLine].nQuantity;
        }

        printf("Part 1: %llu\n", nPartOneAnswer);
        printf("Part 2: %llu\n", nPartTwoAnswer);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kWinningCount);
        free(kCurrentValue);
    }
 
    return 0;
}