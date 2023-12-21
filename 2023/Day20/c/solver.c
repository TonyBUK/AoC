#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE        (1u)
#define AOC_FALSE       (0u)

#define KEY_WIDTH       (5u)
#define KEY_MAX_COUNT   (2u) /* Note: This won't work on the AoC examples, only the real data */
#define KEY_CACHE_SIZE  (1u << (KEY_WIDTH * KEY_MAX_COUNT))

typedef enum EModuleType
{
    EMODULE_BROADCASTER = 0,
    EMODULE_FLIPFLOP    = '%',
    EMODULE_CONJUNCTION = '&'
} EModuleType;

typedef struct SConjunctionType
{
    size_t      kConjunctionLookup[KEY_CACHE_SIZE];
    unsigned    bConjunctionStates[KEY_CACHE_SIZE];
    size_t      nConjunctionCount;
} SConjunctionType;

typedef struct SModuleDataType
{
    EModuleType         eType;
    uint64_t*           kBroadcastModules;
    size_t              nBroadcastModuleCount;
    unsigned            bState;
    SConjunctionType    kConjunctionModules;
} SModuleDataType;

typedef struct SQueueType
{
    uint64_t nSourceKey;
    uint64_t nTargetKey;
    unsigned bPulse;
} SQueueType;

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

uint64_t gcd (uint64_t a, uint64_t b)
{
    unsigned c;
    while (a != 0)
    {
        c = a;
        a = b % a;
        b = c;
    }
    return b;
}
uint64_t lcm(uint64_t a, uint64_t b)
{
    return (b / gcd(a, b)) * a;
}

uint64_t lcmArray(uint64_t* n, size_t size)
{
    uint64_t nLastLCM;
    size_t   i;

    if (size < 2)
    {
        return 0;
    }

    nLastLCM = lcm(n[0], n[1]);

    for(i = 2; i < size; ++i)
    {
        nLastLCM = lcm(nLastLCM, n[i]);
    }

    return nLastLCM;
}

uint64_t getKey(char** pKey)
{
    uint64_t nKey   = 0;
    size_t   nCount = 0;

    while ((**pKey >= 'a') && (**pKey <= 'z') && (nCount < KEY_MAX_COUNT))
    {
        nKey <<= KEY_WIDTH;
        nKey  |= **pKey - 'a' + 1;

        ++nCount;
        ++*pKey;
    }

    return nKey;
}

uint64_t getKeyFromConst(const char* pKey)
{
    char* pKeyCopy = (char*)pKey;
    return getKey(&pKeyCopy);
}

void getKeyFromEncoded(const uint64_t nKey, char* kKey)
{
    size_t   nCount   = 0;
    uint64_t nKeyCopy = nKey;

    while (nCount < KEY_MAX_COUNT)
    {
        kKey[KEY_MAX_COUNT - nCount - 1] = (nKeyCopy & ((1 << KEY_WIDTH) - 1)) - 1 + 'a';
        nKeyCopy = nKeyCopy >> KEY_WIDTH;
        ++nCount;
    }

    kKey[nCount] = '\0';
}

void updateBroadSources(char** pSources, SModuleDataType* pModule, const size_t nBroadcastModuleMaxSize)
{
    size_t   nEntry = 0;

    /* Should only encounter this once */
    assert(0 == pModule->nBroadcastModuleCount);
    pModule->kBroadcastModules = (uint64_t*)malloc(nBroadcastModuleMaxSize * sizeof(uint64_t));

    while (**pSources != '\0')
    {
        assert(nEntry < nBroadcastModuleMaxSize);
        pModule->kBroadcastModules[nEntry++] = getKey(pSources);

        if (**pSources != '\0')
        {
            *pSources += 2;
        }
    }

    pModule->nBroadcastModuleCount = nEntry;
}

/* Push the Button Frank! */
uint64_t pushButton(const SModuleDataType* kModuleConfiguration, SModuleDataType* kModuleConfigurationLive, const int64_t nIterations, const uint64_t nStartKey, const SConjunctionType* kFilterKey, const uint64_t nFilterKey, const uint64_t nTargetKey, const unsigned bTargetPulse, unsigned bDebug)
{
    uint64_t    nLowPulses     = 0;
    uint64_t    nHighPulses    = 0;
    uint64_t    nCycle         = 1;

    size_t      nQueueHead     = 0;
    size_t      nQueueTail     = 0;
    SQueueType  kQueue[KEY_CACHE_SIZE];

    memcpy(kModuleConfigurationLive, kModuleConfiguration, sizeof(SModuleDataType) * KEY_CACHE_SIZE);

    while ((-1 == nIterations) || (nCycle <= nIterations))
    {
        kQueue[nQueueTail].nSourceKey = nStartKey;
        kQueue[nQueueTail].nTargetKey = nStartKey;
        kQueue[nQueueTail].bPulse     = AOC_FALSE;
        nQueueTail = (nQueueTail + 1) % KEY_CACHE_SIZE;

        if (bDebug)
        {
            printf("New Queue...\n");
        }

        while (nQueueHead != nQueueTail)
        {
            /* Pop the Queue Item */
            const SQueueType* pQueueEntry = &kQueue[nQueueHead];
            nQueueHead = (nQueueHead + 1) % KEY_CACHE_SIZE;

            if (bDebug)
            {
                char kSourceKey[KEY_MAX_COUNT + 1];

                if (0 == pQueueEntry->nTargetKey)
                {
                    printf("button");
                }
                else if (0 == pQueueEntry->nSourceKey)
                {
                    printf("broadcaster");
                }
                else
                {
                    getKeyFromEncoded(pQueueEntry->nSourceKey, kSourceKey);
                    printf("%s", kSourceKey);
                }

                printf("%s -> ", (pQueueEntry->bPulse ? " -high" : " -low"));

                if (0 == pQueueEntry->nTargetKey)
                {
                    printf("broadcaster");
                }
                else
                {
                    getKeyFromEncoded(pQueueEntry->nTargetKey, kSourceKey);
                    printf("%s", kSourceKey);
                }

                printf("\n");
            }

            if (pQueueEntry->bPulse)
            {
                ++nHighPulses;
            }
            else
            {
                ++nLowPulses;
            }

            /* Part Two */
            if (kFilterKey)
            {
                if (nTargetKey == pQueueEntry->nTargetKey)
                {
                    if (bTargetPulse == pQueueEntry->bPulse)
                    {
                        return nCycle;
                    }
                }
            }

            /* Skip Outputs */
            if (0 == kModuleConfigurationLive[pQueueEntry->nTargetKey].nBroadcastModuleCount)
            {
                continue;
            }
            else
            {
                SModuleDataType* pModule = &kModuleConfigurationLive[pQueueEntry->nTargetKey];

                switch (pModule->eType)
                {
                    case EMODULE_BROADCASTER:
                    {
                        /* roadcasters don't do anything special... */
                        size_t nBroadcastModule;
                        for (nBroadcastModule = 0; nBroadcastModule < pModule->nBroadcastModuleCount; ++nBroadcastModule)
                        {
                            kQueue[nQueueTail].nSourceKey = pQueueEntry->nTargetKey;
                            kQueue[nQueueTail].nTargetKey = pModule->kBroadcastModules[nBroadcastModule];
                            kQueue[nQueueTail].bPulse     = AOC_FALSE;

                            nQueueTail = (nQueueTail + 1) % KEY_CACHE_SIZE;
                            assert(nQueueHead != nQueueTail);
                        }

                    } break;

                    case EMODULE_FLIPFLOP:
                    {
                        /* Only Process Low Pulses */
                        if (AOC_FALSE == pQueueEntry->bPulse)
                        {
                            size_t nBroadcastModule;

                            /* Invert the State */
                            pModule->bState = (AOC_TRUE == pModule->bState) ? AOC_FALSE : AOC_TRUE;

                            for (nBroadcastModule = 0; nBroadcastModule < pModule->nBroadcastModuleCount; ++nBroadcastModule)
                            {
                                kQueue[nQueueTail].nSourceKey = pQueueEntry->nTargetKey;
                                kQueue[nQueueTail].nTargetKey = pModule->kBroadcastModules[nBroadcastModule];
                                kQueue[nQueueTail].bPulse     = pModule->bState;

                                nQueueTail = (nQueueTail + 1) % KEY_CACHE_SIZE;
                                assert(nQueueHead != nQueueTail);
                            }
                        }

                    } break;

                    case EMODULE_CONJUNCTION:
                    {
                        /* Update the Internal State */
                        const size_t nConjunctionLookup = pModule->kConjunctionModules.kConjunctionLookup[pQueueEntry->nSourceKey];
                              size_t nConjunctionModule;
                              size_t nBroadcastModule;
                        pModule->kConjunctionModules.bConjunctionStates[nConjunctionLookup] = pQueueEntry->bPulse;

                        /* Part Two - Force Filtered Items to Good */
                        if (kFilterKey)
                        {
                            if (pQueueEntry->nTargetKey == nFilterKey)
                            {
                                for (nConjunctionModule = 0; nConjunctionModule < kFilterKey->nConjunctionCount; ++nConjunctionModule)
                                {
                                    if (kFilterKey->bConjunctionStates[nConjunctionModule])
                                    {
                                        pModule->kConjunctionModules.bConjunctionStates[nConjunctionModule] = AOC_TRUE;
                                    }
                                }
                            }
                        }

                        /* Calculate the Single State */
                        pModule->bState = AOC_FALSE;
                        for (nConjunctionModule = 0; nConjunctionModule < pModule->kConjunctionModules.nConjunctionCount; ++nConjunctionModule)
                        {
                            if (AOC_FALSE == pModule->kConjunctionModules.bConjunctionStates[nConjunctionModule])
                            {
                                pModule->bState = AOC_TRUE;
                                break;
                            }
                        }

                        for (nBroadcastModule = 0; nBroadcastModule < pModule->nBroadcastModuleCount; ++nBroadcastModule)
                        {
                            kQueue[nQueueTail].nSourceKey = pQueueEntry->nTargetKey;
                            kQueue[nQueueTail].nTargetKey = pModule->kBroadcastModules[nBroadcastModule];
                            kQueue[nQueueTail].bPulse     = pModule->bState;

                            nQueueTail = (nQueueTail + 1) % KEY_CACHE_SIZE;
                            assert(nQueueHead != nQueueTail);
                        }
                    } break;

                    default:
                    {
                        assert(0);
                    }
                }
            }
        }

        ++nCycle;
    }

    return nHighPulses * nLowPulses;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        const uint64_t          nStartKey   = 0;
        const uint64_t          nEndKey     = getKeyFromConst("rx");

        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;
        size_t                  nMaxLineLength;

        uint64_t                nSourceKey  = 0;

        SModuleDataType*        kModuleConfiguration;
        SModuleDataType*        kModuleConfigurationLive;
        SConjunctionType        kFilterKey;
        uint64_t                kCycles[KEY_CACHE_SIZE];

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength, 0);
        fclose(pData);

        kModuleConfiguration     = (SModuleDataType*)malloc(KEY_CACHE_SIZE * sizeof(SModuleDataType));
        kModuleConfigurationLive = (SModuleDataType*)malloc(KEY_CACHE_SIZE * sizeof(SModuleDataType));
        for (nLine = 0; nLine < KEY_CACHE_SIZE; ++nLine)
        {
            kModuleConfiguration[nLine].nBroadcastModuleCount                 = 0;
            kModuleConfiguration[nLine].kConjunctionModules.nConjunctionCount = 0;
        }

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            const uint64_t nKey = getKeyFromConst(&kLines[nLine][1]);
            char*          pBroadcast = strstr(kLines[nLine], " -> ");
            assert(pBroadcast);
            pBroadcast += 4;

            switch (kLines[nLine][0])
            {
                case EMODULE_FLIPFLOP:
                {
                    updateBroadSources(&pBroadcast, &kModuleConfiguration[nKey], nMaxLineLength);
                    kModuleConfiguration[nKey].bState = AOC_FALSE;
                    kModuleConfiguration[nKey].eType  = EMODULE_FLIPFLOP;
                } break;

                case EMODULE_CONJUNCTION:
                {
                    updateBroadSources(&pBroadcast, &kModuleConfiguration[nKey], nMaxLineLength);
                    kModuleConfiguration[nKey].bState = AOC_FALSE;
                    kModuleConfiguration[nKey].eType  = EMODULE_CONJUNCTION;
                } break;

                default:
                {
                    assert(strstr(kLines[nLine], "broadcaster") == kLines[nLine]);
                    updateBroadSources(&pBroadcast, &kModuleConfiguration[nStartKey], nMaxLineLength);
                    kModuleConfiguration[nStartKey].bState = AOC_FALSE;
                    kModuleConfiguration[nStartKey].eType  = EMODULE_BROADCASTER;
                } break;
            }
        }

        for (nLine = 0; nLine < KEY_CACHE_SIZE; ++nLine)
        {
            /* Initialise all the States a Conjunction Node can receive */
            if (kModuleConfiguration[nLine].nBroadcastModuleCount)
            {
                size_t nBroadcastModule;

                for (nBroadcastModule = 0; nBroadcastModule < kModuleConfiguration[nLine].nBroadcastModuleCount; ++nBroadcastModule)
                {
                    const uint64_t nBroadcastKey = kModuleConfiguration[nLine].kBroadcastModules[nBroadcastModule];

                    if  (kModuleConfiguration[nBroadcastKey].nBroadcastModuleCount > 0)
                    {
                        const size_t nCount = kModuleConfiguration[nBroadcastKey].kConjunctionModules.nConjunctionCount;

                        kModuleConfiguration[nBroadcastKey].kConjunctionModules.kConjunctionLookup[nLine]  = nCount;
                        kModuleConfiguration[nBroadcastKey].kConjunctionModules.bConjunctionStates[nCount] = AOC_FALSE;

                        ++kModuleConfiguration[nBroadcastKey].kConjunctionModules.nConjunctionCount;

                        assert(kModuleConfiguration[nBroadcastKey].kConjunctionModules.nConjunctionCount < KEY_CACHE_SIZE);
                    }
                }
            }
        }

        printf("Part 1: %llu\n", pushButton(kModuleConfiguration, kModuleConfigurationLive, 1000, nStartKey, NULL, 0, 0, AOC_FALSE, AOC_FALSE));

        /* Part 2 is interested in frequencies */

        /* First find the Source Module */
        for (nLine = 0; nLine < KEY_CACHE_SIZE; ++nLine)
        {
            if (kModuleConfiguration[nLine].nBroadcastModuleCount)
            {
                size_t nBroadcastModule;

                for (nBroadcastModule = 0; nBroadcastModule < kModuleConfiguration[nLine].nBroadcastModuleCount; ++nBroadcastModule)
                {
                    if (nEndKey == kModuleConfiguration[nLine].kBroadcastModules[nBroadcastModule])
                    {
                        nSourceKey = nLine;
                        break;
                    }
                }
            }
        }
        assert(nSourceKey);

        /* Can't be bothered to solve this generically... it's going to be a Conjunction Module */
        assert(EMODULE_CONJUNCTION == kModuleConfiguration[nSourceKey].eType);

        /* Treat this as if it's separate loops, so we filter out all but one of the conjuncted nodes at a time */
        kFilterKey.nConjunctionCount = kModuleConfiguration[nSourceKey].kConjunctionModules.nConjunctionCount;
        for (nLine = 0; nLine < kFilterKey.nConjunctionCount; ++nLine)
        {
            size_t nFilterKey;

            for (nFilterKey = 0; nFilterKey < kFilterKey.nConjunctionCount; ++nFilterKey)
            {
                kFilterKey.bConjunctionStates[nFilterKey] = (nLine == nFilterKey) ? AOC_FALSE : AOC_TRUE;
            }

            kCycles[nLine] = pushButton(kModuleConfiguration, kModuleConfigurationLive, -1, nStartKey, &kFilterKey, nSourceKey, nEndKey, AOC_FALSE, AOC_FALSE);
        }

        /* Find the LCM */
        printf("Part 2: %llu\n", lcmArray(kCycles, kFilterKey.nConjunctionCount));

        /* Free any Allocated Memory */
        for (nLine = 0; nLine < KEY_CACHE_SIZE; ++nLine)
        {
            if (kModuleConfiguration[nLine].nBroadcastModuleCount)
            {
                free(kModuleConfiguration[nLine].kBroadcastModules);
            }
        }
        free(kBuffer);
        free(kLines);
        free(kModuleConfiguration);
    }

    return 0;
}