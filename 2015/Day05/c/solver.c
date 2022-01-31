#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

unsigned isNicePart1(const char* kName)
{
    const char          VOWELS[]        = "aeiou";
    const char          BAD_WORDS[][3]  = {"ab", "cd", "pq", "xy"}; /* 2 chars + NUL terminator */
    const unsigned long NEEDED_VOWELS   = 3;
    unsigned long       nVowelCount     = 0;
    unsigned            bRepeat         = FALSE;
    size_t              i;

    /* Eliminate the Bad Words */
    for (i = 0; i < (sizeof(BAD_WORDS) / sizeof(BAD_WORDS[0])); ++i)
    {
        if (strstr(kName, BAD_WORDS[i]))
        {
            return FALSE;
        }
    }

    /* Find Repetitions and Vowels */
    for (i = 0; i < strlen(kName); ++i)
    {
        if (nVowelCount < NEEDED_VOWELS)
        {
            const char kLetter[2] = {kName[i], '\0'};
            if (strstr(VOWELS, kLetter))
            {
                ++nVowelCount;
            }
        }

        if (FALSE == bRepeat)
        {
            if (i > 0)
            {
                if (kName[i] == kName[i-1])
                {
                    bRepeat = TRUE;
                }
            }
        }

        if ((nVowelCount >= NEEDED_VOWELS) && bRepeat)
        {
            break;
        }
    }

    return (nVowelCount >= NEEDED_VOWELS) && bRepeat;
}

unsigned isNicePart2(const char* kName)
{
    // Repeated Words
    const size_t SIZE   = strlen(kName);
    unsigned     bFound = FALSE;
    size_t       i;
    for (i = 0; (FALSE == bFound) && (i < SIZE); ++i)
    {
        size_t j;
        for (j = i + 2; (FALSE == bFound) && (j < SIZE); ++j)
        {
            bFound = ((kName[i] == kName[j]) && (kName[i+1] == kName[j+1])) ? TRUE : FALSE;
        }
    }

    // Sandwiched Letter
    if (bFound)
    {
        bFound = FALSE;
        for (i = 1; (FALSE == bFound) && (i < (SIZE-1)); ++i)
        {
            bFound = (kName[i-1] == kName[i+1]) ? TRUE : FALSE;
        }
    }

    return bFound;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char*               pName;

        unsigned long       nPart1 = 0;
        unsigned long       nPart2 = 0;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*)        malloc((nFileSize+1)  * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        pName = strtok(kBuffer, "\n");

        while (pName)
        {
            if (isNicePart1(pName)) ++nPart1;
            if (isNicePart2(pName)) ++nPart2;

            pName = strtok(NULL, "\n");
        }

        printf("Part 1: %ld\n", nPart1);
        printf("Part 2: %ld\n", nPart2);

        free(kBuffer);
    }

    return 0;
}
