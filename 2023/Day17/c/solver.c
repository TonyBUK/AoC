#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

#define X_WIDTH     (8u)
#define Y_WIDTH     (8u)
#define DIR_WIDTH   (2u)
#define COUNT_WIDTH (4u)

#define X_POS       (0u)
#define Y_POS       (X_POS + X_WIDTH)
#define DIR_POS     (Y_POS + Y_WIDTH)
#define COUNT_POS   (DIR_POS + DIR_WIDTH)

#define CACHE_SIZE  (1u << (X_WIDTH + Y_WIDTH + DIR_WIDTH + COUNT_WIDTH))

typedef enum EDirections
{
    EDIRECTION_UP    = 0,
    EDIRECTION_RIGHT = 1,
    EDIRECTION_DOWN  = 2,
    EDIRECTION_LEFT  = 3
} EDirections;

typedef struct SCacheEntry
{
    unsigned bInUse;
    int32_t     nX;
    int32_t     nY;
    EDirections eDirection;
    uint32_t    nCount;
    uint32_t    nHeatTotal;
} SCacheEntry;

typedef struct SQueueEntry
{
    int32_t     nX;
    int32_t     nY;
    EDirections eDirection;
    uint32_t    nCount;
    uint32_t    nHeatTotal;
} SQueueEntry;

#define ENCODE_HEATMAP_CACHE_KEY(x, y, dir, count) (((x) << X_POS) | ((y) << Y_POS) | ((dir) << DIR_POS) | ((count) << COUNT_POS))

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

uint32_t processCrucible(const char** kHeatMap, const size_t nHeatMapWidth, const size_t nHeadMapHeight, const size_t nMinMovesBeforeTurning, const size_t nMaxMovesBeforeTurning, SCacheEntry* kCache, SQueueEntry** kPrimaryQueue, size_t* nPrimaryQueueSize, SQueueEntry** kSecondaryQueue, size_t* nSecondaryQueueSize)
{
    const int       nTargetX                = nHeatMapWidth - 1;
    const int       nTargetY                = nHeadMapHeight - 1;
    const int       kDirections[4][2]       =
    {
        { -1,  0 }, /* Up */
        {  0,  1 }, /* Right */
        {  1,  0 }, /* Down */
        {  0, -1 }  /* Left */
    };

    const EDirections kTurns[4][2] =
    {
        { EDIRECTION_LEFT,  EDIRECTION_RIGHT }, /* Up */
        { EDIRECTION_UP,    EDIRECTION_DOWN  }, /* Right */
        { EDIRECTION_LEFT,  EDIRECTION_RIGHT }, /* Down */
        { EDIRECTION_UP,    EDIRECTION_DOWN  }  /* Left */
    };

    uint32_t        nCacheKey;
    uint32_t        nBestHeatLoss           = (uint32_t)-1;

    SQueueEntry**   kQueue                  = kPrimaryQueue;
    size_t          nQueueCount;
    SQueueEntry**   kNextQueue              = kSecondaryQueue;
    size_t          nNextQueueCount         = 0;

    for (nCacheKey = 0; nCacheKey < CACHE_SIZE; ++nCacheKey)
    {
        kCache[nCacheKey].bInUse = AOC_FALSE;
    }

    for (nQueueCount = 0; nQueueCount < 2; ++nQueueCount)
    {
        (*kQueue)[nQueueCount].nX          = 0;
        (*kQueue)[nQueueCount].nY          = 0;
        (*kQueue)[nQueueCount].nCount      = 0;
        (*kQueue)[nQueueCount].nHeatTotal  = 0;
    }

    (*kQueue)[0].eDirection   = EDIRECTION_DOWN;
    (*kQueue)[1].eDirection   = EDIRECTION_RIGHT;

    while (nQueueCount > 0)
    {
        const SQueueEntry* kCurrent = &(*kQueue)[nQueueCount - 1];

        /* Get the next item in the queue */
        --nQueueCount;

        while (1)
        {
            /* Detect if we're now worse than the best solution */
            if (kCurrent->nHeatTotal >= nBestHeatLoss)
            {
                break;
            }

            /* Detect if this is the best solution */
            if ((kCurrent->nX == nTargetX) && (kCurrent->nY == nTargetY))
            {
                if (kCurrent->nHeatTotal < nBestHeatLoss)
                {
                    if (kCurrent->nCount >= nMinMovesBeforeTurning)
                    {
                        nBestHeatLoss = kCurrent->nHeatTotal;
                        break;
                    }
                }

                break;
            }

            /* Detect if we've seen this before... */
            nCacheKey = ENCODE_HEATMAP_CACHE_KEY(kCurrent->nX, kCurrent->nY, kCurrent->eDirection, kCurrent->nCount);
            assert(nCacheKey < CACHE_SIZE);
            if (AOC_FALSE == kCache[nCacheKey].bInUse)
            {
                kCache[nCacheKey].bInUse     = AOC_TRUE;
                kCache[nCacheKey].nHeatTotal = kCurrent->nHeatTotal;

                kCache[nCacheKey].nX         = kCurrent->nX;
                kCache[nCacheKey].nY         = kCurrent->nY;
                kCache[nCacheKey].eDirection = kCurrent->eDirection;
                kCache[nCacheKey].nCount     = kCurrent->nCount;
            }
            else if (kCache[nCacheKey].nHeatTotal > kCurrent->nHeatTotal)
            {
                kCache[nCacheKey].nHeatTotal = kCurrent->nHeatTotal;
                assert(kCache[nCacheKey].nX == kCurrent->nX);
                assert(kCache[nCacheKey].nY == kCurrent->nY);
                assert(kCache[nCacheKey].eDirection == kCurrent->eDirection);
                assert(kCache[nCacheKey].nCount == kCurrent->nCount);
            }
            else
            {
                assert(kCache[nCacheKey].nX == kCurrent->nX);
                assert(kCache[nCacheKey].nY == kCurrent->nY);
                assert(kCache[nCacheKey].eDirection == kCurrent->eDirection);
                assert(kCache[nCacheKey].nCount == kCurrent->nCount);
                break;
            }

            /* Add the Permutations to the Queue */

            /* Try to keep moving in the same direction if allowed */
            if (kCurrent->nCount < nMaxMovesBeforeTurning)
            {
                /* Try to move forward */
                const int nNextY = kCurrent->nY + kDirections[kCurrent->eDirection][0];
                const int nNextX = kCurrent->nX + kDirections[kCurrent->eDirection][1];

                /* Detect if we're still in the map */
                if ((nNextY >= 0) && (nNextY < nHeadMapHeight) && (nNextX >= 0) && (nNextX < nHeatMapWidth))
                {
                    const uint32_t nHeat = kHeatMap[nNextY][nNextX] - '0';

                    /* Add the next move to the Queue */
                    (*kNextQueue)[nNextQueueCount].nX         = nNextX;
                    (*kNextQueue)[nNextQueueCount].nY         = nNextY;
                    (*kNextQueue)[nNextQueueCount].eDirection = kCurrent->eDirection;
                    (*kNextQueue)[nNextQueueCount].nCount     = kCurrent->nCount + 1;
                    (*kNextQueue)[nNextQueueCount].nHeatTotal = kCurrent->nHeatTotal + nHeat;

                    ++nNextQueueCount;

                    if (nNextQueueCount >= *nSecondaryQueueSize)
                    {
                        *nPrimaryQueueSize   *= 2;
                        *nSecondaryQueueSize *= 2;
                        *kQueue              = (SQueueEntry*)realloc(*kQueue,     *nPrimaryQueueSize   * sizeof(SQueueEntry));
                        *kNextQueue          = (SQueueEntry*)realloc(*kNextQueue, *nSecondaryQueueSize * sizeof(SQueueEntry));

                        assert(*kQueue);
                        assert(*kNextQueue);
                    }
                }
            }

            /* Try to turn if allowed */
            if (kCurrent->nCount >= nMinMovesBeforeTurning)
            {
                size_t nTurn;
                for (nTurn = 0; nTurn < sizeof(kTurns[0]) / sizeof(kTurns[0][0]); ++nTurn)
                {
                    const EDirections eNextDirection = kTurns[kCurrent->eDirection][nTurn];
                    const int         nNextY         = kCurrent->nY + kDirections[eNextDirection][0];
                    const int         nNextX         = kCurrent->nX + kDirections[eNextDirection][1];

                    /* Detect if we're still in the map */
                    if ((nNextX >= 0) && (nNextX < nHeatMapWidth) && (nNextY >= 0) && (nNextY < nHeadMapHeight))
                    {
                        const uint32_t nHeat = kHeatMap[nNextY][nNextX] - '0';

                        /* Add the next move to the Queue */
                        (*kNextQueue)[nNextQueueCount].nX         = nNextX;
                        (*kNextQueue)[nNextQueueCount].nY         = nNextY;
                        (*kNextQueue)[nNextQueueCount].eDirection = eNextDirection;
                        (*kNextQueue)[nNextQueueCount].nCount     = 1;
                        (*kNextQueue)[nNextQueueCount].nHeatTotal = kCurrent->nHeatTotal + nHeat;

                        ++nNextQueueCount;

                        if (nNextQueueCount >= *nSecondaryQueueSize)
                        {
                            *nPrimaryQueueSize   *= 2;
                            *nSecondaryQueueSize *= 2;
                            *kQueue              = (SQueueEntry*)realloc(*kQueue,     *nPrimaryQueueSize   * sizeof(SQueueEntry));
                            *kNextQueue          = (SQueueEntry*)realloc(*kNextQueue, *nSecondaryQueueSize * sizeof(SQueueEntry));

                            assert(*kQueue);
                            assert(*kNextQueue);
                        }
                    }
                }
            }

            break;
        }

        if (nQueueCount == 0)
        {
            /* Swap the Queues */
            SQueueEntry**   kTempQueue      = kNextQueue;
            size_t          nTempQueueSize  = nNextQueueCount;
            kNextQueue                      = kQueue;
            nNextQueueCount                 = 0;
            kQueue                          = kTempQueue;
            nQueueCount                     = nTempQueueSize;
        }
    }

    return nBestHeatLoss;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nMaxLineLength;

        /* 33 Meg to avoid a real hash table... I'll take it! */
        SCacheEntry*            kCache = (SCacheEntry*)malloc(CACHE_SIZE * sizeof(SCacheEntry));

        SQueueEntry*            kPrimaryQueue;
        size_t                  nPrimaryQueueSize;
        SQueueEntry*            kSecondaryQueue;
        size_t                  nSecondaryQueueSize;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength, 0);
        fclose(pData);

        /* Allocate the Initial Queue Sizes */
        nPrimaryQueueSize   = nLineCount * nMaxLineLength;
        nSecondaryQueueSize = nPrimaryQueueSize;
        kPrimaryQueue       = (SQueueEntry*)malloc(nPrimaryQueueSize   * sizeof(SQueueEntry));
        kSecondaryQueue     = (SQueueEntry*)malloc(nSecondaryQueueSize * sizeof(SQueueEntry));

        printf("Part 1: %u\n", processCrucible((const char**)kLines, nMaxLineLength, nLineCount, 1, 3,  kCache, &kPrimaryQueue, &nPrimaryQueueSize, &kSecondaryQueue, &nSecondaryQueueSize));
        printf("Part 2: %u\n", processCrucible((const char**)kLines, nMaxLineLength, nLineCount, 4, 10, kCache, &kPrimaryQueue, &nPrimaryQueueSize, &kSecondaryQueue, &nSecondaryQueueSize));

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kCache);
        free(kPrimaryQueue);
        free(kSecondaryQueue);
    }
 
    return 0;
}