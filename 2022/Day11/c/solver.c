#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>

typedef enum OperationType
{
    OPERATION_ADD  = 0,
    OPERATION_MUL
} TOperationType;

typedef struct MonkeyType
{
    size_t          nItemCount;
    uint64_t*       kItems;
    uint64_t        nDivisor;
    TOperationType  eOperation;
    uint64_t        kOperations[2];
    size_t          kTargetMonkeys[2];
} TMonkeyType;

TOperationType getOperation(FILE** pFile)
{
    const char kData = fgetc(*pFile);
    fgetc(*pFile);

    if      ('+' == kData)
    {
        return OPERATION_ADD;
    }
    else if ('*' == kData)
    {
        return OPERATION_MUL;
    }
    assert(0);
}

long getValue(FILE** pFile, char* pkData)
{
    long nValue = 0;

    /* Handle the Value */
    while (isdigit(*pkData = fgetc(*pFile)))
    {
        nValue = (nValue * 10) + (*pkData - '0');
    }

    return nValue;
}

void skip(FILE** pFile, char* pkData)
{
    while((' ' != *pkData) && ('\n' != *pkData))
    {
        *pkData = fgetc(*pFile);
    }
}

uint64_t processOperation(const TOperationType eOperation, const uint64_t* kOperations, const uint64_t nItem)
{
    const uint64_t LEFT  = (0 == kOperations[0]) ? nItem : kOperations[0];
    const uint64_t RIGHT = (0 == kOperations[1]) ? nItem : kOperations[1];

    if (OPERATION_ADD == eOperation)
    {
        return LEFT + RIGHT;
    }
    else
    {
        return LEFT * RIGHT;
    }

    assert(0);
}

uint64_t processMonkeys(const TMonkeyType* pkMonkeys, const size_t nMonkeyCount, const size_t MAX_ITEMS, const size_t nRounds, const uint64_t nDivisor, const uint64_t nModulo)
{
    TMonkeyType* kMonkeys              = (TMonkeyType*)malloc(nMonkeyCount * sizeof(TMonkeyType));
    uint64_t*    kMonkeyActivity       = (uint64_t*)malloc(nMonkeyCount * sizeof(uint64_t));
    uint64_t     kMaxMonkeyActivity[2] = {0};
    size_t       nMonkey;
    size_t       nRound;

    /* Copy the Monkies */
    for (nMonkey = 0; nMonkey < nMonkeyCount; ++nMonkey)
    {
        memcpy(&kMonkeys[nMonkey], &pkMonkeys[nMonkey], sizeof(TMonkeyType));
        kMonkeys[nMonkey].kItems = (uint64_t*)malloc(MAX_ITEMS * sizeof(uint64_t));
        memcpy(kMonkeys[nMonkey].kItems, pkMonkeys[nMonkey].kItems, kMonkeys[nMonkey].nItemCount * sizeof(uint64_t));
        kMonkeyActivity[nMonkey] = 0;
    }

    /* Process the Monkies */
    for (nRound = 0; nRound < nRounds; ++nRound)
    {
        for (nMonkey = 0; nMonkey < nMonkeyCount; ++nMonkey)
        {
            size_t nItem;

            /* Increment the Activity Count */
            kMonkeyActivity[nMonkey] += kMonkeys[nMonkey].nItemCount;

            /* Process all of the items... */
            for (nItem = 0; nItem < kMonkeys[nMonkey].nItemCount; ++nItem)
            {
                /* Process the Current Item */
                uint64_t nNewItemValue = processOperation(kMonkeys[nMonkey].eOperation,
                                                          kMonkeys[nMonkey].kOperations,
                                                          kMonkeys[nMonkey].kItems[nItem]) / nDivisor;
                if (nModulo) nNewItemValue = nNewItemValue % nModulo;

                /* Throw the Item to the Appropriate Monkey */
                if (0 == (nNewItemValue % kMonkeys[nMonkey].nDivisor))
                {
                    const size_t nTargetMonkey = kMonkeys[nMonkey].kTargetMonkeys[1];
                    kMonkeys[nTargetMonkey].kItems[kMonkeys[nTargetMonkey].nItemCount++] = nNewItemValue;
                }
                else
                {
                    const size_t nTargetMonkey = kMonkeys[nMonkey].kTargetMonkeys[0];
                    kMonkeys[nTargetMonkey].kItems[kMonkeys[nTargetMonkey].nItemCount++] = nNewItemValue;
                }
            }

            /* The Current Monkey has known thrown all of its items */
            kMonkeys[nMonkey].nItemCount = 0;
        }
    }

    /* De-allocate the Monkey Copy/Determine the Most Active Monkeys */
    for (nMonkey = 0; nMonkey < nMonkeyCount; ++nMonkey)
    {
        if (kMonkeyActivity[nMonkey] > kMaxMonkeyActivity[0])
        {
            kMaxMonkeyActivity[1] = kMaxMonkeyActivity[0];
            kMaxMonkeyActivity[0] = kMonkeyActivity[nMonkey];
        }
        else if (kMonkeyActivity[nMonkey] > kMaxMonkeyActivity[1])
        {
            kMaxMonkeyActivity[1] = kMonkeyActivity[nMonkey];
        }

        free(kMonkeys[nMonkey].kItems);
    }
    free(kMonkeys);
    free(kMonkeyActivity);

    return kMaxMonkeyActivity[0] * kMaxMonkeyActivity[1];
}

#define LINE_1_LITERAL "Monkey "
#define LINE_2_LITERAL "  Starting items: "
#define LINE_3_LITERAL "  Operation: new = "
#define LINE_4_LITERAL "  Test: divisible by "
#define LINE_5_LITERAL "    If true: throw to monkey "
#define LINE_6_LITERAL "    If false: throw to monkey "
#define LINE_7_LITERAL ""

/* sizeof is resolved at compile time, not run-time, but *will* include the NUL
 * terminator.
 */
#define LITERAL_STRING_LENGTH(X) ((sizeof(X) / sizeof(char)) - 1)

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        size_t          MAX_MONKEYS;
        size_t          MAX_ITEMS;

        TMonkeyType*    kMonkeys;

        size_t          nLine = 0;
        size_t          nSize = 0;

        size_t          nMonkey;
        size_t          nMonkeyCount;
        uint64_t        nLCM  = 1;

        fseek(pFile, 0, SEEK_END);
        nSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        /* Divide the Size by the Maximum Number of Monkeys that could be squeezed in:
         * Monkey                        (7 bytes)
         *   Starting items:             (17 bytes)
         *   Operation: new =            (18 bytes)
         *   Test: divisible by          (21 bytes)
         *     If true: throw to monkey  (29 bytes)
         *     If false: throw to monkey (30 bytes)
         */
        MAX_MONKEYS = nSize / (LITERAL_STRING_LENGTH(LINE_1_LITERAL)+1 +
                               LITERAL_STRING_LENGTH(LINE_2_LITERAL)+1 +
                               LITERAL_STRING_LENGTH(LINE_3_LITERAL)+1 +
                               LITERAL_STRING_LENGTH(LINE_4_LITERAL)+1 +
                               LITERAL_STRING_LENGTH(LINE_5_LITERAL)+1 +
                               LITERAL_STRING_LENGTH(LINE_6_LITERAL)+1 +
                               LITERAL_STRING_LENGTH(LINE_7_LITERAL)+1);
        MAX_ITEMS   = nSize;

        /* Allocate / Initialise the Monkeys */
        kMonkeys = (TMonkeyType*)malloc(MAX_MONKEYS * sizeof(TMonkeyType));
        memset(kMonkeys, 0, MAX_MONKEYS * sizeof(TMonkeyType));

        /* Parse the Monkeys */
        nMonkeyCount = 0;
        while (!feof(pFile))
        {
            char kData = fgetc(pFile);

            switch (nLine)            
            {
                case 1:
                {
                    /* Skip the Pre-amble "  Starting items: " */
                    fseek(pFile, LITERAL_STRING_LENGTH(LINE_2_LITERAL)-1, SEEK_CUR);

                    /* Allocate / Default the Items */
                    kMonkeys[nMonkeyCount].kItems     = malloc(MAX_ITEMS * sizeof(uint64_t));
                    kMonkeys[nMonkeyCount].nItemCount = 0;

                    /* Store the Items */
                    while ('\n' != kData)
                    {
                        const uint64_t nData = getValue(&pFile, &kData);
                        if (nData)
                        {
                            kMonkeys[nMonkeyCount].kItems[kMonkeys[nMonkeyCount].nItemCount++] = nData;
                        }
                    }
                } break;

                case 2:
                {
                    /* Skip the Pre-amble "  Operation: new = " */
                    fseek(pFile, LITERAL_STRING_LENGTH(LINE_3_LITERAL)-1, SEEK_CUR);

                    /* Store the Operation */
                    kMonkeys[nMonkeyCount].kOperations[0] = getValue(&pFile, &kData);
                    skip(&pFile, &kData);
                    kMonkeys[nMonkeyCount].eOperation     = getOperation(&pFile);
                    kMonkeys[nMonkeyCount].kOperations[1] = getValue(&pFile, &kData);
                    skip(&pFile, &kData);
                } break;

                case 3:
                {
                    /* Skip the Pre-amble "  Test: divisible by " */
                    fseek(pFile, LITERAL_STRING_LENGTH(LINE_4_LITERAL)-1, SEEK_CUR);

                    /* Store the Divisor */
                    kMonkeys[nMonkeyCount].nDivisor = getValue(&pFile, &kData);
                } break;

                case 4:
                {
                    /* Skip the Pre-amble "    If true: throw to monkey " */
                    fseek(pFile, LITERAL_STRING_LENGTH(LINE_5_LITERAL)-1, SEEK_CUR);

                    /* Store the True Monkey */
                    kMonkeys[nMonkeyCount].kTargetMonkeys[1] = getValue(&pFile, &kData);
                } break;

                case 5:
                {
                    /* Skip the Pre-amble "    If false: throw to monkey " */
                    fseek(pFile, LITERAL_STRING_LENGTH(LINE_6_LITERAL)-1, SEEK_CUR);

                    /* Store the False Monkey */
                    kMonkeys[nMonkeyCount].kTargetMonkeys[0] = getValue(&pFile, &kData);

                    ++nMonkeyCount;
                } break;

                default: break;
            }

            if ('\n' == kData)
            {
                nLine = (nLine + 1) % 7;
            }
        }

        /* Close the File */
        fclose(pFile);

        /* Process Part 1 */
        printf("Part 1: %llu\n", processMonkeys(kMonkeys, nMonkeyCount, MAX_ITEMS, 20, 3, 0));

        /* Process Part 2 */

        /* Calculate the LCM */
        for (nMonkey = 0; nMonkey < nMonkeyCount; ++nMonkey)
        {
            nLCM *= kMonkeys[nMonkey].nDivisor;
        }
        printf("Part 2: %llu\n", processMonkeys(kMonkeys, nMonkeyCount, MAX_ITEMS, 10000, 1, nLCM));

        /* Clean Up */
        for (nMonkey = 0; nMonkey < MAX_MONKEYS; ++nMonkey)
        {
            if (kMonkeys[nMonkey].kItems)
            {
                free(kMonkeys[nMonkey].kItems);
            }
        }
        free(kMonkeys);
    }

    return 0;
}
