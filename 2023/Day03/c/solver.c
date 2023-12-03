#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define AOC_FALSE (0u)
#define AOC_TRUE  (1u)

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

        int*                    kNumericIndex;

        size_t                  nWidth  = 0;

        size_t                  nX, nY;

        size_t                  nUniqueNumerCount = 0;
        unsigned long*          kUniqueNumbers;

        int                     nLastUniqueNumber = -1;

        unsigned long           nPart1Answer      = 0;
        unsigned long           nPart2Answer      = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, &nFileLength);

        /* Cruedly create the Mechanism by which we flag each number as unique */
        kNumericIndex  = (int*)malloc(nFileLength * sizeof(int));
        kUniqueNumbers = (unsigned long*)malloc(nFileLength * sizeof(unsigned long));

        for (nY = 0; nY < nLineCount; ++nY)
        {
            unsigned bProcessingNumber = AOC_FALSE;

            /* Calculate the Width */
            if (0 == nWidth)
            {
                nWidth = strlen(kLines[nY]);
            }
            else
            {
                assert(nWidth == strlen(kLines[nY]));
            }

            /* Iterate through the Row and:
             * Allocate identifiers to each Unique Value
             */
            for (nX = 0; nX < nWidth; ++nX)
            {
                if (isdigit(kLines[nY][nX]))
                {
                    if (AOC_FALSE == bProcessingNumber)
                    {
                        kUniqueNumbers[nUniqueNumerCount] = strtoul(&kLines[nY][nX], NULL, 10);

                        ++nUniqueNumerCount;
                        bProcessingNumber = AOC_TRUE;
                    }

                    kNumericIndex[(nY * nWidth) + nX] = nUniqueNumerCount - 1;
                }
                else
                {
                    bProcessingNumber = AOC_FALSE;
                    kNumericIndex[(nY * nWidth) + nX] = -1;
                }
            }
        }

        /* Now solve both parts... */
        for (nY = 0; nY < nLineCount; ++nY)
        {
            for (nX = 0; nX < nWidth; ++nX)
            {
                const int nCurrentUniqueNumber = kNumericIndex[(nY * nWidth) + nX];

                /* Part 1 */
                if (nLastUniqueNumber != nCurrentUniqueNumber)
                {
                    if (nCurrentUniqueNumber >= 0)
                    {
                        const size_t tYStart = (nY > 0)                ? (nY - 1) : nY;
                        const size_t tYEnd   = (nY < (nLineCount - 1)) ? (nY + 1) : nY;
                        const size_t tXStart = (nX > 0)                ? (nX - 1) : nX;
                        const size_t tXEnd   = (nX < (nWidth - 1))     ? (nX + 1) : nX;

                        size_t tY, tX;

                        for (tY = tYStart; (tY <= tYEnd) && (nCurrentUniqueNumber != nLastUniqueNumber); ++tY)
                        {
                            for (tX = tXStart; (tX <= tXEnd) && (nCurrentUniqueNumber != nLastUniqueNumber); ++tX)
                            {
                                if (('.' != kLines[tY][tX]) && !isdigit(kLines[tY][tX]))
                                {
                                    nPart1Answer     += kUniqueNumbers[nCurrentUniqueNumber];
                                    nLastUniqueNumber = nCurrentUniqueNumber;
                                }
                            }
                        }
                    }
                }

                /* Part 2 */
                if ('*' == kLines[nY][nX])
                {
                    const size_t tYStart = (nY > 0)                ? (nY - 1) : nY;
                    const size_t tYEnd   = (nY < (nLineCount - 1)) ? (nY + 1) : nY;
                    const size_t tXStart = (nX > 0)                ? (nX - 1) : nX;
                    const size_t tXEnd   = (nX < (nWidth - 1))     ? (nX + 1) : nX;

                    size_t tY, tX;

                    unsigned long kGearUniqueNumbers[4] = {-1, -1, -1, -1};
                    unsigned long nGearUniqueCount      = 1;

                    for (tY = tYStart; (tY <= tYEnd) && (nGearUniqueCount <= 3); ++tY)
                    {
                        for (tX = tXStart; (tX <= tXEnd) && (nGearUniqueCount <= 3); ++tX)
                        {
                            if (kNumericIndex[(tY * nWidth) + tX] > 0)
                            {
                                if (kGearUniqueNumbers[nGearUniqueCount - 1] != kNumericIndex[(tY * nWidth) + tX])
                                {
                                    kGearUniqueNumbers[nGearUniqueCount] = kNumericIndex[(tY * nWidth) + tX];
                                    ++nGearUniqueCount;
                                }
                            }
                        }
                    }

                    if (3 == nGearUniqueCount)
                    {
                        nPart2Answer += (kUniqueNumbers[kGearUniqueNumbers[1]] * kUniqueNumbers[kGearUniqueNumbers[2]]);
                    }
                }
            }
        }

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kNumericIndex);
        free(kUniqueNumbers);

        printf("Part 1: %lu\n", nPart1Answer);
        printf("Part 2: %lu\n", nPart2Answer);
    }
 
    return 0;
}