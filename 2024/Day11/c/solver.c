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
 
typedef HASHMAP(uint64_t, size_t) stoneCountType;
 
/*
 * Use generic hash algorithm supplied by the hashmap library.
 */
size_t hash_uint64(const uint64_t *key)
{
    return hashmap_hash_default(key, sizeof(*key));
}
 
int compare_uint64(const uint64_t *a, const uint64_t *b)
{
    return memcmp(a, b, sizeof(uint64_t));
}
 
void putEntry(stoneCountType* kStoneMap, const uint64_t nStoneNumber, const size_t nCount)
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
 
int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*    kData     = NULL;
 
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
                stoneCountType* kStonesMap = (stoneCountType*)malloc(sizeof(stoneCountType));
                size_t          i;
 
                hashmap_init(kStonesMap, hash_uint64, compare_uint64);
 
                /* Decode the Start List */
                const char*    kEntry    = kData;
                do
                {
                    const char*     kEndEntry = strstr(kEntry, " ");
 
                    putEntry(kStonesMap, strtoull(kEntry, NULL, 10), 1);
 
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
                    stoneCountType* kNewStonesMap = (stoneCountType*)malloc(sizeof(stoneCountType));
                    stoneCountType* kTempMap;
                    hashmap_init(kNewStonesMap, hash_uint64, compare_uint64);
 
                    hashmap_foreach(pStoneNumber, pStoneCount, kStonesMap)
                    {
                        if (*pStoneNumber == 0)
                        {
                            putEntry(kNewStonesMap, 1, *pStoneCount);
                        }
                        else
                        {
                            uint64_t kDigits[20];
                            uint64_t nBufferedValue = *pStoneNumber;
                            size_t nDigits        = 0;
                            while (nBufferedValue > 0)
                            {
                                kDigits[nDigits++] = nBufferedValue % 10;
                                nBufferedValue /= 10;
                            }
 
                            if (0 == (nDigits % 2))
                            {
                                const size_t nHalfDigits = nDigits / 2;
                                uint64_t nLeft  = 0;
                                uint64_t nRight = 0;
                                size_t nDigit;
                                for (nDigit = 0; nDigit < nHalfDigits; ++nDigit)
                                {
                                    nLeft  = (10 * nLeft)  + kDigits[nDigits - nDigit - 1];
                                    nRight = (10 * nRight) + kDigits[nDigits - nDigit - 1 - nHalfDigits];
                                }
 
                                putEntry(kNewStonesMap, nLeft,  *pStoneCount);
                                putEntry(kNewStonesMap, nRight, *pStoneCount);
                            }
                            else
                            {
                                putEntry(kNewStonesMap, *pStoneNumber * 2024,  *pStoneCount);
                            }
                        }
                    }
 
                    if ((25 == i) || (75 == i))
                    {
                        uint64_t nCount = 0;
 
                        hashmap_foreach(pStoneNumber, pStoneCount, kNewStonesMap)
                        {
                            nCount += *pStoneCount;
                        }
 
                        printf("Part %s: %llu\n", (25 == i) ? "1" : "2", nCount);
                    }
 
                    /* Swap the Hash Maps */
                    kTempMap      = kNewStonesMap;
                    kNewStonesMap = kStonesMap;
                    kStonesMap    = kTempMap;
 
                    hashmap_foreach(pStoneNumberFree, pStoneCountFree, kNewStonesMap)
                    {
                        free((uint64_t*)pStoneNumberFree);
                        free((size_t*)pStoneCountFree);
                    }
 
                    hashmap_cleanup(kNewStonesMap);
                    free(kNewStonesMap);
 
                }
 
                hashmap_foreach(pStoneNumberFree, pStoneCountFree, kStonesMap)
                {
                    free((uint64_t*)pStoneNumberFree);
                    free((size_t*)pStoneCountFree);
                }
 
                hashmap_cleanup(kStonesMap);
                free(kStonesMap);
            }
 
        }
        fclose(pData);
 
        free(kData);
    }
 
    return 0;
}