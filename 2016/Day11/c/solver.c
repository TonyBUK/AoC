#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define MAX_FLOORS    4
#define MAX_ITEMS     (((sizeof(uint64_t) * 8) / 2) / BITS_PER_ITEM)
#define BITS_PER_ITEM 2
#define generateMask(nItem) (1 << ((nItem) * BITS_PER_ITEM))
#define Move(nFloor, nNewFloor, kFloorLookup) \
    kFloorLookup.kElements[nFloor]    ^= kFloorLookup.nMask; \
    kFloorLookup.kElements[nNewFloor] |= kFloorLookup.nMask;

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

typedef enum ItemType {E_GENERATOR = 0, E_MICROCHIP = 1} ItemType;

typedef struct CacheType
{
    uint64_t nCache;
    uint64_t nDepth;
} CacheType;

typedef struct FloorProcessType
{
    uint64_t  nMask;
    uint64_t* kElements;
} FloorProcessType;

uint64_t solveRecursive(const uint64_t nFloor, const uint64_t nDepth, const uint64_t nBestSolution, uint64_t* kGenerators, uint64_t* kMicrochips, CacheType** kCache, uint64_t* nCacheSize, uint64_t* nMaxCacheSize);

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

uint64_t registerElement(const char* kElement, uint64_t* nItems, char** kItems)
{
    uint64_t nItem;
    for (nItem = 0; nItem < *nItems; ++nItem)
    {
        if (0 == strcmp(kItems[nItem], kElement))
        {
            return generateMask(nItem);
        }
    }

    assert((*nItems + 1) < MAX_ITEMS);

    kItems[*nItems] = (char*)malloc((strlen(kElement) + 1) * sizeof(char));
    strcpy(kItems[*nItems], kElement);

    *nItems += 1;

    return generateMask(*nItems - 1);
}

uint64_t countElements(const uint64_t nElements)
{
    uint64_t nCount         = 0;
    uint64_t nElementsLocal = nElements;
    while (nElementsLocal != 0)
    {
        if (0x1 == (nElementsLocal & 0x1))
        {
            ++nCount;
        }

        nElementsLocal = nElementsLocal >> BITS_PER_ITEM;
    }
    return nCount;
}

int compare(const void* a, const void* b)
{
    const uint64_t nLeft  = *(uint64_t*)a;
    const uint64_t nRight = *(uint64_t*)b;

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

uint64_t elementState(const uint64_t kElements, const uint64_t* kOther, uint64_t* nLength, uint64_t* nCount)
{
    const uint64_t nUnused = (uint64_t)-1;
    uint64_t       kElementsOnFloor[MAX_ITEMS];
    uint64_t       nMask   = 0;
    uint64_t       i;

    *nCount = 0;

    for (i = 0; i < MAX_ITEMS; ++i)
    {
        const uint64_t nCandidate = generateMask(i);
        kElementsOnFloor[i] = nUnused;
        if (nCandidate == (kElements & nCandidate))
        {
            uint64_t j;
            for (j = 0; j < MAX_FLOORS; ++j)
            {
                if (nCandidate == (nCandidate & kOther[j]))
                {
                    kElementsOnFloor[i] = j;
                    break;
                }
            }
        }
    }

    qsort(kElementsOnFloor, MAX_ITEMS, sizeof(uint64_t), &compare);

    *nLength = 0;
    for (i = 0; i < MAX_ITEMS; ++i)
    {
        if (kElementsOnFloor[i] < MAX_FLOORS)
        {
            nMask    |= generateMask(i) * kElementsOnFloor[i];
            *nLength += BITS_PER_ITEM;
            *nCount += 1;
        }
        else
        {
            break;
        }
    }

    assert(*nCount <= 7);

    return nMask;
}

uint64_t state(const uint64_t* kGenerators, const uint64_t* kMicrochips)
{
    uint64_t i;
    uint64_t nCumulativeShift = 24;
    uint64_t nSizeShift       = 0;
    uint64_t nMask            = 0;
    uint64_t nCount           = 0;

    for (i = 0; i < MAX_FLOORS; ++i)
    {
        uint64_t       nItemLength;
        const uint64_t nFloorMask = elementState(kGenerators[i], kMicrochips, &nItemLength, &nCount);

        nMask |= nFloorMask << nCumulativeShift;
        nMask |= nCount << nSizeShift;
        nCumulativeShift += nItemLength;
        nSizeShift       += 3;
    }

    assert(nCumulativeShift < (sizeof(uint64_t) * 8));

    for (i = 0; i < MAX_FLOORS; ++i)
    {
        uint64_t       nItemLength;
        const uint64_t nFloorMask = elementState(kMicrochips[i], kGenerators, &nItemLength, &nCount);

        nMask |= nFloorMask << nCumulativeShift;
        nMask |= nCount << nSizeShift;
        nCumulativeShift += nItemLength;
        nSizeShift       += 3;
    }

    assert(nSizeShift       <= 24);
    assert(nCumulativeShift < (sizeof(uint64_t) * 8));

    return nMask;
}

uint64_t minMoves(const uint64_t* kGenerators, const uint64_t* kMicrochips)
{
    uint64_t nCount = 0;
    size_t   i;

    for (i = 0; i < (MAX_FLOORS - 1); ++i)
    {
        const uint64_t nFloorCount = countElements(kGenerators[i]) + countElements(kMicrochips[i]);

        if (0 != nFloorCount)
        {
            const uint64_t nFloorMultiplier = MAX_FLOORS - i - 1;

            if (nFloorCount <= 2) nCount += nFloorMultiplier;
            else                  nCount += (3 + ((nFloorCount - 3) * 2)) * nFloorMultiplier;
        }
    }

    return nCount;
}

bool valid(const uint64_t* kGenerators, const uint64_t* kMicrochips)
{
    size_t i;
    for (i = 0; i < MAX_FLOORS; ++i)
    {
        if (0 != kGenerators[i])
        {
            if (kMicrochips[i] != (kMicrochips[i] & kGenerators[i]))
            {
                return false;
            }
        }
    }
    return true;
}

bool solved(const uint64_t* kGenerators, const uint64_t* kMicrochips)
{
    size_t i;
    for (i = 0; i < (MAX_FLOORS - 1); ++i)
    {
        if (0 != kGenerators[i])
        {
            return false;
        }
        else if (0 != kMicrochips[i])
        {
            return false;
        }
    }

    assert(kGenerators[MAX_FLOORS-1] == kMicrochips[MAX_FLOORS-1]);

    return true;
}

void AddToLookup(const uint64_t nElements, const uint64_t nIgnoreList, uint64_t* kTarget, uint64_t* nCount, FloorProcessType* kFloorLookup)
{
    uint64_t i;
    for (i = 0; i < MAX_ITEMS; ++i)
    {
        const uint64_t nCandidate = generateMask(i);

        if (0 == (nCandidate & nIgnoreList))
        {
            if (nCandidate == (nCandidate & nElements))
            {
                kFloorLookup[*nCount].nMask     = nCandidate;
                kFloorLookup[*nCount].kElements = kTarget;

                *nCount += 1;
            }
        }
    }
}

bool checkCache(const uint64_t nFloor, const uint64_t nDepth, uint64_t* kGenerators, uint64_t* kMicrochips, CacheType** kCache, uint64_t* nCacheSize, uint64_t* nMaxCacheSize)
{
    const uint64_t nState = state(kGenerators, kMicrochips);
    uint64_t i;
    bool     bFound = false;

    for (i = 0; i < nCacheSize[nFloor]; ++i)
    {
        if (nState == kCache[nFloor][i].nCache)
        {
            if (nDepth >= kCache[nFloor][i].nDepth)
            {
                return true;
            }

            kCache[nFloor][i].nDepth = nDepth;
            bFound                   = true;
            break;
        }
    }

    if (false == bFound)
    {
        nCacheSize[nFloor] += 1;

        if (nCacheSize[nFloor] == nMaxCacheSize[nFloor])
        {
            nMaxCacheSize[nFloor] *= 2;
            {
                CacheType* kNewCache = (CacheType*)malloc(nMaxCacheSize[nFloor] * sizeof(CacheType));
                for (i = 0; i < nCacheSize[nFloor]; ++i)
                {
                    kNewCache[i] = kCache[nFloor][i];
                }
                free(kCache[nFloor]);
                kCache[nFloor] = kNewCache;
            }
        }

        assert(nCacheSize[nFloor] < nMaxCacheSize[nFloor]);
        kCache[nFloor][nCacheSize[nFloor] - 1].nCache = nState;
        kCache[nFloor][nCacheSize[nFloor] - 1].nDepth = nDepth;
    }

    return false;
}

uint64_t checkPermutations(const uint64_t nFloor, const uint64_t nDepth, const uint64_t nBestSolution, uint64_t* kGenerators, uint64_t* kMicrochips, CacheType** kCache, uint64_t* nCacheSize, uint64_t* nMaxCacheSize)
{
    const int64_t kTests[4][2] =
    {
        {    1, 2   },
        {   -1, 1   },
        {    1, 1   },
        {   -1, 2   }
    };

    FloorProcessType kFloorLookup[MAX_ITEMS * 2];
    uint64_t         nFloorLookupSize = 0;
    uint64_t         nTest;

    uint64_t        nBestSolutionLocal = nBestSolution;

    /* One thing we can optimize is on floors where multiple pairs exist, ignore all but one of the pairs
        * We can do this because the pairs are essentially interchangeable
        */
    uint64_t         nIgnoreMask = kGenerators[nFloor] & kMicrochips[nFloor];

    /* Ahem... all but *one* of the pairs */
    if (0 != nIgnoreMask)
    {
        uint64_t i;
        for (i = 0; i < MAX_ITEMS; ++i)
        {
            const uint64_t nCandidate = generateMask(i);
            if (nCandidate == (nCandidate & nIgnoreMask))
            {
                nIgnoreMask ^= nCandidate;
                break;
            }
        }
    }

    /* Create the List of Microchips/Generators on each Floor as something we can trivially iterate on and filter */
    AddToLookup(kGenerators[nFloor], nIgnoreMask, kGenerators, &nFloorLookupSize, kFloorLookup);
    AddToLookup(kMicrochips[nFloor], nIgnoreMask, kMicrochips, &nFloorLookupSize, kFloorLookup);

    for (nTest = 0; nTest < (sizeof(kTests) / sizeof(kTests[0])); ++nTest)
    {
        const int64_t* kTest     = kTests[nTest];
        const int64_t  nNewFloor = (int64_t)nFloor + kTest[0];
        uint64_t       i;
        if (nNewFloor < 0)           continue;
        if (nNewFloor >= MAX_FLOORS) continue;

        for (i = 0; i < nFloorLookupSize - kTest[1] + 1; ++i)
        {
            Move(nFloor, nNewFloor, kFloorLookup[i]);

            if (2 == kTest[1])
            {
                uint64_t j;
                for (j = i + 1; j < nFloorLookupSize; ++j)
                {
                    Move(nFloor, nNewFloor, kFloorLookup[j]);
                    nBestSolutionLocal = solveRecursive(nNewFloor, nDepth + 1, nBestSolutionLocal, kGenerators, kMicrochips, kCache, nCacheSize, nMaxCacheSize);
                    Move(nNewFloor, nFloor, kFloorLookup[j]);
                }
            }
            else if (1 == kTest[1])
            {
                nBestSolutionLocal = solveRecursive(nNewFloor, nDepth + 1, nBestSolutionLocal, kGenerators, kMicrochips, kCache, nCacheSize, nMaxCacheSize);
            }
            else
            {
                assert(false);
            }

            Move(nNewFloor, nFloor, kFloorLookup[i]);
        }
    }

    return nBestSolutionLocal;
}

uint64_t solveRecursive(const uint64_t nFloor, const uint64_t nDepth, const uint64_t nBestSolution, uint64_t* kGenerators, uint64_t* kMicrochips, CacheType** kCache, uint64_t* nCacheSize, uint64_t* nMaxCacheSize)
{
    /* Have We Failed? */
    if (false == valid(kGenerators, kMicrochips))
    {
        return nBestSolution;
    }

    /* Are there enough moves remaining to solve? */
    if ((nDepth + minMoves(kGenerators, kMicrochips)) >= nBestSolution)
    {
        return nBestSolution;
    }

    /* Have we encountered this game state before? */
    if (checkCache(nFloor, nDepth, kGenerators, kMicrochips, kCache, nCacheSize, nMaxCacheSize))
    {
        return nBestSolution;
    }

    /* Have we Solved the Puzzle? */
    if (solved(kGenerators, kMicrochips))
    {
        return nDepth;
    }

    /* Otherwise... carry on solving the Puzzle */

    return checkPermutations(nFloor, nDepth, nBestSolution, kGenerators, kMicrochips, kCache, nCacheSize, nMaxCacheSize);
}

uint64_t solve(uint64_t* kGenerators, uint64_t* kMicrochips)
{
    uint64_t       nSolution;

    uint64_t       nInitialCacheSize[MAX_FLOORS]    = {2048,2048,2048,2048};
    uint64_t       nCacheSize[MAX_FLOORS]           = {0,0,0,0};
    const uint64_t nInitial                         = (uint64_t)-1;
    CacheType*     kCache[MAX_FLOORS];
    uint64_t       i;

    for (i = 0; i < MAX_FLOORS; ++i)
    {
        kCache[i] = (CacheType*)malloc(nInitialCacheSize[i] * sizeof(CacheType));
    }

    nSolution =  solveRecursive(0, 0, nInitial, kGenerators, kMicrochips, kCache, nCacheSize, nInitialCacheSize);

    for (i = 0; i < MAX_FLOORS; ++i)
    {
        free(kCache[i]);
    }

    return nSolution;
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

        uint64_t                kGenerators[MAX_FLOORS]     = {0};
        uint64_t                kMicrochips[MAX_FLOORS]     = {0};

        char*                   kItems[MAX_ITEMS];
        uint64_t                nItems                      = 0;

        bool                    bProcessingElement          = false;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        for (i = 0; i < nLineCount; ++i)
        {
            char*  p = strtok(kLines[i], " ");

            while (p)
            {
                if (bProcessingElement)
                {
                    ItemType eType = E_GENERATOR;
                    if (strstr(p, "-compatible"))
                    {
                        eType = E_MICROCHIP;
                        *strstr(p, "-compatible") = '\0';
                    }

                    {
                        const uint64_t nMask = registerElement(p, &nItems, kItems);
                        if (E_GENERATOR == eType)
                        {
                            kGenerators[i] |= nMask;
                        }
                        else
                        {
                            kMicrochips[i] |= nMask;
                        }
                    }
                    bProcessingElement = false;
                }
                else if (0 == strcmp("a", p))
                {
                    bProcessingElement = true;
                }

                p = strtok(NULL, " ");
            }
        }

        printf("Part 1: %llu\n", solve(kGenerators, kMicrochips));
        /* Add the Elements for Part 2 */
        {
            const uint64_t nEleriumMask   = registerElement("elerium",   &nItems, kItems);
            const uint64_t nDilithiumMask = registerElement("dilithium", &nItems, kItems);
            kGenerators[0] |= nEleriumMask | nDilithiumMask;
            kMicrochips[0] |= nEleriumMask | nDilithiumMask;
        }

        printf("Part 2: %llu\n", solve(kGenerators, kMicrochips));

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        for (i = 0; i < nItems; ++i)
        {
            free(kItems[i]);
        }
    }

    return 0;
}
