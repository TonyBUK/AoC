#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>

#define AOC_TRUE    (1u)
#define AOC_FALSE   (0u)

#define AXIS_CHECK(a1, a2, nIdenticalCount, nAdjacentCount) \
                                                            \
    if ((a1) == (a2))                                       \
    {                                                       \
        ++nIdenticalCount;                                  \
    }                                                       \
    else if (1 == abs((int)(a1) - (int)(a2)))               \
    {                                                       \
        ++nAdjacentCount;                                   \
    }                                                       \
    else                                                    \
    {                                                       \
        return AOC_FALSE;                                   \
    }

typedef struct CubeType
{
    unsigned char X;
    unsigned char Y;
    unsigned char Z;
} TCubeType;

unsigned isAdjacentCubes(const TCubeType* kCube1, const TCubeType* kCube2)
{
    size_t  nIdenticalCount = 0u;
    size_t  nAdjacentCount  = 0u;    

    AXIS_CHECK(kCube1->X, kCube2->X, nIdenticalCount, nAdjacentCount);
    AXIS_CHECK(kCube1->Y, kCube2->Y, nIdenticalCount, nAdjacentCount);
    AXIS_CHECK(kCube1->Z, kCube2->Z, nIdenticalCount, nAdjacentCount);

    return ((2 == nIdenticalCount) && (1 == nAdjacentCount)) ? AOC_TRUE : AOC_FALSE;
}

unsigned char getValue(FILE** pFile, const unsigned char nMax)
{
    unsigned short nValue = 0;
    unsigned char  kData;

    while (isdigit(kData = fgetc(*pFile)))
    {
        if (feof(*pFile))
        {
            break;
        }
        nValue = (nValue * 10) + (kData - '0');
        assert(nValue <= nMax);
    }

    return (unsigned char)nValue;
}

size_t getCubeIndex(const TCubeType* kCube)
{
    return ((size_t)kCube->X << 16u) |
           ((size_t)kCube->Y <<  8u) |
           ((size_t)kCube->Z);
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const int       ADJACENT_CUBE_DELTAS[][3] =
        {
            {-1,  0,  0},
            { 1,  0,  0},
            { 0, -1,  0},
            { 0,  1,  0},
            { 0,  0, -1},
            { 0,  0,  1}
        };

        const int       DIAGONAL_CUBE_DELTAS[][3] =
        {
            {-1, -1, -1},
            { 1, -1, -1},
            {-1,  1, -1},
            { 1,  1, -1},
            {-1,  0, -1},
            { 1,  0, -1},
            { 0, -1, -1},
            { 0,  1, -1},

            {-1, -1,  0},
            { 1, -1,  0},
            {-1,  1,  0},
            { 1,  1,  0},

            {-1, -1,  1},
            { 1, -1,  1},
            {-1,  1,  1},
            { 1,  1,  1},
            {-1,  0,  1},
            { 1,  0,  1},
            { 0, -1,  1},
            { 0,  1,  1}
        };

        long            nFileSize;

        TCubeType*      kCubes;
        size_t          nCubeSizeMax;
        size_t          nCubeSize           = 0u;

        unsigned*       kCubeMap;
        unsigned*       kWaterCubeMap;

        TCubeType*      kWaterCubesQueue;
        size_t          nWaterCubeQueueMax;
        size_t          nWaterCubeQueueHead = 0u;
        size_t          nWaterCubeQueueTail = 0u;

        unsigned char   nMinX               = (unsigned char)-1;
        unsigned char   nMaxX               = 0u;
        unsigned char   nMinY               = (unsigned char)-1;
        unsigned char   nMaxY               = 0u;
        unsigned char   nMinZ               = (unsigned char)-1;
        unsigned char   nMaxZ               = 0u;

        size_t          nSurfaceAreaCount;

        size_t          i;

        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        nCubeSizeMax        = ((size_t)nFileSize / 7) + 1;
        nWaterCubeQueueMax  = nCubeSizeMax * 26;

        kCubes              = (TCubeType*)malloc(nCubeSizeMax       * sizeof(TCubeType));
        kCubeMap            = (unsigned*) malloc((1 << 24)          * sizeof(unsigned));
        kWaterCubeMap       = (unsigned*) malloc((1 << 24)          * sizeof(unsigned));
        kWaterCubesQueue    = (TCubeType*)malloc(nWaterCubeQueueMax * sizeof(TCubeType));

        assert(kCubes);
        assert(kCubeMap);
        assert(kWaterCubeMap);
        assert(kWaterCubesQueue);

        memset(kCubeMap,      0, (1 << 24) * sizeof(unsigned));
        memset(kWaterCubeMap, 0, (1 << 24) * sizeof(unsigned));

        /* Read all of the Cubes.
         *
         * Note : As long as all of the cubes are correct relative to each other, the actual
         *        positions are arbitrary, so we add 10 to each cube to try to force all adjacent
         *        cubes to never go negative, whilst staying in the 8 bit range.  This will
         *        simplify any hash mapping later.
         */
        while (!feof(pFile))
        {
            kCubes[nCubeSize].X = getValue(&pFile, 245u) + 10;
            kCubes[nCubeSize].Y = getValue(&pFile, 245u) + 10;
            kCubes[nCubeSize].Z = getValue(&pFile, 245u) + 10;

            if (kCubes[nCubeSize].X < nMinX) nMinX = kCubes[nCubeSize].X;
            if (kCubes[nCubeSize].X > nMaxX) nMaxX = kCubes[nCubeSize].X;
            if (kCubes[nCubeSize].Y < nMinY) nMinY = kCubes[nCubeSize].Y;
            if (kCubes[nCubeSize].Y > nMaxY) nMaxY = kCubes[nCubeSize].Y;
            if (kCubes[nCubeSize].Z < nMinZ) nMinZ = kCubes[nCubeSize].Z;
            if (kCubes[nCubeSize].Z > nMaxZ) nMaxZ = kCubes[nCubeSize].Z;

            ++nCubeSize;
        }

        fclose(pFile);

        /* Calculate the Surface Area, this will be 6 * Number of Cubes, with 2 subtracted
         * for each pair of adjacent cubes.
         *
         * Note: We can also pre-emptively queue up the initial set of data for Part 2.
         */
        nSurfaceAreaCount = nCubeSize * 6u;
        for (i = 0u; i < nCubeSize; ++i)
        {
            size_t j;
            for (j = i + 1u; j < nCubeSize; ++j)
            {
                if (isAdjacentCubes(&kCubes[i], &kCubes[j]))
                {
                    nSurfaceAreaCount -= 2;
                }
            }

            kCubeMap[getCubeIndex(&kCubes[i])] = AOC_TRUE;

            if (kCubes[i].X == nMinX)
            {
                kWaterCubesQueue[nWaterCubeQueueTail] = kCubes[i];
                kWaterCubesQueue[nWaterCubeQueueTail++].X -= 1;

            }
            else if (kCubes[i].X == nMaxX)
            {
                kWaterCubesQueue[nWaterCubeQueueTail] = kCubes[i];
                kWaterCubesQueue[nWaterCubeQueueTail++].X += 1;
            }
            else if (kCubes[i].Y == nMinY)
            {
                kWaterCubesQueue[nWaterCubeQueueTail] = kCubes[i];
                kWaterCubesQueue[nWaterCubeQueueTail++].Y -= 1;
            }
            else if (kCubes[i].Y == nMaxY)
            {
                kWaterCubesQueue[nWaterCubeQueueTail] = kCubes[i];
                kWaterCubesQueue[nWaterCubeQueueTail++].Y += 1;
            }
            else if (kCubes[i].Z == nMinZ)
            {
                kWaterCubesQueue[nWaterCubeQueueTail] = kCubes[i];
                kWaterCubesQueue[nWaterCubeQueueTail++].Z -= 1;
            }
            else if (kCubes[i].Z == nMaxZ)
            {
                kWaterCubesQueue[nWaterCubeQueueTail] = kCubes[i];
                kWaterCubesQueue[nWaterCubeQueueTail++].Z += 1;
            }
        }

        printf("Part 1: %zu\n", nSurfaceAreaCount);

        nSurfaceAreaCount = 0u;
        while (nWaterCubeQueueHead != nWaterCubeQueueTail)
        {
            const size_t nWaterQueueIndex           = getCubeIndex(&kWaterCubesQueue[nWaterCubeQueueHead]);
            size_t       nCandidateWaterCubeTail    = nWaterCubeQueueTail;
            unsigned     bAdjacent                  = AOC_FALSE;

            /* Ensure we've not visited this item already... */
            if (AOC_FALSE == kWaterCubeMap[nWaterQueueIndex])
            {
                kWaterCubeMap[nWaterQueueIndex] = AOC_TRUE;
                for (i = 0; i < (sizeof(ADJACENT_CUBE_DELTAS))/sizeof(ADJACENT_CUBE_DELTAS[0]); ++i)
                {
                    TCubeType kCube = kWaterCubesQueue[nWaterCubeQueueHead];
                    size_t    nCubeIndex;
                    kCube.X    += ADJACENT_CUBE_DELTAS[i][0];
                    kCube.Y    += ADJACENT_CUBE_DELTAS[i][1];
                    kCube.Z    += ADJACENT_CUBE_DELTAS[i][2];
                    nCubeIndex  = getCubeIndex(&kCube);
                    
                    if (AOC_TRUE == kCubeMap[nCubeIndex])
                    {
                        ++nSurfaceAreaCount;
                        bAdjacent = AOC_TRUE;
                    }
                    else if (AOC_FALSE == kWaterCubeMap[nCubeIndex])
                    {
                        kWaterCubesQueue[nCandidateWaterCubeTail] = kCube;
                        nCandidateWaterCubeTail = (nCandidateWaterCubeTail + 1) % nWaterCubeQueueMax;
                    }
                }

                if (AOC_FALSE == bAdjacent)
                {
                    for (i = 0; i < (sizeof(DIAGONAL_CUBE_DELTAS))/sizeof(DIAGONAL_CUBE_DELTAS[0]); ++i)
                    {
                        TCubeType kCube = kWaterCubesQueue[nWaterCubeQueueHead];

                        kCube.X    += DIAGONAL_CUBE_DELTAS[i][0];
                        kCube.Y    += DIAGONAL_CUBE_DELTAS[i][1];
                        kCube.Z    += DIAGONAL_CUBE_DELTAS[i][2];

                        if (AOC_TRUE == kCubeMap[getCubeIndex(&kCube)])
                        {
                            bAdjacent = AOC_TRUE;
                            break;
                        }
                    }
                }

                if (AOC_TRUE == bAdjacent)
                {
                    nWaterCubeQueueTail = nCandidateWaterCubeTail;
                }
            }

            nWaterCubeQueueHead = (nWaterCubeQueueHead + 1) % nWaterCubeQueueMax;
        }

        printf("Part 2: %zu\n", nSurfaceAreaCount);

        free(kCubes);
        free(kCubeMap);
        free(kWaterCubeMap);
        free(kWaterCubesQueue);
    }

    return 0;
}