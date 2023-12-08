#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

typedef struct tHand
{
    int64_t  nHandStrength; /* Signed makes compareHands simpler... */
    uint64_t nBet;
} tHand;

int compareHands (const void* a, const void* b)
{
    const tHand* pLeft =  (const tHand*)a;
    const tHand* pRight = (const tHand*)b;
    return (int)(pLeft->nHandStrength - pRight->nHandStrength);
}
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

uint64_t calculateStrength(const uint64_t nInitialStrength, const char* kRawHand, const unsigned bJoker)
{
    /* This indexes the value of a card by its ASCII value to prevent searching.
     * It's the equivalent of:
     *
     * CARDS[0]['2'] = 0;
     * CARDS[0]['3'] = 1;
     * CARDS[0]['4'] = 2;
     * CARDS[0]['5'] = 3;
     * CARDS[0]['6'] = 4;
     * CARDS[0]['7'] = 5;
     * CARDS[0]['8'] = 6;
     * CARDS[0]['9'] = 7;
     * CARDS[0]['T'] = 8;
     * CARDS[0]['J'] = 9;
     * CARDS[0]['Q'] = 10;
     * CARDS[0]['K'] = 11;
     * CARDS[0]['A'] = 12;
     *
     * CARDS[1]['J'] = 0;
     * CARDS[1]['2'] = 1;
     * CARDS[1]['3'] = 2;
     * CARDS[1]['4'] = 3;
     * CARDS[1]['5'] = 4;
     * CARDS[1]['6'] = 5;
     * CARDS[1]['7'] = 6;
     * CARDS[1]['8'] = 7;
     * CARDS[1]['9'] = 8;
     * CARDS[1]['T'] = 9;
     * CARDS[1]['Q'] = 10;
     * CARDS[1]['K'] = 11;
     * CARDS[1]['A'] = 12;
     *
     * I do this to support older compilers that don't support designated initializers.
     */
    const uint64_t CARDS[][128] = {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 9, 11, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 10, 0xFF, 0xFF, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 1, 2, 3, 4, 5, 6, 7, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 11, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 10, 0xFF, 0xFF, 9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
    };
    const uint64_t BASE      = 13;
    uint64_t       nStrength = nInitialStrength;
    size_t   i;

    assert(strlen(kRawHand) == 5);
    for (i = 0; i < 5; ++i)
    {
        nStrength = (nStrength * BASE) + CARDS[bJoker][kRawHand[i]];
    }

    return nStrength;
}

size_t getRepetition(const char* kRawHand, char kCard)
{
    size_t i;
    size_t nCount = 0;

    assert(strlen(kRawHand) == 5);
    for (i = 0; i < 5; ++i)
    {
        if (kRawHand[i] == kCard)
        {
            ++nCount;
        }
    }

    return nCount;
}

void processHand(char* kRawHand, tHand* pHand, const unsigned bJoker)
{
    size_t   i;
    char     kProcessed[6]    = {'\0'};
    char     kCurrent[2]      = {'\0', '\0'};
    char     kPlayedHandBuffer[6];
    char*    kPlayedHand      = kRawHand;
    size_t   kRepitions[2]    = {0, 0};
    size_t   nPeakRepetitions = 0;
    uint64_t nStrength;

    assert(strlen(kRawHand) == 5);

    /* Process the Joker (substituting the most frequent value by the Joker) */
    if (AOC_TRUE == bJoker)
    {
        if (strstr(kRawHand, "J"))
        {
            char kPotentialHand[6] = {'\0', '\0', '\0', '\0', '\0', '\0'};
            kPlayedHand = kPlayedHandBuffer;
            for (i = 0; i < 5; ++i)
            {
                size_t j;
                size_t nRepetitions;

                /* Skip Jokers */
                if (kRawHand[i] == 'J') continue;

                /* Skip items we've processed */
                kCurrent[0] = kRawHand[i];
                if (strstr(kProcessed, kCurrent)) continue;

                /* Prepare the Hand... */
                nRepetitions = 0;
                for (j = 0; j < 5; ++j)
                {
                    if (kRawHand[j] == 'J')
                    {
                        kPotentialHand[j] = kRawHand[i];
                    }
                    else
                    {
                        if (kRawHand[j] == kRawHand[i])
                        {
                            ++nRepetitions;
                        }
                        kPotentialHand[j] = kRawHand[j];
                    }
                }

                if (nRepetitions > nPeakRepetitions)
                {
                    nPeakRepetitions = nRepetitions;
                    strcpy(kPlayedHand, kPotentialHand);
                }

                strcat(kProcessed, kCurrent);
            }
        }
    }

    /* Calculate the Top 2 Repetition Counts for the Hand, which is enough to uniquely
     * identify what hand it is.
     */
    kProcessed[0] = '\0';
    for (i = 0; i < 5; ++i)
    {
        kCurrent[0] = kPlayedHand[i];
        if (NULL == strstr(kProcessed, kCurrent))
        {
            const size_t nCount = getRepetition(kPlayedHand, kPlayedHand[i]);

            if (nCount > kRepitions[0])
            {
                kRepitions[1] = kRepitions[0];
                kRepitions[0] = nCount;
            }
            else if (nCount > kRepitions[1])
            {
                kRepitions[1] = nCount;
            }

            strcat(kProcessed, kCurrent);
        }
    }

    /* Calculate the Strength of the Hand Itself, and the
     * numerical value given.
     *
     * From weakest to Strongest
     * 
     *  kRepetitions[0]   kRepetitions[1]  Hand
     *  ================:=================:=================
     *  1               : X               : High Card       (1)
     *  2               : 1               : One Pair        (2)
     *  2               : 2               : Two Pair        (3)
     *  3               : 1               : Three of a Kind (4)
     *  3               : 2               : Full House      (5)
     *  4               : X               : Four of a Kind  (6)
     *  5               : X               : Five of a Kind  (7)
     */
    if (kRepitions[0] >= 4)
    {
        nStrength = kRepitions[0] + 2;
    }
    else if (kRepitions[0] == 1)
    {
        nStrength = 1;
    }
    else if (kRepitions[0] == 3)
    {
        nStrength = kRepitions[0] + ((kRepitions[1] == 2) ? 2 : 1);
    }
    else if (kRepitions[0] == 2)
    {
        nStrength = kRepitions[0] + ((kRepitions[1] == 2) ? 1 : 0);
    }

    pHand->nHandStrength = calculateStrength(nStrength, kRawHand, bJoker);
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

        tHand*                  kHandsPartOne;
        tHand*                  kHandsPartTwo;

        uint64_t                nPartOneScore = 0;
        uint64_t                nPartTwoScore = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pData);

        kHandsPartOne = (tHand*)malloc(nLineCount * sizeof(tHand));
        kHandsPartTwo = (tHand*)malloc(nLineCount * sizeof(tHand));

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            /* Store the Bet */
            char* pBet = strstr(kLines[nLine], " ");
            assert(pBet);
            kHandsPartOne[nLine].nBet = strtoll(&pBet[1], NULL, 10);
            kHandsPartTwo[nLine].nBet = kHandsPartOne[nLine].nBet;

            /* Abuse the Nul Terminator to make each line only show the hand */
            pBet[0] = '\0';

            /* Process the Hand */
            processHand(kLines[nLine], &kHandsPartOne[nLine], AOC_FALSE);
            processHand(kLines[nLine], &kHandsPartTwo[nLine], AOC_TRUE);
        }

        qsort(kHandsPartOne, nLineCount, sizeof(tHand), compareHands);
        qsort(kHandsPartTwo, nLineCount, sizeof(tHand), compareHands);

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            nPartOneScore += (nLine + 1) * kHandsPartOne[nLine].nBet;
            nPartTwoScore += (nLine + 1) * kHandsPartTwo[nLine].nBet;
        }

        printf("Part 1: %llu\n", nPartOneScore);
        printf("Part 2: %llu\n", nPartTwoScore);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kHandsPartOne);
        free(kHandsPartTwo);
    }
 
    return 0;
}