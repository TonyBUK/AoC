#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength)
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

    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

int compare (const void* a, const void* b)
{
    return (int) (((int64_t)*(size_t*)a -
                   (int64_t)*(size_t*)b));
}

bool validTriangle(const size_t x, const size_t y, const size_t z)
{
    size_t kTriangle[] = {x,y,z};
    qsort (kTriangle, sizeof(kTriangle) / sizeof(kTriangle[0]), sizeof(kTriangle[0]), compare);
    return (bool)((kTriangle[0] + kTriangle[1]) > kTriangle[2]);
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  i;
        size_t                  nLineCount;
        size_t*                 kTriangleLengths;

        size_t                  nTriangle;
        size_t                  nValidCount;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        kTriangleLengths = (size_t*)malloc(nLineCount * 3 * sizeof(size_t));

        nTriangle = 0;
        for (i = 0; i < nLineCount; ++i)
        {
            char*  pToken = strtok(kLines[i], " ");
            while (pToken)
            {
                if (*pToken != '\0')
                {
                    kTriangleLengths[nTriangle] = strtoull(pToken, NULL, 10);
                    ++nTriangle;
                }
                pToken = strtok(NULL, " ");
            }
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        /* Count the Triangles by Row */
        nValidCount = 0;
        for (nTriangle = 0; nTriangle < nLineCount; ++nTriangle)
        {
            if (validTriangle(kTriangleLengths[(nTriangle * 3) + 0],
                              kTriangleLengths[(nTriangle * 3) + 1],
                              kTriangleLengths[(nTriangle * 3) + 2]))
            {
                ++nValidCount;
            }
        }
        printf("Part 1: %u\n", (uint32_t)nValidCount);

        /* Count the Triangles by Column */
        nValidCount = 0;
        for (nTriangle = 0; nTriangle < nLineCount; ++nTriangle)
        {
            if (validTriangle(kTriangleLengths[((((nTriangle / 3) * 3) + 0) * 3) + (nTriangle % 3)],
                              kTriangleLengths[((((nTriangle / 3) * 3) + 1) * 3) + (nTriangle % 3)],
                              kTriangleLengths[((((nTriangle / 3) * 3) + 2) * 3) + (nTriangle % 3)]))
            {
                ++nValidCount;
            }
        }
        printf("Part 2: %u\n", (uint32_t)nValidCount);

        free(kTriangleLengths);
    }

    return 0;
}
