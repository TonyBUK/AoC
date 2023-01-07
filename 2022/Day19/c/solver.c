#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define AOC_TRUE    (1u)
#define AOC_FALSE   (0u)

#define AOC_MAX(a,b) (((a) > (b)) ? (a) : (b))

#define QUEUE_SIZE  (1024u)
#define APPEND_TO_QUEUE(Q, QT, QS, nNewTime, nNewOreCount, nNewOreBotCount, nNewClayCount, nNewClayBotCount, nNewObsidianCount, nNewObsidianBotCount, nNewGeodeCount, nNewGeodeBotCount)    \
    Q[QT].nTime             = (nNewTime);               \
    Q[QT].nOreCount         = (nNewOreCount);           \
    Q[QT].nOreBotCount      = (nNewOreBotCount);        \
    Q[QT].nClayCount        = (nNewClayCount);          \
    Q[QT].nClayBotCount     = (nNewClayBotCount);       \
    Q[QT].nObsidianCount    = (nNewObsidianCount);      \
    Q[QT].nObsidianBotCount = (nNewObsidianBotCount);   \
    Q[QT].nGeodeCount       = (nNewGeodeCount);         \
    Q[QT].nGeodeBotCount    = (nNewGeodeBotCount);      \
    ++QT;                                               \
    assert(QT < QS);

#define getTriangleNumber(x) (((x)*((x)+1))/2)

typedef struct BlueprintType
{
    size_t  nOreBotOreCost;
    size_t  nClayBotOreCost;
    size_t  nObsidianBotOreCost;
    size_t  nObsidianBotClayCost;
    size_t  nGeodeBotOreCost;
    size_t  nGeodeBotObsidianCost;
} TBlueprintType;

typedef struct QueueType
{
    size_t  nTime;
    size_t  nOreCount;
    size_t  nOreBotCount;
    size_t  nClayCount;
    size_t  nClayBotCount;
    size_t  nObsidianCount;
    size_t  nObsidianBotCount;
    size_t  nGeodeCount;
    size_t  nGeodeBotCount;
} TQueueType;

size_t getValue(FILE** pFile)
{
    size_t          nValue = 0;
    unsigned char   kData;

    while (isdigit(kData = fgetc(*pFile)))
    {
        nValue = (nValue * 10) + (kData - '0');
    }

    return nValue;
}

size_t calculateMaxGeodes(const TBlueprintType* kBlueprint, const size_t nMaxTime)
{
    const size_t    nMaxOreCost     = AOC_MAX(kBlueprint->nOreBotOreCost,
                                      AOC_MAX(kBlueprint->nClayBotOreCost,
                                      AOC_MAX(kBlueprint->nObsidianBotOreCost,
                                              kBlueprint->nGeodeBotOreCost)));
    size_t          nMaxGeodeCount  = 0u;

    TQueueType      kQueue[QUEUE_SIZE];
    size_t          nQueueSize      = 1u;

    /* Add the First Queue Entry... */
    kQueue[0].nTime                 = 0u;
    kQueue[0].nOreCount             = 0u;
    kQueue[0].nOreBotCount          = 1u;
    kQueue[0].nClayCount            = 0u;
    kQueue[0].nClayBotCount         = 0u;
    kQueue[0].nObsidianCount        = 0u;
    kQueue[0].nObsidianBotCount     = 0u;
    kQueue[0].nGeodeCount           = 0u;
    kQueue[0].nGeodeBotCount        = 0u;

    while (nQueueSize > 0u)
    {
        const size_t  nTime             = kQueue[nQueueSize-1].nTime;
        const size_t  nOreCount         = kQueue[nQueueSize-1].nOreCount;
        const size_t  nOreBotCount      = kQueue[nQueueSize-1].nOreBotCount;
        const size_t  nClayCount        = kQueue[nQueueSize-1].nClayCount;
        const size_t  nClayBotCount     = kQueue[nQueueSize-1].nClayBotCount;
        const size_t  nObsidianCount    = kQueue[nQueueSize-1].nObsidianCount;
        const size_t  nObsidianBotCount = kQueue[nQueueSize-1].nObsidianBotCount;
        const size_t  nGeodeCount       = kQueue[nQueueSize-1].nGeodeCount;
        const size_t  nGeodeBotCount    = kQueue[nQueueSize-1].nGeodeBotCount;

        size_t bClay                   = AOC_FALSE;
        size_t bOre                    = AOC_FALSE;

        /* Pop the Entry from the Queue */
        --nQueueSize;

        /* Determine if this is the best outcome */
        if (nGeodeCount > nMaxGeodeCount)
        {
            nMaxGeodeCount = nGeodeCount;
        }

        /* Do nothing else if we've used all of our allotted time. */
        if (nTime == nMaxTime) continue;

        /* Tries to crudely look ahead assuming that if we built a new bot each cycle,
         * could we ever beat the best geode count found so-far.
         */
        if (nMaxGeodeCount > 0u)
        {
            const size_t nBestGeodeCount = nGeodeCount +
                                           ((nMaxTime - nTime) * nGeodeBotCount) +
                                           getTriangleNumber(nMaxTime - nTime);
            if (nBestGeodeCount < nMaxGeodeCount) continue;
        }

        /* The only reason to build this bot is to get more geodes, and the earliest
         * it'll have an impact is in 2 minutes time.
         * Minute 1: Time required to Build Geode Bot
         * Minute 2: Collect extra Geode
         */
        if ((nMaxTime - nTime) >= 2u)
        {
            /* If we can build a Geode Bot this time, do so, it will never not be the best move,
             * so skip all others.
             */
            if ((nOreCount      >= kBlueprint->nGeodeBotOreCost) &&
                (nObsidianCount >= kBlueprint->nGeodeBotObsidianCost))
            {
                APPEND_TO_QUEUE(kQueue, nQueueSize, QUEUE_SIZE,
                                nTime + 1u,
                                nOreCount + nOreBotCount - kBlueprint->nGeodeBotOreCost,
                                nOreBotCount,
                                nClayCount + nClayBotCount,
                                nClayBotCount,
                                nObsidianCount + nObsidianBotCount - kBlueprint->nGeodeBotObsidianCost,
                                nObsidianBotCount,
                                nGeodeCount + nGeodeBotCount,
                                nGeodeBotCount + 1u);
                continue;
            }
        }

        /* The only reason to build this bot is to get more geodes, and the earliest
         * it'll have an impact is in 4 minutes time.
         * Minute 1: Time required to Build Obsidian Bot
         * Minute 2: Collect extra Obsidian
         * Minute 3: Time required to Build Geode Bot
         * Minute 4: Collect extra Geode
         */
        if ((nMaxTime - nTime) >= 4u)
        {
            if ((nOreCount  >= kBlueprint->nObsidianBotOreCost) &&
                (nClayCount >= kBlueprint->nObsidianBotClayCost))
            {
                /* There's no point building more bots than the Geode Bot costs (Obsidian) */

                /* If we can build an Obsidian Bot at this time, it appears to be the best move,
                 * so skip all others.
                 * Note 100% sure of this, but seems to work
                 */
                if (nObsidianBotCount < kBlueprint->nGeodeBotObsidianCost)
                {
                    APPEND_TO_QUEUE(kQueue, nQueueSize, QUEUE_SIZE,
                                    nTime + 1u,
                                    nOreCount + nOreBotCount - kBlueprint->nObsidianBotOreCost,
                                    nOreBotCount,
                                    nClayCount + nClayBotCount - kBlueprint->nObsidianBotClayCost,
                                    nClayBotCount,
                                    nObsidianCount + nObsidianBotCount,
                                    nObsidianBotCount + 1u,
                                    nGeodeCount + nGeodeBotCount,
                                    nGeodeBotCount);
                    continue;
                }
            }
        }

        /* The only reason to build this bot is to get more geodes, and the earliest
         * it'll have an impact is in 6 minutes time.
         * Minute 1: Time required to Build Clay Bot
         * Minute 2: Collect extra Clay
         * Minute 3: Time required to Build Obsidian Bot
         * Minute 4: Collect extra Obsidian
         * Minute 5: Time required to Build Geode Bot
         * Minute 6: Collect extra Geode
         */
        if ((nMaxTime - nTime) >= 6u)
        {
            if (nOreCount  >= kBlueprint->nClayBotOreCost)
            {
                /* There's no point building more bots than the Obsiadian Bot costs (Clay) */
                if (nClayBotCount < kBlueprint->nObsidianBotClayCost)
                {
                    APPEND_TO_QUEUE(kQueue, nQueueSize, QUEUE_SIZE,
                                    nTime + 1u,
                                    nOreCount + nOreBotCount - kBlueprint->nClayBotOreCost,
                                    nOreBotCount,
                                    nClayCount + nClayBotCount,
                                    nClayBotCount + 1u,
                                    nObsidianCount + nObsidianBotCount,
                                    nObsidianBotCount,
                                    nGeodeCount + nGeodeBotCount,
                                    nGeodeBotCount);
                }
                bClay = AOC_TRUE;
            }
        }

        /* The only reason to build this bot is to get more geodes, and the earliest
         * it'll have an impact is in 8 minutes time.
         * Minute 1: Time required to Build Ore Bot
         * Minute 2: Collect extra Ore
         * Minute 3: Time required to Build Clay Bot
         * Minute 4: Collect extra Clay
         * Minute 5: Time required to Build Obsidian Bot
         * Minute 6: Collect extra Obsidian
         * Minute 7: Time required to Build Geode Bot
         * Minute 8: Collect extra Geode
         */
        if ((nMaxTime - nTime) >= 8u)
        {
            if (nOreCount >= kBlueprint->nOreBotOreCost)
            {
                /* There's no point building more bots than the Max of:
                 * - Ore Bot Cost (Ore)
                 * - Clay Bot Cost (Ore)
                 * - Obsdian Bot Cost (Ore)
                 * - Geode Bot Cost (Ore)
                 */
                if (nOreBotCount < nMaxOreCost)
                {
                    APPEND_TO_QUEUE(kQueue, nQueueSize, QUEUE_SIZE,
                                    nTime + 1u,
                                    nOreCount + nOreBotCount - kBlueprint->nOreBotOreCost,
                                    nOreBotCount + 1u,
                                    nClayCount + nClayBotCount,
                                    nClayBotCount,
                                    nObsidianCount + nObsidianBotCount,
                                    nObsidianBotCount,
                                    nGeodeCount + nGeodeBotCount,
                                    nGeodeBotCount);
                }
                bOre = AOC_TRUE;
            }
        }

        /* Only do nothing if we didn't build either an ore or a clay bot */
        if ((bOre == AOC_FALSE) || (bClay == AOC_FALSE))
        {
            APPEND_TO_QUEUE(kQueue, nQueueSize, QUEUE_SIZE,
                            nTime + 1u,
                            nOreCount + nOreBotCount,
                            nOreBotCount,
                            nClayCount + nClayBotCount,
                            nClayBotCount,
                            nObsidianCount + nObsidianBotCount,
                            nObsidianBotCount,
                            nGeodeCount + nGeodeBotCount,
                            nGeodeBotCount);
        }
    }

    return nMaxGeodeCount;
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");

    if (pFile)
    {
        long            nFileSize;

        TBlueprintType* kBlueprints;
        size_t          nBlueprintMaxSize;
        size_t          nBlueprintSize      = 0u;

        size_t          i;
        size_t          nGeodeSum           = 0u;
        size_t          nGeodeProduct       = 1u;

        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        nBlueprintMaxSize = (size_t)(nFileSize / 160) + 1u;

        kBlueprints = (TBlueprintType*)malloc(nBlueprintMaxSize * sizeof(TBlueprintType));
        assert(kBlueprints);

        while (!feof(pFile))
        {
            size_t nBlueprintId;

            fseek(pFile, 10, SEEK_CUR);
            nBlueprintId = getValue(&pFile);
            assert(nBlueprintId == (nBlueprintSize + 1u));

            /* Ore Bot - Ore */
            fseek(pFile, 22, SEEK_CUR);
            kBlueprints[nBlueprintSize].nOreBotOreCost          = getValue(&pFile);
            assert(kBlueprints[nBlueprintSize].nOreBotOreCost);

            /* Clay Bot - Ore */
            fseek(pFile, 27, SEEK_CUR);
            kBlueprints[nBlueprintSize].nClayBotOreCost         = getValue(&pFile);
            assert(kBlueprints[nBlueprintSize].nClayBotOreCost);

            /* Obsidian Bot - Ore / Clay */
            fseek(pFile, 31, SEEK_CUR);
            kBlueprints[nBlueprintSize].nObsidianBotOreCost     = getValue(&pFile);
            assert(kBlueprints[nBlueprintSize].nObsidianBotOreCost);

            fseek(pFile, 8, SEEK_CUR);
            kBlueprints[nBlueprintSize].nObsidianBotClayCost    = getValue(&pFile);
            assert(kBlueprints[nBlueprintSize].nObsidianBotClayCost);

            /* Geode Bot - Ore / Obsidian */
            fseek(pFile, 29, SEEK_CUR);
            kBlueprints[nBlueprintSize].nGeodeBotOreCost        = getValue(&pFile);
            assert(kBlueprints[nBlueprintSize].nGeodeBotOreCost);

            fseek(pFile, 8, SEEK_CUR);
            kBlueprints[nBlueprintSize].nGeodeBotObsidianCost   = getValue(&pFile);
            assert(kBlueprints[nBlueprintSize].nGeodeBotObsidianCost);

            while ('\n' != fgetc(pFile))
            {
                if (feof(pFile))
                {
                    break;
                }
            }

            ++nBlueprintSize;
        }

        fclose(pFile);

        assert(nBlueprintSize >= 3u);

        for (i = 0u; i < nBlueprintSize; ++i)
        {
            nGeodeSum += (i+1) * calculateMaxGeodes(&kBlueprints[i], 24u);
        }
        printf("Part 1: %zu\n", nGeodeSum);

        for (i = 0u; i < 3; ++i)
        {
            nGeodeProduct *= calculateMaxGeodes(&kBlueprints[i], 32u);
        }
        printf("Part 2: %zu\n", nGeodeProduct);

        free(kBlueprints);
    }

    return 0;
}
