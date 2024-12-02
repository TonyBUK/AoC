#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

#define ABSDIFF(x,y) ((x) > (y) ? (x) - (y) : (y) - (x))

uint32_t GetNextValue(FILE** pData, unsigned* bEOL)
{
    uint32_t nDestinationId = 0u;
    uint32_t nLength        = 0u;
 
    *bEOL = AOC_FALSE;

    while (!feof(*pData))
    {
        char kData = fgetc(*pData);
 
        if ((' ' == kData) && (0 == nLength))
        {
            continue;
        }

        if ((' ' == kData) || ('\n' == kData) || !!feof(*pData))
        {
            if (0 == nLength)
            {
                return 0xFFFFFFFFu;
            }
            else
            {
                *bEOL = (' ' != kData) ? AOC_TRUE : AOC_FALSE;
                return nDestinationId;
            }
        }

        assert((kData >= '0') && (kData <= '9'));
        nDestinationId = (nDestinationId * 10u) + (kData - '0');
        ++nLength;
    }
 
    return 0xFFFFFFFFu;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        uint64_t nSafeCountPartOne  = 0u;
        uint64_t nSafeCountPartTwo  = 0u;

        while (!feof(pData))
        {
            unsigned bEOL;
            unsigned bFirst         = AOC_TRUE;
            unsigned bSafePartOne   = AOC_TRUE;
            unsigned bSafePartTwo   = AOC_TRUE;
            unsigned bSafe          = AOC_TRUE;
            unsigned bDampened      = AOC_FALSE;

            // At any given time we need the last four values
            // to determine direction and skipping without having
            // to guess or store the entire list.
            uint32_t kValueBuffer[4];
            uint8_t  nBufferCount = 0u;

            // Note: This does depend on there being at least 4 values...

            do
            {            
                const uint32_t nValue = GetNextValue(&pData, &bEOL);

                // Append to the Buffer
                if (nBufferCount < 4)
                {
                    kValueBuffer[nBufferCount++] = nValue;
                    if (nBufferCount < 4)
                    {
                        continue;
                    }
                }
                else
                {
                    kValueBuffer[0] = kValueBuffer[1];
                    kValueBuffer[1] = kValueBuffer[2];
                    kValueBuffer[2] = kValueBuffer[3];
                    kValueBuffer[3] = nValue;
                }

                // Determine the Direction is safe
                const unsigned bDirection1 = kValueBuffer[0] < kValueBuffer[1];
                const unsigned bDirection2 = kValueBuffer[1] < kValueBuffer[2];
                const unsigned bDirection3 = kValueBuffer[2] < kValueBuffer[3];
                bSafe = (bDirection1 == bDirection2) && (bDirection2 == bDirection3);

                // Determine the Deltas are safe
                const uint32_t nDelta1 = ABSDIFF(kValueBuffer[0], kValueBuffer[1]);
                const uint32_t nDelta2 = ABSDIFF(kValueBuffer[1], kValueBuffer[2]);
                const uint32_t nDelta3 = ABSDIFF(kValueBuffer[2], kValueBuffer[3]);
                const unsigned bDelta1 = (nDelta1 >= 1 && nDelta1 <= 3);
                const unsigned bDelta2 = (nDelta2 >= 1 && nDelta2 <= 3);
                const unsigned bDelta3 = (nDelta3 >= 1 && nDelta3 <= 3);
                bSafe &= bDelta1 && bDelta2 && bDelta3;

                bSafePartOne &= bSafe;

                // A mismatch has been detected, so try and find one item to remove that fixes this.
                if (bSafe != bSafePartTwo)
                {
                    if (AOC_FALSE == bDampened)
                    {
                        unsigned bMidPointTest = AOC_FALSE;

                        if ((bDirection1 != bDirection2) || !bDelta1)
                        {
                            // Note: Because we've read ahead by 4, we can safely remove
                            //       the first element without checking, since the only
                            //       way this would be wrong is if we'd already shifted
                            //       the array, however, for that to have happened, we'd
                            //       have already evaluated it in the lookahead anyway.
                            if ((bDirection2 == bDirection3) && bDelta2)
                            {
                                // Problem is with the first element so just remove it
                                kValueBuffer[0] = kValueBuffer[1];
                                kValueBuffer[1] = kValueBuffer[2];
                                kValueBuffer[2] = kValueBuffer[3];
                                --nBufferCount;
                            }
                            else
                            {
                                bMidPointTest = AOC_TRUE;
                            }
                        }
                        else if (bDirection2 != bDirection3 || !bDelta3)
                        {
                            if ((bDirection1 == bDirection2) && bDelta2)
                            {
                                // Problem is with the last element so just remove it
                                --nBufferCount;
                            }
                            else
                            {
                                bMidPointTest = AOC_TRUE;
                            }
                        }
                        else // !bDelta2
                        {
                            bMidPointTest = AOC_TRUE;
                        }

                        if (bMidPointTest)
                        {
                            // This is a Mid-Point problem but we have two options
                            // to choose from.

                            // Option 1: Remove the Second Element
                            const uint32_t nSpeculativeDelta1 = ABSDIFF(kValueBuffer[0], kValueBuffer[2]);
                            const unsigned bSpeculativeDelta1 = (nSpeculativeDelta1 >= 1 && nSpeculativeDelta1 <= 3);
                            const unsigned bSpeculativeDirection1 = kValueBuffer[0] < kValueBuffer[2];

                            if ((bSpeculativeDirection1 == bDirection3) && bSpeculativeDelta1)
                            {
                                // Remove the Second Element
                                kValueBuffer[1] = kValueBuffer[2];
                                kValueBuffer[2] = kValueBuffer[3];
                                --nBufferCount;
                            }
                            else
                            {
                                // Option 2: Remove the Third Element
                                const uint32_t nSpeculativeDelta2 = ABSDIFF(kValueBuffer[0], kValueBuffer[3]);
                                const unsigned bSpeculativeDelta2 = (nSpeculativeDelta2 >= 1 && nSpeculativeDelta2 <= 3);
                                const unsigned bSpeculativeDirection2 = kValueBuffer[1] < kValueBuffer[3];

                                if ((bDirection1 == bSpeculativeDirection2) && bSpeculativeDelta2)
                                {
                                    // Remove the Third Element
                                    kValueBuffer[2] = kValueBuffer[3];
                                    --nBufferCount;
                                }
                                else
                                {
                                    bSafePartTwo = AOC_FALSE;
                                }
                            }
                        }

                        bDampened = AOC_TRUE;
                    }
                    else
                    {
                        // We've already dampened once, so this is not safe...
                        bSafePartTwo = AOC_FALSE;
                    }
                }
            } while (!bEOL);

            nSafeCountPartOne += bSafePartOne;
            nSafeCountPartTwo += bSafePartTwo;
        }

        printf("Part 1: %llu\n", nSafeCountPartOne);
        printf("Part 2: %llu\n", nSafeCountPartTwo);
    }
 
    return 0;
}