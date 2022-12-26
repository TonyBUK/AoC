#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>

#define AOC_TRUE    ((unsigned)1)
#define AOC_FALSE   ((unsigned)0)

#define GET_MAP_INDEX(X,Y,XOFF,YOFF,WIDTH) \
    (((Y)-(YOFF)) * (WIDTH)) + ((X)-(XOFF))

#define GET_MAP_X(INDEX,XOFF,YOFF,WIDTH) \
    ((INDEX) % (WIDTH)) + (XOFF)

#define GET_MAP_Y(INDEX,XOFF,YOFF,WIDTH) \
    ((INDEX) / (WIDTH)) + (YOFF)

#define GET_MAP_ENTRY(X,Y,XOFF,YOFF,WIDTH,MAP) \
    MAP[GET_MAP_INDEX((X),(Y),(XOFF),(YOFF),(WIDTH))]

typedef struct CoordinateType
{
    unsigned long X;
    unsigned long Y;
} TCoordinateType;

typedef struct RangeType
{
    TCoordinateType kStart;
    TCoordinateType kEnd;
} TRangeType;

void printSandState(const unsigned long nWidth,
                    const unsigned long nHeight,
                    const unsigned long nMinX,
                    const unsigned long nMaxX,
                    const unsigned long nMinY,
                    const unsigned long nMaxY,
                    unsigned*           kCollisionMap,
                    unsigned*           kSettledSandMap,
                    unsigned*           kFallingSandMap)
{
    unsigned long Y;

    for (Y = nMinY; Y <= nMaxY; ++Y)
    {
        unsigned long X;
        for (X = nMinX; X <= nMaxX; ++X)
        {
            if      (GET_MAP_ENTRY(X,Y, nMinX, nMinY, nWidth, kCollisionMap))   printf("#");
            else if (GET_MAP_ENTRY(X,Y, nMinX, nMinY, nWidth, kSettledSandMap)) printf("O");
            else if (GET_MAP_ENTRY(X,Y, nMinX, nMinY, nWidth, kFallingSandMap)) printf("~");
            else                                                                printf(".");
        }
        printf("\n");
    }
}

unsigned long getValue(FILE** pFile, unsigned* bEOL)
{
    unsigned long nValue = 0;
    unsigned char kData;

    while (isdigit(kData = fgetc(*pFile)))
    {
        if (!feof(*pFile))
        {
            nValue = (nValue * 10lu) + (unsigned long)(kData - (unsigned char)'0');
        }
        else
        {
            break;
        }
    }

    *bEOL = ('\n' == kData) ? AOC_TRUE : AOC_FALSE;

    return nValue;
}

long normalisedDelta(const unsigned long nLeft, const unsigned long nRight)
{
    if (nRight == nLeft) return  0;
    if (nRight >  nLeft) return  1;
                         return -1;
}

unsigned long processSandFloor(const unsigned long nWidth,
                               const unsigned long nHeight,
                               const unsigned long nMinX,
                               const unsigned long nMaxX,
                               const unsigned long nMinY,
                               const unsigned long nMaxY,
                               const unsigned long nEntryX,
                               const unsigned long nEntryY,
                               const unsigned      bEndlessVoid,
                               unsigned*           kCollisionMap,
                               unsigned*           kSettledSandMap,
                               unsigned*           kFallingSandMap,
                               unsigned*           kQueueMap,
                               unsigned long*      kQueueEntry)
{
    const unsigned long nQueueLength = nWidth * nHeight;
    unsigned long       nSandCount = 0;
    unsigned long       nQueueHead = 0;
    unsigned long       nQueueTail = 1;

    /* Update the "Queue" */
    GET_MAP_ENTRY(nEntryX, nEntryY, nMinX, nMinY, nWidth, kQueueMap)   = AOC_TRUE;
    kQueueEntry[nQueueHead] = GET_MAP_INDEX(nEntryX, nEntryY, nMinX, nMinY, nWidth);

    /* Process the Queue*/
    while (nQueueHead != nQueueTail)
    {
        const unsigned long X = GET_MAP_X(kQueueEntry[nQueueHead], nMinX, nMinY, nWidth);
        const unsigned long Y = GET_MAP_Y(kQueueEntry[nQueueHead], nMinX, nMinY, nWidth);

        nQueueHead = (nQueueHead + 1) % nQueueLength;

        /* Process the current Queue Entry */
        if (GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kQueueMap))
        {
            unsigned long nYDown;
            unsigned long nXLeft;
            unsigned long nXRight;
            long          nDeltaX;

            GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kQueueMap) = AOC_FALSE;

            /* If a Queue Position points to Sand, ignore it */
            if (GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kSettledSandMap))
            {
                continue;
            }

            /* Indicate the Item is Falling */
            GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kFallingSandMap) = AOC_TRUE;

            /* Limit based on the Endless Void */
            if (bEndlessVoid)
            {
                if (Y >= nMaxY)
                {
                    continue;
                }
            }

            /* Try to Move Down */
            nYDown = Y + 1;
            if ((AOC_FALSE == GET_MAP_ENTRY(X, nYDown, nMinX, nMinY, nWidth, kCollisionMap)) &&
                (AOC_FALSE == GET_MAP_ENTRY(X, nYDown, nMinX, nMinY, nWidth, kSettledSandMap)) &&
                ((nYDown < nMaxY) || (AOC_TRUE == bEndlessVoid)))
            {
                if (AOC_FALSE == GET_MAP_ENTRY(X, nYDown, nMinX, nMinY, nWidth, kQueueMap))
                {
                    GET_MAP_ENTRY(X, nYDown, nMinX, nMinY, nWidth, kQueueMap) = AOC_TRUE;
                    kQueueEntry[nQueueTail] = GET_MAP_INDEX(X, nYDown, nMinX, nMinY, nWidth);
                    nQueueTail = (nQueueTail + 1) % nQueueLength;
                }
                continue;
            }

            /* Try to Move Left */
            nXLeft = X - 1;
            if ((AOC_FALSE == GET_MAP_ENTRY(nXLeft, nYDown, nMinX, nMinY, nWidth, kCollisionMap)) &&
                (AOC_FALSE == GET_MAP_ENTRY(nXLeft, nYDown, nMinX, nMinY, nWidth, kSettledSandMap)) &&
                ((nYDown < nMaxY) || (AOC_TRUE == bEndlessVoid)))
            {
                if (AOC_FALSE == GET_MAP_ENTRY(nXLeft, nYDown, nMinX, nMinY, nWidth, kQueueMap))
                {
                    GET_MAP_ENTRY(nXLeft, nYDown, nMinX, nMinY, nWidth, kQueueMap) = AOC_TRUE;
                    kQueueEntry[nQueueTail] = GET_MAP_INDEX(nXLeft, nYDown, nMinX, nMinY, nWidth);
                    nQueueTail = (nQueueTail + 1) % nQueueLength;
                }
                continue;
            }

            /* Try to Move Right */
            nXRight = X + 1;
            if ((AOC_FALSE == GET_MAP_ENTRY(nXRight, nYDown, nMinX, nMinY, nWidth, kCollisionMap)) &&
                (AOC_FALSE == GET_MAP_ENTRY(nXRight, nYDown, nMinX, nMinY, nWidth, kSettledSandMap)) &&
                ((nYDown < nMaxY) || (AOC_TRUE == bEndlessVoid)))
            {
                if (AOC_FALSE == GET_MAP_ENTRY(nXRight, nYDown, nMinX, nMinY, nWidth, kQueueMap))
                {
                    GET_MAP_ENTRY(nXRight, nYDown, nMinX, nMinY, nWidth, kQueueMap) = AOC_TRUE;
                    kQueueEntry[nQueueTail] = GET_MAP_INDEX(nXRight, nYDown, nMinX, nMinY, nWidth);
                    nQueueTail = (nQueueTail + 1) % nQueueLength;
                }
                continue;
            }

            /* Handle either the Endless Void or Settling on the Infinite Floor */
            if (bEndlessVoid)
            {
                if (Y < nMaxY)
                {
                    GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kSettledSandMap) = AOC_TRUE;
                    GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kFallingSandMap) = AOC_FALSE;
                    ++nSandCount;
                }
            }
            else
            {
                GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kSettledSandMap) = AOC_TRUE;
                GET_MAP_ENTRY(X, Y, nMinX, nMinY, nWidth, kFallingSandMap) = AOC_FALSE;
                ++nSandCount;
            }


            /* Add any Falling Neighbours back into the Queue
                * Note: We only care about above
                */
            for (nDeltaX = -1; nDeltaX <= 1; ++nDeltaX)
            {
                const unsigned long nNeighbourX;
                if (GET_MAP_ENTRY(X+nDeltaX, Y-1, nMinX, nMinY, nWidth, kFallingSandMap))
                {
                    if (AOC_FALSE == GET_MAP_ENTRY(X+nDeltaX, Y-1, nMinX, nMinY, nWidth, kQueueMap))
                    {
                        GET_MAP_ENTRY(X+nDeltaX, Y-1, nMinX, nMinY, nWidth, kQueueMap) = AOC_TRUE;
                        kQueueEntry[nQueueTail] = GET_MAP_INDEX(X+nDeltaX, Y-1, nMinX, nMinY, nWidth);
                        nQueueTail = (nQueueTail + 1) % nQueueLength;
                    }
                }
            }
        }
    }
    

    return nSandCount;
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        long            nFileSize;
        unsigned long   nMaxRangePairsCount;
        unsigned long   nRangePairCount         = 0;
        unsigned long   nRangePair;

        TRangeType*     kRanges;
        TCoordinateType kStart;
        TCoordinateType kEnd;

        unsigned long   nMinX                   = 0xFFFFFFlu;
        unsigned long   nMaxX                   = 0x000000lu;
        unsigned long   nMinY                   = 0xFFFFFFlu;
        unsigned long   nMaxY                   = 0x000000lu;
        unsigned long   nFloorHalfWidth;

        unsigned long   nWidth;
        unsigned long   nHeight;

        unsigned*       kCollisionMap;
        unsigned*       kSettledSandMap;
        unsigned*       kFallingSandMap;
        unsigned*       kQueueMap;
        unsigned long*  kQueueEntry;

        /* Calculate the File Size */
        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        /* Derive a Lazy prediction of the minimum number of ranges */
        nMaxRangePairsCount = ((unsigned long)nFileSize / 10) + 1;

        /* Allocate Memory for Type used to store the Raw Input Data */
        kRanges = (TRangeType*)malloc(nMaxRangePairsCount * sizeof(TRangeType));

        /* Read the Input File and store the Ranges */
        while (!feof(pFile))
        {
            unsigned bEOL;

            /* Populate the Start Value */
            kStart.X = getValue(&pFile, &bEOL);
            kStart.Y = getValue(&pFile, &bEOL);

            while (1)
            {
                /* This loop is designed to ensure all values will go through
                 * kStart.
                 */
                if (kStart.X < nMinX) nMinX = kStart.X;
                if (kStart.X > nMaxX) nMaxX = kStart.X;
                if (kStart.Y < nMinY) nMinY = kStart.Y;
                if (kStart.Y > nMaxY) nMaxY = kStart.Y;

                if (!!feof(pFile) || (AOC_TRUE == bEOL))
                {
                    break;
                }

                /* Skipe "-> "*/
                fseek(pFile, 3, SEEK_CUR);

                /* Read the End Position */
                kEnd.X = getValue(&pFile, &bEOL);
                kEnd.Y = getValue(&pFile, &bEOL);

                /* Update the Ranges */
                kRanges[nRangePairCount].kStart = kStart;
                kRanges[nRangePairCount].kEnd   = kEnd;
                ++nRangePairCount;

                /* Set the Start to the Current End */
                kStart = kEnd;
            }
        }
        fclose(pFile);

        /* Now the Ranges are stored, and we know the boundaries,
         * we need to store the collision data.
         *
         * The ideal scenario will be to store three sets of data.
         * 
         * 1. Collisions.
         * 2. Settled Sand.
         * 3. Falling Sand.
         * 
         * We know the Height already based on the Y range, X is
         * trickier as Part 2 will introduce an infinite floor,
         * meaning we need to predict how wide this should be.
         * 
         * This is actually something we can derive from the
         * Triangle Number.
         * 
         *      #
         *     ###
         *    #####
         * 
         * Where the Height, which is known to be the origin point of the Drop to the Floor
         * can be used to predict the Width as follows:
         * 
         * Width = 1 + ((Height-1) * 2)
         */
        nMinY           = 0;
        nMaxY          += 2;
        nHeight         = nMaxY - nMinY;
        nFloorHalfWidth = 1 + nHeight;
        nMinX          -= nFloorHalfWidth;
        nMaxX          += nFloorHalfWidth;
        nWidth          = nMaxX - nMinX;

        /* And now we know our ranges, we can adjust Min/Max's, and start creating Data Sets */
        kCollisionMap       = (unsigned*)     malloc(nWidth * nHeight * sizeof(unsigned));
        kSettledSandMap     = (unsigned*)     malloc(nWidth * nHeight * sizeof(unsigned));
        kFallingSandMap     = (unsigned*)     malloc(nWidth * nHeight * sizeof(unsigned));
        kQueueMap           = (unsigned*)     malloc(nWidth * nHeight * sizeof(unsigned));
        kQueueEntry         = (unsigned long*)malloc(nWidth * nHeight * sizeof(unsigned long));

        /* Reset the States */
        memset(kCollisionMap,   AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kSettledSandMap, AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kFallingSandMap, AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kQueueMap,       AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kQueueEntry,     0,         nWidth * nHeight * sizeof(unsigned long));

        /* Update the Collision Map */
        for (nRangePair = 0; nRangePair < nRangePairCount; ++nRangePair)
        {
            const long      nXDelta   = normalisedDelta(kRanges[nRangePair].kStart.X, kRanges[nRangePair].kEnd.X);
            const long      nYDelta   = normalisedDelta(kRanges[nRangePair].kStart.Y, kRanges[nRangePair].kEnd.Y);
            TCoordinateType kPosition = kRanges[nRangePair].kStart;

            while (1)
            {
                /* Flag this Position as Occupied */
                GET_MAP_ENTRY(kPosition.X, kPosition.Y, nMinX, nMinY, nWidth, kCollisionMap) = AOC_TRUE;

                if ((kPosition.X == kRanges[nRangePair].kEnd.X) &&
                    (kPosition.Y == kRanges[nRangePair].kEnd.Y))
                {
                    break;
                }

                kPosition.X += nXDelta;
                kPosition.Y += nYDelta;
            }
        }

        /*
        printSandState(nWidth, nHeight,
                       nMinX, nMaxX, nMinY, nMaxY,
                       kCollisionMap,
                       kSettledSandMap,
                       kFallingSandMap);
        */

        printf("Part 1: %lu\n", processSandFloor(nWidth, nHeight,
                                                 nMinX, nMaxX, nMinY, nMaxY,
                                                 500, 0,
                                                 AOC_TRUE,
                                                 kCollisionMap,
                                                 kSettledSandMap,
                                                 kFallingSandMap,
                                                 kQueueMap,
                                                 kQueueEntry));

        /* Reset the States */
        memset(kSettledSandMap, AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kFallingSandMap, AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kQueueMap,       AOC_FALSE, nWidth * nHeight * sizeof(unsigned));
        memset(kQueueEntry,     0,         nWidth * nHeight * sizeof(unsigned long));

        printf("Part 2: %lu\n", processSandFloor(nWidth, nHeight,
                                                 nMinX, nMaxX, nMinY, nMaxY,
                                                 500, 0,
                                                 AOC_FALSE,
                                                 kCollisionMap,
                                                 kSettledSandMap,
                                                 kFallingSandMap,
                                                 kQueueMap,
                                                 kQueueEntry));

        /* Free all Allocations */
        free(kRanges);
        free(kCollisionMap);
        free(kSettledSandMap);
        free(kFallingSandMap);
        free(kQueueMap);
        free(kQueueEntry);
    }

    return 0;
}
