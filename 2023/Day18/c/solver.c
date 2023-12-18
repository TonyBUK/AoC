#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

typedef enum ELineParsingState
{
    eLineParsingState_DirectionOne  = 0,
    eLineParsingState_DistanceOne,
    eLineParsingState_DistanceTwo_0,
    eLineParsingState_DistanceTwo_1,
    eLineParsingState_DistanceTwo_2,
    eLineParsingState_DistanceTwo_3,
    eLineParsingState_DistanceTwo_4,
    eLineParsingState_DirectionTwo,
    eLineParsingState_Complete
} ELineParsingState;

void calculateNextPoint(const char kDirection, const int64_t nDistance, const int64_t nPrevX, const int64_t nPrevY, int64_t* nX, int64_t* nY, int64_t* nPerimiter)
{
    switch(kDirection)
    {
        case 'R':
        {
            *nX = nPrevX + nDistance;
            *nY = nPrevY;
            break;
        }

        case 'D':
        {
            *nX = nPrevX;
            *nY = nPrevY + nDistance;
            break;
        }

        case 'L':
        {
            *nX = nPrevX - nDistance;
            *nY = nPrevY;
            break;
        }

        case 'U':
        {
            *nX = nPrevX;
            *nY = nPrevY - nDistance;
            break;
        }
    }

    *nPerimiter += nDistance;
}

uint8_t decodeHex(const char kHex)
{
    if ((kHex >= '0') && (kHex <= '9'))
    {
        return kHex - '0';
    }
    else if ((kHex >= 'a') && (kHex <= 'f'))
    {
        return kHex - 'a' + 10;
    }

    return 0xFF;
}

int64_t partialShoelaceTheorem(const int64_t nPrevX, const int64_t nPrevY, const int64_t nX, const int64_t nY)
{
    return (nPrevX * nY) - (nX * nPrevY);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        const char              kDirectionChars[] = {'R', 'D', 'L', 'U'};

        int64_t                 nPerimiterPartOne = 0;
        int64_t                 nPerimiterPartTwo = 0;
        int64_t                 nAreaPartOne      = 0;
        int64_t                 nAreaPartTwo      = 0;

        int64_t                 nPrevXPartOne     = 0;
        int64_t                 nPrevYPartOne     = 0;
        int64_t                 nPrevXPartTwo     = 0;
        int64_t                 nPrevYPartTwo     = 0;

        while (!feof(pData))
        {
            char                kDirectionPartOne;
            size_t              nLineLength;
            char                kDirectionPartTwo;
            int64_t             nDistancePartOne    = 0;
            int64_t             nDistancePartTwo    = 0;
            int64_t             nXPartOne;
            int64_t             nYPartOne;
            int64_t             nXPartTwo;
            int64_t             nYPartTwo;
            char                kData           = 0;
            ELineParsingState   eState          = eLineParsingState_DirectionOne;

            while (((kData = fgetc(pData)) != '\n') && !feof(pData))
            {
                if (' ' == kData)
                {
                    /* Move to the Next State */
                    ++eState;
                    continue;
                }

                switch (eState)
                {
                    /* Part 1 - Direction */
                    case eLineParsingState_DirectionOne:
                    {
                        kDirectionPartOne = kData;
                    } break;

                    /* Part 1 - Distance (keep going until we reach a space) */
                    case eLineParsingState_DistanceOne:
                    {
                        nDistancePartOne = (nDistancePartOne * 10) + (kData - '0');
                    } break;

                    /* Part 2 - Hexadecimal (5 chars) */
                    case eLineParsingState_DistanceTwo_0:
                    case eLineParsingState_DistanceTwo_1:
                    case eLineParsingState_DistanceTwo_2:
                    case eLineParsingState_DistanceTwo_3:
                    case eLineParsingState_DistanceTwo_4:
                    {
                        uint8_t nHex = decodeHex(kData);
                        if (nHex != 0xFF)
                        {
                            nDistancePartTwo = (nDistancePartTwo * 16) + (int64_t)nHex;
                            ++eState;
                        }
                    } break;

                    /* Part 2 - Direction */
                    case eLineParsingState_DirectionTwo:
                    {
                        kDirectionPartTwo = kDirectionChars[kData - '0'];
                        eState = eLineParsingState_Complete;
                    } break;

                    default: break;
                }
            }

            /* Calculate the Next Point / Perimiter Length based on Direction/Distance */
            calculateNextPoint(kDirectionPartOne, nDistancePartOne, nPrevXPartOne, nPrevYPartOne, &nXPartOne, &nYPartOne, &nPerimiterPartOne);
            calculateNextPoint(kDirectionPartTwo, nDistancePartTwo, nPrevXPartTwo, nPrevYPartTwo, &nXPartTwo, &nYPartTwo, &nPerimiterPartTwo);

            /* Calculate the next term in the Shoelace Theorem */
            nAreaPartOne += partialShoelaceTheorem(nPrevXPartOne, nPrevYPartOne, nXPartOne, nYPartOne);
            nAreaPartTwo += partialShoelaceTheorem(nPrevXPartTwo, nPrevYPartTwo, nXPartTwo, nYPartTwo);

            /* Set all the Previous Positions */
            nPrevXPartOne = nXPartOne;
            nPrevYPartOne = nYPartOne;
            nPrevXPartTwo = nXPartTwo;
            nPrevYPartTwo = nYPartTwo;
        }
        fclose(pData);

        /* The final step for the Shoelace Theorem is divide by 2, then we need to add on half the perimiter as the integer
         * coordinates count half the perimiter with this method.
         */
        printf("Part 1: %lld\n", (nAreaPartOne / 2) + (nPerimiterPartOne / 2) + 1);
        printf("Part 2: %lld\n", (nAreaPartTwo / 2) + (nPerimiterPartTwo / 2) + 1);
    }
 
    return 0;
}