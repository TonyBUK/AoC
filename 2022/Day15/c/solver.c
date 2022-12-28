#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define AOC_MIN(A,B) \
    (((A) <= (B)) ? (A) : (B))

#define AOC_MAX(A,B) \
    (((A) >= (B)) ? (A) : (B))

typedef struct LineType
{
    int64_t nStartX;
    int64_t nStartY;
    int64_t nEndX;
    int64_t nEndY;
} TLineType;

typedef struct SensorType
{
    int64_t     nSensorX;
    int64_t     nSensorY;
    int64_t     nBeaconX;
    int64_t     nBeaconY;
    int64_t     nManhattenDistance;
    TLineType   kLines[4];
} TSensorType;

typedef struct BoundaryType
{
    int64_t  nStartX;
    int64_t  nEndX;
    unsigned bValid;
} TBoundaryType;

int64_t getValue(FILE** pFile)
{
    int64_t nSign  = 1;
    int64_t nValue = 0;
    char    kData  = fgetc(*pFile);

    if ('-' == kData)
    {
        nSign = -1;
        kData = fgetc(*pFile);
    }

    do
    {
        if (feof(*pFile))
        {
            break;
        }

        nValue = (nValue * 10) + (int64_t)(kData - '0');
    } while(isdigit(kData = fgetc(*pFile)));

    return nValue * nSign;
}

void getManhattenXBoundsForSensor(const TSensorType* pkSensor, const int64_t nRow, TBoundaryType* pkBoundary)
{
    const int64_t nRowDelta = llabs(pkSensor->nSensorY - nRow);

    if (nRowDelta > pkSensor->nManhattenDistance)
    {
        pkBoundary->bValid = AOC_FALSE;
    }
    else
    {
        const int64_t nX = pkSensor->nManhattenDistance - nRowDelta;
        pkBoundary->bValid = AOC_TRUE;
        pkBoundary->nStartX = pkSensor->nSensorX - nX;
        pkBoundary->nEndX   = pkSensor->nSensorX + nX;
    }
}

void getManhattenXBoundsForSensors(const TSensorType* kSensors, const unsigned long nSensorCount, const int64_t nRow, TBoundaryType* kBoundaries, unsigned long* pnBoundaryCount)
{
    unsigned long i;

    *pnBoundaryCount = 0;
    for (i = 0; i < nSensorCount; ++i)
    {
        getManhattenXBoundsForSensor(&kSensors[i], nRow, &kBoundaries[*pnBoundaryCount]);
        if (kBoundaries[*pnBoundaryCount].bValid)
        {
            *pnBoundaryCount += 1;
        }
    }
}

void combineManhattenXBoundsForSensor(TBoundaryType* pkBoundary1, TBoundaryType* pkBoundary2)
{
    if (AOC_MIN(pkBoundary1->nEndX, pkBoundary2->nEndX) >= AOC_MAX(pkBoundary1->nStartX, pkBoundary2->nStartX))
    {
        pkBoundary1->nStartX = AOC_MIN(pkBoundary1->nStartX, pkBoundary2->nStartX);
        pkBoundary1->nEndX   = AOC_MAX(pkBoundary1->nEndX, pkBoundary2->nEndX);
        pkBoundary2->bValid  = AOC_FALSE;
    }
}

unsigned long combineManhattenIntersections(const unsigned long nIntersectionCount, TBoundaryType* kIntersections)
{
    unsigned long   nNewIntersectionCount   = 0;
    unsigned        bMerged;
    unsigned long   i;

    /* Combine the Intersections */
    do
    {
        bMerged = AOC_FALSE;

        for (i = 0; i < nIntersectionCount; ++i)
        {
            unsigned long j;
            if (AOC_FALSE == kIntersections[i].bValid) continue;

            for (j = i + 1; j < nIntersectionCount; ++j)
            {
                if (AOC_FALSE == kIntersections[j].bValid) continue;

                combineManhattenXBoundsForSensor(&kIntersections[i], &kIntersections[j]);

                if (AOC_FALSE == kIntersections[j].bValid)
                {
                    bMerged = AOC_TRUE;
                }
            }
        }
    } while (bMerged);

    /* Shunt the Intersections and Adjust the Count */
    for (i = 0; i < nIntersectionCount; ++i)
    {
        unsigned long j;
        if (AOC_TRUE == kIntersections[i].bValid)
        {
            ++nNewIntersectionCount;
            continue;
        }

        for (j = i + 1; j < nIntersectionCount; ++j)
        {
            if (AOC_FALSE == kIntersections[j].bValid) continue;
            kIntersections[i] = kIntersections[j];
            ++nNewIntersectionCount;
        }
    }

    return nNewIntersectionCount;
}

int64_t det(const int64_t nX1Diff, const int64_t nX2Diff, const int64_t nY1Diff, const int64_t nY2Diff)
{
    return (nX1Diff * nY2Diff) - (nX2Diff * nY1Diff);
}

void addToQueue(const int64_t nRow, const int64_t nStartY, const int64_t nEndY, int64_t* pkQueue, unsigned long* pnQueueCount, unsigned* kQueueEntry)
{
    if ((nRow >= nStartY) && (nRow <= nEndY))
    {
        if (AOC_FALSE == kQueueEntry[nRow])
        {
            pkQueue[*pnQueueCount] = nRow;
            *pnQueueCount         += 1;
            kQueueEntry[nRow]      = AOC_TRUE;
        }
    }
}

void getLineIntersection(const TLineType* pkLine1, const TLineType* pkLine2, const int64_t nStartY, const int64_t nEndY, int64_t* pkQueue, unsigned long* pnQueueCount, unsigned* kQueueEntry)
{
    const int64_t nX1Diff   = pkLine1->nStartX - pkLine1->nEndX;
    const int64_t nY1Diff   = pkLine1->nStartY - pkLine1->nEndY;

    const int64_t nX2Diff   = pkLine2->nStartX - pkLine2->nEndX;
    const int64_t nY2Diff   = pkLine2->nStartY - pkLine2->nEndY;

    const int64_t nDiv      = det(nX1Diff, nX2Diff, nY1Diff, nY2Diff);

    if (0 != nDiv)
    {
        const int64_t nD1         = det(pkLine1->nStartX, pkLine1->nEndX, pkLine1->nStartY, pkLine1->nEndY);
        const int64_t nD2         = det(pkLine2->nStartX, pkLine2->nEndX, pkLine2->nStartY, pkLine2->nEndY);
        const int64_t nYNumerator = det(nD1, nD2, nY1Diff, nY2Diff);
        const int64_t nY          = nYNumerator / nDiv;

        if ((nY >= nStartY) && (nY <= nEndY))
        {
            if (0 == (nYNumerator % nDiv))
            {
                addToQueue(nY,   nStartY, nEndY, pkQueue, pnQueueCount, kQueueEntry);
                addToQueue(nY+1, nStartY, nEndY, pkQueue, pnQueueCount, kQueueEntry);
            }
            else
            {
                addToQueue(nY,   nStartY, nEndY, pkQueue, pnQueueCount, kQueueEntry);
                addToQueue(nY+1, nStartY, nEndY, pkQueue, pnQueueCount, kQueueEntry);
                addToQueue(nY-1, nStartY, nEndY, pkQueue, pnQueueCount, kQueueEntry);
            }
        }
    }
}

void getYIntersections(const TSensorType* kSensors, const unsigned long nSensorCount, const int64_t nStartY, const int64_t nEndY, int64_t* pkQueue, unsigned long* pnQueueCount, unsigned* kQueueEntry)
{
    unsigned long i;
    for (i = 0; i < nSensorCount; ++i)
    {
        unsigned long j;
        for (j = i + 1; j < nSensorCount; ++j)
        {
            unsigned long k;

            if ((llabs(kSensors[i].nSensorX - kSensors[j].nSensorX) >
                 (kSensors[i].nManhattenDistance + kSensors[j].nManhattenDistance)) ||
                (llabs(kSensors[i].nSensorY - kSensors[j].nSensorY) >
                 (kSensors[i].nManhattenDistance + kSensors[j].nManhattenDistance)))
            {
                continue;
            }

            for (k = 0; k < 4; ++k)
            {
                unsigned long l;
                for (l = 0; l < 4; ++l)
                {
                    getLineIntersection(&kSensors[i].kLines[k],
                                        &kSensors[j].kLines[l],
                                        nStartY,
                                        nEndY,
                                        pkQueue,
                                        pnQueueCount,
                                        kQueueEntry);
                }
            }
        }
    }
}

unsigned solveQueue(const TSensorType* kSensors, const unsigned long nSensorCount, const int64_t* kQueue, const unsigned long nQueueSize, TBoundaryType* kIntersections, int64_t* nSolved)
{
    unsigned long nIntersectionCount;
    unsigned long i;

    for (i = 0; i < nQueueSize; ++i)
    {
        getManhattenXBoundsForSensors(kSensors, nSensorCount, kQueue[i], kIntersections, &nIntersectionCount);
        nIntersectionCount = combineManhattenIntersections(nIntersectionCount, kIntersections);
        if (2 == nIntersectionCount)
        {
            const int64_t nPair1Start = AOC_MIN(kIntersections[0].nStartX, kIntersections[1].nEndX);
            const int64_t nPair1End   = AOC_MAX(kIntersections[0].nStartX, kIntersections[1].nEndX);
            const int64_t nPair2Start = AOC_MIN(kIntersections[0].nEndX,   kIntersections[1].nStartX);
            const int64_t nPair2End   = AOC_MAX(kIntersections[0].nEndX,   kIntersections[1].nStartX);
            int64_t nSolvedX;

            if (2 == (nPair1End - nPair1Start))
            {
                nSolvedX = nPair1Start + 1;
            }
            else if (2 == (nPair2End - nPair2Start))
            {
                nSolvedX = nPair2Start + 1;
            }
            else
            {
                assert(0);
            }

            *nSolved = (nSolvedX * 4000000ll) + kQueue[i];

            return AOC_TRUE;
        }
    }

    return AOC_FALSE;
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const int64_t   nPart1TargetRow     = 2000000ll;
        const int64_t   nPart2StartX        = 0;
        const int64_t   nPart2EndX          = 4000000ll;
        const int64_t   nPart2StartY        = 0;
        const int64_t   nPart2EndY          = 4000000ll;

        long            nFileSize;

        unsigned long   nSensorCountMax;
        unsigned long   nSensorCount        = 0;
        TSensorType*    kSensors;

        TBoundaryType*  kIntersections;
        unsigned long   nIntersectionCount  = 0;

        int64_t         nIntersectionRange;
        unsigned long   i;

        int64_t*        kQueue;
        unsigned long   nQueueSize          = 0;
        unsigned*       kQueueEntry;

        unsigned        bSolved             = AOC_FALSE;
        int64_t         nSolved;

        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        /* If we assume EOL, with all values being single digit, then 50 is the smallest
         * expected line size.
         */
        nSensorCountMax = (nFileSize+1) / 50;
        assert(nSensorCountMax > 0);

        kSensors        = (TSensorType*)  malloc(nSensorCountMax * sizeof(TSensorType));
        kIntersections  = (TBoundaryType*)malloc(nSensorCountMax * sizeof(TBoundaryType));

        /* Note : Primary Queue is Line Intersections, and we want +/- 1 Wiggle Room */
        kQueue          = (int64_t*)      malloc(nSensorCountMax * (nSensorCountMax-1) * 2 * 3 * sizeof(int64_t));
        kQueueEntry     = (unsigned*)     malloc((nPart2EndY - nPart2StartY + 1) * sizeof(unsigned));

        assert(kSensors);
        assert(kIntersections);
        assert(kQueue);
        assert(kQueueEntry);

        memset(kQueueEntry, 0, (nPart2EndY - nPart2StartY + 1) * sizeof(unsigned));

        /* Read the Sensor/Beacon Data */
        while (!feof(pFile))
        {
            /* Read the Sensor/Beacon Data */
            fseek(pFile, 12, SEEK_CUR);
            kSensors[nSensorCount].nSensorX = getValue(&pFile);
            fseek(pFile, 3, SEEK_CUR);
            kSensors[nSensorCount].nSensorY = getValue(&pFile);
            fseek(pFile, 24, SEEK_CUR);
            kSensors[nSensorCount].nBeaconX = getValue(&pFile);
            fseek(pFile, 3, SEEK_CUR);
            kSensors[nSensorCount].nBeaconY = getValue(&pFile);

            /* Calculate Manhatten Distance */
            kSensors[nSensorCount].nManhattenDistance = llabs(kSensors[nSensorCount].nSensorX -
                                                              kSensors[nSensorCount].nBeaconX) +
                                                        llabs(kSensors[nSensorCount].nSensorY -
                                                              kSensors[nSensorCount].nBeaconY);

            /* Store the Lines for Part 2 */
            kSensors[nSensorCount].kLines[0].nStartX = kSensors[nSensorCount].nSensorX;
            kSensors[nSensorCount].kLines[0].nStartY = kSensors[nSensorCount].nSensorY +
                                                       kSensors[nSensorCount].nManhattenDistance;
            kSensors[nSensorCount].kLines[0].nEndX   = kSensors[nSensorCount].nSensorX +
                                                       kSensors[nSensorCount].nManhattenDistance;
            kSensors[nSensorCount].kLines[0].nEndY   = kSensors[nSensorCount].nSensorY;

            kSensors[nSensorCount].kLines[1].nStartX = kSensors[nSensorCount].kLines[0].nEndX;
            kSensors[nSensorCount].kLines[1].nStartY = kSensors[nSensorCount].kLines[0].nEndY;
            kSensors[nSensorCount].kLines[1].nEndX   = kSensors[nSensorCount].nSensorX;
            kSensors[nSensorCount].kLines[1].nEndY   = kSensors[nSensorCount].nSensorY -
                                                       kSensors[nSensorCount].nManhattenDistance;

            kSensors[nSensorCount].kLines[2].nStartX = kSensors[nSensorCount].kLines[1].nEndX;
            kSensors[nSensorCount].kLines[2].nStartY = kSensors[nSensorCount].kLines[1].nEndY;
            kSensors[nSensorCount].kLines[2].nEndX   = kSensors[nSensorCount].nSensorX -
                                                       kSensors[nSensorCount].nManhattenDistance;
            kSensors[nSensorCount].kLines[2].nEndY   = kSensors[nSensorCount].nSensorY;

            kSensors[nSensorCount].kLines[3].nStartX = kSensors[nSensorCount].kLines[2].nEndX;
            kSensors[nSensorCount].kLines[3].nStartY = kSensors[nSensorCount].kLines[2].nEndY;
            kSensors[nSensorCount].kLines[3].nEndX   = kSensors[nSensorCount].kLines[0].nStartX;
            kSensors[nSensorCount].kLines[3].nEndY   = kSensors[nSensorCount].kLines[0].nStartY;

            /* Pre-emptively collate Part 1 Data */
            getManhattenXBoundsForSensor(&kSensors[nSensorCount], nPart1TargetRow, &kIntersections[nIntersectionCount]);
            if (kIntersections[nIntersectionCount].bValid)
            {
                ++nIntersectionCount;
            }

            ++nSensorCount;
        }
        fclose(pFile);

        /* Part 1: For a Given Row calculate the Occupied Areas */

        /* Merge the Intersections */
        nIntersectionCount = combineManhattenIntersections(nIntersectionCount, kIntersections);

        /* Count the Unique Intersection Ranges */
        nIntersectionRange = 0;
        for (i = 0; i < nIntersectionCount; ++i)
        {
            if (kIntersections[i].bValid)
            {
                nIntersectionRange += kIntersections[i].nEndX - kIntersections[i].nStartX;
            }
        }

        printf("Part 1: %lli\n", nIntersectionRange);

        /* Part 2: Find the only row in a 4,000,001 x 4,000,001 Grid that can contain a Beacon */

        /* First: Try and solve by Intersections... */
        getYIntersections(kSensors, nSensorCount, nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
        bSolved = solveQueue(kSensors, nSensorCount, kQueue, nQueueSize, kIntersections, &nSolved);

        /* Second: Try and solve by Peaks/Centers */
        if (AOC_FALSE == bSolved)
        {
            nQueueSize = 0;

            for (i = 0; i < nSensorCount; ++i)
            {
                int64_t j;
                for (j = -1; j <= 1; ++j)
                {
                    const int64_t nTopY     = kSensors[i].nSensorY - kSensors[i].nManhattenDistance;
                    const int64_t nTopYN    = nTopY - 1;
                    const int64_t nTopYP    = nTopY + 1;

                    const int64_t nMiddleY  = kSensors[i].nSensorY;
                    const int64_t nMiddleYN = nMiddleY - 1;
                    const int64_t nMiddleYP = nMiddleY + 1;

                    const int64_t nBottomY  = kSensors[i].nSensorY + kSensors[i].nManhattenDistance;
                    const int64_t nBottomYN = nBottomY - 1;
                    const int64_t nBottomYP = nBottomY + 1;

                    addToQueue(nTopY,     nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                    addToQueue(nTopYN,    nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                    addToQueue(nTopYP,    nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);

                    addToQueue(nMiddleY,  nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                    addToQueue(nMiddleYN, nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                    addToQueue(nMiddleYP, nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);

                    addToQueue(nBottomY,  nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                    addToQueue(nBottomYN, nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                    addToQueue(nBottomYP, nPart2StartY, nPart2EndY, kQueue, &nQueueSize, kQueueEntry);
                }
            }
            bSolved = solveQueue(kSensors, nSensorCount, kQueue, nQueueSize, kIntersections, &nSolved);
        }

        if (AOC_TRUE == bSolved)
        {
            printf("Part 2: %lli\n", nSolved);
        }
        else
        {
            assert(0);
        }


        free(kSensors);
        free(kIntersections);
        free(kQueue);
        free(kQueueEntry);
    }

    return 0;
}
