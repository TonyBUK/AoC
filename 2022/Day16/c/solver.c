#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <memory.h>
#include <assert.h>

#define MAX_LINKS (64)

typedef struct RawValveType
{
    uint16_t  kValveID;
    uint16_t  nLinkCount;
    uint16_t* kLinks;
    uint64_t  nFlowRate;

    uint64_t  nBitmaskID;
} TRawValveType;

typedef struct ValveType
{
    uint16_t kValveID;

    uint32_t nID;
    uint64_t nBitmaskID;

    uint64_t nFlowRate;
    size_t   nRouteLengths[MAX_LINKS];
} TValveType;

uint64_t getValue(FILE** pFile)
{
    uint64_t       nValue = 0;
    unsigned char kData;

    while (isdigit(kData = fgetc(*pFile)))
    {
        if (!feof(*pFile))
        {
            nValue = (nValue * 10lu) + (uint64_t)(kData - (uint64_t)'0');
        }
        else
        {
            break;
        }
    }

    return nValue;
}

size_t routeLength(const uint16_t nCurrentValveId, const uint16_t nEndValveId, const uint64_t nVisitedMask, const size_t nTime, const size_t nBestTime, const TRawValveType* kValves, const size_t* kValveIndexLookup, const uint64_t* kValveBitmaskLoookup)
{
    const size_t   nCurrentIndex   = kValveIndexLookup[nCurrentValveId];
    const uint64_t nCurrentBitMask = kValveBitmaskLoookup[nCurrentValveId];
    size_t         nLocalBestTime  = nBestTime;

    size_t i;

    if (nCurrentValveId == nEndValveId)
    {
        return nTime;
    }
    else if (nTime > nBestTime)
    {
        return nBestTime;
    }

    for (i = 0; i < kValves[nCurrentIndex].nLinkCount; ++i)
    {
        const uint64_t nCandidateBitMask = kValveBitmaskLoookup[kValves[nCurrentIndex].kLinks[i]];
        size_t nCandidateTime;

        if (0 == (nCandidateBitMask & nVisitedMask))
        {
            nCandidateTime = routeLength(kValves[nCurrentIndex].kLinks[i],
                                         nEndValveId,
                                         nVisitedMask | nCandidateBitMask,
                                         nTime + 1, nLocalBestTime,
                                         kValves,
                                         kValveIndexLookup, kValveBitmaskLoookup);
            if (nCandidateTime < nLocalBestTime)
            {
                nLocalBestTime = nCandidateTime;
            }
        }
    }

    return nLocalBestTime;
}

uint64_t bestFlowRate(const size_t nValveIndex, const TValveType* kValves, const size_t nNumValves, const uint64_t nCurrentPressure, const uint64_t nBestPressure, const uint64_t nVisitedMask, const size_t nRemainingTime, uint64_t* kValveBestPressure, uint64_t* kPermutations, size_t* pnPermutationCount)
{
    uint64_t nLocalBestPressure = nBestPressure;
    size_t   i;

    if (nCurrentPressure < kValveBestPressure[nVisitedMask])
    {
        return nLocalBestPressure;
    }
    else if (nCurrentPressure > nLocalBestPressure)
    {
        nLocalBestPressure = nCurrentPressure;
    }

    if (0 == kValveBestPressure[nVisitedMask])
    {
        if (nCurrentPressure > 0)
        {
            if (kPermutations)
            {
                if (pnPermutationCount)
                {
                    kPermutations[*pnPermutationCount] = nVisitedMask;
                    *pnPermutationCount += 1;
                }
            }
        }
    }

    kValveBestPressure[nVisitedMask] = nCurrentPressure;

    if (0 == nRemainingTime)
    {
        return nLocalBestPressure;
    }

    for (i = 0; i < nNumValves; ++i)
    {
        if (0 == (kValves[i].nBitmaskID & nVisitedMask))
        {
            const long nNewTime = (long)nRemainingTime - kValves[nValveIndex].nRouteLengths[i] - 1;
            if (nNewTime > 0)
            {
                const uint64_t nCandidatePressure = bestFlowRate(i,
                                                                 kValves, nNumValves,
                                                                 nCurrentPressure + (kValves[i].nFlowRate * nNewTime),
                                                                 nLocalBestPressure,
                                                                 nVisitedMask | kValves[i].nBitmaskID,
                                                                 (size_t)nNewTime,
                                                                 kValveBestPressure,
                                                                 kPermutations, pnPermutationCount);
                if (nCandidatePressure > nLocalBestPressure)
                {
                    nLocalBestPressure = nCandidatePressure;
                }
            }
        }
    }

    return nLocalBestPressure;
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const uint16_t  nStartValveId          = (((uint16_t)'A') << 8) | ((uint16_t)'A');

        long            nFileSize;
        size_t          nValveCountMax;

        size_t          nImportantValveCount    = 0u;
        size_t          nTotalValveCount        = 0u;
        size_t          nStartValveIndex        = 0u;
        uint64_t        nStartValveBitMask      = 0u;
        TValveType      kImportantValves[MAX_LINKS];

        size_t          nAllValveCount          = 0u;
        TRawValveType*  kAllValves;

        size_t          kValveIndexLookup[65536];
        uint64_t        kValveBitmaskLoookup[65536];
        uint64_t        kPermutations[65536];
        size_t          nPermutationCount       = 0u;

        uint64_t*       kValveBestPressure;
        uint64_t        nCombinedBestPressure   = 0u;

        size_t          i;

        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        nValveCountMax = (nFileSize+1) / 52;

        /* Allocate the Raw Valves */
        kAllValves          = (TRawValveType*)malloc(nValveCountMax * sizeof(TRawValveType));
        for (i = 0; i < nValveCountMax; ++i)
        {
            kAllValves[i].nLinkCount    = 0;
            kAllValves[i].kLinks        = (uint16_t*)malloc(nValveCountMax * sizeof(uint16_t));
        }

        /* Extract the Valves */
        while(!feof(pFile))
        {
            unsigned char kData;

            fseek(pFile, 6, SEEK_CUR);
            kAllValves[nAllValveCount].kValveID = (((uint16_t)fgetc(pFile)) << 8) |
                                                  ((uint16_t)fgetc(pFile));

            kValveIndexLookup[kAllValves[nAllValveCount].kValveID] = nAllValveCount;
            fseek(pFile, 15, SEEK_CUR);

            kAllValves[nAllValveCount].nFlowRate = getValue(&pFile);
            fseek(pFile, 22, SEEK_CUR);

            /* tunnel / tunnels..... */
            while (' ' != fgetc(pFile));

            while ('\n' != (kData = fgetc(pFile)))
            {
                if (feof(pFile)) break;

                if (',' == kData)
                {
                    fgetc(pFile);
                    continue;
                }

                kAllValves[nAllValveCount].kLinks[kAllValves[nAllValveCount].nLinkCount]
                    = (((uint16_t)kData) << 8) |
                      ((uint16_t)fgetc(pFile));

                ++kAllValves[nAllValveCount].nLinkCount;
            }

            ++nAllValveCount;
        }

        fclose(pFile);

        /* Order the (Important) Valves */
        for (i = 0; i < nAllValveCount; ++i)
        {
            if ((kAllValves[i].nFlowRate > 0) || (nStartValveId == kAllValves[i].kValveID))
            {
                assert(nTotalValveCount < MAX_LINKS);
                kImportantValves[nTotalValveCount].kValveID     = kAllValves[i].kValveID;
                kImportantValves[nTotalValveCount].nID          = nTotalValveCount;
                kImportantValves[nTotalValveCount].nBitmaskID   = 1llu << (uint64_t)nTotalValveCount;
                kImportantValves[nTotalValveCount].nFlowRate    = kAllValves[i].nFlowRate;
                kAllValves[i].nBitmaskID                        = kImportantValves[nTotalValveCount].nBitmaskID;
                kValveBitmaskLoookup[kAllValves[i].kValveID]    = kImportantValves[nTotalValveCount].nBitmaskID;

                if (nStartValveId == kAllValves[i].kValveID)
                {
                    nStartValveIndex   = nTotalValveCount;
                    nStartValveBitMask = kImportantValves[nTotalValveCount].nBitmaskID;
                }

                ++nTotalValveCount;
            }
        }
        nImportantValveCount = nTotalValveCount;

        assert(nImportantValveCount <= nAllValveCount);
        assert(nStartValveIndex != 0u);

        /* Allocate Memory for the Best Possible Pressures */
        kValveBestPressure = (uint64_t*)malloc((1llu << (uint64_t)nImportantValveCount) * sizeof(uint64_t));
        assert(kValveBestPressure);

        /* Order the (Unimportant) Valves */
        for (i = 0; i < nAllValveCount; ++i)
        {
            if ((0u == kAllValves[i].nFlowRate) && (nStartValveId != kAllValves[i].kValveID))
            {
                assert(nTotalValveCount < MAX_LINKS);
                kImportantValves[nTotalValveCount].kValveID     = kAllValves[i].kValveID;
                kImportantValves[nTotalValveCount].nID          = nTotalValveCount;
                kImportantValves[nTotalValveCount].nBitmaskID   = 1llu << (uint64_t)nTotalValveCount;
                kAllValves[i].nBitmaskID                        = kImportantValves[nTotalValveCount].nBitmaskID;
                kValveBitmaskLoookup[kAllValves[i].kValveID]    = kImportantValves[nTotalValveCount].nBitmaskID;

                ++nTotalValveCount;
            }
        }

        /* Common: Calculate how to get from each route to each route */
        assert(nTotalValveCount == nAllValveCount);
        for (i = 0; i < nImportantValveCount; ++i)
        {
            size_t j;

            for (j = 0; j < nImportantValveCount; ++j)
            {
                if (j == i)
                {
                    kImportantValves[i].nRouteLengths[j] = 0;
                }
                else
                {
                    kImportantValves[i].nRouteLengths[j] = 
                        routeLength(kImportantValves[i].kValveID, kImportantValves[j].kValveID, kImportantValves[i].nBitmaskID, 0, (size_t)-1, kAllValves, kValveIndexLookup, kValveBitmaskLoookup);
                }
            }
        }

        /* Part 1: Calculate the Best Possible Route */
        memset(kValveBestPressure, 0, (1llu << (uint64_t)nImportantValveCount) * sizeof(uint64_t));
        printf("Part 1: %llu\n", bestFlowRate(nStartValveIndex,
                                              kImportantValves, nImportantValveCount,
                                              0, 0,
                                              kImportantValves[nStartValveIndex].nBitmaskID,
                                              30,
                                              kValveBestPressure,
                                              NULL, NULL));

        /* Part 2, we actually just want to combine permutations for Part 1 using a different end time */
        memset(kValveBestPressure, 0, (1llu << (uint64_t)nImportantValveCount) * sizeof(uint64_t));
        bestFlowRate(nStartValveIndex,
                     kImportantValves, nImportantValveCount,
                     0, 0,
                     kImportantValves[nStartValveIndex].nBitmaskID,
                     26,
                     kValveBestPressure,
                     kPermutations,
                     &nPermutationCount);

        /* Now we just want to combine permutations ensursuring there is no overlap as us/Elephants
         * work independently.
         */
        for (i = 0; i < nPermutationCount; ++i)
        {
            size_t j;
            for (j = i + 1; j < nPermutationCount; ++j)
            {
                /* Except we all go through the Start Index */
                if (nStartValveBitMask == (kPermutations[i] & kPermutations[j]))
                {
                    /* Check to see if this permutation represents an improvement... */
                    const uint64_t nCandidatePressure = kValveBestPressure[kPermutations[i]] + kValveBestPressure[kPermutations[j]];
                    if (nCandidatePressure > nCombinedBestPressure)
                    {
                        nCombinedBestPressure = nCandidatePressure;
                    }
                }
            }
        }
        printf("Part 2: %llu\n", nCombinedBestPressure);

        for (i = 0; i < nValveCountMax; ++i)
        {
            free(kAllValves[i].kLinks);
        }
        free(kAllValves);
        free(kValveBestPressure);
    }

    return 0;
}