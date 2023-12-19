#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define MIN(a, b)       ((a) < (b)) ? (a) : (b)
#define MAX(a, b)       ((a) > (b)) ? (a) : (b)

#define AOC_TRUE        (1u)
#define AOC_FALSE       (0u)

#define RULE_ACCEPT     (0u)
#define RULE_REJECT     (1u)

#define KEY_WIDTH       (5u)
#define KEY_MAX_COUNT   (3u)

#define RULE_MAX_COUNT  (8u)
#define RULE_CACHE_SIZE (1u << (KEY_WIDTH * KEY_MAX_COUNT))

typedef enum ECommandType
{
    ECOMMAND_JUMP       = 0,
    ECOMMAND_COMPARE,
    ECOMMAND_ACCEPT,
    ECOMMAND_REJECT
} ECommandType;

typedef enum EComparisonType
{
    ECOMPARISON_LT      = 0,
    ECOMPARISON_GT,
    ECOMPARISON_NONE
} ECComparisonType;

typedef enum ERegisterType
{
    EREGISTER_X = 0,
    EREGISTER_M,
    EREGISTER_A,
    EREGISTER_S
} ERegisterType;

typedef struct SOperandType
{
    ERegisterType    eOperand;
    ECComparisonType eComparison;
    int64_t          nValue;
    uint64_t         nKey;
} SOperandType;

typedef struct SOpcodeDataType
{
    ECommandType    eCommand;
    SOperandType    kOperand;
} SOpcodeDataType;

typedef struct SRuleType
{
    size_t          nRuleCount;
    SOpcodeDataType kOpcodes[RULE_MAX_COUNT];
} SRuleType;

typedef struct SPartType
{
    uint64_t        nParts[4];
} SPartType;

typedef struct SBackTraceType
{
    uint64_t        nKey;
    size_t          nRule;
} SBackTraceType;

typedef struct SBackTracesTypes
{
    SBackTraceType* kBackTraces;
    size_t          nBackTraceCount;
} SBackTracesTypes;

typedef struct SValidRangeType
{
    uint64_t        nMin;
    uint64_t        nMax;
} SValidRangeType;

typedef struct SValidRegisterRangesType
{
    SValidRangeType kValidRanges[EREGISTER_S + 1];
} SValidRegisterRangesType;

typedef struct SValidRangesType
{
    SValidRegisterRangesType*   kValidRanges;
    size_t                      nValidRangeCount;
} SValidRangesType;

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

uint64_t getValue(char** pKey)
{
    uint64_t nValue = 0;

    while ((**pKey >= '0') && (**pKey <= '9'))
    {
        nValue *= 10;
        nValue += **pKey - '0';
        ++*pKey;
    }

    return nValue;
}

uint64_t getKey(char** pKey)
{
    uint64_t nKey   = 0;
    size_t   nCount = 0;

    if (**pKey == 'A')
    {
        ++*pKey;
        return RULE_ACCEPT;
    }
    else if (**pKey == 'R')
    {
        ++*pKey;
        return RULE_REJECT;
    }

    while ((**pKey >= 'a') && (**pKey <= 'z') && (nCount < KEY_MAX_COUNT))
    {
        nKey <<= KEY_WIDTH;
        nKey  |= **pKey - 'a' + 2;

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


ERegisterType getRegister(const char kKey)
{
    switch (kKey)
    {
        case 'x': return EREGISTER_X;
        case 'm': return EREGISTER_M;
        case 'a': return EREGISTER_A;
        case 's': return EREGISTER_S;
    }

    assert(0);
}

ECComparisonType getComparison(const char kKey)
{
    switch (kKey)
    {
        case '<': return ECOMPARISON_LT;
        case '>': return ECOMPARISON_GT;
    }

    assert(0);
}

uint64_t getPartValue(const SPartType* kPart)
{
    return kPart->nParts[EREGISTER_X] + kPart->nParts[EREGISTER_M] + kPart->nParts[EREGISTER_A] + kPart->nParts[EREGISTER_S];
}

unsigned processPart(const SRuleType* kRules, const uint64_t nFirstRule, const SPartType* kPart)
{
    uint64_t    nCurrentRule = nFirstRule;

    while (1)
    {
        size_t   nRule;
        unsigned bChangeRule = AOC_FALSE;

        for (nRule = 0; nRule < kRules[nCurrentRule].nRuleCount; ++nRule)
        {
            switch (kRules[nCurrentRule].kOpcodes[nRule].eCommand)
            {
                case ECOMMAND_JUMP:
                {
                    nCurrentRule    = kRules[nCurrentRule].kOpcodes[nRule].kOperand.nKey;
                    bChangeRule     = AOC_TRUE;
                } break;

                case ECOMMAND_COMPARE:
                {
                    const uint64_t nValue = kPart->nParts[kRules[nCurrentRule].kOpcodes[nRule].kOperand.eOperand];

                    switch (kRules[nCurrentRule].kOpcodes[nRule].kOperand.eComparison)
                    {
                        case ECOMPARISON_LT:
                        {
                            if (nValue < kRules[nCurrentRule].kOpcodes[nRule].kOperand.nValue)
                            {
                                nCurrentRule    = kRules[nCurrentRule].kOpcodes[nRule].kOperand.nKey;
                                bChangeRule     = AOC_TRUE;
                            }
                        } break;

                        case ECOMPARISON_GT:
                        {
                            if (nValue > kRules[nCurrentRule].kOpcodes[nRule].kOperand.nValue)
                            {
                                nCurrentRule    = kRules[nCurrentRule].kOpcodes[nRule].kOperand.nKey;
                                bChangeRule     = AOC_TRUE;
                            }
                        } break;

                        default :
                        {
                            assert(0);
                        }
                    }
                } break;

                case ECOMMAND_ACCEPT:
                {
                    return AOC_TRUE;
                }

                case ECOMMAND_REJECT:
                {
                    return AOC_FALSE;
                }
            
            }

            if (bChangeRule)
            {
                break;
            }
        }
    }
}

void getValidRanges(const uint64_t nRuleKey, const uint64_t nRuleKeySource, const size_t nRuleCommand, const SRuleType* kRules, const SBackTracesTypes* kBackTraces, const uint64_t nTargetNode, const SValidRegisterRangesType kValidRange, SValidRangesType* kValidRanges)
{
    size_t                      nLocalRuleCommand   = nRuleCommand;
    SValidRegisterRangesType    kLocalValidRange    = kValidRange;

    while (1)
    {
        size_t  nBackTrace;

        while (1)
        {
            /* Grab the Rule */
            const SOpcodeDataType* kRule = &kRules[nRuleKey].kOpcodes[nLocalRuleCommand];

            /* Process the Comparison (we don't need to do anything if it's an unconditional jump) */
            if (ECOMMAND_COMPARE == kRule->eCommand)
            {
                if ((nRuleKeySource == kRule->kOperand.nKey) && (nRuleCommand == nLocalRuleCommand))
                {
                    if (ECOMPARISON_GT == kRule->kOperand.eComparison)
                    {
                        kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMin = MAX(kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMin, kRule->kOperand.nValue + 1);
                    }
                    else
                    {
                        kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMax = MIN(kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMax, kRule->kOperand.nValue - 1);
                    }
                }
                else
                {
                    if (ECOMPARISON_LT == kRule->kOperand.eComparison)
                    {
                        kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMin = MAX(kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMin, kRule->kOperand.nValue);
                    }
                    else
                    {
                        kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMax = MIN(kLocalValidRange.kValidRanges[kRule->kOperand.eOperand].nMax, kRule->kOperand.nValue);
                    }
                }
            }

            /* This path is now impossible... */
            if (kLocalValidRange.kValidRanges[EREGISTER_X].nMin > kLocalValidRange.kValidRanges[EREGISTER_X].nMax)
            {
                return;
            }

            if (0 == nLocalRuleCommand)
            {
                break;
            }
            --nLocalRuleCommand;
        }

        /* We've reached the rule we actually care about */
        if (nRuleKey == nTargetNode)
        {
            /* Store the Valid Range */
            kValidRanges->kValidRanges[kValidRanges->nValidRangeCount] = kLocalValidRange;
            ++kValidRanges->nValidRangeCount;
            break;
        }

        /* Iterate through all the possible ways we could have gotten to this node */
        for (nBackTrace = 0; nBackTrace < kBackTraces[nRuleKey].nBackTraceCount; ++nBackTrace)
        {
            /* Recurse */
            getValidRanges(kBackTraces[nRuleKey].kBackTraces[nBackTrace].nKey, nRuleKey, kBackTraces[nRuleKey].kBackTraces[nBackTrace].nRule, kRules, kBackTraces, nTargetNode, kLocalValidRange, kValidRanges);
        }

        break;
    }
}

uint64_t calculatePermuations(const SValidRegisterRangesType* kValidRange)
{
    return (kValidRange->kValidRanges[EREGISTER_X].nMax - kValidRange->kValidRanges[EREGISTER_X].nMin + 1) *
           (kValidRange->kValidRanges[EREGISTER_M].nMax - kValidRange->kValidRanges[EREGISTER_M].nMin + 1) *
           (kValidRange->kValidRanges[EREGISTER_A].nMax - kValidRange->kValidRanges[EREGISTER_A].nMin + 1) *
           (kValidRange->kValidRanges[EREGISTER_S].nMax - kValidRange->kValidRanges[EREGISTER_S].nMin + 1);
}

void getOverlap(const SValidRegisterRangesType* kRangeOne, const SValidRegisterRangesType* kRangeTwo, SValidRegisterRangesType* kOverlap)
{
    kOverlap->kValidRanges[EREGISTER_X].nMin = MAX(kRangeOne->kValidRanges[EREGISTER_X].nMin, kRangeTwo->kValidRanges[EREGISTER_X].nMin);
    kOverlap->kValidRanges[EREGISTER_X].nMax = MIN(kRangeOne->kValidRanges[EREGISTER_X].nMax, kRangeTwo->kValidRanges[EREGISTER_X].nMax);
    kOverlap->kValidRanges[EREGISTER_M].nMin = MAX(kRangeOne->kValidRanges[EREGISTER_M].nMin, kRangeTwo->kValidRanges[EREGISTER_M].nMin);
    kOverlap->kValidRanges[EREGISTER_M].nMax = MIN(kRangeOne->kValidRanges[EREGISTER_M].nMax, kRangeTwo->kValidRanges[EREGISTER_M].nMax);
    kOverlap->kValidRanges[EREGISTER_A].nMin = MAX(kRangeOne->kValidRanges[EREGISTER_A].nMin, kRangeTwo->kValidRanges[EREGISTER_A].nMin);
    kOverlap->kValidRanges[EREGISTER_A].nMax = MIN(kRangeOne->kValidRanges[EREGISTER_A].nMax, kRangeTwo->kValidRanges[EREGISTER_A].nMax);
    kOverlap->kValidRanges[EREGISTER_S].nMin = MAX(kRangeOne->kValidRanges[EREGISTER_S].nMin, kRangeTwo->kValidRanges[EREGISTER_S].nMin);
    kOverlap->kValidRanges[EREGISTER_S].nMax = MIN(kRangeOne->kValidRanges[EREGISTER_S].nMax, kRangeTwo->kValidRanges[EREGISTER_S].nMax);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        const uint64_t                  nStartKey   = getKeyFromConst("in");
        const uint64_t                  nEndKey     = getKeyFromConst("A");
        const SValidRegisterRangesType  kStartRange = { { { 1, 4000 }, { 1, 4000 }, { 1, 4000 }, { 1, 4000 } } };

        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;

        SRuleType*              kRules;
        SPartType*              kParts;
        size_t                  nPartCount;
        size_t                  nPart       = 0;

        SBackTracesTypes*       kBackTraces;
        SValidRangesType        kValidRanges;
        size_t                  nRange;

        uint64_t*               kUniqueKeys;
        size_t                  nUniqueKeyCount = 0;

        uint64_t                nPartOneResult = 0;
        int64_t                 nPartTwoResult = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Rule/Backtrace Cache */
        kRules      = (SRuleType*)malloc(RULE_CACHE_SIZE * sizeof(SRuleType));
        kBackTraces = (SBackTracesTypes*)malloc(RULE_CACHE_SIZE * sizeof(SBackTracesTypes));
        kUniqueKeys = (uint64_t*)malloc(RULE_CACHE_SIZE * sizeof(uint64_t));

        for (nLine = 0; nLine < RULE_CACHE_SIZE; ++nLine)
        {
            kBackTraces[nLine].nBackTraceCount = 0;
        }

        /* Allocate the Valid Ranges */
        kValidRanges.nValidRangeCount   = 0;
        kValidRanges.kValidRanges       = (SValidRegisterRangesType*)malloc(nLineCount * sizeof(SValidRegisterRangesType));

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            size_t   nRuleCount = 0;
            uint64_t nKey;
            uint64_t nJumpKey;
            char*    pItem = kLines[nLine];
            char*    pComparison;

            /* Terminate once we've reached a blank line */
            if (kLines[nLine][0] == '\0')
            {
                break;
            }

            /* Get the Key */
            nKey = getKey(&pItem);
            assert(nKey < RULE_CACHE_SIZE);

            /* Store the Unique Keys */
            kUniqueKeys[nUniqueKeyCount++] = nKey;

            /* Move past the brace*/
            assert(*pItem == '{');
            ++pItem;

            /* Keep looping whilst there's a comparison to process */
            while ((pComparison = strstr(pItem, ":")))
            {
                const ERegisterType    eRegister   = getRegister(pItem[0]);
                const ECComparisonType eComparison = getComparison(pItem[1]);
                      uint64_t         nComparison;
                      uint64_t         nTrueKey;
                pItem       += 2;
                nComparison  = getValue(&pItem);
                assert(*pItem == ':');
                ++pItem;

                /* At the very least there's a True Key */
                nTrueKey = getKey(&pItem);
                assert(*pItem == ',');
                ++pItem;

                /* Store the Backtrace */
                if (0 == kBackTraces[nTrueKey].nBackTraceCount)
                {
                    kBackTraces[nTrueKey].kBackTraces = (SBackTraceType*)malloc(nLineCount * sizeof(SBackTraceType));
                }
                kBackTraces[nTrueKey].kBackTraces[kBackTraces[nTrueKey].nBackTraceCount].nKey  = nKey;
                kBackTraces[nTrueKey].kBackTraces[kBackTraces[nTrueKey].nBackTraceCount].nRule = nRuleCount;
                ++kBackTraces[nTrueKey].nBackTraceCount;

                /* Store the Rule */
                kRules[nKey].kOpcodes[nRuleCount].eCommand              = ECOMMAND_COMPARE;
                kRules[nKey].kOpcodes[nRuleCount].kOperand.eOperand     = eRegister;
                kRules[nKey].kOpcodes[nRuleCount].kOperand.eComparison  = eComparison;
                kRules[nKey].kOpcodes[nRuleCount].kOperand.nValue       = nComparison;
                kRules[nKey].kOpcodes[nRuleCount].kOperand.nKey         = nTrueKey;
                ++nRuleCount;
            }

            /* The Last Item should always be an unconditonal jump */
            nJumpKey = getKey(&pItem);
            kRules[nKey].kOpcodes[nRuleCount].eCommand      = ECOMMAND_JUMP;
            kRules[nKey].kOpcodes[nRuleCount].kOperand.nKey = nJumpKey;

            /* Store the Backtrace */
            if (0 == kBackTraces[nJumpKey].nBackTraceCount)
            {
                kBackTraces[nJumpKey].kBackTraces = (SBackTraceType*)malloc(nLineCount * sizeof(SBackTraceType));
            }
            kBackTraces[nJumpKey].kBackTraces[kBackTraces[nJumpKey].nBackTraceCount].nKey  = nKey;
            kBackTraces[nJumpKey].kBackTraces[kBackTraces[nJumpKey].nBackTraceCount].nRule = nRuleCount;
            ++kBackTraces[nJumpKey].nBackTraceCount;

            ++nRuleCount;

            /* Store the Rule Count */
            kRules[nKey].nRuleCount = nRuleCount;
        }

        ++nLine;

        /* Store the Accept/Reject Special Cases */
        kRules[RULE_ACCEPT].nRuleCount              = 1;
        kRules[RULE_ACCEPT].kOpcodes[0].eCommand    = ECOMMAND_ACCEPT;
        kRules[RULE_REJECT].nRuleCount              = 1;
        kRules[RULE_REJECT].kOpcodes[0].eCommand    = ECOMMAND_REJECT;

        /* Store the Unique Keys */
        kUniqueKeys[nUniqueKeyCount++] = RULE_ACCEPT;
        kUniqueKeys[nUniqueKeyCount++] = RULE_REJECT;

        /* Anything left in the Line Count is now a Part */
        nPartCount = nLineCount - nLine;
        kParts     = (SPartType*)malloc(nPartCount * sizeof(SPartType));

        /* Read the Parts */
        for (; nLine < nLineCount; ++nLine)
        {
            size_t nRegister;
            char* pItem = kLines[nLine];
            ERegisterType eRegister;
            assert(*pItem == '{');

            ++pItem;

            for (nRegister = 0; nRegister <= EREGISTER_S; ++nRegister)
            {
                eRegister = getRegister(pItem[0]);
                assert(nRegister == eRegister);
                assert(pItem[1] == '=');
                pItem += 2;

                kParts[nPart].nParts[eRegister] = getValue(&pItem);
                assert((*pItem == ',') || (*pItem == '}'));
                ++pItem;
            }

            ++nPart;
        }
        assert(nPart == nPartCount);

        /* Part 1 - Check the Part Totals for any Accepted Rule */
        for (nPart = 0; nPart < nPartCount; ++nPart)
        {
            if (AOC_TRUE == processPart(kRules, nStartKey, &kParts[nPart]))
            {
                nPartOneResult += getPartValue(&kParts[nPart]);
            }
        }

        printf("Part 1: %llu\n", nPartOneResult);

        /* Part 2 - Permutations */
        getValidRanges(nEndKey, nEndKey, 0, kRules, kBackTraces, nStartKey, kStartRange, &kValidRanges);

        for (nRange = 0; nRange < kValidRanges.nValidRangeCount; ++nRange)
        {
            size_t                      nOverLapRange;
            SValidRegisterRangesType    kOverlapRange;

            nPartTwoResult += (int64_t)calculatePermuations(&kValidRanges.kValidRanges[nRange]);

            for (nOverLapRange = nRange + 1; nOverLapRange < kValidRanges.nValidRangeCount; ++nOverLapRange)
            {
                /* Calculate Overlap */
                getOverlap(&kValidRanges.kValidRanges[nRange], &kValidRanges.kValidRanges[nOverLapRange], &kOverlapRange);
                if ((kOverlapRange.kValidRanges[EREGISTER_X].nMin <= kOverlapRange.kValidRanges[EREGISTER_X].nMax) &&
                    (kOverlapRange.kValidRanges[EREGISTER_M].nMin <= kOverlapRange.kValidRanges[EREGISTER_M].nMax) &&
                    (kOverlapRange.kValidRanges[EREGISTER_A].nMin <= kOverlapRange.kValidRanges[EREGISTER_A].nMax) &&
                    (kOverlapRange.kValidRanges[EREGISTER_S].nMin <= kOverlapRange.kValidRanges[EREGISTER_S].nMax))
                {
                    nPartTwoResult -= (int64_t)calculatePermuations(&kOverlapRange);
                }
            }
        }

        printf("Part 2: %lld\n", nPartTwoResult);

        /* Free the Backtraces */
        for (nLine = 0; nLine < nUniqueKeyCount; ++nLine)
        {
            if (kBackTraces[kUniqueKeys[nLine]].nBackTraceCount > 0)
            {
                free(kBackTraces[kUniqueKeys[nLine]].kBackTraces);
            }
        }

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kRules);
        free(kParts);
        free(kBackTraces);
        free(kValidRanges.kValidRanges);
        free(kUniqueKeys);
    }
 
    return 0;
}