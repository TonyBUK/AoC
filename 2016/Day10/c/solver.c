#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

typedef enum TargetType {E_BOT, E_OUTPUT} TargetType;

typedef struct ActionType
{
    TargetType  eTarget;
    int64_t     nTargetId;
} ActionType;

typedef struct BotType
{
    int64_t     kChips[2];
    size_t      nChipCount;
    ActionType  kRules[2];
} BotType;

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

    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

void processBot(const size_t nTokenIndex, const char* kToken, BotType* kBots, const size_t nBotCount, int64_t* pnSourceId, size_t* pnTargetCount)
{
    assert(nTokenIndex < 12);
    switch(nTokenIndex)
    {
        case 1:
        {
            *pnSourceId = strtoll(kToken, NULL, 10);
            assert(*pnSourceId >= 0);
            assert(*pnSourceId < nBotCount);
        } break;

        case 5:
        case 10:
        {
            if (strcmp("bot", kToken) == 0)
            {
                kBots[*pnSourceId].kRules[*pnTargetCount].eTarget = E_BOT;
            }
            else if (strcmp("output", kToken) == 0)
            {
                kBots[*pnSourceId].kRules[*pnTargetCount].eTarget = E_OUTPUT;
            }
            else
            {
                assert(false);
            }
        } break;

        case 6:
        case 11:
        {
            const int64_t nTargetId = strtoll(kToken, NULL, 10);
            assert(nTargetId >= 0);
            assert(nTargetId < nBotCount);
            kBots[*pnSourceId].kRules[*pnTargetCount].nTargetId = nTargetId;
            ++(*pnTargetCount);
        } break;

        default: break;
    }
}

void sortLowest(int64_t* pnLeft, int64_t* pnRight)
{
    if (*pnLeft > *pnRight)
    {
        const int64_t nTemp = *pnLeft;
        *pnLeft = *pnRight;
        *pnRight = nTemp;
    }
}

void processValue(const size_t nTokenIndex, const char* kToken, int64_t* pnValue, BotType* kBots, const size_t nBotCount, int64_t* kQueue, size_t* nQueueSize)
{
    assert(nTokenIndex < 6);
    switch (nTokenIndex)
    {
        case 1:
        {
            *pnValue = strtoll(kToken, NULL, 10);
        } break;

        case 5:
        {
            const int64_t nTargetId = strtoll(kToken, NULL, 10);
            assert(nTargetId >= 0);
            assert(nTargetId < nBotCount);
            assert(kBots[nTargetId].nChipCount < 2);

            kBots[nTargetId].kChips[kBots[nTargetId].nChipCount] = *pnValue;
            ++kBots[nTargetId].nChipCount;

            if (2 == kBots[nTargetId].nChipCount)
            {
                sortLowest(&kBots[nTargetId].kChips[0], &kBots[nTargetId].kChips[1]);
                kQueue[*nQueueSize] = nTargetId;
                ++(*nQueueSize);
            }
        } break;

        default: break;
    }
}

void processBotCommand(BotType* kBots, const int64_t nBotId, int64_t* kOutputBins, int64_t* kQueue, size_t* nQueueSize)
{
    size_t i;
    for (i = 0; i < 2; ++i)
    {
        const int64_t nTargetId = kBots[nBotId].kRules[i].nTargetId;
        
        /* If the Target is a Bot, move the chip to the Target Bot */
        if (E_BOT == kBots[nBotId].kRules[i].eTarget)
        {
            assert(kBots[nTargetId].nChipCount < 2);
            kBots[nTargetId].kChips[kBots[nTargetId].nChipCount] = kBots[nBotId].kChips[i];

            ++kBots[nTargetId].nChipCount;

            /* Determine if Target Bot has enough chips to need adding to the queue */
            if (2 == kBots[nTargetId].nChipCount)
            {
                sortLowest(&kBots[nTargetId].kChips[0], &kBots[nTargetId].kChips[1]);
                kQueue[*nQueueSize] = nTargetId;
                ++(*nQueueSize);
            }
        }
        /* Otherwise Move the Chip to the Output Bin */
        else if (E_OUTPUT == kBots[nBotId].kRules[i].eTarget)
        {
            kOutputBins[nTargetId] = kBots[nBotId].kChips[i];
        }
    }
    kBots[nBotId].nChipCount = 0;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const int64_t           kChipComparison[2]  = {17, 61};

        char*                   kBuffer;
        char**                  kLines;

        size_t                  nLineCount;
        size_t                  i;

        BotType*                kBots;
        int64_t*                kOutputBins;
        int64_t*                kQueue;
        size_t                  nQueueSize      = 0;

        int64_t                 nPart1BotId;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        /* Dangerous and Lazy Assumption Time.......
         * 
         * Rather than pre-parse the data to extract the Lowest/Highest Id's for the Bins,
         * let's just pre-allocate them based on the number of lines assuming:
         * 1. Each bot has a rule
         * 2. There can never be more output bins than bots.
         * 3. The Id's of the Bots/Bins is sequential.
         * 
         * Absolutely none of this is guarenteed, but does seem to hold true for the input
         * data provided.
         */

        kBots       = (BotType*)malloc(nLineCount * sizeof(BotType));
        kOutputBins = (int64_t*)malloc(nLineCount * sizeof(int64_t));
        kQueue      = (int64_t*)malloc(nLineCount * sizeof(int64_t));

        memset(kBots,       0, nLineCount * sizeof(BotType));
        memset(kOutputBins, 0, nLineCount * sizeof(int64_t));

        /* Parse the raw text and configure the Bots/Output Bins */
        for (i = 0; i < nLineCount; ++i)
        {
            bool    bIsValue;
            int64_t nSourceId;
            size_t  nTargetCount = 0;
            int64_t nChipValue;

            size_t j = 0;
            char*  p = strtok(kLines[i], " ");

            while (p)
            {
                if (0 == j)
                {
                    if (strcmp(p, "bot") == 0)
                    {
                        bIsValue = false;
                    }
                    else if (strcmp(p, "value") == 0)
                    {
                        bIsValue = true;
                    }
                    else
                    {
                        assert(false);
                    }
                }
                else if (!bIsValue)
                {
                    processBot(j, p, kBots, nLineCount, &nSourceId, &nTargetCount);
                }
                else
                {
                    processValue(j, p, &nChipValue, kBots, nLineCount, kQueue, &nQueueSize);
                }

                ++j;
                p = strtok(NULL, " ");
            }
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        /* Keep going until there are no Bots that can give any chips */
        while (nQueueSize > 0)
        {
            const int64_t nBotId = kQueue[nQueueSize-1];
            --nQueueSize;

            /* Test for the Part 1 Solution */
            if ((kBots[nBotId].kChips[0] == kChipComparison[0]) &&
                (kBots[nBotId].kChips[1] == kChipComparison[1]))
            {
                nPart1BotId = nBotId;
            }

            /* Process the High/Low Commands for each Bot */
            processBotCommand(kBots, nBotId, kOutputBins, kQueue, &nQueueSize);
        }

        assert(nLineCount >= 3);
        printf("Part 1: %llu\n", (uint64_t)nPart1BotId);
        printf("Part 2: %lli\n", kOutputBins[0] * kOutputBins[1] * kOutputBins[2]);

        free(kBots);
        free(kOutputBins);
        free(kQueue);
    }

    return 0;
}
