#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define MAX_SIZE_T (size_t)((uint64_t)(-1))

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

size_t calculateQEFromMask(const size_t* kPackages, const size_t nPackagesCount, const size_t nMask)
{
    size_t nProduct = 1;
    size_t i;

    for (i = 0; i < nPackagesCount; ++i)
    {
        if ((1 << i) & nMask)
        {
            nProduct *= kPackages[i];
        }
    }
    return nProduct;
}

size_t calculateCountFromMask(const size_t nMask)
{
    size_t nLength    = 0;
    size_t nLocalMask = nMask;

    while (nLocalMask > 0)
    {
        if (nLocalMask & 1)
        {
            ++nLength;
        }
        nLocalMask /= 2;
    }
    return nLength;
}

size_t getCombinedWeight(const size_t* kPackages, const size_t nPackagesCount)
{
    size_t i;
    size_t nSum = 0;
    for (i = 0; i < nPackagesCount; ++i)
    {
        nSum += kPackages[i];
    }
    return nSum;
}

typedef struct PermutationType
{
    size_t nCount;
    size_t nQuantumEntanglement;
    size_t nMask;
} PermutationType;

#define getPermutationIndexesDefault(kPackages, nPackagesCount, nTargetWeight, pkPermutations, pnPermutationsCount, pnPermutationsMax) \
     getPermutationIndexes(              kPackages,              nPackagesCount,              nTargetWeight,                   pkPermutations,         pnPermutationsCount,         pnPermutationsMax,                    0,                     0,                   0)
void getPermutationIndexes(const size_t* kPackages, const size_t nPackagesCount, const size_t nTargetWeight, PermutationType** pkPermutations, size_t* pnPermutationsCount, size_t* pnPermutationsMax, size_t nCurrentIndex, size_t nCurrentWeight, size_t nCurrentMask)
{
    size_t i;
    for (i = nCurrentIndex; i < nPackagesCount; ++i)
    {
        const size_t nDeltaMask     = 1 << i;
        const size_t nPackageWeight = kPackages[i];

        if ((nCurrentWeight + nPackageWeight) < nTargetWeight)
        {
            /* Recurse... */
            getPermutationIndexes(kPackages, nPackagesCount, nTargetWeight, pkPermutations, pnPermutationsCount, pnPermutationsMax, i + 1, nCurrentWeight + nPackageWeight, nCurrentMask | nDeltaMask);
        }
        else if ((nCurrentWeight + nPackageWeight) == nTargetWeight)
        {
            const size_t nNewMask = nCurrentMask | nDeltaMask;

            /* Store the Quantity, Quantum Entanglement and Mask
             * This allows for the packages to be sorted, which means the first viable solution
             * found when iterating will *always* have the best package in terms of Quantity/QE.
             */
            PermutationType kPermutation = {calculateCountFromMask(nNewMask), calculateQEFromMask(kPackages, nPackagesCount, nNewMask), nNewMask};

            /* Determine if Space exists for the copy, if it doesn't:
             * 1. Allocate twice the space.
             * 2. Copy the old buffer to the new buffer.
             * 3. Delete the old buffer
             * 4. Set the Buffer Reference to use the new buffer
             */
            if ((*pnPermutationsCount + 1) >= *pnPermutationsMax)
            {
                PermutationType* pTemp;

                pTemp = (PermutationType*)malloc(*pnPermutationsMax * 2 * sizeof(PermutationType));
                memcpy(pTemp, *pkPermutations, *pnPermutationsMax * sizeof(PermutationType));
                *pnPermutationsMax *= 2;
                free(*pkPermutations);
                *pkPermutations = pTemp;
            }
            (*pkPermutations)[*pnPermutationsCount] = kPermutation;
            *pnPermutationsCount += 1;

            /* The Weights are sorted so there's no more possible solutions at this depth */
            break;
        }
        else
        {
            /* The Weights are sorted so there's no more possible solutions at this depth */
            break;
        }
    }
}

int permutationCompare(const void * a, const void * b)
{
    const PermutationType* left  = (PermutationType*) a;
    const PermutationType* right = (PermutationType*) b;

    if (left->nCount != right->nCount)
    {
        if (left->nCount > right->nCount)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (left->nQuantumEntanglement > right->nQuantumEntanglement)
        {
            return 1;
        }
        else if (left->nQuantumEntanglement == right->nQuantumEntanglement)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
}

#define getLowestQEDefault(kPermutations, nPermutationsCount, nCompartmentCount) \
       getLowestQE(                       kPermutations,              nPermutationsCount,              nCompartmentCount,             MAX_SIZE_T,                          0,                          0,                   1, 0)
size_t getLowestQE(const PermutationType* kPermutations, const size_t nPermutationsCount, const size_t nCompartmentCount, const size_t nLowestQE, const size_t nCurrentIndex, const size_t nCombinedMask, const size_t nDepth, const size_t nCandidateQE)
{
    size_t nLocalLowestQE = nLowestQE;
    size_t i;

    for (size_t i = nCurrentIndex; i < nPermutationsCount; ++i)
    {
        /* An easy way of telling whether two masks have no bits in common is to compare the OR to the ADD.
         * If there's no bits in common, the result should be equal, if not, then they share bits.
         */
        const size_t nDeltaMask = kPermutations[i].nMask | nCombinedMask;
        if (nDeltaMask == (kPermutations[i].nMask + nCombinedMask))
        {
            if (nDepth == nCompartmentCount)
            {
                return nCandidateQE;
            }
            else
            {
                nLocalLowestQE = getLowestQE(kPermutations, nPermutationsCount, nCompartmentCount, nLocalLowestQE, i+1, nDeltaMask, nDepth + 1, (nDepth == 1) ? kPermutations[i].nQuantumEntanglement : nCandidateQE);
                if (nLocalLowestQE != MAX_SIZE_T)
                {
                    return nLocalLowestQE;
                }
            }
        }
    }

    return nLocalLowestQE;
}

size_t getPermutationWithLowestQE(const size_t* kPackages, const size_t nPackagesCount, const size_t nCompartmentCount)
{
    const size_t        nGroupTarget        = getCombinedWeight(kPackages, nPackagesCount) / nCompartmentCount;
    PermutationType*    kPermutations;
    size_t              nPermutationsCount  = 0;
    size_t              nPermutationsMax    = 1024 * 1024; /* Increase this value to prevent re-allocations */
    size_t              nLowestEntanglement;

    kPermutations = (PermutationType*)malloc(nPermutationsMax * sizeof(PermutationType));

    getPermutationIndexesDefault(kPackages, nPackagesCount, nGroupTarget, &kPermutations, &nPermutationsCount, &nPermutationsMax);
    qsort(kPermutations, nPermutationsCount, sizeof(PermutationType), &permutationCompare);

    nLowestEntanglement = getLowestQEDefault(kPermutations, nPermutationsCount, nCompartmentCount);
    free(kPermutations);

    return nLowestEntanglement;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t*                 kPackages;
        size_t                  nPackagesCount;
        size_t                  i;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nPackagesCount);
        fclose(pInput);

        kPackages = (size_t*)malloc(nPackagesCount * sizeof(size_t));

        /* Convert the Packages Weight */
        for (i = 0; i < nPackagesCount; ++i)
        {
            kPackages[i] = strtoull(kLines[i], NULL, 10);
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %llu\n", (uint64_t)getPermutationWithLowestQE(kPackages, nPackagesCount, 3));
        printf("Part 2: %llu\n", (uint64_t)getPermutationWithLowestQE(kPackages, nPackagesCount, 4));

        free(kPackages);
    }

    return 0;
}
