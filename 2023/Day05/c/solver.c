#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

#ifndef MAX
    #define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ifndef MIN
    #define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

#define EXPANSION (16)

typedef struct tMapping
{
    int64_t  nSourceStart;
    int64_t  nDestinationStart;
    int64_t  nLength;
    unsigned bUsed;
} tMapping;

typedef struct tSeed
{
    int64_t  nSeedStart;
    int64_t  nLength;
    unsigned bUsed;
} tSeed;

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength)
{
    const unsigned long     nStartPos   = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    char*                   pLine;
    size_t                  nLineCount  = 0;
 
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
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

void AppendToDataSet(tSeed** kSeeds, size_t* pMaxSeedCount, const tSeed* pSeed)
{
    const size_t nSeedCount = *pMaxSeedCount;
    size_t       nSeed;
    tSeed*       kNewSeeds;

    /* Try to fill in any gaps... */
    for (nSeed = 0; nSeed < nSeedCount; ++nSeed)
    {
        if (AOC_FALSE == (*kSeeds)[nSeed].bUsed)
        {
            (*kSeeds)[nSeed].nSeedStart = pSeed->nSeedStart;
            (*kSeeds)[nSeed].nLength    = pSeed->nLength;
            (*kSeeds)[nSeed].bUsed      = AOC_TRUE;
            return;
        }
    }

    /* Otherwise, double the buffer, copy, and append */
    kNewSeeds       = (tSeed*)realloc(*kSeeds, *pMaxSeedCount * EXPANSION * sizeof(tSeed));
    assert(kNewSeeds);
    *kSeeds         = kNewSeeds;
    *pMaxSeedCount *= EXPANSION;

    memset(&kNewSeeds[nSeedCount], 0, (*pMaxSeedCount-nSeedCount) * sizeof(tSeed));

    AppendToDataSet(kSeeds, pMaxSeedCount, pSeed);
}

unsigned findIntersection(const int64_t nSeedStart, const int64_t nSeedLength, const int64_t nRangeStart, const int64_t nRangeLength, int64_t* pIntersectionStart, int64_t* pIntersectionEnd)
{
    const int64_t nStart = MAX(nSeedStart,               nRangeStart);
    const int64_t nEnd   = MIN(nSeedStart + nSeedLength, nRangeStart + nRangeLength) - 1;

    if (nStart < nEnd)
    {
        *pIntersectionStart = nStart;
        *pIntersectionEnd   = nEnd;
        return AOC_TRUE;
    }

    return AOC_FALSE;
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

        size_t                  nMapCount               = 0;
        size_t                  nMaxMaps                = 0;
        size_t                  nCurrentMap             = 0;
        size_t                  nCurrentSubMap;

        tSeed*                  kSeedsPartOne           = NULL;
        tSeed*                  kSeedsPartTwo           = NULL;
        tSeed*                  kSeedsPartTwoCurrent    = NULL;
        tSeed*                  kSeedsPartTwoUpdated    = NULL;
        tSeed*                  kSeedsPartTwoDecimated  = NULL;
        size_t                  nSeed;
        size_t                  nMaxSeedCount;

        tMapping*               kMappings               = NULL;
        tMapping*               kCurrentMappings;

        char*                   pSeed;
        size_t                  nSeedCountPartOne       = 0;
        size_t                  nSeedCountPartTwo       = 0;

        int64_t                 nLowestLocation         = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL);

        /* Count how many Maps there are.
         *
         * Note: As a bodge, let's just assume it's all in order to prevent hashing or
         *       searches.
         */
        for (nLine = 1; nLine < nLineCount; ++nLine)
        {
            if (strstr(kLines[nLine], "map:"))
            {
                ++nMapCount;
                nCurrentMap = 0;
            }
            else
            {
                ++nCurrentMap;
                if (nCurrentMap > nMaxMaps)
                {
                    nMaxMaps = nCurrentMap;
                }
            }
        }

        /* Allocate our Seeds (overegg it) */
        nMaxSeedCount          = strlen(kLines[0]);
        kSeedsPartOne          = (tSeed*)malloc(nMaxSeedCount * sizeof(tSeed));
        kSeedsPartTwo          = (tSeed*)malloc(nMaxSeedCount * sizeof(tSeed));
        kSeedsPartTwoCurrent   = (tSeed*)malloc(nMaxSeedCount * sizeof(tSeed));
        kSeedsPartTwoUpdated   = (tSeed*)malloc(nMaxSeedCount * sizeof(tSeed));
        kSeedsPartTwoDecimated = (tSeed*)malloc(nMaxSeedCount * sizeof(tSeed));

        /* Allocate our Mappings */
        kMappings = (tMapping*)calloc(nMapCount * nMaxMaps, sizeof(tMapping));

        /* Parse the Seeds */
        pSeed = kLines[0];
        do
        {
            pSeed = strstr(pSeed, " ");
            if (pSeed)
            {
                ++pSeed;
                kSeedsPartOne[nSeedCountPartOne].nSeedStart = strtoull(pSeed, NULL, 10);
                kSeedsPartOne[nSeedCountPartOne].nLength    = 1;
                kSeedsPartOne[nSeedCountPartOne].bUsed      = AOC_TRUE;

                if ((nSeedCountPartOne % 2) == 0)
                {
                    kSeedsPartTwo[nSeedCountPartTwo].nSeedStart = kSeedsPartOne[nSeedCountPartOne].nSeedStart;
                }
                else
                {
                    kSeedsPartTwo[nSeedCountPartTwo].nLength = kSeedsPartOne[nSeedCountPartOne].nSeedStart;
                    kSeedsPartTwo[nSeedCountPartTwo].bUsed   = AOC_TRUE;
                    ++nSeedCountPartTwo;
                }

                ++nSeedCountPartOne;
            }
        } while (pSeed);

        assert((nSeedCountPartOne % 2) == 0);

        /* Parse the Maps */
        nCurrentMap    = 0;
        nCurrentSubMap = 0;

        kCurrentMappings = &kMappings[nCurrentMap * nMaxMaps];

        for (nLine = 2; nLine < nLineCount; ++nLine)
        {
            if (strstr(kLines[nLine], "map:"))
            {
                ++nCurrentMap;
                nCurrentSubMap = 0;

                kCurrentMappings = &kMappings[nCurrentMap * nMaxMaps];
            }
            else
            {
                /* Destination */
                char* pMapRange = kLines[nLine];
                kCurrentMappings[nCurrentSubMap].nDestinationStart = strtoull(pMapRange, NULL, 10);

                /* Source */
                pMapRange = strstr(pMapRange, " ");
                assert(pMapRange);
                ++pMapRange;
                kCurrentMappings[nCurrentSubMap].nSourceStart      = strtoull(pMapRange, NULL, 10);

                /* Length */
                pMapRange = strstr(pMapRange, " ");
                assert(pMapRange);
                ++pMapRange;
                kCurrentMappings[nCurrentSubMap].nLength           = strtoull(pMapRange, NULL, 10);

                /* Used */
                kCurrentMappings[nCurrentSubMap].bUsed             = AOC_TRUE;

                ++nCurrentSubMap;
            }
        }

        /* Part 1 - No Length, only seeds... */
        /* Note: We can be destructive here... */
        for (nCurrentMap = 0; nCurrentMap < nMapCount; ++nCurrentMap)
        {
            const tMapping* pCurrentMappings = &kMappings[nCurrentMap * nMaxMaps];

            for (nSeed = 0; nSeed < nSeedCountPartOne; ++nSeed)
            {
                for (nCurrentSubMap = 0; nCurrentSubMap < nMaxMaps; ++nCurrentSubMap)
                {
                    if (AOC_FALSE == pCurrentMappings[nCurrentSubMap].bUsed)
                    {
                        break;
                    }

                    if ((kSeedsPartOne[nSeed].nSeedStart >= pCurrentMappings[nCurrentSubMap].nSourceStart) &&
                        (kSeedsPartOne[nSeed].nSeedStart <= (pCurrentMappings[nCurrentSubMap].nSourceStart + pCurrentMappings[nCurrentSubMap].nLength - 1)))
                    {
                        kSeedsPartOne[nSeed].nSeedStart = kSeedsPartOne[nSeed].nSeedStart -
                                                          pCurrentMappings[nCurrentSubMap].nSourceStart +
                                                          pCurrentMappings[nCurrentSubMap].nDestinationStart;
                        break;
                    }
                }
            }
        }

        /* Print the Lowest Location */
        nLowestLocation = kSeedsPartOne[0].nSeedStart;
        for (nSeed = 1; nSeed < nSeedCountPartOne; ++nSeed)
        {
            if (kSeedsPartOne[nSeed].nSeedStart < nLowestLocation)
            {
                nLowestLocation = kSeedsPartOne[nSeed].nSeedStart;
            }
        }

        printf("Part 1: %lld\n", nLowestLocation);

        /* Part 2 - Seeds are now Ranged */

        /* This finally needs *proper* dynamic memory allocation */
        // nMaxSeedCount
        for (nSeed = 0; nSeed < nSeedCountPartTwo; ++nSeed)
        {
            size_t nSeedCount    = 1;
            size_t i;

            memset(kSeedsPartTwoCurrent,   0, nMaxSeedCount * sizeof(tSeed));
            memset(kSeedsPartTwoUpdated,   0, nMaxSeedCount * sizeof(tSeed));

            kSeedsPartTwoCurrent[0].nSeedStart = kSeedsPartTwo[nSeed].nSeedStart;
            kSeedsPartTwoCurrent[0].nLength    = kSeedsPartTwo[nSeed].nLength;
            kSeedsPartTwoCurrent[0].bUsed      = AOC_TRUE;
            assert(kSeedsPartTwoCurrent[0].bUsed);

            for (nCurrentMap = 0; nCurrentMap < nMapCount; ++nCurrentMap)
            {
                const tMapping* pCurrentMappings = &kMappings[nCurrentMap * nMaxMaps];
                size_t nCurrentItem;

                /* Copy the Decimation Buffer */
                memset(kSeedsPartTwoDecimated, 0, nMaxSeedCount * sizeof(tSeed));
                memset(kSeedsPartTwoUpdated,   0, nMaxSeedCount * sizeof(tSeed));
                memcpy(kSeedsPartTwoDecimated, kSeedsPartTwoCurrent, nSeedCount * sizeof(tSeed));

                for (nCurrentSubMap = 0; nCurrentSubMap < nMaxMaps; ++nCurrentSubMap)
                {
                    if (AOC_FALSE == pCurrentMappings[nCurrentSubMap].bUsed)
                    {
                        break;
                    }

                    for (nCurrentItem = 0; nCurrentItem < nSeedCount; ++nCurrentItem)
                    {
                        int64_t nIntersectionStart;
                        int64_t nIntersectionEnd;
                        if (AOC_TRUE == findIntersection(kSeedsPartTwoCurrent[nCurrentItem].nSeedStart,
                                                         kSeedsPartTwoCurrent[nCurrentItem].nLength,
                                                         pCurrentMappings[nCurrentSubMap].nSourceStart,
                                                         pCurrentMappings[nCurrentSubMap].nLength,
                                                         &nIntersectionStart, &nIntersectionEnd))
                        {
                            const tSeed kSeed =
                            {
                                nIntersectionStart - pCurrentMappings[nCurrentSubMap].nSourceStart + pCurrentMappings[nCurrentSubMap].nDestinationStart,
                                nIntersectionEnd   - nIntersectionStart + 1,
                                AOC_TRUE
                            };
                            const size_t nMaxSeedCountBuffered = nMaxSeedCount;

                            AppendToDataSet(&kSeedsPartTwoUpdated, &nMaxSeedCount, &kSeed);

                            if (nMaxSeedCountBuffered != nMaxSeedCount)
                            {
                                tSeed* kNewSeedBuffer;

                                kNewSeedBuffer = (tSeed*)realloc(kSeedsPartTwoCurrent, nMaxSeedCount * sizeof(tSeed));
                                assert(kNewSeedBuffer);
                                kSeedsPartTwoCurrent = kNewSeedBuffer;

                                kNewSeedBuffer = (tSeed*)realloc(kSeedsPartTwoDecimated, nMaxSeedCount * sizeof(tSeed));
                                assert(kNewSeedBuffer);
                                kSeedsPartTwoDecimated = kNewSeedBuffer;
                            }
                        }
                    }

                    /* Decimate the Seeds */
                    for (nCurrentItem = 0; nCurrentItem < nMaxSeedCount; ++nCurrentItem)
                    {
                        int64_t nIntersectionStart;
                        int64_t nIntersectionEnd;

                        if (AOC_FALSE == kSeedsPartTwoDecimated[nCurrentItem].bUsed) continue;

                        if (AOC_TRUE == findIntersection(kSeedsPartTwoDecimated[nCurrentItem].nSeedStart,
                                                         kSeedsPartTwoDecimated[nCurrentItem].nLength,
                                                         pCurrentMappings[nCurrentSubMap].nSourceStart,
                                                         pCurrentMappings[nCurrentSubMap].nLength,
                                                         &nIntersectionStart, &nIntersectionEnd))
                        {
                            const size_t nMaxSeedCountBuffered = nMaxSeedCount;

                            /* Remove the Current Seed */
                            kSeedsPartTwoDecimated[nCurrentItem].bUsed = AOC_FALSE;

                            if (nIntersectionStart > kSeedsPartTwoDecimated[nCurrentItem].nSeedStart)
                            {
                                const tSeed kSeed =
                                {
                                    kSeedsPartTwoDecimated[nCurrentItem].nSeedStart,
                                    nIntersectionStart - kSeedsPartTwoDecimated[nCurrentItem].nSeedStart,
                                    AOC_TRUE
                                };

                                AppendToDataSet(&kSeedsPartTwoDecimated, &nMaxSeedCount, &kSeed);
                            }

                            if (nIntersectionEnd < (kSeedsPartTwoDecimated[nCurrentItem].nSeedStart + kSeedsPartTwoDecimated[nCurrentItem].nLength - 1))
                            {
                                const tSeed kSeed =
                                {
                                    nIntersectionEnd + 1,
                                    kSeedsPartTwoDecimated[nCurrentItem].nSeedStart + kSeedsPartTwoDecimated[nCurrentItem].nLength - nIntersectionEnd - 1,
                                    AOC_TRUE
                                };

                                AppendToDataSet(&kSeedsPartTwoDecimated, &nMaxSeedCount, &kSeed);
                            }

                            if (nMaxSeedCount != nMaxSeedCountBuffered)
                            {
                                tSeed* kNewSeedBuffer;

                                kNewSeedBuffer = (tSeed*)realloc(kSeedsPartTwoCurrent, nMaxSeedCount * sizeof(tSeed));
                                assert(kNewSeedBuffer);
                                kSeedsPartTwoCurrent = kNewSeedBuffer;

                                kNewSeedBuffer = (tSeed*)realloc(kSeedsPartTwoUpdated, nMaxSeedCount * sizeof(tSeed));
                                assert(kNewSeedBuffer);
                                kSeedsPartTwoUpdated = kNewSeedBuffer;
                            }
                        }
                    }
                }

                /* Copy the Updated/Decimated Buffers Buffer */
                nSeedCount = 0;
                memset(kSeedsPartTwoCurrent, 0, nMaxSeedCount * sizeof(tSeed));
                for (nCurrentItem = 0; nCurrentItem < nMaxSeedCount; ++nCurrentItem)
                {
                    if (AOC_TRUE == kSeedsPartTwoDecimated[nCurrentItem].bUsed)
                    {
                        kSeedsPartTwoCurrent[nSeedCount] = kSeedsPartTwoDecimated[nCurrentItem];
                        ++nSeedCount;
                        assert(nSeedCount <= nMaxSeedCount);
                    }

                    if (AOC_TRUE == kSeedsPartTwoUpdated[nCurrentItem].bUsed)
                    {
                        kSeedsPartTwoCurrent[nSeedCount] = kSeedsPartTwoUpdated[nCurrentItem];
                        ++nSeedCount;
                        assert(nSeedCount <= nMaxSeedCount);
                    }
                }
            }

            /* Find the Lowest Location encountered so far */
            if (0 == nSeed)
            {
                nLowestLocation = kSeedsPartTwoCurrent[0].nSeedStart;
            }

            for (i = 0; i < nSeedCount; ++i)
            {
                if (AOC_FALSE == kSeedsPartTwoCurrent[i].bUsed) continue;

                if (kSeedsPartTwoCurrent[i].nSeedStart < nLowestLocation)
                {
                    nLowestLocation = kSeedsPartTwoCurrent[i].nSeedStart;
                }
            }
        }
        printf("Part 2: %lld\n", nLowestLocation);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kSeedsPartOne);
        free(kSeedsPartTwo);
        free(kSeedsPartTwoCurrent);
        free(kSeedsPartTwoUpdated);
        free(kSeedsPartTwoDecimated);
        free(kMappings);
    }
 
    return 0;
}