#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define MAX_SIZE_T (size_t)((uint64_t)(-1))

typedef struct coreStatsType
{
    int64_t         nDamage;
    int64_t         nArmor;
} coreStatsType;

typedef struct playerType
{
    coreStatsType   kCoreStats;
    int64_t         nHealth;
} playerType;

typedef struct itemType
{
    coreStatsType   kCoreStats;
    int64_t         nCost;
} itemType;

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount)
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

    *pkFileBuffer   = (char*) malloc((nFileSize+1)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));

    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    (*pkFileBuffer)[nFileSize] = '\0';

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

    *pnLineCount = nLineCount;
}

bool playGame(const playerType* kPlayerStats, const playerType* kBossStats)
{
    int64_t nPlayerHealth = kPlayerStats->nHealth;
    int64_t nBossHealth   = kBossStats->nHealth;

    while (true)
    {
        /* Player Turn */
        if (kPlayerStats->kCoreStats.nDamage > kBossStats->kCoreStats.nArmor)
        {
            nBossHealth -= kPlayerStats->kCoreStats.nDamage - kBossStats->kCoreStats.nArmor;
            if (nBossHealth <= 0) return true;
        }

        /* Boss Turn */
        if (kBossStats->kCoreStats.nDamage > kPlayerStats->kCoreStats.nArmor)
        {
            nPlayerHealth -= kBossStats->kCoreStats.nDamage - kPlayerStats->kCoreStats.nArmor;
            if (nPlayerHealth <= 0) return false;
        }
    }
}

void playGamePermutations(const itemType*   pkWeapons,
                          const size_t      nWeaponsSize,
                          const itemType*   pkArmor,
                          const size_t      nArmorSize,
                          const itemType*   pkRings,
                          const size_t      nRingsSize,
                          const playerType* pkBossStats,
                          const int64_t     nPlayerHealth,
                          size_t*           pnLeastGold,
                          size_t*           pnMostGold)
{
    size_t nLeastGold = MAX_SIZE_T;
    size_t nMostGold  = 0;

    /* The Number of Ring Permutations is going to be:
     * (n-1 + n-2 .. 1)
     * i.e. for 5 rings:
     * 4 + 3 + 2 + 1
     * Which is the old classic triangle number...
     */
    const size_t nRingPermutationsSize      = ((nRingsSize-1) * nRingsSize) / 2;
    const itemType**   kRingPermutations    = (const itemType**)malloc((nRingPermutationsSize * 2) * sizeof(itemType*));
    size_t       i;
    size_t       j;
    size_t       nWeapon;
    size_t       nArmor;
    size_t       nRingPair;

    assert(pkWeapons);
    assert(pkArmor);
    assert(pkRings);
    assert(pkBossStats);
    assert(pnLeastGold);
    assert(pnMostGold);

    /* Precalculate the Ring Permutations */
    nRingPair = 0;
    for (i = 0; i < nRingsSize; ++i)
    {
        for (size_t j = i + 1; j < nRingsSize; ++j)
        {
            kRingPermutations[(nRingPair * 2) + 0] = &pkRings[i];
            kRingPermutations[(nRingPair * 2) + 1] = &pkRings[j];
            ++nRingPair;
        }
    }

    /* For each possible Load Out */
    for (nWeapon = 0; nWeapon < nWeaponsSize; ++nWeapon)
    {
        for (nArmor = 0; nArmor < nArmorSize; ++nArmor)
        {
            for (nRingPair = 0; nRingPair < nRingPermutationsSize; ++nRingPair)
            {
                const itemType*              pkRing1          = kRingPermutations[(nRingPair * 2) + 0];
                const itemType*              pkRing2          = kRingPermutations[(nRingPair * 2) + 1];
                const playerType             kPlayerStats     = {{pkWeapons[nWeapon].kCoreStats.nDamage + pkArmor[nArmor].kCoreStats.nDamage + pkRing1->kCoreStats.nDamage + pkRing2->kCoreStats.nDamage,
                                                                  pkWeapons[nWeapon].kCoreStats.nArmor  + pkArmor[nArmor].kCoreStats.nArmor  + pkRing1->kCoreStats.nArmor  + pkRing2->kCoreStats.nArmor},
                                                                 nPlayerHealth};
                const size_t                 nCost            = (size_t)
                                                                (pkWeapons[nWeapon].nCost               + pkArmor[nArmor].nCost              + pkRing1->nCost              + pkRing2->nCost);

                /* Play the Game with the current Loadout
                 * If we win, we're interested in the least gold used,
                 * otherwise we're interested in the most
                 */
                if (playGame(&kPlayerStats, pkBossStats))
                {
                    if (nCost < nLeastGold) nLeastGold = nCost;
                }
                else
                {
                    if (nCost > nMostGold) nMostGold = nCost;
                }
            }
        }
    }

    *pnLeastGold = nLeastGold;
    *pnMostGold  = nMostGold;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        const itemType          kWeapons[] =
        {
            {{4, 0},  8},   /* Dagger */
            {{5, 0}, 10},   /* Shortsword */
            {{6, 0}, 25},   /* Warhammer */
            {{7, 0}, 40},   /* Longsword */
            {{8, 0}, 74}    /* Greataxe */
        };

        const itemType          kArmor[] =
        {
            {{0, 0},   0},  /* None */
            {{0, 1},  13},  /* Leather */
            {{0, 2},  31},  /* Chainmail */
            {{0, 3},  53},  /* Splintmail */
            {{0, 4},  75},  /* Bandedmail */
            {{0, 5}, 102}   /* Platemail */
        };

        const itemType          kRings[] =
        {
            {{0, 0},   0},  /* None */
            {{0, 0},   0},  /* None */
            {{1, 0},  25},  /* Damage +1 */
            {{2, 0},  50},  /* Damage +2 */
            {{3, 0}, 100},  /* Damage +3 */
            {{0, 1},  20},  /* Defense +1 */
            {{0, 2},  40},  /* Defense +2 */
            {{0, 3},  80}   /* Defense +3 */
        };

        playerType              kBoss;

        size_t                  i;
        size_t                  nLineCount;
        size_t                  nLeastGold;
        size_t                  nMostGold;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount);
        fclose(pInput);

        assert(3 == nLineCount);

        /* Store the Boss Stats */
        for (i = 0; i < nLineCount; ++i)
        {
            if (strstr(kLines[i], "Hit Points: "))
            {
                kBoss.nHealth = strtoll(&kLines[i][12], NULL, 10);
            }
            else if (strstr(kLines[i], "Damage: "))
            {
                kBoss.kCoreStats.nDamage = strtoll(&kLines[i][8], NULL, 10);
            }
            else if (strstr(kLines[i], "Armor: "))
            {
                kBoss.kCoreStats.nArmor = strtoll(&kLines[i][7], NULL, 10);
            }
            else
            {
                assert(false);
            }
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        /* Solve the Puzzle */
        playGamePermutations(kWeapons,
                             sizeof(kWeapons)/sizeof(itemType),
                             kArmor,
                             sizeof(kArmor)/sizeof(itemType),
                             kRings,
                             sizeof(kRings)/sizeof(itemType),
                             &kBoss,
                             100,
                             &nLeastGold,
                             &nMostGold);
        printf("Part 1: %u\n", (uint32_t)nLeastGold);
        printf("Part 2: %u\n", (uint32_t)nMostGold);
    }

    return 0;
}
