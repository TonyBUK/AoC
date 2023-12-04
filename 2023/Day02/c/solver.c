#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_FALSE (0u)
#define AOC_TRUE  (1u)

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        unsigned long nPartOne      = 0u;
        unsigned long nPartTwo      = 0u;

        /* Extract the Max Red/Green/Blue per Game */
        while (!feof(pData))
        {
            unsigned      bGameIdFound      = AOC_FALSE;
            unsigned      bSpaceFound       = AOC_FALSE;
            unsigned      bProcessingNumber = AOC_FALSE;
            unsigned      bCubeIdentified   = AOC_FALSE;

            unsigned long nGameId           = 0u;
            unsigned long nCubeCount        = 0u;

            char          kData;
            unsigned long nMaxRed           = 0u;
            unsigned long nMaxGreen         = 0u;
            unsigned long nMaxBlue          = 0u;

            while (('\n' != (kData = fgetc(pData))) && (!feof(pData)))
            {
                /* First find the Game Id */
                if (AOC_FALSE == bGameIdFound)
                {
                    if (AOC_FALSE == bSpaceFound)
                    {
                        if (' ' == kData)
                        {
                            bSpaceFound = AOC_TRUE;
                        }
                    }
                    else if (':' == kData)
                    {
                        bGameIdFound      = AOC_TRUE;
                        bSpaceFound       = AOC_FALSE;
                        bProcessingNumber = AOC_FALSE;
                        bCubeIdentified   = AOC_FALSE;
                        nCubeCount        = 0u;
                    }
                    else
                    {
                        assert((kData >= '0') && (kData <= '9'));
                        nGameId = (nGameId * 10) + (kData - '0');
                    }
                }
                else
                {
                    /* Then find the Cube Amounts as pairs of data split by spaces, Number, then Colour. */
                    if (' ' == kData)
                    {
                        if (AOC_FALSE == bProcessingNumber)
                        {
                            bProcessingNumber = AOC_TRUE;
                            bCubeIdentified   = AOC_FALSE;
                        }
                        else
                        {
                            bProcessingNumber = AOC_FALSE;
                        }
                    }
                    else if (AOC_TRUE == bProcessingNumber)
                    {
                        assert((kData >= '0') && (kData <= '9'));
                        nCubeCount = (nCubeCount * 10) + (kData - '0');
                    }
                    else if (AOC_FALSE == bCubeIdentified)
                    {
                        if ('r' == kData)
                        {
                            bCubeIdentified = AOC_TRUE;
                            if (nCubeCount > nMaxRed) nMaxRed = nCubeCount;
                            nCubeCount = 0u;
                        }
                        else if ('g' == kData)
                        {
                            bCubeIdentified = AOC_TRUE;
                            if (nCubeCount > nMaxGreen) nMaxGreen = nCubeCount;
                            nCubeCount = 0u;
                        }
                        else if ('b' == kData)
                        {
                            bCubeIdentified = AOC_TRUE;
                            if (nCubeCount > nMaxBlue) nMaxBlue = nCubeCount;
                            nCubeCount = 0u;
                        }
                    }
                }
            }

            /* Part 1 */
            if ((nMaxRed <= 12) && (nMaxGreen <= 13) && (nMaxBlue <= 14))
            {
                nPartOne += nGameId;
            }

            /* Part 2 */
            nPartTwo += nMaxRed * nMaxGreen * nMaxBlue;
        }

        fclose(pData);

        printf("Part 1: %lu\n", nPartOne);
        printf("Part 2: %lu\n", nPartTwo);
    }
 
    return 0;
}