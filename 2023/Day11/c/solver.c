#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define PART_ONE_ENLARGMENT (2)
#define PART_TWO_ENLARGMENT (1000000)
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)
 
#ifndef ABS
    #define ABS(x) (((x) < 0) ? (-(x)) : (x))
#endif
 
typedef struct tGalaxyPointType
{
    uint64_t nX;
    uint64_t nY;
} tGalaxyPointType;
 
/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
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
 
        assert(pLine < &(*pkFileBuffer)[nReadCount]);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;
 
        pLine = strtok(NULL, (bProcessUnix ? "\n" : "\r\n"));
 
        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
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
 
uint64_t calculateManhattenDistance(const tGalaxyPointType* kPointA, const tGalaxyPointType* kPointB)
{
    return (uint64_t)(ABS((int64_t)kPointA->nY - (int64_t)kPointB->nY) + ABS((int64_t)kPointA->nX - (int64_t)kPointB->nX));
}
 
void enlargeGalaxy(const tGalaxyPointType* kPoint, const uint64_t* kGalaxyEnlargementColumn, const uint64_t* kGalaxyEnlargementRows, const uint64_t nScaling, tGalaxyPointType* kBuffer)
{
    kBuffer->nX = kPoint->nX + (kGalaxyEnlargementColumn[kPoint->nX] * (nScaling - 1));
    kBuffer->nY = kPoint->nY + (kGalaxyEnlargementRows[kPoint->nY]   * (nScaling - 1));
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
        size_t                  nMaxLineLength;
 
        uint64_t*               kGalaxyEnlargementColumns;
        uint64_t*               kGalaxyEnlargementRows;
        tGalaxyPointType*       kGalaxies;
        tGalaxyPointType*       kGalaxiesEnlargedPartOne;
        tGalaxyPointType*       kGalaxiesEnlargedPartTwo;
        size_t                  nGalaxyCount                        = 0;
 
        uint64_t                nCumulativeGalaxyColumnExpansion    = 0;
        uint64_t                nCumulativeGalaxyRowExpansion       = 0;
 
        int                     nX, nY;
        size_t                  i, j;
 
        uint64_t                nManhattenSumPartOne                = 0;
        uint64_t                nManhattenSumPartTwo                = 0;
 
        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength);
        fclose(pData);
 

        kGalaxyEnlargementColumns = (uint64_t*)malloc(nMaxLineLength * sizeof(uint64_t));
        kGalaxyEnlargementRows    = (uint64_t*)malloc(nLineCount * sizeof(uint64_t));
        kGalaxies                 = (tGalaxyPointType*)malloc(nLineCount * nMaxLineLength * sizeof(tGalaxyPointType));
 
        for (nX = 0; nX < nMaxLineLength; ++nX)
        {
            unsigned bBlankColumn = AOC_TRUE;
 
            for (nY = 0; nY < nLineCount; ++nY)
            {
                if (0 == nX)
                {
                    if (NULL == strstr(kLines[nY], "#"))
                    {
                        ++nCumulativeGalaxyRowExpansion;
                    }
                    kGalaxyEnlargementRows[nY] = nCumulativeGalaxyRowExpansion;
                }
 
                if ('#' == kLines[nY][nX])
                {
                    bBlankColumn = AOC_FALSE;
 
                    kGalaxies[nGalaxyCount].nX = nX;
                    kGalaxies[nGalaxyCount].nY = nY;
 
                    ++nGalaxyCount;
                }
            }
 
            if (bBlankColumn)
            {
                ++nCumulativeGalaxyColumnExpansion;
            }
            kGalaxyEnlargementColumns[nX] = nCumulativeGalaxyColumnExpansion;
        }
 
        /* Enlarge the Galaxies*/
        kGalaxiesEnlargedPartOne = (tGalaxyPointType*)malloc(nGalaxyCount * sizeof(tGalaxyPointType));
        kGalaxiesEnlargedPartTwo = (tGalaxyPointType*)malloc(nGalaxyCount * sizeof(tGalaxyPointType));
        for (i = 0; i < nGalaxyCount; ++i)
        {
            enlargeGalaxy(&kGalaxies[i], kGalaxyEnlargementColumns, kGalaxyEnlargementRows, PART_ONE_ENLARGMENT, &kGalaxiesEnlargedPartOne[i]);
            enlargeGalaxy(&kGalaxies[i], kGalaxyEnlargementColumns, kGalaxyEnlargementRows, PART_TWO_ENLARGMENT, &kGalaxiesEnlargedPartTwo[i]);
        }
 
        /* Sum the Manhatten Distances */
        for (i = 0; i < nGalaxyCount; ++i)
        {
            for (j = i + 1; j < nGalaxyCount; ++j)
            {
                nManhattenSumPartOne += calculateManhattenDistance(&kGalaxiesEnlargedPartOne[i], &kGalaxiesEnlargedPartOne[j]);
                nManhattenSumPartTwo += calculateManhattenDistance(&kGalaxiesEnlargedPartTwo[i], &kGalaxiesEnlargedPartTwo[j]);
            }
        }
 
        printf("Part 1: %lld\n", nManhattenSumPartOne);
        printf("Part 2: %lld\n", nManhattenSumPartTwo);
 
        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kGalaxyEnlargementColumns);
        free(kGalaxyEnlargementRows);
        free(kGalaxiesEnlargedPartOne);
        free(kGalaxiesEnlargedPartTwo);
    }
 
    return 0;
}