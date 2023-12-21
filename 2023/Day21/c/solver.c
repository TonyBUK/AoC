#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE        (1u)
#define AOC_FALSE       (0u)

#define MAX_STEPS_PART_ONE  (64u)
#define MAX_STEPS_PART_TWO  (26501365u)

#define STEP_WIDTH          (9u)
#define X_WIDTH             (8u)
#define Y_WIDTH             (8u)

#define STEP_SHIFT          (0u)
#define X_SHIFT             (STEP_SHIFT + STEP_WIDTH)
#define Y_SHIFT             (X_SHIFT + X_WIDTH)
#define CACHE_SIZE          (1 << (Y_SHIFT + Y_WIDTH))

#define ENCODE_POSITION(nStep, nX, nY) (((uint64_t)(nStep) << STEP_SHIFT) | ((uint64_t)(nX) << X_SHIFT) | ((uint64_t)(nY) << Y_SHIFT))

typedef struct SQueueType
{
    uint64_t nStep;
    int64_t  nX;
    int64_t  nY;
} SQueueType;

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

uint64_t finalPositionCount(const uint64_t nIterations, const int64_t nStartX, const int64_t nStartY, const size_t nGridWidth, const size_t nGridHeight, const char** kGrid, SQueueType* kQueue, size_t* nQueueMaxSize, unsigned* kCache)
{
    const int64_t kNeighbours[4][2] =
    {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
    };

    uint64_t nFinalPositionCount = 0;
    size_t   nQueueSize          = 1;

    memset(kCache, 0u, sizeof(unsigned) * CACHE_SIZE);
    kQueue[0].nStep = 0;
    kQueue[0].nX    = nStartX;
    kQueue[0].nY    = nStartY;

    while (nQueueSize > 0)
    {
        size_t   nNeighbour;
        uint64_t nPositionKey;

        /* Pop the Queue Entry */
        const SQueueType kQueueEntry = kQueue[nQueueSize - 1];
        --nQueueSize;

        /* Mark as Seen */
        nPositionKey = ENCODE_POSITION(kQueueEntry.nStep, kQueueEntry.nX, kQueueEntry.nY);
        assert(nPositionKey < CACHE_SIZE);
        kCache[nPositionKey] = AOC_TRUE;

        /* Check whether we've finished walking */
        if (kQueueEntry.nStep == nIterations)
        {
            ++nFinalPositionCount;
            continue;
        }

        /* Check the Neighbours */
        for (nNeighbour = 0; nNeighbour < (sizeof(kNeighbours) / sizeof(kNeighbours[0])); ++nNeighbour)
        {
            const int64_t  nNextPositionX = kQueueEntry.nX + kNeighbours[nNeighbour][1];
            const int64_t  nNextPositionY = kQueueEntry.nY + kNeighbours[nNeighbour][0];
            const uint64_t nNextStep      = kQueueEntry.nStep + 1;

            if ((nNextPositionX < 0) || (nNextPositionX >= (int64_t)nGridWidth) || (nNextPositionY < 0) || (nNextPositionY >= (int64_t)nGridHeight))
            {
                continue;
            }

            nPositionKey = ENCODE_POSITION(nNextStep, nNextPositionX, nNextPositionY);
            assert(nPositionKey < CACHE_SIZE);

            if (kCache[nPositionKey])
            {
                continue;
            }
            else if ('#' == kGrid[nNextPositionY][nNextPositionX])
            {
                continue;
            }
            else
            {
                kQueue[nQueueSize].nStep = nNextStep;
                kQueue[nQueueSize].nX    = nNextPositionX;
                kQueue[nQueueSize].nY    = nNextPositionY;
                ++nQueueSize;

                if (nQueueSize >= *nQueueMaxSize)
                {
                    *nQueueMaxSize *= 2;
                    kQueue = (SQueueType*)realloc(kQueue, sizeof(SQueueType) * *nQueueMaxSize);
                    assert(kQueue);
                }
            }
        }
    }

    return nFinalPositionCount;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nHeight;
        size_t                  nLine;
        size_t                  nWidth;

        size_t                  nQueueMaxSize;
        SQueueType*             kQueue;
        unsigned*               kCache;

        int64_t                 nStartX;
        int64_t                 nStartY;

        uint64_t                nVisitedCount;

        uint64_t                nGridSize;

        uint64_t                nOddGrids;
        uint64_t                nEvenGrids;
        uint64_t                nOdds;
        uint64_t                nEvens;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /* Take a Stab at the Queue Size */
        nQueueMaxSize = nHeight * nWidth;

        /* Allocate the Queue / Cache */
        kQueue = (SQueueType*)malloc(sizeof(SQueueType) * nQueueMaxSize);
        kCache = (unsigned*)malloc(sizeof(unsigned) * CACHE_SIZE);
        assert(kQueue);
        assert(kCache);

        for (nLine = 0; nLine < nHeight; ++nLine)
        {
            size_t   nCol;
            unsigned bFound = AOC_FALSE;
            for (nCol = 0; nCol < nWidth; ++nCol)
            {
                if ('S' == kLines[nLine][nCol])
                {
                    nStartX = (int64_t)nCol;
                    nStartY = (int64_t)nLine;
                    bFound  = AOC_TRUE;
                    break;
                }
            }

            if (bFound)
            {
                break;
            } 
        }

        /* Part 1 - Simple Case */
        printf("Part 1: %llu\n", finalPositionCount(MAX_STEPS_PART_ONE, nStartX, nStartY, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache));

        /* Part 2 - Process this as a Rhombus... but it's too big, so we need to subdivide it into the smaller
         *          components that make up the Rhombus.
         8
         * This will consist of:
         *
         * - Whole Grids, which will have odd/even patterns of steps in an alternating pattern.
         * - Diagonals, which will have a pattern of steps that will repeat in two patterns for each diagonal.
         * - Corners, which will have a pattern of steps that will unique for each corner.
         */
        assert(nHeight == nWidth);

        /* Calculate the Number of Grids in the Rhombus */
        nGridSize = (MAX_STEPS_PART_TWO / nHeight) - 1;

        /* Calculate the Number of Whole Grids in the Rhombus */
        nOddGrids   = (((nGridSize + 0) / 2) * 2) + 1;
        nOddGrids  *= nOddGrids;
        nEvenGrids  = ((nGridSize + 1) / 2) * 2;
        nEvenGrids *= nEvenGrids;

        /* Calculate the Odds/Evens in the Grid */
        nOdds       = finalPositionCount((nWidth * 2) + 1, nStartX, nStartY, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache);
        nEvens      = finalPositionCount((nWidth * 2) + 0, nStartX, nStartY, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache);

        /* Calculate the Number of Wholey Contained Grids */
        nVisitedCount = (nOddGrids * nOdds) + (nEvenGrids * nEvens);

        /* Handle the Diagonals, these repeat in two patterns, and will of course differ on direction. */
        nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth / 2) - 1,       nWidth - 1, nHeight - 1, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Top Left (Starting Right Middle) */
        nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) / 2) - 1, nWidth - 1, nHeight - 1, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Top Left (Ending at Left Middle) */
        nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth / 2) - 1,       nWidth - 1, 0,           nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Top Right (starting Left Middle) */
        nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) / 2) - 1, nWidth - 1, 0,           nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Top Right (Ending at Right Middle) */
        nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth / 2) - 1,       0,          0,           nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Bottom Right (Starting Left Middle) */
        nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) / 2) - 1, 0,          0,           nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Bottom Right (Ending at Right Middle) */
        nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth / 2) - 1,       0,          nHeight - 1, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Bottom Left (Starting Right Middle) */
        nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) / 2) - 1, 0,          nHeight - 1, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /* Bottom Left (Ending at Left Middle) */

        /* Now Handle the Edges */
        nVisitedCount += finalPositionCount(nHeight - 1, nHeight - 1, nStartY,    nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /*  Top */
        nVisitedCount += finalPositionCount(nWidth  - 1, nStartX,     0,          nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /*  Right */
        nVisitedCount += finalPositionCount(nHeight - 1, 0,           nStartY,    nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /*  Bottom */
        nVisitedCount += finalPositionCount(nWidth  - 1, nStartX,     nWidth - 1, nWidth, nHeight, (const char**)kLines, kQueue, &nQueueMaxSize, kCache); /*  Left */

        printf("Part 2: %llu\n", nVisitedCount);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kQueue);
        free(kCache);
    }

    return 0;
}