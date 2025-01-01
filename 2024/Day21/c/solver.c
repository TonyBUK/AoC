#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
 
#define AOC_TRUE  (1)
#define AOC_FALSE (0)

/* TODO: This is really just me being lazy and not allowing realloc's if the direction buffer is exceeded...
 *       As it happens for my puzzle input, the max length is only ever 14.
 */
#define DIRECTION_SIZE_INIT (256)

#define PART_ONE_ROUNDS (2)
#define PART_TWO_ROUNDS (25)

#define KEYPAD_WIDTH        (3)
#define KEYPAD_HEIGHT       (4)
#define DIRECTIONPAD_WIDTH  (3)
#define DIRECTIONPAD_HEIGHT (2)

#define TO_GRID(nX, nY, nWidth) ((nY * nWidth) + nX)

#define TO_SEEN(nRobot, nKey, nextKey) ((nRobot << 6) | (nKey << 3) | nextKey)
#define SEEN_SIZE_PART_ONE ((1 << 6) * PART_ONE_ROUNDS)
#define SEEN_SIZE_PART_TWO ((1 << 6) * PART_TWO_ROUNDS)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct PadPositionType
{
    size_t nX;
    size_t nY;
} PadPositionType;

typedef enum DirectionKeypadType
{
    KEYPAD_ILLEGAL = 0,
    KEYPAD_A       = 1,
    KEYPAD_UP      = 2,
    KEYPAD_DOWN    = 3,
    KEYPAD_LEFT    = 4,
    KEYPAD_RIGHT   = 5
} DirectionKeypadType;

typedef enum NumericalKeypadType
{
    _KEYPAD_ILLEGAL = 0,
    _KEYPAD_A       = 1,
    KEYPAD_0        = 2,
    KEYPAD_1        = 3,
    KEYPAD_2        = 4,
    KEYPAD_3        = 5,
    KEYPAD_4        = 6,
    KEYPAD_5        = 7,
    KEYPAD_6        = 8,
    KEYPAD_7        = 9,
    KEYPAD_8        = 10,
    KEYPAD_9        = 11
} NumericalKeypadType;

typedef struct KeyRouteType
{
    unsigned char*       kDirections;
    size_t               nDirectionCount;
    size_t               nDirectionCapacity;
    struct KeyRouteType* pkNext;
} KeyRouteType;

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
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
 
    *pkFileBuffer   = (char*) malloc((nFileSize+3)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);
 
    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }
 
    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if (0 == bProcessUnix)
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\r';
            (*pkFileBuffer)[nFileSize+1] = '\n';
            (*pkFileBuffer)[nFileSize+2] = '\0';
        }
    }
    else
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\n';
            (*pkFileBuffer)[nFileSize+1] = '\0';
        }
    }
 
    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);
 
        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;
 
            assert(pLine < &(*pkFileBuffer)[nFileSize]);
 
            (*pkLines)[nLineCount++] = pLine;
 
            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';
 
            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
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

unsigned char decodeKeypad(const char kKey)
{
    switch(kKey)
    {
        case 'A': return KEYPAD_A;
        case '0': return KEYPAD_0;
        case '1': return KEYPAD_1;
        case '2': return KEYPAD_2;
        case '3': return KEYPAD_3;
        case '4': return KEYPAD_4;
        case '5': return KEYPAD_5;
        case '6': return KEYPAD_6;
        case '7': return KEYPAD_7;
        case '8': return KEYPAD_8;
        case '9': return KEYPAD_9;
        case '^': return KEYPAD_UP;
        case 'v': return KEYPAD_DOWN;
        case '<': return KEYPAD_LEFT;
        case '>': return KEYPAD_RIGHT;

        default:
        {
            assert(0);
        }
    }
}

size_t shortestKeySequences(const PadPositionType* kKeypadLayout, const unsigned char* kKeypadLayoutGrid, const unsigned char* kSequence, const size_t nSequenceLength, const unsigned char kCurrentKey, KeyRouteType* kRouteRoot, KeyRouteType** kRouteCurrent, const size_t nBestLength, unsigned char* kRoute, size_t nRouteLength)
{
    unsigned char       kNextKey         = kSequence[0];
    size_t              nBestLengthLocal = nBestLength;

    int                 nHorizontalDelta;
    int                 nVerticalDelta;

    unsigned            bHorizontalRequired;
    unsigned            bVerticalRequired;

    /* Is this a demonstrably worse solution? */
    if (nRouteLength > nBestLength)
    {
        return nBestLength;
    }

    if (0u == nSequenceLength)
    {
        /* Update the Best Length */

        if (nRouteLength < nBestLengthLocal)
        {
            nBestLengthLocal = nRouteLength;
            *kRouteCurrent   = kRouteRoot;
        }

        assert((*kRouteCurrent)->nDirectionCapacity >= nRouteLength);
        memcpy((*kRouteCurrent)->kDirections, kRoute, nRouteLength * sizeof(unsigned char));

        (*kRouteCurrent)->nDirectionCount = nRouteLength;

        /* We've reached the end of the sequence, add it to the list */
        if (NULL == (*kRouteCurrent)->pkNext)
        {
            (*kRouteCurrent)->pkNext             = (KeyRouteType*)malloc(sizeof(KeyRouteType));
            (*kRouteCurrent)                     = (*kRouteCurrent)->pkNext;
            (*kRouteCurrent)->pkNext             = NULL;
            (*kRouteCurrent)->nDirectionCapacity = DIRECTION_SIZE_INIT;
            (*kRouteCurrent)->nDirectionCount    = 0;
            (*kRouteCurrent)->kDirections        = (unsigned char*)malloc(DIRECTION_SIZE_INIT * sizeof(unsigned char));
        }
        else
        {
            *kRouteCurrent = (*kRouteCurrent)->pkNext;
            (*kRouteCurrent)->nDirectionCount    = 0;
        }

        return nBestLengthLocal;
    }

    /*
     * The optimal solution will *always* use the same key repeated as many times as possible.
     *
     * So really every solution boils down to:
     *
     * 1. Horizontal, Vertical
     * 2. Vertical, Horizontal
     *
     * With the more optimal solution not truly being known until we move on to the next part of the sequence.
     */
    nHorizontalDelta    = (int)kKeypadLayout[kNextKey].nX - (int)kKeypadLayout[kCurrentKey].nX;
    nVerticalDelta      = (int)kKeypadLayout[kNextKey].nY - (int)kKeypadLayout[kCurrentKey].nY;
    bHorizontalRequired = (0 != nHorizontalDelta) ? AOC_TRUE : AOC_FALSE;
    bVerticalRequired   = (0 != nVerticalDelta)   ? AOC_TRUE : AOC_FALSE;

    if ((AOC_TRUE == bHorizontalRequired) && (AOC_TRUE == bVerticalRequired))
    {
        /* Make sure we're not going through the bottom left */
        const PadPositionType kHorizontalExtremity = {kKeypadLayout[kCurrentKey].nX + nHorizontalDelta, kKeypadLayout[kCurrentKey].nY};
        const PadPositionType kVerticalExtremity   = {kKeypadLayout[kCurrentKey].nX                   , kKeypadLayout[kCurrentKey].nY + nVerticalDelta};

        if (KEYPAD_ILLEGAL != kKeypadLayoutGrid[TO_GRID(kHorizontalExtremity.nX, kHorizontalExtremity.nY, KEYPAD_WIDTH)])
        {
            size_t i;
            size_t nNewRouteLength = nRouteLength;

            /* Horizontal, Vertical */
            for (i = 0; i < abs(nHorizontalDelta); ++i)
            {
                kRoute[nNewRouteLength++] = (nHorizontalDelta < 0) ? KEYPAD_LEFT : KEYPAD_RIGHT;
            }

            for (i = 0; i < abs(nVerticalDelta); ++i)
            {
                kRoute[nNewRouteLength++] = (nVerticalDelta < 0) ? KEYPAD_UP : KEYPAD_DOWN;
            }

            kRoute[nNewRouteLength++] = KEYPAD_A;

            assert(nNewRouteLength < DIRECTION_SIZE_INIT);

            /* Recurse */
            nBestLengthLocal = shortestKeySequences(kKeypadLayout, kKeypadLayoutGrid, &kSequence[1], nSequenceLength - 1, kNextKey, kRouteRoot, kRouteCurrent, nBestLengthLocal, kRoute, nNewRouteLength);
        }

        if (KEYPAD_ILLEGAL != kKeypadLayoutGrid[TO_GRID(kVerticalExtremity.nX, kVerticalExtremity.nY, KEYPAD_WIDTH)])
        {
            size_t i;
            size_t nNewRouteLength = nRouteLength;

            /* Vertical, Horizontal */
            for (i = 0; i < abs(nVerticalDelta); ++i)
            {
                kRoute[nNewRouteLength++] = (nVerticalDelta < 0) ? KEYPAD_UP : KEYPAD_DOWN;
            }

            for (i = 0; i < abs(nHorizontalDelta); ++i)
            {
                kRoute[nNewRouteLength++] = (nHorizontalDelta < 0) ? KEYPAD_LEFT : KEYPAD_RIGHT;
            }

            kRoute[nNewRouteLength++] = KEYPAD_A;

            assert(nNewRouteLength < DIRECTION_SIZE_INIT);

            /* Recurse */
            nBestLengthLocal = shortestKeySequences(kKeypadLayout, kKeypadLayoutGrid, &kSequence[1], nSequenceLength - 1, kNextKey, kRouteRoot, kRouteCurrent, nBestLengthLocal, kRoute, nNewRouteLength);
        }
    }
    else if (AOC_TRUE == bHorizontalRequired)
    {
            size_t i;
            size_t nNewRouteLength = nRouteLength;

            /* Horizontal */
            for (i = 0; i < abs(nHorizontalDelta); ++i)
            {
                kRoute[nNewRouteLength++] = (nHorizontalDelta < 0) ? KEYPAD_LEFT : KEYPAD_RIGHT;
            }

            kRoute[nNewRouteLength++] = KEYPAD_A;

            assert(nNewRouteLength < DIRECTION_SIZE_INIT);

            /* Recurse */
            nBestLengthLocal = shortestKeySequences(kKeypadLayout, kKeypadLayoutGrid, &kSequence[1], nSequenceLength - 1, kNextKey, kRouteRoot, kRouteCurrent, nBestLengthLocal, kRoute, nNewRouteLength);
    }
    else if (AOC_TRUE == bVerticalRequired)
    {
            size_t i;
            size_t nNewRouteLength = nRouteLength;

            /* Vertical */
            for (i = 0; i < abs(nVerticalDelta); ++i)
            {
                kRoute[nNewRouteLength++] = (nVerticalDelta < 0) ? KEYPAD_UP : KEYPAD_DOWN;
            }

            kRoute[nNewRouteLength++] = KEYPAD_A;

            assert(nNewRouteLength < DIRECTION_SIZE_INIT);

            /* Recurse */
            nBestLengthLocal = shortestKeySequences(kKeypadLayout, kKeypadLayoutGrid, &kSequence[1], nSequenceLength - 1, kNextKey, kRouteRoot, kRouteCurrent, nBestLengthLocal, kRoute, nNewRouteLength);
    }
    else
    {
        /* Button Press Only */
            size_t nNewRouteLength = nRouteLength;

            kRoute[nNewRouteLength++] = KEYPAD_A;

            assert(nNewRouteLength < DIRECTION_SIZE_INIT);

            /* Recurse */
            nBestLengthLocal = shortestKeySequences(kKeypadLayout, kKeypadLayoutGrid, &kSequence[1], nSequenceLength - 1, kNextKey, kRouteRoot, kRouteCurrent, nBestLengthLocal, kRoute, nNewRouteLength);
    }

    return nBestLengthLocal;
}

uint64_t robotSequences(const PadPositionType* kKeypadLayout, const unsigned char* kKeypadLayoutGrid, const unsigned char* kSequence, const size_t nSequenceLength, const size_t nCurrentRobot, const size_t nMaxRobots, uint64_t* kCache)
{
    unsigned char kCurrentKey = KEYPAD_A;
    uint64_t      nCurrentLengthLevel = 0;
    size_t        i;
    unsigned char kRoute[DIRECTION_SIZE_INIT];
    KeyRouteType  kRouteRoot;
    KeyRouteType* pkRoute;

    kRouteRoot.nDirectionCount    = 0;
    kRouteRoot.nDirectionCapacity = 0;
    kRouteRoot.kDirections        = NULL;
    kRouteRoot.pkNext             = NULL;

    /* Iterate through the sequence */
    for (i = 0; i < nSequenceLength; ++i)
    {
        /* Check if we've seen this sequence before */
        const size_t nCacheKey = TO_SEEN(nCurrentRobot, kCurrentKey, kSequence[i]);

        if (0 == kCache[nCacheKey])
        {
            size_t nRouteLength;

            /* Calculate the shortest key sequence(s) */
            pkRoute = &kRouteRoot;
            if (NULL == kRouteRoot.kDirections)
            {
                kRouteRoot.nDirectionCapacity = DIRECTION_SIZE_INIT;
                kRouteRoot.kDirections        = (unsigned char*)malloc(DIRECTION_SIZE_INIT * sizeof(unsigned char));
            }

            nRouteLength = shortestKeySequences(kKeypadLayout, kKeypadLayoutGrid, &kSequence[i], 1, kCurrentKey, &kRouteRoot, &pkRoute, (size_t)-1, kRoute, 0);

            /* If we're at the last robot, just add the length */
            if ((nCurrentRobot + 1) == nMaxRobots)
            {
                kCache[nCacheKey]    = nRouteLength;
                nCurrentLengthLevel += nRouteLength;
            }
            else
            {
                /* Otherwise we need to calculate the next robot's sequence, so pick the shortest one */
                uint64_t nMinDeltaLength = (uint64_t)-1;
                pkRoute = &kRouteRoot;

                while (pkRoute)
                {
                    if (pkRoute->nDirectionCount == kRouteRoot.nDirectionCount)
                    {
                        uint64_t nDeltaLength = robotSequences(kKeypadLayout, kKeypadLayoutGrid, pkRoute->kDirections, pkRoute->nDirectionCount, nCurrentRobot + 1, nMaxRobots, kCache);
                        nMinDeltaLength = MIN(nMinDeltaLength, nDeltaLength);

                        pkRoute = pkRoute->pkNext;
                    }
                    else
                    {
                        break;
                    }
                }

                nCurrentLengthLevel += nMinDeltaLength;
                kCache[nCacheKey]    = nMinDeltaLength;
            }
        }
        else
        {
            /* Increment by the previously calculate Delta */
            nCurrentLengthLevel += kCache[nCacheKey];
        }

        /* Update the Current Key */
        kCurrentKey = kSequence[i];
    }

    pkRoute = &kRouteRoot;
    while (pkRoute)
    {
        KeyRouteType* pkNext = pkRoute->pkNext;

        if (pkRoute->kDirections)
        {
            free(pkRoute->kDirections);
        }

        if (pkRoute != &kRouteRoot)
        {
            free(pkRoute);
        }

        pkRoute = pkNext;
    }

    /* Return the Length */
    return nCurrentLengthLevel;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        const PadPositionType       kKeypadLegalLayout[KEYPAD_9 + 1] =
        {
                    {-1, -1},
            /* A */ { 2,  3},
            /* 0 */ { 1,  3},
            /* 1 */ { 0,  2},
            /* 2 */ { 1,  2},
            /* 3 */ { 2,  2},
            /* 4 */ { 0,  1},
            /* 5 */ { 1,  1},
            /* 6 */ { 2,  1},
            /* 7 */ { 0,  0},
            /* 8 */ { 1,  0},
            /* 9 */ { 2,  0}
        };
        const PadPositionType       kDirectionpadLegalLayout[KEYPAD_RIGHT + 1] =
        {
                    {-1, -1},
            /* A */ { 2,  0},
            /* ^ */ { 1,  0},
            /* v */ { 1,  1},
            /* < */ { 0,  1},
            /* > */ { 2,  1}
        };

        unsigned char               kKeypadLayout[KEYPAD_WIDTH * KEYPAD_HEIGHT];
        unsigned char               kDirectionpadLayout[DIRECTIONPAD_WIDTH * DIRECTIONPAD_HEIGHT];

        char*                       kPassCodeBuffer;
        char**                      kPassCodes;
        unsigned char*              kPassCodeEnum;

        unsigned char*              kRoute;

        size_t                      nNumPassCodes;
        size_t                      nMaxPassCodeLength;
        size_t                      i;

        KeyRouteType                kNumpadKeyRoutes;
        KeyRouteType*               pkNumpadKeyRoutes;
        uint64_t*                   kCachePartOne;
        uint64_t*                   kCachePartTwo;

        uint64_t                    nPartOne = 0;
        uint64_t                    nPartTwo = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kPassCodeBuffer, &kPassCodes, &nNumPassCodes, NULL, &nMaxPassCodeLength, 0);
        fclose(pData);

        /* Initalise the Seen Buffers for Parts One/Two */
        kPassCodeEnum = (unsigned char*)malloc(nMaxPassCodeLength * sizeof(unsigned char));
        kCachePartOne = (uint64_t*)calloc(SEEN_SIZE_PART_ONE, sizeof(uint64_t));
        kCachePartTwo = (uint64_t*)calloc(SEEN_SIZE_PART_TWO, sizeof(uint64_t));
        kRoute        = (unsigned char*)malloc(DIRECTION_SIZE_INIT * sizeof(unsigned char));

        /* Initialise the Position/Reverse Position Lookups for Keypad Traversal */
        memset(&kKeypadLayout, 0, sizeof(kKeypadLayout));
        memset(&kDirectionpadLayout, 0, sizeof(kDirectionpadLayout));

        for (i = 1; i < (sizeof(kKeypadLegalLayout) / sizeof(kKeypadLegalLayout[0])); ++i)
        {
            if (i < (sizeof(kKeypadLegalLayout) / sizeof(kKeypadLegalLayout[0])))
            {
                kKeypadLayout[TO_GRID(kKeypadLegalLayout[i].nX, kKeypadLegalLayout[i].nY, KEYPAD_WIDTH)] = i;
            }

            if (i < (sizeof(kDirectionpadLegalLayout) / sizeof(kDirectionpadLegalLayout[0])))
            {
                kDirectionpadLayout[TO_GRID(kDirectionpadLegalLayout[i].nX, kDirectionpadLegalLayout[i].nY, DIRECTIONPAD_WIDTH)] = i;
            }
        }

        /* Initialise the Output Route Buffers */
        kNumpadKeyRoutes.nDirectionCapacity = DIRECTION_SIZE_INIT;
        kNumpadKeyRoutes.kDirections        = (unsigned char*)malloc(DIRECTION_SIZE_INIT * sizeof(unsigned char));
        kNumpadKeyRoutes.pkNext             = NULL;

        /* Calculate the Results */
        for (i = 0; i < nNumPassCodes; ++i)
        {
            uint64_t nBestLengthPartOne = (uint64_t)-1;
            uint64_t nBestLengthPartTwo = (uint64_t)-1;
            uint64_t nIntResult         = 0;
            size_t   j;

            for (j = 0; j < strlen(kPassCodes[i]); ++j)
            {
                kPassCodeEnum[j] = decodeKeypad(kPassCodes[i][j]);
            }

            /* Calculate the Keypad Sequence */
            pkNumpadKeyRoutes = &kNumpadKeyRoutes;
            shortestKeySequences(kKeypadLegalLayout, kKeypadLayout, kPassCodeEnum, strlen(kPassCodes[i]), KEYPAD_A, &kNumpadKeyRoutes, &pkNumpadKeyRoutes, (size_t)-1, kRoute, 0);

            /* Iterate through the Robots for the Specified Number of Robots and Compute the Best Length */
            pkNumpadKeyRoutes = &kNumpadKeyRoutes;
            while (pkNumpadKeyRoutes)
            {
                if (pkNumpadKeyRoutes->nDirectionCount == kNumpadKeyRoutes.nDirectionCount)
                {
                    const uint64_t nLengthPartOne = robotSequences(kDirectionpadLegalLayout, kDirectionpadLayout, pkNumpadKeyRoutes->kDirections, pkNumpadKeyRoutes->nDirectionCount, 0, PART_ONE_ROUNDS, kCachePartOne);
                    const uint64_t nLengthPartTwo = robotSequences(kDirectionpadLegalLayout, kDirectionpadLayout, pkNumpadKeyRoutes->kDirections, pkNumpadKeyRoutes->nDirectionCount, 0, PART_TWO_ROUNDS, kCachePartTwo);

                    nBestLengthPartOne = MIN(nBestLengthPartOne, nLengthPartOne);
                    nBestLengthPartTwo = MIN(nBestLengthPartTwo, nLengthPartTwo);

                    pkNumpadKeyRoutes = pkNumpadKeyRoutes->pkNext;
                }
                else
                {
                    break;
                }
            }

            /* Calculate the Integer Component of the Result */
            for (j = 0; j < strlen(kPassCodes[i]); ++j)
            {
                if (kPassCodes[i][j] >= '0' && kPassCodes[i][j] <= '9')
                {
                    nIntResult = (nIntResult * 10) + (kPassCodes[i][j] - '0');
                }
            }

            /* Update the Part One/Two Results */
            nPartOne += nIntResult * nBestLengthPartOne;
            nPartTwo += nIntResult * nBestLengthPartTwo;
        }

        /* Output the Results */
        printf("Part 1: %" PRIu64 "\n", nPartOne);
        printf("Part 2: %" PRIu64 "\n", nPartTwo);

        /* Cleanup */
        pkNumpadKeyRoutes = &kNumpadKeyRoutes;
        while (pkNumpadKeyRoutes)
        {
            KeyRouteType* pkNext = pkNumpadKeyRoutes->pkNext;

            if (pkNumpadKeyRoutes->kDirections)
            {
                free(pkNumpadKeyRoutes->kDirections);
            }

            if (pkNumpadKeyRoutes != &kNumpadKeyRoutes)
            {
                free(pkNumpadKeyRoutes);
            }

            pkNumpadKeyRoutes = pkNext;
        }

        free(kPassCodes);
        free(kPassCodeBuffer);
        free(kPassCodeEnum);
        free(kCachePartOne);
        free(kCachePartTwo);
        free(kRoute);
    }
 
    return 0;
}