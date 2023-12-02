#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define GAME_STRING        ("Game ")
#define GAME_STRING_LENGTH (sizeof(GAME_STRING) - 1)

unsigned long getMaxColour(const char* kColour, const char* kLine)
{
    unsigned long nMaxColour = 0;
    char*         pBuffer;

    pBuffer = strstr(kLine, kColour);
    while (pBuffer)
    {
        /* The Digit always precedes this... */
        char* pLocalBuffer = pBuffer - 1;
        unsigned long nColourCount;
        while (*pLocalBuffer != ' ')
        {
            --pLocalBuffer;
        }
        nColourCount = strtoul(pLocalBuffer + 1, NULL, 10);
        if (nColourCount > nMaxColour)
        {
            nMaxColour = nColourCount;
        }

        pBuffer = strstr(pBuffer+1, kColour);
    }

    return nMaxColour;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        size_t        nBufferLength = 0;
        char*         kLine         = NULL;

        unsigned long nPartOne      = 0u;
        unsigned long nPartTwo      = 0u;

        /* Extract the Max Red/Green/Blue per Game */
        while (-1 != getline(&kLine, &nBufferLength, pData))
        {
            const unsigned long nGameId = strtoul(strstr(kLine, GAME_STRING) + GAME_STRING_LENGTH, NULL, 10);

            /* Max Colours */
            const unsigned long nRed    = getMaxColour(" red",   kLine);
            const unsigned long nGreen  = getMaxColour(" green", kLine);
            const unsigned long nBlue   = getMaxColour(" blue",  kLine);

            /* Part 1 */
            if ((nRed <= 12) && (nGreen <= 13) && (nBlue <= 14))
            {
                nPartOne += nGameId;
            }

            /* Part 2 */
            nPartTwo += nRed * nGreen * nBlue;
        }
        free(kLine);

        fclose(pData);

        printf("Part 1: %lu\n", nPartOne);
        printf("Part 2: %lu\n", nPartTwo);
    }
 
    return 0;
}