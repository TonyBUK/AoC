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

typedef struct HashmapActionBufferType
{
    HashmapActionType* kBuffer;
    size_t             nSize;
    size_t             nMaxSize;
} HashmapActionBufferType;

typedef struct StoneNumberBufferType
{
    uint64_t* kBuffer;
    size_t    nSize;
    size_t    nMaxSize;
} StoneNumberBufferType;

typedef struct StoneCountBufferType
{
    size_t*   kBuffer;
    size_t    nSize;
    size_t    nMaxSize;
} StoneCountBufferType;

typedef HASHMAP(uint64_t, size_t) stoneCountType;

/*
 * As the input is a 64 bit value, just use it directly rather than hashing.
 */
size_t hash_uint64(const uint64_t* key)
{
    return (size_t)*key;
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

void AddToChangeList(HashmapActionBufferType* kStoneMapBuffer, const uint64_t nStoneNumber, const size_t nCount)
{
    if (kStoneMapBuffer->nSize >= kStoneMapBuffer->nMaxSize)
    {
        kStoneMapBuffer->nMaxSize *= 2;
        kStoneMapBuffer->kBuffer   = (HashmapActionType*)realloc(kStoneMapBuffer->kBuffer, kStoneMapBuffer->nMaxSize * sizeof(HashmapActionType));
    }

    kStoneMapBuffer->kBuffer[kStoneMapBuffer->nSize].nStoneNumber = nStoneNumber;
    kStoneMapBuffer->kBuffer[kStoneMapBuffer->nSize].nCount       = nCount;

    kStoneMapBuffer->nSize += 1;
}

void addToEntry(stoneCountType* kStoneMap, const uint64_t nStoneNumber, const size_t nCount, StoneNumberBufferType* kStoneNumberBuffer, StoneCountBufferType* kStoneCountBuffer)
{
    size_t* pExistingCount = hashmap_get(kStoneMap, &nStoneNumber);
    if (pExistingCount)
    {
        *pExistingCount += nCount;
    }
    else
    {
        if (kStoneNumberBuffer->nSize >= kStoneNumberBuffer->nMaxSize)
        {
            kStoneNumberBuffer->nMaxSize *= 2;
            kStoneNumberBuffer->kBuffer = (uint64_t*)realloc(kStoneNumberBuffer->kBuffer, kStoneNumberBuffer->nMaxSize * sizeof(uint64_t));
        }

        if (kStoneCountBuffer->nSize >= kStoneCountBuffer->nMaxSize)
        {
            kStoneCountBuffer->nMaxSize *= 2;
            kStoneCountBuffer->kBuffer = (size_t*)realloc(kStoneCountBuffer->kBuffer, kStoneCountBuffer->nMaxSize * sizeof(size_t));
        }

        uint64_t* pStoneNumber = &kStoneNumberBuffer->kBuffer[kStoneNumberBuffer->nSize++];
        size_t*   pStoneCount  = &kStoneCountBuffer->kBuffer[kStoneCountBuffer->nSize++];
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
        char*                   kData                   = NULL;
        HashmapActionBufferType kStoneMapAdditions      = { NULL, 0u, 16384u };
        HashmapActionBufferType kStoneMapSubtractions   = { NULL, 0u, 16384u };

        /*
         * The Hash Map needs the Address of the Key/Data items we're storing, meaning stack pointers don't work great here,
         * and we don't want to heap allocate every time we add an item, so instead we'll pre-allocate a large buffer for
         * each and use that to store the data.
         */
        StoneNumberBufferType   kStoneNumberBuffer      = { NULL, 0u, 16384u };
        StoneCountBufferType    kStoneCountBuffer       = { NULL, 0u, 16384u };

        kStoneMapAdditions.kBuffer    = (HashmapActionType*)malloc(kStoneMapAdditions.nMaxSize    * sizeof(HashmapActionType));
        kStoneMapSubtractions.kBuffer = (HashmapActionType*)malloc(kStoneMapSubtractions.nMaxSize * sizeof(HashmapActionType));
        kStoneNumberBuffer.kBuffer    = (uint64_t*)malloc(kStoneNumberBuffer.nMaxSize * sizeof(uint64_t));
        kStoneCountBuffer.kBuffer     = (size_t*)malloc(kStoneCountBuffer.nMaxSize * sizeof(size_t));

        while (!feof(pData))
        {
            size_t nSize;
            size_t nLength;

            /* Solve both Parts... */
            while ((int64_t)(nLength = getline(&kData, &nSize, pData)) != -1)
            {
                const uint64_t* pStoneNumber;
                const size_t*   pStoneCount;
                stoneCountType  kStonesMap;
                size_t          i;

                kStoneNumberBuffer.nSize = 0u;
                kStoneCountBuffer.nSize  = 0u;
 
                hashmap_init(&kStonesMap, hash_uint64, compare_uint64);
 
                /* Decode the Start List */
                const char*    kEntry    = kData;
                do
                {
                    const char* kEndEntry = strstr(kEntry, " ");
 
                    addToEntry(&kStonesMap, strtoull(kEntry, NULL, 10), 1,
                               &kStoneNumberBuffer, &kStoneCountBuffer);
 
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
                    kStoneMapAdditions.nSize    = 0u;
                    kStoneMapSubtractions.nSize = 0u;
 
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
                            AddToChangeList(&kStoneMapSubtractions, 0, *pStoneCount);
                            AddToChangeList(&kStoneMapAdditions,    1, *pStoneCount);
                        }
                        else
                        {
                            /* Split if Digit Count is Even */
                            const uint64_t nBufferedValue     = *pStoneNumber;
                            uint64_t       nBufferedValueCopy = nBufferedValue;
                            size_t         nDigits            = 0;
                            while (nBufferedValueCopy > 0)
                            {
                                nBufferedValueCopy /= 10;
                                ++nDigits;
                            }
 
                            if (0 == (nDigits % 2))
                            {
                                const size_t nHalfDigits = nDigits / 2;
                                size_t       nDivisor   = 1;
                                uint64_t     nLeft;
                                uint64_t     nRight;
                                size_t       nDigit;
                                for (nDigit = 0; nDigit < nHalfDigits; ++nDigit)
                                {
                                    nDivisor *= 10;
                                }

                                nLeft  = nBufferedValue / nDivisor;
                                nRight = nBufferedValue % nDivisor;

                                AddToChangeList(&kStoneMapSubtractions, *pStoneNumber, *pStoneCount);
                                AddToChangeList(&kStoneMapAdditions,    nLeft,         *pStoneCount);
                                AddToChangeList(&kStoneMapAdditions,    nRight,        *pStoneCount);
                            }
                            /* Multiply by 2024 Otherwise */
                            else
                            {
                                AddToChangeList(&kStoneMapSubtractions, *pStoneNumber,        *pStoneCount);
                                AddToChangeList(&kStoneMapAdditions,    *pStoneNumber * 2024, *pStoneCount);
                            }
                        }
                    }

                    /* Apply the Deltas to the Existing Hashmap */
                    for (j = 0; j < kStoneMapAdditions.nSize; ++j)
                    {
                        addToEntry(&kStonesMap, kStoneMapAdditions.kBuffer[j].nStoneNumber, kStoneMapAdditions.kBuffer[j].nCount,
                                   &kStoneNumberBuffer, &kStoneCountBuffer);
                    }
                    for (j = 0; j < kStoneMapSubtractions.nSize; ++j)
                    {
                        removeFromEntry(&kStonesMap, kStoneMapSubtractions.kBuffer[j].nStoneNumber, kStoneMapSubtractions.kBuffer[j].nCount);
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
 
                hashmap_cleanup(&kStonesMap);
            }
 
        }
        fclose(pData);
 
        free(kStoneMapAdditions.kBuffer);
        free(kStoneMapSubtractions.kBuffer);
        free(kStoneNumberBuffer.kBuffer);
        free(kStoneCountBuffer.kBuffer);
        free(kData);
    }
 
    return 0;
}