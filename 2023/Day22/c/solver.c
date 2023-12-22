#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE        (1u)
#define AOC_FALSE       (0u)

typedef struct SPoint3DType
{
    int64_t nX;
    int64_t nY;
    int64_t nZ;
} SPoint3DType;

typedef struct SBrick3DType
{
    size_t       nIndex;
    SPoint3DType kStart;
    SPoint3DType kEnd;
} SBrick3DType;

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

/* Function for Sorting by the Z Axis of the Bricks */
int compareBricks(const void* pA, const void* pB)
{
    const SBrick3DType* pBrickA = (const SBrick3DType*)pA;
    const SBrick3DType* pBrickB = (const SBrick3DType*)pB;

    return pBrickA->kStart.nZ - pBrickB->kStart.nZ;
}

void getPoint(const char* kLine, SPoint3DType* kPoint)
{
    const char* pX = kLine;
    const char* pY  = strchr(pX, ',');
    const char* pZ  = strchr(pY + 1, ',');

    assert(pX);
    assert(pY);
    assert(pZ);

    kPoint->nX = strtoll(pX, NULL, 10);
    kPoint->nY = strtoll(pY + 1, NULL, 10);
    kPoint->nZ = strtoll(pZ + 1, NULL, 10);
}

void getBrick(const char* kLine, SBrick3DType* kPoint)
{
    const char* pStart = kLine;
    const char* pEndN  = strchr(pStart, '~');
    const char* pEnd   = pEndN + 1;

    assert(pStart);
    assert(pEndN);

    getPoint(pStart, &kPoint->kStart);
    getPoint(pEnd,   &kPoint->kEnd);
}

SBrick3DType* getClosestNeighbourBelow(SBrick3DType* kBrick, SBrick3DType* kBricks, size_t nCount)
{
    size_t nBrick;

    unsigned bFound = AOC_FALSE;
    size_t   nClosestBrick;
    int64_t  nClosestDistance;

    for (nBrick = 0; nBrick < nCount; ++nBrick)
    {
        if (kBrick == &kBricks[nBrick])
        {
            continue;
        }

        if ((0 > kBricks[nBrick].kEnd.nZ) ||
            (kBrick->kEnd.nZ   < 0))
        {
            continue;
        }

        if ((kBrick->kStart.nX <= kBricks[nBrick].kEnd.nX)   &&
            (kBrick->kEnd.nX   >= kBricks[nBrick].kStart.nX) &&
            (kBrick->kStart.nY <= kBricks[nBrick].kEnd.nY)   &&
            (kBrick->kEnd.nY   >= kBricks[nBrick].kStart.nY))
        {
            const int64_t nDistance = kBrick->kStart.nZ - kBricks[nBrick].kEnd.nZ;
            if (nDistance < 0) continue;
            if (AOC_FALSE == bFound)
            {
                bFound           = AOC_TRUE;
                nClosestBrick    = nBrick;
                nClosestDistance = nDistance;
                assert(nClosestDistance > 0);
            }
            else if (nDistance < nClosestDistance)
            {
                nClosestBrick    = nBrick;
                nClosestDistance = nDistance;
                assert(nClosestDistance > 0);
            }
        }
    }

    if (AOC_TRUE == bFound)
    {
        return &kBricks[nClosestBrick];
    }
    return NULL;
}

void getNeighboursAbove(SBrick3DType* kBrick, SBrick3DType* kBricks, size_t nCount, SBrick3DType** kNeighbours, size_t* nNeighbourCount)
{
    size_t nBrick;

    *nNeighbourCount = 0;

    for (nBrick = 0; nBrick < nCount; ++nBrick)
    {
        if (kBrick == &kBricks[nBrick])
        {
            continue;
        }

        if (kBricks[nBrick].kStart.nZ != (kBrick->kEnd.nZ + 1))
        {
            continue;
        }

        if ((kBricks[nBrick].kStart.nX <= kBrick->kEnd.nX) &&
            (kBricks[nBrick].kEnd.nX   >= kBrick->kStart.nX)   &&
            (kBricks[nBrick].kStart.nY <= kBrick->kEnd.nY) &&
            (kBricks[nBrick].kEnd.nY   >= kBrick->kStart.nY))
        {
            kNeighbours[*nNeighbourCount] = &kBricks[nBrick];
            ++(*nNeighbourCount);
        }
    }
}

void getNeighboursBelow(SBrick3DType* kBrick, SBrick3DType* kBricks, size_t nCount, SBrick3DType** kNeighbours, size_t* nNeighbourCount)
{
    size_t nBrick;

    *nNeighbourCount = 0;

    for (nBrick = 0; nBrick < nCount; ++nBrick)
    {
        if (kBrick == &kBricks[nBrick])
        {
            continue;
        }

        if ((kBricks[nBrick].kEnd.nZ + 1) != kBrick->kStart.nZ)
        {
            continue;
        }

        if ((kBricks[nBrick].kStart.nX <= kBrick->kEnd.nX) &&
            (kBricks[nBrick].kEnd.nX   >= kBrick->kStart.nX)   &&
            (kBricks[nBrick].kStart.nY <= kBrick->kEnd.nY) &&
            (kBricks[nBrick].kEnd.nY   >= kBrick->kStart.nY))
        {
            kNeighbours[*nNeighbourCount] = &kBricks[nBrick];
            ++(*nNeighbourCount);
        }
    }
}

uint64_t calculateChainReaction(const size_t nBrickIndex, SBrick3DType* kBricks, const size_t nCount, unsigned* kBrickFilter, SBrick3DType** kBricksAbove, SBrick3DType** kBricksBelow)
{
    uint64_t nChain = 0u;
    size_t   nBricksAbove;
    size_t   nBricksBelow;
    size_t   nBrickAbove;

    /* Filter the Current Brick */
    assert(AOC_FALSE   == kBrickFilter[kBricks[nBrickIndex].nIndex]);
    assert(nBrickIndex == kBricks[nBrickIndex].nIndex);

    kBrickFilter[kBricks[nBrickIndex].nIndex] = AOC_TRUE;

    /* Find any Above Neighbours for the Current Brick */
    getNeighboursAbove(&kBricks[nBrickIndex], kBricks, nCount, kBricksAbove, &nBricksAbove);

    /* If they have no neighbours below (because we removed the brick of interest), they're unstable. */
    for (nBrickAbove = 0; nBrickAbove < nBricksAbove; ++nBrickAbove)
    {
        size_t   nBrickBelow;
        size_t   nBrickBelowCount   = 0;

        if (kBrickFilter[(kBricksAbove[nBrickAbove])->nIndex])
        {
            continue;
        }

        /* Find any Below Neighbours for the Current Above */
        getNeighboursBelow((kBricksAbove[nBrickAbove]), kBricks, nCount, kBricksBelow, &nBricksBelow);

        for (nBrickBelow = 0; nBrickBelow < nBricksBelow; ++nBrickBelow)
        {
            if (AOC_FALSE == kBrickFilter[kBricksBelow[nBrickBelow]->nIndex])
            {
                ++nBrickBelowCount;
            }
        }

        /* If no bricks are below then we're unstable */
        if (0 == nBrickBelowCount)
        {
            /* Recursively calculate the chain length of the bricks above */
            nChain += 1 + calculateChainReaction(kBricksAbove[nBrickAbove]->nIndex, kBricks, nCount, kBrickFilter, &kBricksAbove[nBricksAbove], &kBricksBelow[nBricksBelow]);
        }
    }

    return nChain;
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

        SBrick3DType*           kBricks;
        SBrick3DType**          kBricksAbove;
        SBrick3DType**          kBricksBelow;

        unsigned*               kBrickFilter;

        uint64_t                nDisintegratedBrickCount = 0;
        uint64_t                nChainReactionCount      = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Bricks */
        kBricks = (SBrick3DType*)malloc(sizeof(SBrick3DType) * nLineCount);
        kBricksAbove = (SBrick3DType**)malloc(sizeof(SBrick3DType*) * nLineCount);
        kBricksBelow = (SBrick3DType**)malloc(sizeof(SBrick3DType*) * nLineCount);
        kBrickFilter = (unsigned*)malloc(sizeof(unsigned) * nLineCount);

        assert(kBricks);
        assert(kBricksAbove);
        assert(kBricksBelow);
        assert(kBrickFilter);

        /* Read the Bricks */
        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            getBrick(kLines[nLine], &kBricks[nLine]);
            kBricks[nLine].nIndex = nLine;
        }

        /* Part 1 - Find the Number of Bricks that can be Disintegrated */

        /* First... move all the bricks downwards until they're either on the ground or
         * ontop of other bricks.
         */

        /* Sort the Bricks by their Z Axis */
        qsort(kBricks, nLineCount, sizeof(SBrick3DType), compareBricks);

        /* Move the Bricks Downwards, starting from the bottom... */
        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            /* Correct the Index... Euurgh C... */
            kBricks[nLine].nIndex = nLine;

            /* Find the Closest Brick Below */
            SBrick3DType* pBrickBelow = getClosestNeighbourBelow(&kBricks[nLine], kBricks, nLineCount);

            if (NULL == pBrickBelow)
            {
                const int64_t nBufferedStartZ = kBricks[nLine].kStart.nZ;
                /* If no brick exists, it's a straight shot to the ground */
                kBricks[nLine].kStart.nZ = 0;
                kBricks[nLine].kEnd.nZ  -= nBufferedStartZ;
            }
            else
            {
                const int64_t nDistance = kBricks[nLine].kStart.nZ - pBrickBelow->kEnd.nZ - 1;
                kBricks[nLine].kStart.nZ -= nDistance;
                kBricks[nLine].kEnd.nZ   -= nDistance;
            }
        }

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            unsigned bOtherwiseSupported;
            size_t   nNeighbour;
            size_t   nBrickAboveCount;
            size_t   nBrickBelowCount;

            /* Find any Above Neighbours for the Current Brick */
            getNeighboursAbove(&kBricks[nLine], kBricks, nLineCount, kBricksAbove, &nBrickAboveCount);

            if (0 == nBrickAboveCount)
            {
                ++nDisintegratedBrickCount;
                continue;
            }

            bOtherwiseSupported = AOC_TRUE;

            for (nNeighbour = 0; nNeighbour < nBrickAboveCount; ++nNeighbour)
            {
                /* Check whether the Neighbour is Supported by another Brick */
                getNeighboursBelow(kBricksAbove[nNeighbour], kBricks, nLineCount, kBricksBelow, &nBrickBelowCount);

                if (nBrickBelowCount <= 1)
                {
                    bOtherwiseSupported = AOC_FALSE;
                    break;
                }
            }

            if (bOtherwiseSupported)
            {
                ++nDisintegratedBrickCount;
            }
        }

        printf("Part 1: %llu\n", nDisintegratedBrickCount);

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            memset(kBrickFilter, 0u, sizeof(unsigned) * nLineCount);
            nChainReactionCount += calculateChainReaction(nLine, kBricks, nLineCount, kBrickFilter, kBricksAbove, kBricksBelow);
        }
        printf("Part 2: %llu\n", nChainReactionCount);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kBricks);
        free(kBricksAbove);
        free(kBricksBelow);
        free(kBrickFilter);
    }

    return 0;
}