#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

/* Needed for Hash Map */
typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

#include "hashmap/include/hashmap.h"

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

typedef struct fabricateRuleType
{
    char*   pInputMolecule;
    char*   pReplacementMolecule;
} fabricateRuleType;

typedef HASHMAP(char, bool) permutationType;
bool         bDummy              = false;

void clearHashMap(permutationType* pHashMap)
{
    const char* pTemp;
    /* Free all the Keys */
    hashmap_foreach_key(pTemp, pHashMap)
    {
        free((char*)pTemp);
    }
    hashmap_clear(pHashMap);
}

void getPermutations(const char* kMedicineMolecule, const fabricateRuleType* kModuleFabricationRules, const size_t nModuleFabricationRulesSize, permutationType* kPermutations)
{
    const size_t nMedicineMolecueLen = strlen(kMedicineMolecule);

    /* Reserve enough memory for the temporary string */
    char*        szTemp              = (char*)malloc(nMedicineMolecueLen * 2 * sizeof(char));

    size_t       i;

    hashmap_clear(kPermutations);

    /* For each rule */
    for (i = 0; i < nModuleFabricationRulesSize; ++i)
    {
        /* Replace all instances of the substring */
        char* pSubString = strstr(kMedicineMolecule, kModuleFabricationRules[i].pInputMolecule);
        while (pSubString)
        {
            strncpy(szTemp, kMedicineMolecule, (pSubString - kMedicineMolecule));
            szTemp[pSubString - kMedicineMolecule] = '\0';
            strcat(szTemp, kModuleFabricationRules[i].pReplacementMolecule);
            strcat(szTemp, &pSubString[strlen(kModuleFabricationRules[i].pInputMolecule)]);

            hashmap_put(kPermutations, strdup(szTemp), &bDummy);

            pSubString = strstr(pSubString+1, kModuleFabricationRules[i].pInputMolecule);
        }
    }

    free(szTemp);
}

int size_t_compare(const void* p1, const void* p2)
{
    return (int64_t)*((size_t*)p1) - (int64_t)*((size_t*)p2);
}
#define findFavouriteMoleculeDefault(kCurrentMolecules, kModuleFabricationRules, nModuleFabricationRulesSize, kFavouriteMolecule) \
       findFavouriteMolecule(                 kCurrentMolecules,                          kModuleFabricationRules,              nModuleFabricationRulesSize,             kFavouriteMolecule, /*const size_t nDepth*/ 1, /*const size_t nBestDepth */ MAX_SIZE_T)
size_t findFavouriteMolecule(permutationType* kCurrentMolecules, const fabricateRuleType* kModuleFabricationRules, const size_t nModuleFabricationRulesSize, const char* kFavouriteMolecule,   const size_t nDepth    ,   const size_t nBestDepth              )
{
    /* Note: We could work our way forwards, parsing all possible sets at each level until
     *       we reach an answer, but this soon becomes a "heat death of the universe" style
     *       task where each new depth exponentially adds more permutations.
     *       Instead, if we start from the end and work backwards.  The fact the solutions are
     *       reductive helps reduce the overhead of permutations.
     */
    permutationType                    kNewMolecules;
    size_t                             nBestDepthLocal = nBestDepth;
    const char*                        it;

    hashmap_init(&kNewMolecules, hashmap_hash_string, strcmp);

    hashmap_foreach_key(it, kCurrentMolecules)
    {
        clearHashMap(&kNewMolecules);
        getPermutations(it, kModuleFabricationRules, nModuleFabricationRulesSize, &kNewMolecules);

        if (NULL != (void*)hashmap_get(&kNewMolecules, kFavouriteMolecule))
        {
            clearHashMap(&kNewMolecules);
            hashmap_cleanup(&kNewMolecules);
            return nDepth;
        }
        else if ((nDepth + 1) < nBestDepthLocal)
        {
            /* Rather than just randomly iterating through the solutions, sort them by length and parse
             * each group separately...
             */
            size_t*     kNewMoleculesSizes    = (size_t*)malloc(hashmap_size(&kNewMolecules) * sizeof(size_t));
            size_t      nNumNewMoleculesSizes = 0;
            const char* itNew;
            size_t      i;
            permutationType kNewMoleculesSizeSorted;
            hashmap_init(&kNewMoleculesSizeSorted, hashmap_hash_string, strcmp);

            hashmap_foreach_key(itNew, &kNewMolecules)
            {
                const size_t nLength = strlen(itNew);
                bool         bUnique = true;
                for (i = 0; (true == bUnique) && (i < nNumNewMoleculesSizes); ++i)
                {
                    if (nLength == kNewMoleculesSizes[i])
                    {
                        bUnique = false;
                    }
                }

                if (bUnique)
                {
                    kNewMoleculesSizes[nNumNewMoleculesSizes] = nLength;
                    ++nNumNewMoleculesSizes;
                }
            }

            /* Parse each group from shortest to longest */
            qsort(kNewMoleculesSizes, nNumNewMoleculesSizes, sizeof(size_t), size_t_compare);
            for (i = 0; (i < nNumNewMoleculesSizes) && (MAX_SIZE_T == nBestDepthLocal); ++i)
            {
                clearHashMap(&kNewMoleculesSizeSorted);
                hashmap_foreach_key(itNew, &kNewMolecules)
                {
                    if (kNewMoleculesSizes[i] == strlen(itNew))
                    {
                        hashmap_put(&kNewMoleculesSizeSorted, strdup(itNew), &bDummy);
                    }
                }
                nBestDepthLocal = findFavouriteMolecule(&kNewMoleculesSizeSorted, kModuleFabricationRules, nModuleFabricationRulesSize, kFavouriteMolecule, nDepth + 1, nBestDepthLocal);
            }

            clearHashMap(&kNewMoleculesSizeSorted);
            hashmap_cleanup(&kNewMoleculesSizeSorted);
            free(kNewMoleculesSizes);

            /* Assume that once we've found any solution, it's the best one.
                * Whether this stands up to scrutiny or not for other inputs is something
                * I'd be interested to find out...
                * */
            if (nBestDepthLocal != MAX_SIZE_T)
            {
                clearHashMap(&kNewMolecules);
                hashmap_cleanup(&kNewMolecules);
                return nBestDepthLocal;
            }
        }
    }

    clearHashMap(&kNewMolecules);
    hashmap_cleanup(&kNewMolecules);

    return nBestDepthLocal;
}

void swapRules(fabricateRuleType* kModuleFabricationRules, const size_t nModuleFabricationRulesSize)
{
    size_t i;
    // For each rule
    for (i = 0; i < nModuleFabricationRulesSize; ++i)
    {
        char* pTmp = kModuleFabricationRules[i].pInputMolecule;
        kModuleFabricationRules[i].pInputMolecule       = kModuleFabricationRules[i].pReplacementMolecule;
        kModuleFabricationRules[i].pReplacementMolecule = pTmp;

        // This assumes that we are always reductive, so assert that theory
        assert(strlen(kModuleFabricationRules[i].pInputMolecule) >= strlen(kModuleFabricationRules[i].pReplacementMolecule));
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        fabricateRuleType*      kModuleFabricationRules;
        size_t                  nModuleFabricationRulesSize;
        char*                   kModuleCalibrationString;

        size_t                  i;

        permutationType         kPermutationMap;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nModuleFabricationRulesSize);
        fclose(pInput);

        /* Allocate the Rule Size */
        kModuleFabricationRules = (fabricateRuleType*)malloc((nModuleFabricationRulesSize-2) * sizeof(fabricateRuleType));

        /* Store each Rule */
        for (i = 0; i < (nModuleFabricationRulesSize-2); ++i)
        {
            size_t j = 0;
            char*  p = strtok(kLines[i], " => ");
            while (p)
            {
                assert(j < 2);
                if (0 == j)
                {
                    kModuleFabricationRules[i].pInputMolecule = p;
                }
                else
                {
                    kModuleFabricationRules[i].pReplacementMolecule = p;
                }

                p = strtok(NULL, " => ");
                ++j;
            }
        }

        /* Copy the Calibration String */
        kModuleCalibrationString = kLines[nModuleFabricationRulesSize-1];

        /* Allocate the Permutation Map */
        hashmap_init(&kPermutationMap, hashmap_hash_string, strcmp);

        /* Note: We've only copied pointers to the line data so we can't free anything
         *       until the very end.
         */
        nModuleFabricationRulesSize -= 2;
        getPermutations(kModuleCalibrationString, kModuleFabricationRules, nModuleFabricationRulesSize, &kPermutationMap);

        printf("Part 1: %u\n", (uint32_t)hashmap_size(&kPermutationMap));

        /* Swap the Rules and initialise the Permutations to the current Calibration String */
        swapRules(kModuleFabricationRules, nModuleFabricationRulesSize);
        clearHashMap(&kPermutationMap);
        hashmap_put(&kPermutationMap, strdup(kModuleCalibrationString), &bDummy);

        printf("Part 2: %u\n", (uint32_t)findFavouriteMoleculeDefault(&kPermutationMap, kModuleFabricationRules, nModuleFabricationRulesSize, "e"));

        /* Cleanup the Hash Map */
        clearHashMap(&kPermutationMap);
        hashmap_cleanup(&kPermutationMap);

        /* Free the Fabrication Rules */
        free(kModuleFabricationRules);

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);
  }

    return 0;
}
