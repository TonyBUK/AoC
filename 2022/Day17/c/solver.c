#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <assert.h>

/* Needed for Hash Map */
typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

#include "hashmap/include/hashmap.h"

#define AOC_TRUE                (1)
#define AOC_FALSE               (0)
#define ROCK_COUNT              (5)
#define ROCK_HEIGHT             (4)
#define ROCK_WIDTH              (4)
#define COLLISION_HEIGHT        (256)
#define COLLISION_WIDTH         (7)
#define COLLISION_WIDTH_PADDED  (COLLISION_WIDTH + ROCK_WIDTH)
#define INITIAL_HISTORY_SIZE    (65536)

/* Note: Padding for Collisions is needed to simplify the case of testing
 *       right edge collision.  As the rocks as defined such that the
 *       bottom/left of the rock is algined to the bottom/left of the
 *       rocks collision map, and that the rocks collision map is defined
 *       by the largest rocks width/height, it means the worst case is
 *       three blank areas to the right.  If we only had a collision width
 *       which had incorporated a single column to the right, we'd need
 *       to constant range test to make sure we don't overflow the column,
 *       whereas by extending the wall, we keep a nice easy general case at
 *       the expense of a little more RAM used.
 */

typedef struct CollisionHistoryType
{
    unsigned char kPeaks[COLLISION_WIDTH];
    size_t        nRock;
    size_t        nJetStreamPosition;
} TCollisionHistoryType;

typedef HASHMAP(TCollisionHistoryType, uint64_t) TCollisionHistoryHashMapType;

size_t genericHash(const TCollisionHistoryType* key)
{
    return hashmap_hash_default(key, sizeof(*key));
}

int compareCollisionMap(const TCollisionHistoryType* a, const TCollisionHistoryType* b)
{
    return memcmp(a, b, sizeof(TCollisionHistoryType));
}

void addToTowerHeightHistory(unsigned char** kTowerHeightHistory, size_t* pnTowerHeightHistorySize, size_t* pnTowerHeightHistoryMaxSize, const unsigned char nTowerHeightDelta)
{
    if ((*pnTowerHeightHistorySize + 1) == *pnTowerHeightHistoryMaxSize)
    {
        unsigned char* kNewTowerHeightHistory;

        *pnTowerHeightHistoryMaxSize *= 2;
        kNewTowerHeightHistory = (unsigned char*)malloc(*pnTowerHeightHistoryMaxSize * sizeof(unsigned char));
        memcpy(kNewTowerHeightHistory, *kTowerHeightHistory, *pnTowerHeightHistorySize * sizeof(unsigned char));
        free(*kTowerHeightHistory);
        *kTowerHeightHistory = kNewTowerHeightHistory;
    }

    (*kTowerHeightHistory)[*pnTowerHeightHistorySize] = nTowerHeightDelta;
    *pnTowerHeightHistorySize += 1;
}

unsigned testCollision(const unsigned kCollisionMap[COLLISION_HEIGHT][COLLISION_WIDTH_PADDED], const unsigned kRock[ROCK_HEIGHT][ROCK_WIDTH], const size_t nRockX, const size_t nRockY, const size_t nHead)
{
    size_t Y;
    for (Y = 0; Y < ROCK_HEIGHT; ++Y)
    {
        const size_t nCollisionY = (nRockY + Y + nHead + COLLISION_HEIGHT - (ROCK_HEIGHT - 1)) % COLLISION_HEIGHT;
        size_t X;
        assert(nCollisionY < 256);
        for (X = 0; X < ROCK_WIDTH; ++X)
        {
            if (kRock[Y][X])
            {
                const size_t nCollisionX = nRockX + X;
                if (kCollisionMap[nCollisionY][nCollisionX])
                {
                    return AOC_TRUE;
                }
            }
        }

    }

    return AOC_FALSE;
}

void printState(const unsigned kCollisionMap[COLLISION_HEIGHT][COLLISION_WIDTH_PADDED], const size_t nCollisionMapHead)
{
    size_t i = nCollisionMapHead;

    do
    {
        size_t x;

        printf("%-3zu : ", i);

        for (x = 0; x < COLLISION_WIDTH_PADDED; ++x)
        {
            if (AOC_FALSE == kCollisionMap[i][x])
            {
                printf(".");
            }
            else
            {
                printf("#");
            }
        }
        printf("\n");
        i = (i + 1) % COLLISION_HEIGHT;
    } while (nCollisionMapHead != i);

    printf("\n");
}

uint64_t getTowerHeight(const uint64_t nTotalFallingRocks, const char* kJetStream, const size_t nJetStreamSize)
{
    const unsigned kRocks[ROCK_COUNT][ROCK_HEIGHT][ROCK_WIDTH] =
    {
        {
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_TRUE,  AOC_TRUE,  AOC_TRUE,  AOC_TRUE   }   /* #### */
        },
        {
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_FALSE, AOC_TRUE,  AOC_FALSE, AOC_FALSE  },  /* .#.. */
            {   AOC_TRUE,  AOC_TRUE,  AOC_TRUE,  AOC_FALSE  },  /* ###. */
            {   AOC_FALSE, AOC_TRUE,  AOC_FALSE, AOC_FALSE  }   /* .#.. */
        },
        {
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_FALSE, AOC_FALSE, AOC_TRUE,  AOC_FALSE  },  /* ..#. */
            {   AOC_FALSE, AOC_FALSE, AOC_TRUE,  AOC_FALSE  },  /* ..#. */
            {   AOC_TRUE,  AOC_TRUE,  AOC_TRUE,  AOC_FALSE  }   /* ###. */
        },
        {
            {   AOC_TRUE,  AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* #... */
            {   AOC_TRUE,  AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* #... */
            {   AOC_TRUE,  AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* #... */
            {   AOC_TRUE,  AOC_FALSE, AOC_FALSE, AOC_FALSE  }   /* #... */
        },
        {
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_FALSE, AOC_FALSE, AOC_FALSE, AOC_FALSE  },  /* .... */
            {   AOC_TRUE,  AOC_TRUE,  AOC_FALSE, AOC_FALSE  },  /* ##.. */
            {   AOC_TRUE,  AOC_TRUE,  AOC_FALSE, AOC_FALSE  }   /* ##.. */
        }
    };

    /* Collision Map */
    unsigned        kCollisionMap[COLLISION_HEIGHT][COLLISION_WIDTH_PADDED];
    size_t          nCollisionMapHead   = 0u;

    uint64_t                        nCurrentRock                    = 0u;
    size_t                          nJetStreamPosition              = 0u;
    uint64_t                        nTowerHeight                    = 0llu;
    unsigned char*                  kTowerHeightHistory;
    size_t                          nTowerHeightHistorySize         = 0u;
    size_t                          nTowerHeightHistoryMaxSize      = INITIAL_HISTORY_SIZE;
    TCollisionHistoryHashMapType    kTowerStateHashMap;
    uint64_t*                       pnTowerStateHashData;
    size_t                          X, Y;
    size_t                          nHead                           = COLLISION_HEIGHT - (ROCK_HEIGHT + 1);
    uint64_t                        nLoopOffset;
    uint64_t                        nLoopInterval;
    unsigned                        bLoopFound                      = AOC_FALSE;

    unsigned char                   kDeltaHeights[COLLISION_WIDTH]  =
    {
        4u, 4u, 4u, 4u, 4u, 4u, 4u
    };

    size_t                          nMinimumDeltaHeight = 4u;

    kTowerHeightHistory = (unsigned char*)malloc(nTowerHeightHistoryMaxSize * sizeof(unsigned char));

    /* Initialise the Collision Map */
    for (Y = 0; Y < COLLISION_HEIGHT; ++Y)
    {
        /* Left Column */
        kCollisionMap[Y][0] = AOC_TRUE;

        if (Y == (COLLISION_HEIGHT - 1))
        {
            for (X = 1; X <= COLLISION_WIDTH; ++X)
            {
                kCollisionMap[Y][X] = AOC_TRUE;
            }
        }
        else
        {
            for (X = 1; X <= COLLISION_WIDTH; ++X)
            {
                kCollisionMap[Y][X] = AOC_FALSE;
            }
        }

        /* Right Column (Padded) */
        for (X = COLLISION_WIDTH + 1; X < COLLISION_WIDTH_PADDED; ++X)
        {
            kCollisionMap[Y][X] = AOC_TRUE;
        }
    }

    /* Allocate the Tower States Hash Map */
    hashmap_init(&kTowerStateHashMap, genericHash, compareCollisionMap);

    /* Process the Falling Rocks until we either complete the requested sequence, or find
     * a loop from which we can derive the requested sequence.
     */
    while (nCurrentRock < nTotalFallingRocks)
    {
        /* Initial State of the Falling Rock */
        size_t nRockX                       = 3u;
        size_t nRockY                       = 0;
        size_t nRockType                    = (size_t)(nCurrentRock % ROCK_COUNT);
        size_t nJetStreamPositionAtStart    = nJetStreamPosition;

        /* Move the Rock until a Collision is detected */
        while(1)
        {
            const char   nRockXDelta     = kJetStream[nJetStreamPosition];
            const size_t nCandidateRockX = (size_t)((char)nRockX + nRockXDelta);
            const size_t nCandidateRockY = nRockY + 1;

            /* Make sure we haven't overflown the circular buffer. */
            assert(nRockY < (256-4));

            if (AOC_FALSE == testCollision(kCollisionMap, kRocks[nRockType], nCandidateRockX, nRockY, nHead))
            {
                nRockX = nCandidateRockX;
            }

            if (AOC_FALSE == testCollision(kCollisionMap, kRocks[nRockType], nRockX, nCandidateRockY, nHead))
            {
                nRockY = nCandidateRockY;
            }
            else
            {
                const size_t  nCollisionPoint        = (nHead + COLLISION_HEIGHT + nRockY) % COLLISION_HEIGHT;
                unsigned char nDeltaShift            = 0u;
                size_t        nNewMinimumHeightDelta = nMinimumDeltaHeight;

                /* 1. Update the Collision Map.
                 * 2. Update the peaks of each Row.
                 * 3. Calculate how far to shift the Head of the circular buffer
                 *    in order to have the next rock item start 4 rows above the
                 *    new peak.
                 *
                 *    Note: This is also applicable to Tower Height.
                 */
                for (Y = 0; Y < ROCK_HEIGHT; ++Y)
                {
                    /* Calculate the Collision Point in the Circular Buffer */
                    const size_t nCollisionPointY = (nCollisionPoint + Y + COLLISION_HEIGHT - ROCK_HEIGHT + 1) % COLLISION_HEIGHT;

                    /* Update the Collision Map for the current row */
                    for (X = 0; X < ROCK_WIDTH; ++X)
                    {
                        if (kRocks[nRockType][Y][X])
                        {
                            const unsigned char nRowDelta = nRockY - (ROCK_HEIGHT - Y - 1);

                            assert(((int)nRockY - ((int)ROCK_HEIGHT - (int)Y - 1)) >= 0);
                            assert(AOC_FALSE == kCollisionMap[nCollisionPointY][nRockX + X]);

                            kCollisionMap[nCollisionPointY][nRockX + X] = AOC_TRUE;

                            /* Using the previous peaks, determine if we've acheived:
                             * 1. A new peak for the current column.
                             * 2. And if so... does this peak now mean all peaks need to
                             *    be shifted to ensure the 4 row gap.
                             */
                            if (nRowDelta < kDeltaHeights[nRockX + X - 1])
                            {
                                /* Determine if all Peaks / Tower Height need adjusting... */
                                if (nRowDelta < nMinimumDeltaHeight)
                                {
                                    const unsigned char nCandidateDeltaShift = nMinimumDeltaHeight - nRowDelta;
                                    if (nCandidateDeltaShift > nDeltaShift)
                                    {
                                        nDeltaShift = nCandidateDeltaShift;
                                    }
                                    nNewMinimumHeightDelta = nRowDelta;
                                }

                                /* Update the Current Peak */
                                kDeltaHeights[nRockX + X - 1] = (unsigned char)nRowDelta;
                            }
                        }
                    }
                }

                /* If we achieved a new highest peak... */
                if (nDeltaShift > 0)
                {
                    size_t nBufferedHead = nHead;
                    size_t nNewHeadPlusRock;

                    /* Adjust the Tower Height */
                    nTowerHeight    += nDeltaShift;

                    /* Adjust the Head of the Circular Buffer */
                    nHead            = (nHead + COLLISION_HEIGHT - nDeltaShift) % COLLISION_HEIGHT;
                    nNewHeadPlusRock = (nHead + COLLISION_HEIGHT - (ROCK_HEIGHT - 1)) % COLLISION_HEIGHT;

                    /* From the row above the old head, to the current head, clear any/all
                     * obstacles to ensure that part of the collision map is always clear.
                     */
                    while (nBufferedHead != nNewHeadPlusRock)
                    {
                        nBufferedHead = (nBufferedHead + COLLISION_HEIGHT - 1) % COLLISION_HEIGHT;
                        for (X = 0; X < COLLISION_WIDTH; ++X)
                        {
                            kCollisionMap[nBufferedHead][X + 1] = AOC_FALSE;
                        }
                    }

                    /* Adjust all the Peaks to accomodate the new Tower Height */
                    for (X = 0; X < COLLISION_WIDTH; ++X)
                    {
                        kDeltaHeights[X] += (unsigned char)nDeltaShift;
                    }
                    nNewMinimumHeightDelta += nDeltaShift;
                }

                /* Update the Tower Height History */
                addToTowerHeightHistory(&kTowerHeightHistory, &nTowerHeightHistorySize, &nTowerHeightHistoryMaxSize, nDeltaShift);

                /* Start trying to detect the loop.
                 *
                 * Note: Because the peaks start in something of a "fake" state to enforce
                 *       a floor, we wait until the circular buffer is saturated, thereby
                 *       removing any possibility of the floor from being considered as
                 *       one of the valid states.
                 */
                if (nTowerHeight > 256u)
                {
                    TCollisionHistoryType*  kHistory = (TCollisionHistoryType*)malloc(sizeof(TCollisionHistoryType));
                    uint64_t*               pkEntry;
                    memcpy(kHistory->kPeaks, kDeltaHeights, sizeof(kDeltaHeights));
                    kHistory->nJetStreamPosition = nJetStreamPositionAtStart;
                    kHistory->nRock = nRockType;

                    pkEntry = hashmap_get(&kTowerStateHashMap, kHistory);
                    if (NULL == pkEntry)
                    {
                        uint64_t* pkCount  = (uint64_t*)malloc(sizeof(uint64_t));
                        *pkCount = nCurrentRock;

                        hashmap_put(&kTowerStateHashMap, kHistory, pkCount);
                    }
                    else
                    {
                        nLoopOffset   = *pkEntry;
                        nLoopInterval = nCurrentRock - nLoopOffset;
                        bLoopFound    = AOC_TRUE;
                    }
                }

                break;
            }

            /* Increment the Jet Stream Position */
            nJetStreamPosition = (nJetStreamPosition + 1) % nJetStreamSize;
        }

        /* Exit Early if we've found a loop... */
        if (bLoopFound)
        {
            break;
        }

        /* Increment the Jet Stream Position */
        nJetStreamPosition = (nJetStreamPosition + 1) % nJetStreamSize;

        ++nCurrentRock;
    }

    /* Cleanup the Tower States Hashmap */
    hashmap_foreach_data(pnTowerStateHashData, &kTowerStateHashMap)
    {
        free(pnTowerStateHashData);
    }
    hashmap_cleanup(&kTowerStateHashMap);

    if (AOC_FALSE == bLoopFound)
    {
        free(kTowerHeightHistory);
        return nTowerHeight;
    }
    else
    {
        const uint64_t nLoopCount     = (nTotalFallingRocks - nLoopOffset) / nLoopInterval;
        const uint64_t nLoopRemainder = ((nTotalFallingRocks - nLoopOffset) % nLoopInterval);

        uint64_t i;
        uint64_t nIntervalHeight = 0u;

        /* Start with the Initial Height */
        nTowerHeight = 0u;
        for (i = 0; i < nLoopOffset; ++i)
        {
            nTowerHeight += kTowerHeightHistory[i];
        }

        /* Now calculate the Interval Sum and add on the Remainder */
        for (i = 0u; i < nLoopInterval; ++i)
        {
            nIntervalHeight += kTowerHeightHistory[i + nLoopOffset];
            if (i < nLoopRemainder)
            {
                nTowerHeight += kTowerHeightHistory[i + nLoopOffset];
            }
        }

        /* Finally add all the Intervals */
        nTowerHeight += nLoopCount * nIntervalHeight;

        free(kTowerHeightHistory);
        return nTowerHeight;
    }
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {

        size_t          nReadCount;

        char*           kDirections;
        size_t          nDirectionCount;

        size_t          i;

        /* Obtain the Number of Directions in the Input Data */
        fseek(pFile, 0, SEEK_END);
        nDirectionCount = (size_t)ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        /* Allocate the Memory for the Directions */
        kDirections = (char*)malloc(nDirectionCount * sizeof(char));
        assert(kDirections);

        /* Read the Directions */
        nReadCount = fread(kDirections, sizeof(char), nDirectionCount, pFile);
        assert(nReadCount == nDirectionCount);
        fclose(pFile);

        /* Convert the Directions into something more manageable */
        for (i = 0; i < nDirectionCount; ++i)
        {
            if ('<' == kDirections[i])
            {
                kDirections[i] = -1;
            }
            else if ('>' == kDirections[i])
            {
                kDirections[i] = 1;
            }
            else
            {
                assert(0);
            }
        }

        printf("Part 1: %llu\n", getTowerHeight(2022llu,          kDirections, nDirectionCount));
        printf("Part 2: %llu\n", getTowerHeight(1000000000000llu, kDirections, nDirectionCount));

        /* Clean Up */
        free(kDirections);
    }

    return 0;
}
