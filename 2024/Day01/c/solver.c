#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* We need to be careful here as the result needs to be signed, meaning
 * we either perform the comparison as a signed operation in a larger
 * type, or just do a comparison.
 */
int compare(const void *a, const void *b)
{
    const uint32_t arg1 = *(const uint32_t*)a;
    const uint32_t arg2 = *(const uint32_t*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return  1;

    return 0;
}

uint32_t GetDestination(FILE** pData)
{
    uint32_t nDestinationId = 0u;
    uint32_t nLength        = 0u;
 
    while (!feof(*pData))
    {
        char kData = fgetc(*pData);
 
        if ((' ' == kData) && (0 == nLength))
        {
            continue;
        }

        if ((' ' == kData) || ('\n' == kData) || !!feof(*pData))
        {
            if (0 == nLength)
            {
                return 0xFFFFFFFFu;
            }
            else
            {
                return nDestinationId;
            }
        }

        assert((kData >= '0') && (kData <= '9'));
        nDestinationId = (nDestinationId * 10u) + (kData - '0');
        ++nLength;
    }
 
    return 0xFFFFFFFFu;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        size_t      nBufferLength;
        uint32_t    nLargestValue = 0x80000u;
        uint32_t*   kLeftList     = NULL;
        uint32_t*   kRightList    = NULL;
        uint32_t*   kCounterList  = NULL;

        /* Get a Safe Buffer Size */
        fseek(pData, 0, SEEK_END);
        nBufferLength = ftell(pData);
        fseek(pData, 0, SEEK_SET);

        /* Allocate Buffer */
        kLeftList    = (uint32_t*)malloc(nBufferLength * sizeof(uint32_t));
        kRightList   = (uint32_t*)malloc(nBufferLength * sizeof(uint32_t));
        kCounterList = (uint32_t*)calloc(nLargestValue,  sizeof(uint32_t));

        /* Read the Data */
        nBufferLength = 0u;
        do
        {
            /* Get the Left/Right Lists */
            kLeftList[nBufferLength]  = GetDestination(&pData);
            kRightList[nBufferLength] = GetDestination(&pData);

            /* Check to see whether the right list has a larger number than the
             * counter list can store, if so, increment.
             */
            if (kRightList[nBufferLength] > nLargestValue)
            {
                kCounterList = (uint32_t*)realloc(kCounterList, nLargestValue * 2 * sizeof(uint32_t));
                memset(&kCounterList[nLargestValue], 0, nLargestValue * sizeof(uint32_t));
                nLargestValue *= 2;
            }

            /* Increment the Seen Counter for this Number which is used for the
             * similarity score.
             */
            ++kCounterList[kRightList[nBufferLength]];

            /* Increment the Buffer Length */
            ++nBufferLength;
        } while (!feof(pData));

        /* Sort the Lists */
        qsort(kLeftList,  nBufferLength, sizeof(uint32_t), compare);
        qsort(kRightList, nBufferLength, sizeof(uint32_t), compare);

        /* Calculate the Distances and Similarity Score */
        uint64_t nDistance   = 0u;
        uint64_t nSimilarity = 0u;
        for (size_t nIndex = 0; nIndex < nBufferLength; ++nIndex)
        {
            /* Part 1: Distances */
            nDistance += MAX(kRightList[nIndex], kLeftList[nIndex]) -
                         MIN(kRightList[nIndex], kLeftList[nIndex]);

            /* Part 2: Similarity */
            nSimilarity += kLeftList[nIndex] * kCounterList[kLeftList[nIndex]];
        }

        printf("Part 1: %llu\n", nDistance);
        printf("Part 2: %llu\n", nSimilarity);

        /* Cleanup */
        free(kLeftList);
        free(kRightList);
        free(kCounterList);
        fclose(pData);
    }
 
    return 0;
}