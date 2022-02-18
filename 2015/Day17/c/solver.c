#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

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
        if (nLineLength > 0)
        {
            if (pLine[nLineLength-1] == '.')
            {
                pLine[nLineLength-1] = '\0';
            }
        }

        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;

        pLine = strtok(NULL, "\n");
    }

    *pnLineCount = nLineCount;
}

#define calculatePermutationsDefault(kContainers, nContainersSize, nTarget, kSolutionCount) calculatePermutations(kContainers, nContainersSize, nTarget, kSolutionCount, /*nCurrent*/ 0, /*nCount*/ 0, /*nIndex*/ 0)
void calculatePermutations(const size_t* kContainers, const size_t nContainersSize, const size_t nTarget, size_t* kSolutionCount, const size_t nCurrent, const size_t nCount, const size_t nIndex)
{
    size_t i;

    /* Update the Permutation if we've reached our target */
    if (nTarget == nCurrent)
    {
        ++kSolutionCount[nCount-1];
        return;
    }

    /* Iterate through the remaining possible permutations */
    for (i = nIndex; i < nContainersSize; ++i)
    {
        const size_t nQuantity = kContainers[i];
        if ((nCurrent + nQuantity) <= nTarget)
        {
            calculatePermutations(kContainers, nContainersSize, nTarget, kSolutionCount, nCurrent + nQuantity, nCount + 1, i + 1);
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        size_t                  nContainerCount;
        size_t*                 kContainers;
        size_t*                 kSolutionCount;

        size_t                  i;

        size_t                  nMin    = 0;
        size_t                  nTotal  = 0;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nContainerCount);
        fclose(pInput);

        /* Store the Container Counts */
        kContainers    = (size_t*)malloc(nContainerCount * sizeof(size_t));
        kSolutionCount = (size_t*)malloc(nContainerCount * sizeof(size_t));
        for (i = 0; i < nContainerCount; ++i)
        {
            kContainers[i]    = strtoull(kLines[i], NULL, 10);
            kSolutionCount[i] = 0;
        }

        /* Free the Line Buffers since we've parsed all the data */
        free(kBuffer);
        free(kLines);

        calculatePermutationsDefault(kContainers, nContainerCount, 150, kSolutionCount);

        for (i = 0; i < nContainerCount; ++i)
        {
            /* Get the total of all permutations for Part 1 */
            nTotal += kSolutionCount[i];

            /* Get the minimum number of containers used for Part 2 */
            if (0 == nMin)
            {
                if (nTotal > 0)
                {
                    nMin = i;
                }
            }
        }

        printf("Part 1: %u\n", (uint32_t)nTotal);
        printf("Part 2: %u\n", (uint32_t)kSolutionCount[nMin]);

        /* Free the Containers/Solution Counts */
        free(kContainers);
        free(kSolutionCount);
   }

    return 0;
}
