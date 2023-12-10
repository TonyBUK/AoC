#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

typedef enum eDirectionType
{
    eDirection_Up       = 1u,
    eDirection_Down     = 2u,
    eDirection_Left     = 4u,
    eDirection_Right    = 8u,
    eDirection_None     = 0u,
    eDirection_All      = 0xFu /* I'm a child... */
} eDirectionType;

#define CONVERT_2D_TO_1D(x, y, width) ((y) * (width) + (x))
#define CONVERT_1D_TO_2D_X(n, width) ((n) % (width))
#define CONVERT_1D_TO_2D_Y(n, width) ((n) / (width))

#define CONVERT_DIRECTION_TO_X_OFFSET(d) ((((d) & eDirection_Left) == eDirection_Left) ? -1 : ((((d) & eDirection_Right) == eDirection_Right) ? 1 : 0))
#define CONVERT_DIRECTION_TO_Y_OFFSET(d) ((((d) & eDirection_Up)   == eDirection_Up)   ? -1 : ((((d) & eDirection_Down)  == eDirection_Down)  ? 1 : 0))
#define CONVERT_X_OFFSET_TO_DIRECTION(o) (((o) == -1) ? eDirection_Left : (((o) == 1) ? eDirection_Right : eDirection_None))
#define CONVERT_Y_OFFSET_TO_DIRECTION(o) (((o) == -1) ? eDirection_Up   : (((o) == 1) ? eDirection_Down  : eDirection_None))

const uint64_t g_kDirectionOffsets[] =
{
/*
    ['F'] = eDirection_Down | eDirection_Right,
    ['-'] = eDirection_Left | eDirection_Right,
    ['7'] = eDirection_Down | eDirection_Left,
    ['|'] = eDirection_Up   | eDirection_Down,
    ['J'] = eDirection_Up   | eDirection_Left,
    ['L'] = eDirection_Up   | eDirection_Right
*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, eDirection_Left | eDirection_Right, 0, 0,
    0, 0, 0, 0, 0, 0, 0, eDirection_Down | eDirection_Left, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, eDirection_Down | eDirection_Right, 0, 0, 0, eDirection_Up | eDirection_Left, 0, eDirection_Up | eDirection_Right, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, eDirection_Up | eDirection_Down, 0, 0, 0
};

const uint64_t g_kDirectionOffsetsInverted[] =
{
    0,
    0,
    0,
    '|',
    0,
    'J',
    '7',
    0,
    0,
    'L',
    'F',
    0,
    '-',
    0,
    0,
    0
};

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
 
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

        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

#define CHECK_DIRECTION(d, n, x, y, w, v, r) \
if (d & n) \
{ \
    if (AOC_FALSE == v[CONVERT_2D_TO_1D(x + CONVERT_DIRECTION_TO_X_OFFSET(d), nY + CONVERT_DIRECTION_TO_Y_OFFSET(d), w)]) \
    { \
        x += CONVERT_DIRECTION_TO_X_OFFSET(d); \
        y += CONVERT_DIRECTION_TO_Y_OFFSET(d); \
        ++r; \
        break; \
    } \
}

uint64_t processNeighbours(const size_t nWidth, const size_t nHeight, char** kMap, unsigned* kVisited, const int nStartX, const int nStartY)
{
    const int kNeighbours[4][2] =
    {
        { 0, -1 },
        { 0,  1 },
        { -1, 0 },
        { 1,  0 }
    };
    const uint64_t nStartIndex = CONVERT_2D_TO_1D(nStartX, nStartY, nWidth);

    uint64_t nResult    = 1; /* Our first move is one away from the start node. */
    uint64_t nStartTile = 0;
    int      nX;
    int      nY;
    int      i;

    for (i = 0; i < (sizeof(kNeighbours)/sizeof(kNeighbours[0])); ++i)
    {
        uint64_t nDirection = eDirection_None;

        const int nSearchX = nStartX + kNeighbours[i][0];
        const int nSearchY = nStartY + kNeighbours[i][1];

        if ((nSearchX < 0) || (nSearchX >= nWidth) || (nSearchY < 0) || (nSearchY >= nHeight)) continue;

        /* Check to see if there was a pipe here... */
        if ((nDirection = g_kDirectionOffsets[kMap[nSearchY][nSearchX]]))
        {
            /* Now check to see if the direction inverted returns to us */
            if (kNeighbours[i][0])
            {
                if (CONVERT_X_OFFSET_TO_DIRECTION(-kNeighbours[i][0]) & g_kDirectionOffsets[kMap[nSearchY][nSearchX]])
                {
                    nX = nStartX + kNeighbours[i][0];
                    nY = nStartY;
                    nStartTile |= CONVERT_X_OFFSET_TO_DIRECTION(kNeighbours[i][0]);
                }
            }
            else if (kNeighbours[i][1])
            {
                if (CONVERT_Y_OFFSET_TO_DIRECTION(-kNeighbours[i][1]) & g_kDirectionOffsets[kMap[nSearchY][nSearchX]])
                {
                    nX = nStartX;
                    nY = nStartY + kNeighbours[i][1];
                    nStartTile |= CONVERT_Y_OFFSET_TO_DIRECTION(kNeighbours[i][1]);
                }
            }
        }
    }

    /* Update the Start Tile */
    kMap[nStartY][nStartX] = g_kDirectionOffsetsInverted[nStartTile];

    /* Finally, add the Start Tile to the Visited List */
    kVisited[nStartIndex] = AOC_TRUE;

    /* Now process the neighbours */
    while (CONVERT_2D_TO_1D(nX, nY, nWidth) != nStartIndex)
    {
        const uint64_t nDirections     = g_kDirectionOffsets[kMap[nY][nX]];

        kVisited[CONVERT_2D_TO_1D(nX, nY, nWidth)] = AOC_TRUE;

        /* Determine the first direction we can actually visit */
        while (1)
        {
            CHECK_DIRECTION(eDirection_Left,  nDirections, nX, nY, nWidth, kVisited, nResult);
            CHECK_DIRECTION(eDirection_Right, nDirections, nX, nY, nWidth, kVisited, nResult);
            CHECK_DIRECTION(eDirection_Up,    nDirections, nX, nY, nWidth, kVisited, nResult);
            CHECK_DIRECTION(eDirection_Down,  nDirections, nX, nY, nWidth, kVisited, nResult);

            assert(0);
        }

        /* Bit of a bodge, but after the first loop, we want to be able to return back to the Start Index */
        if (2 == nResult)
        {
            kVisited[nStartIndex] = AOC_FALSE;
        }
    }

    /* Finally, add the Start Tile to the Visited List */
    kVisited[nStartIndex] = AOC_TRUE;

    return (nResult / 2) + (nResult % 2);
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
        size_t                  nMaxLineLength;

        int                     nX, nY;
        int                     nStartX, nStartY;
        int                     i;

        unsigned*               kVisited;
        uint64_t*               kProcessedQueue;
        unsigned*               kProcessed;
        size_t                  nProcessedQueueCount = 0;

        char*                   kEnlargedMap;
        uint64_t                nInnerCount          = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength);
        fclose(pData);

        /* Allocate the Visited Buffer */
        kVisited        = (unsigned*)calloc(nLineCount * nMaxLineLength, sizeof(unsigned));
        kEnlargedMap    = (char*)malloc(nLineCount * 2 * nMaxLineLength * 2 * sizeof(char));
        kProcessedQueue = (uint64_t*)malloc(nLineCount * 2 * nMaxLineLength * 2 * sizeof(uint64_t));
        kProcessed      = (unsigned*)calloc(nLineCount * 2 * nMaxLineLength * 2, sizeof(unsigned));

        /* Step one - Find the Start Node */
        for (nY = 0; nY < nLineCount; ++nY)
        {
            const char* pStart = strstr(kLines[nY], "S");
            if (pStart)
            {
                nStartY = nY;
                nStartX = pStart - kLines[nY];
                break;
            }
        }

        /* Part 1: Traversing the Pipes */
        printf("Part 1: %llu\n", processNeighbours(nMaxLineLength, nLineCount, kLines, kVisited, nStartX, nStartY));

        /* Part 2: In or Out? */

        /* First, enlarge the map to twice the size */
        for (nY = 0; nY < nLineCount; ++nY)
        {
            /* Clean the Original Map, and Enlarge */
            for (nX = 0; nX < nMaxLineLength; ++nX)
            {
                uint64_t nDirections;
                /* Clean the Original Map */
                if (AOC_FALSE == kVisited[CONVERT_2D_TO_1D(nX, nY, nMaxLineLength)])
                {
                    kLines[nY][nX] = '.';
                }

                /* (X: 0, Y: 0) - Identical */
                kEnlargedMap[CONVERT_2D_TO_1D((nX * 2) + 0, nY * 2, nMaxLineLength * 2)] = kLines[nY][nX];

                /* (X: 1, Y: 0) - Extend Right */
                if (g_kDirectionOffsets[kLines[nY][nX]] & eDirection_Right)
                {
                    kEnlargedMap[CONVERT_2D_TO_1D((nX * 2) + 1, (nY * 2) + 0, nMaxLineLength * 2)] = '-';
                }
                else
                {
                    kEnlargedMap[CONVERT_2D_TO_1D((nX * 2) + 1, (nY * 2) + 0, nMaxLineLength * 2)] = '.';
                }

                /* (X: 0, Y: 1) - Extend Down */
                if (g_kDirectionOffsets[kLines[nY][nX]] & eDirection_Down)
                {
                    kEnlargedMap[CONVERT_2D_TO_1D((nX * 2) + 0, (nY * 2) + 1, nMaxLineLength * 2)] = '|';
                }
                else
                {
                    kEnlargedMap[CONVERT_2D_TO_1D((nX * 2) + 0, (nY * 2) + 1, nMaxLineLength * 2)] = '.';
                }

                /* (X: 0, Y: 0) - Always Empty */
                kEnlargedMap[CONVERT_2D_TO_1D((nX * 2) + 1, (nY * 2) + 1, nMaxLineLength * 2)] = '.';

                if ((0 == nY) || (nY == (nLineCount - 1)))
                {
                    if ('.' == kLines[nY][nX])
                    {
                        kProcessedQueue[nProcessedQueueCount++] = CONVERT_2D_TO_1D((nX * 2) + 0, (nY * 2) + 0, nMaxLineLength * 2);
                    }
                }
            }

            /* Add the Left/Right Columns */
            if ((nY > 0) && (nY < (nLineCount - 1)))
            {
                if ('.' == kLines[nY][0])
                {
                    kProcessedQueue[nProcessedQueueCount++] = CONVERT_2D_TO_1D((0 * 2) + 0, (nY * 2) + 0, nMaxLineLength * 2);
                }
                if ('.' == kLines[nY][nMaxLineLength - 1])
                {
                    kProcessedQueue[nProcessedQueueCount++] = CONVERT_2D_TO_1D(((nMaxLineLength - 1) * 2) + 1, (nY * 2) + 0, nMaxLineLength * 2);
                }
            }
        }

        while (nProcessedQueueCount > 0)
        {
            const int kNeighbours[4][2] =
            {
                { 0, -1 },
                { 0,  1 },
                { -1, 0 },
                { 1,  0 }
            };
            const uint64_t nMapIndex = kProcessedQueue[--nProcessedQueueCount];

            if (kProcessed[nMapIndex] == AOC_TRUE) continue;

            nX = CONVERT_1D_TO_2D_X(nMapIndex, nMaxLineLength * 2);
            nY = CONVERT_1D_TO_2D_Y(nMapIndex, nMaxLineLength * 2);

            for (i = 0; i < (sizeof(kNeighbours) / sizeof(kNeighbours[0])); ++i)
            {
                const int nSearchX = nX + kNeighbours[i][0];
                const int nSearchY = nY + kNeighbours[i][1];
                uint64_t  nSearchIndex;

                if ((nSearchX < 0) || (nSearchX >= (nMaxLineLength * 2)) || (nSearchY < 0) || (nSearchY >= (nLineCount * 2))) continue;

                nSearchIndex = CONVERT_2D_TO_1D(nSearchX, nSearchY, nMaxLineLength * 2);
                if (kProcessed[nMapIndex]) continue;

                if ('.' != kEnlargedMap[nSearchIndex]) continue;

                kProcessedQueue[nProcessedQueueCount++] = nSearchIndex;
            }

            kEnlargedMap[nMapIndex] = '#';
            kProcessed[nMapIndex] = AOC_TRUE;
        }

        for (nY = 0; nY < (nLineCount * 2); nY += 2)
        {
            for (nX = 0; nX < (nMaxLineLength * 2); nX += 2)
            {
                if ('.' == kEnlargedMap[CONVERT_2D_TO_1D(nX, nY, nMaxLineLength * 2)])
                {
                    ++nInnerCount;
                }
            }
        }

        printf("Part 2: %llu\n", nInnerCount);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kVisited);
        free(kEnlargedMap);
        free(kProcessedQueue);
        free(kProcessed);
    }
 
    return 0;
}