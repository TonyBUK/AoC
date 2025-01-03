#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

/* Inputs are two lowercase alpha letters, so we can effectively make this base 26 */
#define CACHE_ENTRY_SIZE (26*26)
#define TO_CACHE(k) (((k[0]-'a') * 26) + (k[1]-'a'))
#define TO_THRUPLE(k) (((size_t)(k[0]) * 26*26*26*26) + ((size_t)(k[1]) * 26*26) + (size_t)(k[2]))

#define FROM_CACHE_1(k) ((char)(((k / 26) % 26) + 'a'))
#define FROM_CACHE_2(k) ((char)((k % 26) + 'a'))

typedef struct CacheType
{
    unsigned char kCache[CACHE_ENTRY_SIZE];
    uint16_t      kCacheArray[CACHE_ENTRY_SIZE];
    size_t        nCacheCount;
} CacheType;

int compare(const void* pLeft, const void* pRight)
{
    const size_t nLeft  = *(uint16_t*)pLeft;
    const size_t nRight = *(uint16_t*)pRight;

    if (nLeft < nRight)
    {
        return -1;
    }
    else if (nLeft > nRight)
    {
        return 1;
    }
    return 0;
}

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+3)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);
 
    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }
 
    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if (0 == bProcessUnix)
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\r';
            (*pkFileBuffer)[nFileSize+1] = '\n';
            (*pkFileBuffer)[nFileSize+2] = '\0';
        }
    }
    else
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\n';
            (*pkFileBuffer)[nFileSize+1] = '\0';
        }
    }
 
    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);
 
        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;
 
            assert(pLine < &(*pkFileBuffer)[nFileSize]);
 
            (*pkLines)[nLineCount++] = pLine;
 
            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';
 
            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

void findThruple(const uint16_t kComputer, const CacheType* kConnectedComputers, uint16_t* kCurrentThruple, const size_t nCurrentThrupleLength, size_t* kThruples, size_t* nThrupleCount, size_t* nThrupleMaxSize)
{
    uint16_t nConnectedComputer;

    kCurrentThruple[nCurrentThrupleLength] = kComputer;

    if (nCurrentThrupleLength == 2)
    {
        const size_t nCurrentThrupleCount = *nThrupleCount;
        size_t       nCurrentThruple;
        size_t       i;
        uint16_t     kSortedThruple[3]    = { kCurrentThruple[0], kCurrentThruple[1], kCurrentThruple[2] };

        /* Sort the Current Thruple */
        if (kSortedThruple[0] > kSortedThruple[1])
        {
            const uint16_t nTemp = kSortedThruple[0];
            kSortedThruple[0]    = kSortedThruple[1];
            kSortedThruple[1]    = nTemp;
        }

        if (kSortedThruple[1] > kSortedThruple[2])
        {
            const uint16_t nTemp = kSortedThruple[1];
            kSortedThruple[1]    = kSortedThruple[2];
            kSortedThruple[2]    = nTemp;
        }

        if (kSortedThruple[0] > kSortedThruple[1])
        {
            const uint16_t nTemp = kSortedThruple[0];
            kSortedThruple[0]    = kSortedThruple[1];
            kSortedThruple[1]    = nTemp;
        }

        assert(kSortedThruple[0] < kSortedThruple[1]);
        assert(kSortedThruple[1] < kSortedThruple[2]);
        assert(kSortedThruple[0] < CACHE_ENTRY_SIZE);
        assert(kSortedThruple[1] < CACHE_ENTRY_SIZE);
        assert(kSortedThruple[2] < CACHE_ENTRY_SIZE);

        /* Decode the Current Thruple */
        nCurrentThruple = TO_THRUPLE(kSortedThruple);

        for (i = 0; i < nCurrentThrupleCount; ++i)
        {
            if (nCurrentThruple == kThruples[i])
            {
                return;
            }
        }

        kThruples[nCurrentThrupleCount] = nCurrentThruple;
        *nThrupleCount                 += 1;

        return;
    }

    for (nConnectedComputer = 0; nConnectedComputer < kConnectedComputers[kComputer].nCacheCount; ++nConnectedComputer)
    {
        const size_t kConnectedComputer = kConnectedComputers[kComputer].kCacheArray[nConnectedComputer];
        unsigned     bValid             = AOC_TRUE;
        size_t       i;

        for (i = 0; i < (nCurrentThrupleLength + 1); ++i)
        {
            if (kConnectedComputer == kCurrentThruple[i])
            {
                bValid = AOC_FALSE;
                break;
            }
            else if (kConnectedComputers[kConnectedComputer].kCache[kCurrentThruple[i]] == AOC_FALSE)
            {
                bValid = AOC_FALSE;
                break;
            }
        }

        if (AOC_TRUE == bValid)
        {
            findThruple(kConnectedComputer, kConnectedComputers, kCurrentThruple, nCurrentThrupleLength+1, kThruples, nThrupleCount, nThrupleMaxSize);
        }
    }
}

size_t findLargestGroupRecurse(const CacheType* kConnectedComputers, const uint16_t* kConnectedComputerKeys, const size_t nConnectedComputerKeyCount, unsigned char* kCurrentGroupConnectionsSubset, uint16_t* kCurrentGroupConnectionsSubsetArray, const size_t nCurrentGroupConnectionsSubsetCount, uint16_t* kCurrentGroup, const size_t nCurrentGroupSize, uint16_t* kLargestGroup, const size_t nLargestGroupSize)
{
    size_t nLargestGroupSizeLocal = nLargestGroupSize;
    size_t i;
    unsigned char kConnectedGroupConnectionsSubset[CACHE_ENTRY_SIZE];

    memset(kConnectedGroupConnectionsSubset, 0, CACHE_ENTRY_SIZE * sizeof(unsigned char));

    /*
     * If the current group is same or smaller than the largest group, then we can stop
     * as this group will never get bigger, only smaller.
     */
    if (nCurrentGroupConnectionsSubsetCount <= nLargestGroupSizeLocal)
    {
        return nLargestGroupSizeLocal;
    }

    /* Is this the largest group so far? */
    if (nCurrentGroupSize > nLargestGroupSizeLocal)
    {
        memcpy(kLargestGroup, kCurrentGroup, nCurrentGroupSize * sizeof(uint16_t));
        nLargestGroupSizeLocal = nCurrentGroupSize;
    }

    for (i = 0; i < nConnectedComputerKeyCount; ++i)
    {
        /* Get the next remaining Computer */
        const uint16_t kComputer = kConnectedComputerKeys[i];
        size_t         nConnectedComputerSubsetCount = 0;
        uint16_t       kConnectedGroupConnectionsSubsetArray[CACHE_ENTRY_SIZE];
        size_t         j;
        unsigned       bAllConnected = AOC_TRUE;

        /* Tentatively create a new group subset (including this computer) */
        for (j = 0; j < nCurrentGroupConnectionsSubsetCount; ++j)
        {
            if (kCurrentGroupConnectionsSubset[kCurrentGroupConnectionsSubsetArray[j]] == AOC_TRUE)
            {
                if (kConnectedComputers[kComputer].kCache[kCurrentGroupConnectionsSubsetArray[j]] == AOC_TRUE)
                {
                    const uint16_t nConnectedComputer = kCurrentGroupConnectionsSubsetArray[j];
                    kConnectedGroupConnectionsSubsetArray[nConnectedComputerSubsetCount++] = nConnectedComputer;
                    kConnectedGroupConnectionsSubset[nConnectedComputer]                   = AOC_TRUE;
                }
            }
        }

        /* If all computers are connected */
        for (j = 0; j < nCurrentGroupSize; ++j)
        {
            if (kConnectedGroupConnectionsSubset[kCurrentGroup[j]] == AOC_FALSE)
            {
                bAllConnected = AOC_FALSE;
                break;
            }
        }

        if (bAllConnected)
        {
            kCurrentGroup[nCurrentGroupSize] = kComputer;

            /* Recurse with the new group subset */
            nLargestGroupSizeLocal = findLargestGroupRecurse(kConnectedComputers, &kConnectedComputerKeys[i+1], nConnectedComputerKeyCount - i - 1, kConnectedGroupConnectionsSubset, kConnectedGroupConnectionsSubsetArray, nConnectedComputerSubsetCount, kCurrentGroup, nCurrentGroupSize+1, kLargestGroup, nLargestGroupSizeLocal);
        }

        /* Cleanup */
        for (j = 0; j < nConnectedComputerSubsetCount; ++j)
        {
            kConnectedGroupConnectionsSubset[kConnectedGroupConnectionsSubsetArray[j]] = AOC_FALSE;
        }
    }

    return nLargestGroupSizeLocal;
}

size_t findLargestGroup(const CacheType* kConnectedComputers, const uint16_t* kConnectedComputerKeys, const size_t nConnectedComputerKeyCount, uint16_t* kLargestGroup)
{
    size_t   nLargestGroupSize = 0;
    size_t   i;
    uint16_t kCurrentGroup[CACHE_ENTRY_SIZE];

    for (i = 0; i < nConnectedComputerKeyCount; ++i)
    {
        /* Get the next Computer of Interest */
        const uint16_t kComputer                           = kConnectedComputerKeys[i];
        const size_t   nCurrentGroupConnectionsSubsetCount = kConnectedComputers[kComputer].nCacheCount;
        unsigned char  kCurrentGroupConnectionsSubset[CACHE_ENTRY_SIZE];
        uint16_t       kCurrentGroupConnectionsSubsetArray[CACHE_ENTRY_SIZE];

        if (nCurrentGroupConnectionsSubsetCount > nLargestGroupSize)
        {
            /* Initialise the Current Group */
            kCurrentGroup[0] = kComputer;

            /* As we have no connections in the current group, then we can start with this computer... */
            memcpy(kCurrentGroupConnectionsSubset, kConnectedComputers[kComputer].kCache, CACHE_ENTRY_SIZE * sizeof(unsigned char));
            memcpy(kCurrentGroupConnectionsSubsetArray, kConnectedComputers[kComputer].kCacheArray, nCurrentGroupConnectionsSubsetCount * sizeof(uint16_t));

            /* Recurse with the new group subset */
            nLargestGroupSize = findLargestGroupRecurse(kConnectedComputers, &kConnectedComputerKeys[i+1], nConnectedComputerKeyCount - i - 1, kCurrentGroupConnectionsSubset, kCurrentGroupConnectionsSubsetArray, nCurrentGroupConnectionsSubsetCount, kCurrentGroup, 1, kLargestGroup, nLargestGroupSize);
        }
    }

    return nLargestGroupSize;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kNetworkBuffer;
        char**                      kNetworkLinks;

        size_t                      nNumNetworkLinks;

        CacheType*                  kCache;
        uint16_t*                   kCacheKeys;
        size_t                      nCacheKeyCount = 0;

        size_t*                     kThruples;
        size_t                      nThrupleCount   = 0;
        size_t                      nThrupleMaxSize = 4096;

        uint16_t                    kLargestGroup[CACHE_ENTRY_SIZE];
        size_t                      nLargestGroupSize;

        size_t                      i;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kNetworkBuffer, &kNetworkLinks, &nNumNetworkLinks, NULL, NULL, 0);
        fclose(pData);

        /*
         * Allocate the Cache
         *
         * Note: Nodes are bidirectional, so assume we're creating a reverse node for each.
         */
        kCache     = (CacheType*)calloc(CACHE_ENTRY_SIZE, sizeof(CacheType));
        kCacheKeys = (uint16_t*)malloc(CACHE_ENTRY_SIZE * sizeof(uint16_t));
        kThruples  = (size_t*)malloc(nThrupleMaxSize * sizeof(size_t));

        for (i = 0; i < nNumNetworkLinks; ++i)
        {
            const char*  kLink1 = kNetworkLinks[i];
            const char*  kLink2 = kLink1 + 3;

            const uint16_t nCacheKey1 = TO_CACHE(kLink1);
            const uint16_t nCacheKey2 = TO_CACHE(kLink2);

            assert(kLink1[2] == '-');
            assert(kLink2[2] == '\0');

            assert(nCacheKey1 < CACHE_ENTRY_SIZE);
            assert(nCacheKey2 < CACHE_ENTRY_SIZE);

            /* Add Key to Cache - Forwards */
            if (kCache[nCacheKey1].nCacheCount == 0)
            {
                kCacheKeys[nCacheKeyCount++] = nCacheKey1;
            }
            kCache[nCacheKey1].kCacheArray[kCache[nCacheKey1].nCacheCount++] = nCacheKey2;
            kCache[nCacheKey1].kCache[nCacheKey2]                            = AOC_TRUE;

            /* Add Key to Cache - Reverse */
            if (kCache[nCacheKey2].nCacheCount == 0)
            {
                kCacheKeys[nCacheKeyCount++] = nCacheKey2;
            }
            kCache[nCacheKey2].kCacheArray[kCache[nCacheKey2].nCacheCount++] = nCacheKey1;
            kCache[nCacheKey2].kCache[nCacheKey1]                            = AOC_TRUE;
        }

        /* Part One: Find the Thruples */
        for (i = 0; i < nCacheKeyCount; ++i)
        {
            const uint16_t nCurrentItemStart = kCacheKeys[i] / 26;
            uint16_t       kCurrentThruple[3];
            if (nCurrentItemStart == ('t' - 'a'))
            {
                findThruple(kCacheKeys[i], kCache, kCurrentThruple, 0, kThruples, &nThrupleCount, &nThrupleMaxSize);
            }
        }

        printf("Part 1: %zu\n", nThrupleCount);

        /* Part Two: Find the Largest Group */

        /*
         *Note: For Part Two, we'll add each computer key to its own list of connected computers (i.e. it
         *       connects to itself.
         */
        for (i = 0; i < nCacheKeyCount; ++i)
        {
            const uint16_t kComputer = kCacheKeys[i];
            kCache[kComputer].kCacheArray[kCache[kComputer].nCacheCount++] = kComputer;
            kCache[kComputer].kCache[kComputer]                            = AOC_TRUE;
        }
        nLargestGroupSize = findLargestGroup(kCache, kCacheKeys, nCacheKeyCount, (uint16_t*)kLargestGroup);
        qsort(kLargestGroup, nLargestGroupSize, sizeof(uint16_t), compare);

        printf("Part 2: ");
        for (i = 0; i < nLargestGroupSize; ++i)
        {
            printf("%c%c", FROM_CACHE_1(kLargestGroup[i]), FROM_CACHE_2(kLargestGroup[i]));
            if (i < nLargestGroupSize-1)
            {
                printf(",");
            }
        }
        printf("\n");

        /* Cleanup */
        free(kNetworkLinks);
        free(kNetworkBuffer);
        free(kCache);
        free(kCacheKeys);
    }
 
    return 0;
}