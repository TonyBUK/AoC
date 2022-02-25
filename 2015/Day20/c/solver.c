#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define MAX_SIZE_T (size_t)((uint64_t)(-1))
#define MIN(A,B) (((A) <= (B)) ? (A) : (B))

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount)
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

    *pkFileBuffer   = (char*) malloc((nFileSize+1)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));

    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    (*pkFileBuffer)[nFileSize] = '\0';

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

    *pnLineCount = nLineCount;
}

size_t findLowestHouseNumber(const size_t nTargetPresents, const size_t nPresentsPerHouse, const size_t nHousesPerElf)
{
    const size_t        nNormalisedTargetPresents = nTargetPresents / nPresentsPerHouse;
    size_t*             kTotalCounts;
    size_t              i;
    size_t              nElf;

    if (nTargetPresents <= nPresentsPerHouse)
    {
        return 1;
    }

    kTotalCounts = (size_t*)malloc((nNormalisedTargetPresents+1) * sizeof(size_t));
    for (i = 1; i <= nNormalisedTargetPresents; ++i)
    {
        kTotalCounts[i] = nPresentsPerHouse;
    }

    /* The Basic Algorithm to process all elves in turn, and increment the present count
     * for all possible houses they can deliver to
     */
    for (nElf = 2; nElf <= nNormalisedTargetPresents; ++nElf)
    {
        // Cache the Present Count
        const size_t nElfPresents = nElf * nPresentsPerHouse;
        size_t       nHouse;

        // For each house the elf can visit
        for (nHouse = 1; nHouse <= MIN(nNormalisedTargetPresents/nElf, nHousesPerElf); ++nHouse)
        {
            // Increment the Presents for the current house
            kTotalCounts[nHouse * nElf] += nElfPresents;
            if (kTotalCounts[nElf] >= nTargetPresents)
            {
                free(kTotalCounts);
                return nElf;
            }
        }
    }

    assert(0);
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;

        size_t                  i;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount);
        fclose(pInput);

        /* For each Number... */
        for (i = 0; i < nLineCount; ++i)
        {
            printf("Part 1: %u\n", (uint32_t)findLowestHouseNumber(strtoull(kLines[i], NULL, 10), 10, MAX_SIZE_T));
            printf("Part 2: %u\n", (uint32_t)findLowestHouseNumber(strtoull(kLines[i], NULL, 10), 11, 50));
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);
  }

    return 0;
}
