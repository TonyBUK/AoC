#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define MAX_SIZE_T (size_t)((uint64_t)(-1))

typedef enum spellNameType
{
    E_SPELL_MAGIC_MISSILE = 0,
    E_SPELL_DRAIN,
    E_SPELL_SHIELD,
    E_SPELL_POISON,
    E_SPELL_RECHARGE,
    E_SPELL_LAST
} spellNameType;

typedef struct coreStatsType
{
    int64_t         nDamage;
    int64_t         nArmor;
} coreStatsType;

typedef struct playerType
{
    coreStatsType   kCoreStats;
    int64_t         nHealth;
    int64_t         nMana;
    int64_t         nDuration[E_SPELL_LAST];
} playerType;

typedef struct spellType
{
    int64_t         nManaCost;
    int64_t         nImmediateDamage;
    int64_t         nImmediateHeal;
    int64_t         nDuration;
    int64_t         nBuff;
} spellType;

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

bool checkEndGame(const playerType* pkPlayer, const playerType* pkBoss, const size_t nManaSpend, size_t* pnMinManaSpend)
{
    if (nManaSpend >= *pnMinManaSpend)
    {
        return true;
    }

    if (pkPlayer->nHealth <= 0)
    {
        return true;
    }

    if (pkBoss->nHealth <= 0)
    {
        *pnMinManaSpend = nManaSpend;
        return true;
    }

    return false;
}

void processCooldowns(playerType* pkPlayer, const spellType* pkSpells)
{
    size_t i;

    for (i = E_SPELL_SHIELD; i < E_SPELL_LAST; ++i)
    {
        if (pkPlayer->nDuration[i] > 0)
        {
            pkPlayer->nDuration[i] -= 1;
            switch(i)
            {
                case E_SPELL_SHIELD:
                {
                    pkPlayer->kCoreStats.nArmor = pkSpells[i].nBuff;
                } break;

                case E_SPELL_POISON:
                {
                    pkPlayer->kCoreStats.nDamage = pkSpells[i].nBuff;
                } break;

                case E_SPELL_RECHARGE:
                {
                    pkPlayer->nMana += pkSpells[i].nBuff;
                } break;

                default:
                {
                    assert(false);
                }
            }
        }
        else
        {
            switch(i)
            {
                case E_SPELL_SHIELD:
                {
                    pkPlayer->kCoreStats.nArmor = 0;
                } break;

                case E_SPELL_POISON:
                {
                    pkPlayer->kCoreStats.nDamage = 0;
                } break;

                case E_SPELL_RECHARGE:
                {
                } break;

                default:
                {
                    assert(false);
                }
            }
        }
    }
}

#define playGamesDefault(pkPlayer, pkBoss, pkSpells, bHardMode) \
       playGames(                  pkPlayer,                   pkBoss,                  pkSpells,            bHardMode,                       0, MAX_SIZE_T)
size_t playGames(const playerType* pkPlayer, const playerType* pkBoss, const spellType* pkSpells, const bool bHardMode, const size_t nManaSpend, const size_t nMinManaSpend)
{
    size_t nBestMinManaSpend = nMinManaSpend;
    size_t i;

    for (i = 0; i < E_SPELL_LAST; ++i)
    {
        playerType         kLocalPlayer    = *pkPlayer;
        playerType         kLocalBoss      = *pkBoss;
        size_t             nLocalManaSpend = nManaSpend;

        /********************************************************************
         * Hard Mode
         ********************************************************************/

        if (bHardMode)
        {
            kLocalPlayer.nHealth -= 1;
            if (checkEndGame(&kLocalPlayer, &kLocalBoss, nLocalManaSpend, &nBestMinManaSpend))
            {
                continue;
            }
        }

        /********************************************************************
         * Player Turn
         ********************************************************************/

        /* Perform all Spells involving Cooldowns */
        processCooldowns(&kLocalPlayer, pkSpells);

        /* Determine if the Current Move is Viable */
        if (pkSpells[i].nManaCost <= kLocalPlayer.nMana)
        {
            kLocalBoss.nHealth   -= pkSpells[i].nImmediateDamage;
            kLocalPlayer.nHealth += pkSpells[i].nImmediateHeal;

            if (pkSpells[i].nDuration > 0)
            {
                if (0 == kLocalPlayer.nDuration[i])
                {
                    kLocalPlayer.nDuration[i] = pkSpells[i].nDuration;
                }
            }

            /* Spend the Mana */
            nLocalManaSpend     += pkSpells[i].nManaCost;
            kLocalPlayer.nMana  -= pkSpells[i].nManaCost;
        }
        else
        {
            continue;
        }

        /* Perform any Damage Over Time Actions */
        kLocalBoss.nHealth -= kLocalPlayer.kCoreStats.nDamage;

        if (checkEndGame(&kLocalPlayer, &kLocalBoss, nLocalManaSpend, &nBestMinManaSpend))
        {
            continue;
        }

        /********************************************************************
         * Boss Turn
         ********************************************************************/

        /* Perform all Spells involving Cooldowns */
        processCooldowns(&kLocalPlayer, pkSpells);

        /* Perform any Damage Over Time Actions */
        kLocalBoss.nHealth -= kLocalPlayer.kCoreStats.nDamage;

        if (checkEndGame(&kLocalPlayer, &kLocalBoss, nLocalManaSpend, &nBestMinManaSpend))
        {
            continue;
        }

        /* Perform the Boss Attack */
        kLocalPlayer.nHealth -= (kLocalBoss.kCoreStats.nDamage - kLocalPlayer.kCoreStats.nArmor);

        if (checkEndGame(&kLocalPlayer, &kLocalBoss, nLocalManaSpend, &nBestMinManaSpend))
        {
            continue;
        }

        /********************************************************************
         * Recurse...
         ********************************************************************/

        nBestMinManaSpend = playGames(&kLocalPlayer, &kLocalBoss, pkSpells, bHardMode, nLocalManaSpend, nBestMinManaSpend);
    }

    return nBestMinManaSpend;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        const spellType         kSpells[] =
        {
            { 53, 4, 0, 0,   0},    /* Magic Missile */
            { 73, 2, 2, 0,   0},    /* Drain */
            {113, 0, 0, 6,   7},    /* Shield */
            {173, 0, 0, 6,   3},    /* Poison */
            {229, 0, 0, 5, 101}     /* Recharge */
        };

        playerType              kPlayer;
        playerType              kBoss;

        size_t                  i;
        size_t                  nLineCount;

        assert(E_SPELL_LAST == (sizeof(kSpells) / sizeof(kSpells[0])));

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount);
        fclose(pInput);

        assert(2 == nLineCount);

        /* Store the Boss Stats */
        kBoss.kCoreStats.nArmor = 0;
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
            else
            {
                assert(false);
            }
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        /* Solve the Puzzle */
        kPlayer.kCoreStats.nDamage          = 0;
        kPlayer.kCoreStats.nArmor           = 0;
        kPlayer.nHealth                     = 50;
        kPlayer.nMana                       = 500;
        kPlayer.nDuration[E_SPELL_SHIELD]   = 0;
        kPlayer.nDuration[E_SPELL_POISON]   = 0;
        kPlayer.nDuration[E_SPELL_RECHARGE] = 0;

        printf("Part 1: %u\n", (uint32_t)playGamesDefault(&kPlayer, &kBoss, kSpells, false));
        printf("Part 2: %u\n", (uint32_t)playGamesDefault(&kPlayer, &kBoss, kSpells, true));
    }

    return 0;
}
