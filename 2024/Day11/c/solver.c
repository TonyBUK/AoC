#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>
 
/* Needed for Hash Map */
typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif
 
#include "hashmap/include/hashmap.h"

typedef struct HashmapActionType
{
    uint64_t nStoneNumber;
    size_t   nCount;
} HashmapActionType;

typedef HASHMAP(uint64_t, size_t) stoneCountType;

/*
 * Use generic hash algorithm supplied by the hashmap library.
 */
size_t hash_uint64(const uint64_t* key)
{
    return hashmap_hash_default(key, sizeof(*key));
}
 
int compare_uint64(const uint64_t* a, const uint64_t* b)
{
    const uint64_t nLeft  = *a;
    const uint64_t nRight = *b;

    if (nLeft < nRight)
    {
        return -1;
    }
    else if (nLeft > nRight)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void AddToChangeList(HashmapActionType** kStoneMapChanges, size_t* nStoneMapChangeSize, size_t* nStoneMapChangeMaxSize, const uint64_t nStoneNumber, const size_t nCount)
{
    if (*nStoneMapChangeSize >= *nStoneMapChangeMaxSize)
    {
        *nStoneMapChangeMaxSize *= 2;
        *kStoneMapChanges = (HashmapActionType*)realloc(*kStoneMapChanges, *nStoneMapChangeMaxSize * sizeof(HashmapActionType));
    }

    (*kStoneMapChanges)[*nStoneMapChangeSize].nStoneNumber = nStoneNumber;
    (*kStoneMapChanges)[*nStoneMapChangeSize].nCount     = nCount;

    *nStoneMapChangeSize += 1;
}
 
void addToEntry(stoneCountType* kStoneMap, const uint64_t nStoneNumber, const size_t nCount)
{
    size_t* pExistingCount = hashmap_get(kStoneMap, &nStoneNumber);
    if (pExistingCount)
    {
        *pExistingCount += nCount;
    }
    else
    {
        uint64_t* pStoneNumber = (uint64_t*)malloc(sizeof(uint64_t));
        size_t*   pStoneCount  = (size_t*)malloc(sizeof(size_t));
        *pStoneNumber = nStoneNumber;
        *pStoneCount  = nCount;
        hashmap_put(kStoneMap, (const uint64_t*)pStoneNumber, pStoneCount);
    }
}
 
void removeFromEntry(stoneCountType* kStoneMap, const uint64_t nStoneNumber, const size_t nCount)
{
    size_t* pExistingCount = hashmap_get(kStoneMap, &nStoneNumber);
    if (pExistingCount)
    {
        *pExistingCount -= nCount;
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*    kData     = NULL;
        HashmapActionType* kStoneMapAdditions;
        size_t             nStoneMapAdditionMaxSize    = 16384u;
        size_t             nStoneMapAdditionSize       = 0u;

        HashmapActionType* kStoneMapSubtractions;
        size_t             nStoneMapSubtractionMaxSize = 16384u;
        size_t             nStoneMapSubtractionSize    = 0u;

        kStoneMapAdditions    = (HashmapActionType*)malloc(nStoneMapAdditionMaxSize    * sizeof(HashmapActionType));
        kStoneMapSubtractions = (HashmapActionType*)malloc(nStoneMapSubtractionMaxSize * sizeof(HashmapActionType));

        while (!feof(pData))
        {
            const uint64_t* pStoneNumberFree;
            const size_t*   pStoneCountFree;
            size_t nSize    = 0u;
            size_t nLength;

            /* Solve both Parts... */
            while ((int64_t)(nLength = getline(&kData, &nSize, pData)) != -1)
            {
                const uint64_t* pStoneNumber;
                const size_t*   pStoneCount;
                stoneCountType  kStonesMap;
                size_t          i;
 
                hashmap_init(&kStonesMap, hash_uint64, compare_uint64);
 
                /* Decode the Start List */
                const char*    kEntry    = kData;
                do
                {
                    const char*     kEndEntry = strstr(kEntry, " ");
 
                    addToEntry(&kStonesMap, strtoull(kEntry, NULL, 10), 1);
 
                    if (NULL == kEndEntry)
                    {
                        kEntry = NULL;
                    }
                    else
                    {
                        kEntry = kEndEntry + 1;
                    }
                } while (kEntry);
 
                for (i = 1; i <= 75; ++i)
                {
                    size_t j;

                    /* Initialise the Deltas */
                    nStoneMapAdditionSize    = 0u;
                    nStoneMapSubtractionSize = 0u;

                    hashmap_foreach(pStoneNumber, pStoneCount, &kStonesMap)
                    {
                        /* Ignore any Decimated Entries, these are left as Removing/Re-Adding is more expensive than ignoring */
                        if (*pStoneCount == 0)
                        {
                            continue;
                        }

                        /* Construct the Deltas to apply to the Hash Map */

                        /* 0 => 1 */
                        if (*pStoneNumber == 0)
                        {
                            AddToChangeList(&kStoneMapSubtractions, &nStoneMapSubtractionSize, &nStoneMapSubtractionMaxSize, 0, *pStoneCount);
                            AddToChangeList(&kStoneMapAdditions,    &nStoneMapAdditionSize,    &nStoneMapAdditionMaxSize,    1, *pStoneCount);
                        }
                        else
                        {
                            /* Split if Digit Count is Even */
                            const uint64_t nBufferedValue = *pStoneNumber;
                            uint64_t nBufferedValueCopy = nBufferedValue;
                            size_t nDigits        = 0;
                            while (nBufferedValueCopy > 0)
                            {
                                nBufferedValueCopy /= 10;
                                ++nDigits;
                            }
 
                            if (0 == (nDigits % 2))
                            {
                                const size_t nHalfDigits = nDigits / 2;
                                uint64_t nLeft;
                                uint64_t nRight;
                                size_t   nDigit;
                                size_t   nDivisor = 1;
                                for (nDigit = 0; nDigit < nHalfDigits; ++nDigit)
                                {
                                    nDivisor *= 10;
                                }

                                nLeft  = nBufferedValue / nDivisor;
                                nRight = nBufferedValue % nDivisor;

                                AddToChangeList(&kStoneMapSubtractions, &nStoneMapSubtractionSize, &nStoneMapSubtractionMaxSize, *pStoneNumber, *pStoneCount);
                                AddToChangeList(&kStoneMapAdditions,    &nStoneMapAdditionSize,    &nStoneMapAdditionMaxSize,    nLeft,         *pStoneCount);
                                AddToChangeList(&kStoneMapAdditions,    &nStoneMapAdditionSize,    &nStoneMapAdditionMaxSize,    nRight,        *pStoneCount);
                            }
                            /* Multiply by 2024 Otherwise */
                            else
                            {
                                AddToChangeList(&kStoneMapSubtractions, &nStoneMapSubtractionSize, &nStoneMapSubtractionMaxSize, *pStoneNumber,        *pStoneCount);
                                AddToChangeList(&kStoneMapAdditions,    &nStoneMapAdditionSize,    &nStoneMapAdditionMaxSize,    *pStoneNumber * 2024, *pStoneCount);
                            }
                        }
                    }

                    /* Apply the Deltas to the Existing Hashmap */
                    for (j = 0; j < nStoneMapAdditionSize; ++j)
                    {
                        addToEntry(&kStonesMap, kStoneMapAdditions[j].nStoneNumber, kStoneMapAdditions[j].nCount);
                    }
                    for (j = 0; j < nStoneMapSubtractionSize; ++j)
                    {
                        removeFromEntry(&kStonesMap, kStoneMapSubtractions[j].nStoneNumber, kStoneMapSubtractions[j].nCount);
                    }

                    if ((25 == i) || (75 == i))
                    {
                        uint64_t nCount = 0;
 
                        hashmap_foreach_data(pStoneCount, &kStonesMap)
                        {
                            nCount += *pStoneCount;
                        }
 
                        printf("Part %s: %llu\n", (25 == i) ? "1" : "2", nCount);
                    }
                }
 
                hashmap_foreach(pStoneNumberFree, pStoneCountFree, &kStonesMap)
                {
                    free((uint64_t*)pStoneNumberFree);
                    free((size_t*)pStoneCountFree);
                }
 
                hashmap_cleanup(&kStonesMap);
            }
 
        }
        fclose(pData);
 
        free(kStoneMapAdditions);
        free(kStoneMapSubtractions);
        free(kData);
    }
 
    return 0;
}