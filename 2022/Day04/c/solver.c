#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

long GetValue(FILE** pFile)
{
    long nValue = 0;
    char kDigit;
    while (isdigit(kDigit = fgetc(*pFile)))
    {
        nValue = nValue * 10 + (kDigit - '0');
    }

    return nValue;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        unsigned long nPart1 = 0;
        unsigned long nPart2 = 0;

        while (!feof(pData))
        {
            /* Read the Four Elements to the Elf Pairs.
             * Note: We can abuse the fact the GetValue function will read one byte *past* the digit
            *        due to the validity check, as since the delimiters are:
            *        -
            *        ,
            *        \n (since we're reading in ASCII mode this should always be single byte)
            *        We get around having to tokenize/test/split.
             */
            const long nPair1Start = GetValue(&pData);
            const long nPair1End   = GetValue(&pData);
            const long nPair2Start = GetValue(&pData);
            const long nPair2End   = GetValue(&pData);

            /* Part 1: Test if one Pair exists entirely within the other */
            if (((nPair1Start >= nPair2Start) && (nPair1End <= nPair2End)) ||
                ((nPair2Start >= nPair1Start) && (nPair2End <= nPair1End)))
            {
                ++nPart1;
                ++nPart2;
            }
            /* Part 2: Test if the Pairs even partially intersect.
             *         Note: We can skip this if Part 1 already triggered since that
                             would also trigger this.
             */
            else if (((nPair1Start >= nPair2Start) && (nPair1Start <= nPair2End)) ||
                     ((nPair1End   >= nPair2Start) && (nPair1End   <= nPair2End)))
            {
                ++nPart2;
            }
        }

        printf("Part 1: %lu\n", nPart1);
        printf("Part 2: %lu\n", nPart2);
 
        fclose(pData);
    }
 
    return 0;
}