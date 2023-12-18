#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

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

void calculateNextPoint(const char kDirection, const int64_t nDistance, const int64_t nPrevX, const int64_t nPrevY, int64_t* nX, int64_t* nY, int64_t* nPerimiter)
{
    switch(kDirection)
    {
        case 'R':
        {
            *nX = nPrevX + nDistance;
            *nY = nPrevY;
            break;
        }

        case 'D':
        {
            *nX = nPrevX;
            *nY = nPrevY + nDistance;
            break;
        }

        case 'L':
        {
            *nX = nPrevX - nDistance;
            *nY = nPrevY;
            break;
        }

        case 'U':
        {
            *nX = nPrevX;
            *nY = nPrevY - nDistance;
            break;
        }
    }

    *nPerimiter += nDistance;
}

int64_t partialShoelaceTheorem(const int64_t nPrevX, const int64_t nPrevY, const int64_t nX, const int64_t nY)
{
    return (nPrevX * nY) - (nX * nPrevY);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        const char              kDirectionChars[] = {'R', 'D', 'L', 'U'};

        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;

        int64_t                 nPerimiterPartOne = 0;
        int64_t                 nPerimiterPartTwo = 0;
        int64_t                 nAreaPartOne      = 0;
        int64_t                 nAreaPartTwo      = 0;

        int64_t                 nPrevXPartOne     = 0;
        int64_t                 nPrevYPartOne     = 0;
        int64_t                 nPrevXPartTwo     = 0;
        int64_t                 nPrevYPartTwo     = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, NULL, 0);
        fclose(pData);

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            const char      kDirectionPartOne = kLines[nLine][0];
            const size_t    nLineLength       = strlen(kLines[nLine]);
            const char      kDirectionPartTwo = kDirectionChars[kLines[nLine][nLineLength-2] - '0'];
            const int64_t   nDistancePartOne  = strtoll(&kLines[nLine][2], NULL, 10);
                  int64_t   nDistancePartTwo;
                  int64_t   nXPartOne;
                  int64_t   nYPartOne;
                  int64_t   nXPartTwo;
                  int64_t   nYPartTwo;

            kLines[nLine][nLineLength-2] = '\0';
            nDistancePartTwo             = strtoll(&kLines[nLine][nLineLength-7], NULL, 16);

            /* Calculate the Next Point / Perimiter Length based on Direction/Distance */
            calculateNextPoint(kDirectionPartOne, nDistancePartOne, nPrevXPartOne, nPrevYPartOne, &nXPartOne, &nYPartOne, &nPerimiterPartOne);
            calculateNextPoint(kDirectionPartTwo, nDistancePartTwo, nPrevXPartTwo, nPrevYPartTwo, &nXPartTwo, &nYPartTwo, &nPerimiterPartTwo);

            /* Calculate the next term in the Shoelace Theorem */
            nAreaPartOne += partialShoelaceTheorem(nPrevXPartOne, nPrevYPartOne, nXPartOne, nYPartOne);
            nAreaPartTwo += partialShoelaceTheorem(nPrevXPartTwo, nPrevYPartTwo, nXPartTwo, nYPartTwo);

            /* Set all the Previous Positions */
            nPrevXPartOne = nXPartOne;
            nPrevYPartOne = nYPartOne;
            nPrevXPartTwo = nXPartTwo;
            nPrevYPartTwo = nYPartTwo;
        }

        /* The final step for the Shoelace Theorem is divide by 2, then we need to add on half the perimiter as the integer
         * coordinates count half the perimiter with this method.
         */
        printf("Part 1: %lld\n", (nAreaPartOne / 2) + (nPerimiterPartOne / 2) + 1);
        printf("Part 2: %lld\n", (nAreaPartTwo / 2) + (nPerimiterPartTwo / 2) + 1);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
    }
 
    return 0;
}