#include <stdio.h>
#include <assert.h>

#define PART_1_SIZE (4)
#define PART_2_SIZE (14)

long GetUniqueOffset(const long BUFFER_SIZE, char* kBuffer, FILE** pData)
{
    long     kBufferTally[256]   = {0};
    long     nBufferNumUniques   = 0;
    long     nBufferPointer      = 0;

    long i;

    /* Load the Tally Buffer */
    for (i = 0; i < BUFFER_SIZE; ++i)
    {
        /* Buffer the Data of Interest */
        const char kData  = fgetc(*pData);
        const long nTally = ++kBufferTally[kData];
        kBuffer[nBufferPointer++] = kData;
        if (1 == nTally)
        {
            ++nBufferNumUniques;
        }
        else if (2 == nTally)
        {
            --nBufferNumUniques;
        }
    }

    /* Solve... */
    while (1)
    {
        /* Buffer the Data of Interest */
        const char kData = fgetc(*pData);

        /* Calculate the Position as part of the Circular Buffer */
        const long nCircularBufferPointer = nBufferPointer % BUFFER_SIZE;

        /* Buffer the Decremented Tally */
        const long nDecrementedTally = --kBufferTally[kBuffer[nCircularBufferPointer]];

        /* Buffer the Incremented Tally */
        const long nIncrementedTally = ++kBufferTally[kData];

        /* Update the Circular Buffer */
        kBuffer[nCircularBufferPointer] = kData;

        /* For the item we've pop'd, there's only two values of interest:
         * 1 : This means the value before was 2, and is now 1, meaning we
         *     need to increment the number of unique entries we've got.
         * 0 : This means the value before was 1, and is now 0, meaning we
         *     need to decrerement the number of unique entries we've got.
         * 
         * Any other number wouldn't change the number of unique entries, for
         * example, if the value was 2, it means that value before was 3, and
         * does not change how many unique entries there are, it just means
         * something that wasn't unique still isn't unique.
         */
        if (1 == nDecrementedTally)
        {
            ++nBufferNumUniques;
        }
        else if (0 == nDecrementedTally)
        {
            --nBufferNumUniques;
        }

        /* For the item we've push'd, there's only two values of interest:
         * 1 : This means the value before was 0, and is now 1, meaning we
         *     need to increment the number of unique entries we've got.
         * 2 : This means the value before was 1, and is now 2, meaning we
         *     need to decrerement the number of unique entries we've got.
         * 
         * Any other number wouldn't change the number of unique entries, for
         * example, if the value was 3, it means that value before was 2, and
         * does not change how many unique entries there are, it just means
         * something that wasn't unique still isn't unique.
         */
        if (1 == nIncrementedTally)
        {
            ++nBufferNumUniques;
        }
        else if (2 == nIncrementedTally)
        {
            --nBufferNumUniques;
        }

        ++nBufferPointer;

        /* Test if the Puzzle has been solved (i.e. the number of unique
         * as entries is the same size the slice size we're interested in)
         */
        if (nBufferNumUniques == BUFFER_SIZE)
        {
            return nBufferPointer;
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        char       kBufferPart1[PART_1_SIZE];
        char       kBufferPart2[PART_2_SIZE];
        const long nBufferPointerPart1 = GetUniqueOffset(PART_1_SIZE, kBufferPart1, &pData);
        const long nBufferPointerPart2 = GetUniqueOffset(PART_2_SIZE, kBufferPart2, &pData);

        printf("Part 1: %li\n", nBufferPointerPart1);
        printf("Part 2: %li\n", nBufferPointerPart1 + nBufferPointerPart2);

        fclose(pData);
    }
}