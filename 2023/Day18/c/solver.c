#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

typedef struct SPointType
{
    int64_t nX;
    int64_t nY;
} SPointType; 

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

int64_t addPoint(const int64_t nPerimeter, const size_t nPoint, const char kDirection, const int64_t nDistance, SPointType* kPoints)
{
    switch(kDirection)
    {
        case 'R':
        {
            kPoints[nPoint].nX = kPoints[nPoint-1].nX + nDistance;
            kPoints[nPoint].nY = kPoints[nPoint-1].nY;
            break;
        }

        case 'D':
        {
            kPoints[nPoint].nX = kPoints[nPoint-1].nX;
            kPoints[nPoint].nY = kPoints[nPoint-1].nY + nDistance;
            break;
        }

        case 'L':
        {
            kPoints[nPoint].nX = kPoints[nPoint-1].nX - nDistance;
            kPoints[nPoint].nY = kPoints[nPoint-1].nY;
            break;
        }

        case 'U':
        {
            kPoints[nPoint].nX = kPoints[nPoint-1].nX;
            kPoints[nPoint].nY = kPoints[nPoint-1].nY - nDistance;
            break;
        }
    }

    return nPerimeter + nDistance;
}

int64_t shoelaceTheorem(const size_t nPointCount, const SPointType* kPoints)
{
    int64_t nArea = 0;
    size_t  nPoint;

    for (nPoint = 1; nPoint < nPointCount; ++nPoint)
    {
        nArea += (kPoints[nPoint-1].nX * kPoints[nPoint].nY) - (kPoints[nPoint].nX * kPoints[nPoint-1].nY);
    }

    return nArea / 2;
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

        SPointType*             kPointsPartOne;
        SPointType*             kPointsPartTwo;
        int64_t                 nPerimiterPartOne = 0;
        int64_t                 nPerimiterPartTwo = 0;
        size_t                  nPointCount       = 1;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Point Buffers */
        kPointsPartOne = (SPointType*)malloc((nLineCount+1) * sizeof(SPointType));
        kPointsPartTwo = (SPointType*)malloc((nLineCount+1) * sizeof(SPointType));

        /* First Point */
        kPointsPartOne[0].nX = 0;
        kPointsPartOne[0].nY = 0;
        kPointsPartTwo[0].nX = 0;
        kPointsPartTwo[0].nY = 0;

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            const char      kDirectionPartOne = kLines[nLine][0];
            const size_t    nLineLength       = strlen(kLines[nLine]);
            const char      kDirectionPartTwo = kDirectionChars[kLines[nLine][nLineLength-2] - '0'];
            const int64_t   nDistancePartOne  = strtoll(&kLines[nLine][2], NULL, 10);
                  int64_t   nDistancePartTwo;

            kLines[nLine][nLineLength-2] = '\0';
            nDistancePartTwo             = strtoll(&kLines[nLine][nLineLength-7], NULL, 16);

            nPerimiterPartOne = addPoint(nPerimiterPartOne, nPointCount, kDirectionPartOne, nDistancePartOne, kPointsPartOne);
            nPerimiterPartTwo = addPoint(nPerimiterPartTwo, nPointCount, kDirectionPartTwo, nDistancePartTwo, kPointsPartTwo);

            ++nPointCount;
        }

        printf("Part 1: %lld\n", shoelaceTheorem(nPointCount, kPointsPartOne) + (nPerimiterPartOne / 2) + 1);
        printf("Part 2: %lld\n", shoelaceTheorem(nPointCount, kPointsPartTwo) + (nPerimiterPartTwo / 2) + 1);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kPointsPartOne);
        free(kPointsPartTwo);
    }
 
    return 0;
}