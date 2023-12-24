#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE            (1u)
#define AOC_FALSE           (0u)

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define ABS(a)              ((a) < 0 ? -(a) : (a))

typedef struct SVector3Type
{
    int64_t k[3];
} SVector3Type;

typedef struct SHailStoneType
{
    SVector3Type    kPosition;
    SVector3Type    kVelocity;
} SHailStoneType;

typedef struct SMatrixRowCalculationType
{
    SVector3Type*   kV1;
    SVector3Type*   kV2;
    SVector3Type*   kP1;
    SVector3Type*   kP2;
    size_t          kIndices[6];
} SMatrixRowCalculationType;

typedef struct SMatrixRowType
{
    double          kElement[7];
} SMatrixRowType;

typedef struct SSolutionRowCalculationType
{
    SVector3Type*   kP1;
    SVector3Type*   kV1;
    SVector3Type*   kP2;
    SVector3Type*   kV2;
    size_t          kIndicex[2];
} SSolutionRowCalculationType;

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

void extractPoints(const char* pCSV, SVector3Type* pVector)
{
    const char* pX          = pCSV;
    const char* pYUnaligned = strchr(pX, ',');
    const char* pY          = pYUnaligned + 2;
    const char* pZUnaligned = strchr(pY, ',');
    const char* pZ          = pZUnaligned + 2;

    assert(pX);
    assert(pYUnaligned);
    assert(pZUnaligned);

    pVector->k[0] = strtoll(pX, NULL, 10);
    pVector->k[1] = strtoll(pY, NULL, 10);
    pVector->k[2] = strtoll(pZ, NULL, 10);
}

double det(const SVector3Type* a, const SVector3Type* b)
{
    return ((double)a->k[0] * (double)b->k[1]) - ((double)a->k[1] * (double)b->k[0]);
}

double detD(const double* a, const SVector3Type* b)
{
    return (a[0] * (double)b->k[1]) - (a[1] * (double)b->k[0]);
}

unsigned lineIntersection(const SVector3Type* kLineAStart, const SVector3Type* kLineAEnd, const SVector3Type* kLineBStart, const SVector3Type* kLineBEnd, double* nIntersectX, double* nIntersectY)
{
    /* This first part will compute the intersection of the lines projected to infinity */
    const SVector3Type  kXDiff = {
        kLineAStart->k[0] - kLineAEnd->k[0],
        kLineBStart->k[0] - kLineBEnd->k[0],
        0
    };

    const SVector3Type  kYDiff = {
        kLineAStart->k[1] - kLineAEnd->k[1],
        kLineBStart->k[1] - kLineBEnd->k[1],
        0
    };

    const double        nDiv = det(&kXDiff, &kYDiff);

          double        nD[2];
          double        nX;
          double        nY;

    if (0 == nDiv)
    {
        return AOC_FALSE;
    }
    nD[0] = det(kLineAStart, kLineAEnd);
    nD[1] = det(kLineBStart, kLineBEnd);

    nX = (double)detD(nD, &kXDiff) / (double)nDiv;
    nY = (double)detD(nD, &kYDiff) / (double)nDiv;

    /* This second part will check if the intersection is within the bounds of the line segments */
    if ((nX >= MIN(kLineAStart->k[0], kLineAEnd->k[0]) && nX <= MAX(kLineAStart->k[0], kLineAEnd->k[0])) &&
        (nY >= MIN(kLineAStart->k[1], kLineAEnd->k[1]) && nY <= MAX(kLineAStart->k[1], kLineAEnd->k[1])) &&
        (nX >= MIN(kLineBStart->k[0], kLineBEnd->k[0]) && nX <= MAX(kLineBStart->k[0], kLineBEnd->k[0])) &&
        (nY >= MIN(kLineBStart->k[1], kLineBEnd->k[1]) && nY <= MAX(kLineBStart->k[1], kLineBEnd->k[1])))
    {
        *nIntersectX = nX;
        *nIntersectY = nY;

        return AOC_TRUE;
    }

    return AOC_FALSE;
}

unsigned testIntersection(const int64_t nMin, const int64_t nMax, const SHailStoneType* kHailStoneA, const SHailStoneType* kHailStoneB)
{
    /* We know the Start Position, so what we need to do for each line is calculate a
     * sensible end point, we can do that by diffing the deltas.
     * Note: This assumes that the Hailstone movement is always at least 1.
     */
    const int64_t  nDeltaXFromMin = MAX(ABS(nMin - kHailStoneA->kPosition.k[0]), ABS(nMin - kHailStoneB->kPosition.k[0]));
    const int64_t  nDeltaXFromMax = MAX(ABS(nMax - kHailStoneA->kPosition.k[0]), ABS(nMax - kHailStoneB->kPosition.k[0]));
    const int64_t  nDeltaYFromMin = MAX(ABS(nMin - kHailStoneA->kPosition.k[1]), ABS(nMin - kHailStoneB->kPosition.k[1]));
    const int64_t  nDeltaYFromMax = MAX(ABS(nMax - kHailStoneA->kPosition.k[1]), ABS(nMax - kHailStoneB->kPosition.k[1]));
    const uint64_t nFutureCycles = MAX(MAX(nDeltaXFromMin, nDeltaXFromMax), MAX(nDeltaYFromMin, nDeltaYFromMax));

    const SVector3Type kHailStoneAEndPoint = {
        kHailStoneA->kPosition.k[0] + (kHailStoneA->kVelocity.k[0] * nFutureCycles),
        kHailStoneA->kPosition.k[1] + (kHailStoneA->kVelocity.k[1] * nFutureCycles),
        0
    };
    const SVector3Type kHailStoneBEndPoint = {
        kHailStoneB->kPosition.k[0] + (kHailStoneB->kVelocity.k[0] * nFutureCycles),
        kHailStoneB->kPosition.k[1] + (kHailStoneB->kVelocity.k[1] * nFutureCycles),
        0
    };

    double   nIntersectionX;
    double   nIntersectionY;
    
    if (AOC_FALSE == lineIntersection(&kHailStoneA->kPosition, &kHailStoneAEndPoint, &kHailStoneB->kPosition, &kHailStoneBEndPoint, &nIntersectionX, &nIntersectionY))
    {
        return AOC_FALSE;
    }
    else if (nIntersectionX >= nMin && nIntersectionX <= nMax && nIntersectionY >= nMin && nIntersectionY <= nMax)
    {
        return AOC_TRUE;
    }

    return AOC_FALSE;
}

/* With thanks to Google */
void calculateMatrixRow(const SMatrixRowCalculationType* kMatrixRowPreCalc, SMatrixRowType* kMatrixRowPostCalc)
{
    const SVector3Type* kV1         = kMatrixRowPreCalc->kV1;
    const SVector3Type* kV2         = kMatrixRowPreCalc->kV2;
    const SVector3Type* kP1         = kMatrixRowPreCalc->kP1;
    const SVector3Type* kP2         = kMatrixRowPreCalc->kP2;
    const size_t*       kIndices    = kMatrixRowPreCalc->kIndices;

    size_t nRow = 0;
    for (nRow = 0; nRow < 6; ++nRow)
    {
        kMatrixRowPostCalc->kElement[nRow] = 0.0;
    }

    kMatrixRowPostCalc->kElement[kIndices[0]] = -(kV1->k[kIndices[1]] - kV2->k[kIndices[1]]);
    kMatrixRowPostCalc->kElement[kIndices[1]] =  (kV1->k[kIndices[0]] - kV2->k[kIndices[0]]);
    
    kMatrixRowPostCalc->kElement[kIndices[3]] =  (kP1->k[kIndices[1]] - kP2->k[kIndices[1]]);
    kMatrixRowPostCalc->kElement[kIndices[4]] = -(kP1->k[kIndices[0]] - kP2->k[kIndices[0]]);
}

double calculateSolutionRow(const SSolutionRowCalculationType* kSolutionRowPreCalc)
{
    const SVector3Type* kP1        = kSolutionRowPreCalc->kP1;
    const SVector3Type* kV1        = kSolutionRowPreCalc->kV1;
    const SVector3Type* kP2        = kSolutionRowPreCalc->kP2;
    const SVector3Type* kV2        = kSolutionRowPreCalc->kV2;
    const size_t*       kIndices   = kSolutionRowPreCalc->kIndicex;

    return (kP1->k[kIndices[1]] * kV1->k[kIndices[0]] - kP2->k[kIndices[1]] * kV2->k[kIndices[0]]) -
           (kP1->k[kIndices[0]] * kV1->k[kIndices[1]] - kP2->k[kIndices[0]] * kV2->k[kIndices[1]]);
}

void gaussianElimination(SMatrixRowType* kA, const double* kB, double* kSolution)
{
    const size_t          n     = 6;
          SMatrixRowType* kM    = kA;

          size_t          i, j, k;

    for (i = 0; i < n; ++i)
    {
        kM[i].kElement[n] = kB[i];
    }

    for (k = 0; k < n; ++k)
    {
        for (i = k; i < n; ++i)
        {
            if (ABS(kM[i].kElement[k]) > ABS(kM[k].kElement[k]))
            {
                SMatrixRowType kTemp = kM[k];
                kM[k] = kM[i];
                kM[i] = kTemp;
            }
        }

        for (j = k + 1; j < n; ++j)
        {
            double f = kM[j].kElement[k] / kM[k].kElement[k];
            for (i = k; i < n + 1; ++i)
            {
                kM[j].kElement[i] -= kM[k].kElement[i] * f;
            }
        }
    }

    for (i = 0; i < n; ++i)
    {
        kSolution[i] = 0;
    }
    kSolution[n - 1] = kM[n - 1].kElement[n] / kM[n - 1].kElement[n - 1];

    for (i = n; i > 0; --i)
    {
        double s = 0;
        for (j = i; j < n; ++j)
        {
            s += kM[i - 1].kElement[j] * kSolution[j];
        }
        kSolution[i - 1] = (kM[i - 1].kElement[n] - s) / kM[i - 1].kElement[i - 1];
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                       kBuffer;
        char**                      kLines;
        size_t                      nHailStoneCount;
        SHailStoneType*             kHailStones;

        size_t                      nHailStone;

        uint64_t                    nIntersections = 0;

        SMatrixRowCalculationType   kInitialMatrix[6];
        SMatrixRowType              kInitialMatrixPostCalc[6];
        SSolutionRowCalculationType kSolutionMatrix[6];
        double                      kSolutionMatrixPostCalc[6];
        size_t                      nRow;
        double                      kSolution[6];

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nHailStoneCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Hailstones */
        kHailStones = (SHailStoneType*)malloc(nHailStoneCount * sizeof(SHailStoneType));

        /* Extract the Hail Stones */
        for (nHailStone = 0; nHailStone < nHailStoneCount; ++nHailStone)
        {
            /* Extract the Current Hail Stone */
            const char* pPoints               = kLines[nHailStone];
            const char* pVelocityPreAlignment = strchr(pPoints, '@');
            const char* pVelocity             = pVelocityPreAlignment + 2;;

            assert(pPoints);
            assert(pVelocityPreAlignment);

            extractPoints(pPoints, &kHailStones[nHailStone].kPosition);
            extractPoints(pVelocity, &kHailStones[nHailStone].kVelocity);
        }

        /* Part One - Find Intersections in the Test Area */
        for (nHailStone = 0; nHailStone < nHailStoneCount; ++nHailStone)
        {
            size_t nOtherHailStone;
            for (nOtherHailStone = nHailStone + 1; nOtherHailStone < nHailStoneCount; ++nOtherHailStone)
            {
                const SHailStoneType* pHailStoneA = &kHailStones[nHailStone];
                const SHailStoneType* pHailStoneB = &kHailStones[nOtherHailStone];

                if (testIntersection(200000000000000ll, 400000000000000ll, pHailStoneA, pHailStoneB))
                {
                    ++nIntersections;
                }
            }
        
        }

        printf("Part 1: %llu\n", nIntersections);

        /* Part Two - Ok Google... */

        /* Initial Matrix */
        kInitialMatrix[0].kV1         = &kHailStones[0].kVelocity;
        kInitialMatrix[0].kV2         = &kHailStones[1].kVelocity;
        kInitialMatrix[0].kP1         = &kHailStones[0].kPosition;
        kInitialMatrix[0].kP2         = &kHailStones[1].kPosition;
        kInitialMatrix[0].kIndices[0] = 0;
        kInitialMatrix[0].kIndices[1] = 1;
        kInitialMatrix[0].kIndices[2] = 2;
        kInitialMatrix[0].kIndices[3] = 3;
        kInitialMatrix[0].kIndices[4] = 4;
        kInitialMatrix[0].kIndices[5] = 5;
        kInitialMatrix[1].kV1         = &kHailStones[0].kVelocity;
        kInitialMatrix[1].kV2         = &kHailStones[2].kVelocity;
        kInitialMatrix[1].kP1         = &kHailStones[0].kPosition;
        kInitialMatrix[1].kP2         = &kHailStones[2].kPosition;
        kInitialMatrix[1].kIndices[0] = 0;
        kInitialMatrix[1].kIndices[1] = 1;
        kInitialMatrix[1].kIndices[2] = 2;
        kInitialMatrix[1].kIndices[3] = 3;
        kInitialMatrix[1].kIndices[4] = 4;
        kInitialMatrix[1].kIndices[5] = 5;

        kInitialMatrix[2].kV1         = &kHailStones[0].kVelocity;
        kInitialMatrix[2].kV2         = &kHailStones[1].kVelocity;
        kInitialMatrix[2].kP1         = &kHailStones[0].kPosition;
        kInitialMatrix[2].kP2         = &kHailStones[1].kPosition;
        kInitialMatrix[2].kIndices[0] = 1;
        kInitialMatrix[2].kIndices[1] = 2;
        kInitialMatrix[2].kIndices[2] = 0;
        kInitialMatrix[2].kIndices[3] = 4;
        kInitialMatrix[2].kIndices[4] = 5;
        kInitialMatrix[2].kIndices[5] = 3;
        kInitialMatrix[3].kV1         = &kHailStones[0].kVelocity;
        kInitialMatrix[3].kV2         = &kHailStones[2].kVelocity;
        kInitialMatrix[3].kP1         = &kHailStones[0].kPosition;
        kInitialMatrix[3].kP2         = &kHailStones[2].kPosition;
        kInitialMatrix[3].kIndices[0] = 1;
        kInitialMatrix[3].kIndices[1] = 2;
        kInitialMatrix[3].kIndices[2] = 0;
        kInitialMatrix[3].kIndices[3] = 4;
        kInitialMatrix[3].kIndices[4] = 5;
        kInitialMatrix[3].kIndices[5] = 3;

        kInitialMatrix[4].kV1         = &kHailStones[0].kVelocity;
        kInitialMatrix[4].kV2         = &kHailStones[1].kVelocity;
        kInitialMatrix[4].kP1         = &kHailStones[0].kPosition;
        kInitialMatrix[4].kP2         = &kHailStones[1].kPosition;
        kInitialMatrix[4].kIndices[0] = 2;
        kInitialMatrix[4].kIndices[1] = 0;
        kInitialMatrix[4].kIndices[2] = 1;
        kInitialMatrix[4].kIndices[3] = 5;
        kInitialMatrix[4].kIndices[4] = 3;
        kInitialMatrix[4].kIndices[5] = 4;
        kInitialMatrix[5].kV1         = &kHailStones[0].kVelocity;
        kInitialMatrix[5].kV2         = &kHailStones[2].kVelocity;
        kInitialMatrix[5].kP1         = &kHailStones[0].kPosition;
        kInitialMatrix[5].kP2         = &kHailStones[2].kPosition;
        kInitialMatrix[5].kIndices[0] = 2;
        kInitialMatrix[5].kIndices[1] = 0;
        kInitialMatrix[5].kIndices[2] = 1;
        kInitialMatrix[5].kIndices[3] = 5;
        kInitialMatrix[5].kIndices[4] = 3;
        kInitialMatrix[5].kIndices[5] = 4;

        /* Solution Matrix */
        kSolutionMatrix[0].kP1         = &kHailStones[0].kPosition;
        kSolutionMatrix[0].kV1         = &kHailStones[0].kVelocity;
        kSolutionMatrix[0].kP2         = &kHailStones[1].kPosition;
        kSolutionMatrix[0].kV2         = &kHailStones[1].kVelocity;
        kSolutionMatrix[0].kIndicex[0] = 0;
        kSolutionMatrix[0].kIndicex[1] = 1;
        kSolutionMatrix[1].kP1         = &kHailStones[0].kPosition;
        kSolutionMatrix[1].kV1         = &kHailStones[0].kVelocity;
        kSolutionMatrix[1].kP2         = &kHailStones[2].kPosition;
        kSolutionMatrix[1].kV2         = &kHailStones[2].kVelocity;
        kSolutionMatrix[1].kIndicex[0] = 0;
        kSolutionMatrix[1].kIndicex[1] = 1;

        kSolutionMatrix[2].kP1         = &kHailStones[0].kPosition;
        kSolutionMatrix[2].kV1         = &kHailStones[0].kVelocity;
        kSolutionMatrix[2].kP2         = &kHailStones[1].kPosition;
        kSolutionMatrix[2].kV2         = &kHailStones[1].kVelocity;
        kSolutionMatrix[2].kIndicex[0] = 1;
        kSolutionMatrix[2].kIndicex[1] = 2;
        kSolutionMatrix[3].kP1         = &kHailStones[0].kPosition;
        kSolutionMatrix[3].kV1         = &kHailStones[0].kVelocity;
        kSolutionMatrix[3].kP2         = &kHailStones[2].kPosition;
        kSolutionMatrix[3].kV2         = &kHailStones[2].kVelocity;
        kSolutionMatrix[3].kIndicex[0] = 1;
        kSolutionMatrix[3].kIndicex[1] = 2;

        kSolutionMatrix[4].kP1         = &kHailStones[0].kPosition;
        kSolutionMatrix[4].kV1         = &kHailStones[0].kVelocity;
        kSolutionMatrix[4].kP2         = &kHailStones[1].kPosition;
        kSolutionMatrix[4].kV2         = &kHailStones[1].kVelocity;
        kSolutionMatrix[4].kIndicex[0] = 2;
        kSolutionMatrix[4].kIndicex[1] = 0;
        kSolutionMatrix[5].kP1         = &kHailStones[0].kPosition;
        kSolutionMatrix[5].kV1         = &kHailStones[0].kVelocity;
        kSolutionMatrix[5].kP2         = &kHailStones[2].kPosition;
        kSolutionMatrix[5].kV2         = &kHailStones[2].kVelocity;
        kSolutionMatrix[5].kIndicex[0] = 2;
        kSolutionMatrix[5].kIndicex[1] = 0;

        for (nRow = 0; nRow < 6; ++nRow)
        {
            calculateMatrixRow(&kInitialMatrix[nRow], &kInitialMatrixPostCalc[nRow]);
            kSolutionMatrixPostCalc[nRow] = calculateSolutionRow(&kSolutionMatrix[nRow]);
        }

        gaussianElimination(kInitialMatrixPostCalc, kSolutionMatrixPostCalc, kSolution);

        printf("Part 2: %lli\n", (int64_t)(kSolution[0] + kSolution[1] + kSolution[2] + 0.5));

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kHailStones);
    }

    return 0;
}