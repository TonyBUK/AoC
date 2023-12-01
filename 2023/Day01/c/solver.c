#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

void ProcessDigit(const char* kBuffer, unsigned* bFirst, unsigned long* nFirst, unsigned long* nLast)
{
    if ((kBuffer[0] >= '0') && (kBuffer[0] <= '9'))
    {
        if (*bFirst)
        {
            *nFirst = kBuffer[0] - '0';
            *nLast  = *nFirst;
            *bFirst = AOC_FALSE;
        }
        else
        {
            *nLast = kBuffer[0] - '0';
        }
    }
}

void ProcessDigits(char* kBuffer, const size_t nLength, unsigned long* nPartOneCalibrationSum, unsigned long* nPartTwoCalibrationSum)
{
    const char    DIGITS[]          = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    const char*   DIGITS_AS_WORDS[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    unsigned      bFirstPartOne = AOC_TRUE;
    unsigned      bFirstPartTwo = AOC_TRUE;
    unsigned long nFirstPartOne, nLastPartOne, nFirstPartTwo, nLastPartTwo;
    size_t i;

    for (i = 0; i < nLength; ++i)
    {
        size_t j;

        ProcessDigit(&kBuffer[i], &bFirstPartOne, &nFirstPartOne, &nLastPartOne);

        for (j = 0; j < (sizeof(DIGITS_AS_WORDS) / sizeof(char*)); ++j)
        {
            if (strstr(&kBuffer[i], DIGITS_AS_WORDS[j]) == &kBuffer[i])
            {
                kBuffer[i]      = DIGITS[j];
                break;
            }
        }

        ProcessDigit(&kBuffer[i], &bFirstPartTwo, &nFirstPartTwo, &nLastPartTwo);
    }
    assert(AOC_FALSE == bFirstPartOne);
    assert(AOC_FALSE == bFirstPartTwo);

    *nPartOneCalibrationSum += (nFirstPartOne * 10u) + nLastPartOne;
    *nPartTwoCalibrationSum += (nFirstPartTwo * 10u) + nLastPartTwo;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char* kBuffer = NULL;

        unsigned long nPart1CalibrationSum = 0;
        unsigned long nPart2CalibrationSum = 0;
        size_t        nBufferLength;

        while (-1 != getline(&kBuffer, &nBufferLength, pData))
        {
            ProcessDigits(kBuffer, strlen(kBuffer), &nPart1CalibrationSum, &nPart2CalibrationSum);
        }

        free(kBuffer);
        fclose(pData);

        printf("Part 1: %lu\n", nPart1CalibrationSum);
        printf("Part 2: %lu\n", nPart2CalibrationSum);
    }
 
    return 0;
}