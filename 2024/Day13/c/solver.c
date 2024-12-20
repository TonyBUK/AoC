#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

uint64_t countRounds(const uint64_t nXA, const uint64_t nYA, const uint64_t nXB, const uint64_t nYB, const uint64_t nPrizeA, const uint64_t nPrizeB)
{
    /*
     * Pen and Paper Time
     *
     * We start with the following simultaneous equations:
     *
     * X1 * a + X2 * b = P1
     * Y1 * a + Y2 * b = P2
     *
     * We can re-arrange these to:
     *
     * b = P1 - X1 * a
     *     -----------
     *         X2
     *
     * We can then b in the second equation, and after some algebra shuffling we get:
     *
     * a = Y2 * P1 - X2 * P2
     *     -----------------
     *     Y2 * X1 - X2 * Y1
     *
     * We could also do the reverse for b, but we don't need to as if we've solved a, b is trivial.
    */

    /* Solve a: Whole Numbers Only */
    const int64_t nANum = (int64_t)nYB * (int64_t)nPrizeA - (int64_t)nXB * (int64_t)nPrizeB;
    const int64_t nADem = (int64_t)nYB * (int64_t)nXA - (int64_t)nXB * (int64_t)nYA;
    if (nANum % nADem != 0)
    {
        return 0;
    }
    else
    {
        const uint64_t nA = (uint64_t)(nANum / nADem);

        /* Solve b: Whole Numbers Only */
        const int64_t nBNum = (int64_t)nPrizeA - (int64_t)nXA * (int64_t)nA;
        const int64_t nBDem = (int64_t)nXB;
        if (nBNum % nBDem != 0)
        {
            return 0;
        }
        else
        {
            const uint64_t nB = (uint64_t)(nBNum / nBDem);

            /* Return the Result */
            return 3 * nA + nB;
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*  kLine = NULL;
        size_t nLength;

        uint64_t nXA;
        uint64_t nYA;
        uint64_t nXB;
        uint64_t nYB;
        uint64_t nPrizeA;
        uint64_t nPrizeB;
        
        size_t   nLine = 0;

        uint64_t nRoundsPartOne = 0;
        uint64_t nRoundsPartTwo = 0;

        while(getline(&kLine, &nLength, pData) != -1)
        {
            switch(nLine)
            {
                case 0: /* Button A */
                {
                    const char* kX = strstr(kLine, "+");
                    assert(kX);
                    const char* kY = strstr(kX + 1, "+");
                    assert(kY);
                    nXA = strtoull(kX+1, NULL, 10);
                    nYA = strtoull(kY+1, NULL, 10);
                    ++nLine;
                } break;

                case 1: /* Button B */
                {
                    const char* kX = strstr(kLine, "+");
                    assert(kX);
                    const char* kY = strstr(kX + 1, "+");
                    assert(kY);
                    nXB = strtoull(kX+1, NULL, 10);
                    nYB = strtoull(kY+1, NULL, 10);
                    ++nLine;
                } break;

                case 2: /* Prize */
                {
                    const char* kX = strstr(kLine, "=");
                    assert(kX);
                    const char* kY = strstr(kX + 1, "=");
                    assert(kY);
                    nPrizeA = strtoull(kX+1, NULL, 10);
                    nPrizeB = strtoull(kY+1, NULL, 10);

                    nRoundsPartOne += countRounds(nXA, nYA, nXB, nYB, nPrizeA, nPrizeB);
                    nRoundsPartTwo += countRounds(nXA, nYA, nXB, nYB, nPrizeA + 10000000000000llu, nPrizeB + 10000000000000llu);

                    ++nLine;
                } break;

                case 3: /* Blank */
                {
                    nLine = 0;
                } break;
            }
        }

        printf("Part One: %" PRIu64 "\n", nRoundsPartOne);
        printf("Part Two: %" PRIu64 "\n", nRoundsPartTwo);

        free(kLine);
    }
 
    return 0;
}