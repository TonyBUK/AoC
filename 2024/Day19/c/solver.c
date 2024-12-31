#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct TowelType
{
    char*  kTowel;
    size_t nLength;
} TowelType;

/* Needed for Hash Map */
typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif
 
#include "hashmap/include/hashmap.h"

typedef HASHMAP(char, uint64_t) PatternHashMapType;

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

int compare(const void *a, const void *b)
{
    const TowelType* kLeft = (const TowelType*)a;
    const TowelType* kRight = (const TowelType*)b;

    if (kLeft->nLength < kRight->nLength)
    {
        return 1;
    }
    else if (kLeft->nLength > kRight->nLength)
    {
        return -1;
    }

    return 0;
}

void AddPattern(PatternHashMapType* kPatternHashmap, const char* const kPattern, const uint64_t nPermutations, char** kKeyBuffer, size_t* nKeyBufferLength, size_t* nKeyBufferMaxSize, uint64_t** kPermutationBuffer, size_t* nPermutationBufferLength, size_t* nPermutationBufferMaxSize)
{
    uint64_t* pExistingCount = hashmap_get(kPatternHashmap, kPattern);
    if (pExistingCount)
    {
        assert(0);
    }
    else
    {
        const size_t nPatternLength      = strlen(kPattern) + 1;
        const size_t nNewKeyBufferLength = *nKeyBufferLength + nPatternLength;
        if (nNewKeyBufferLength >= *nKeyBufferMaxSize)
        {
            char* kOldBuffer = *kKeyBuffer;
            *nKeyBufferMaxSize *= 2;
            *kKeyBuffer = (char*)realloc(*kKeyBuffer, *nKeyBufferMaxSize * sizeof(char));
            assert(kOldBuffer == *kKeyBuffer);
        }

        if (*nPermutationBufferLength >= *nPermutationBufferMaxSize)
        {
            uint64_t* kOldBuffer = *kPermutationBuffer;
            *nPermutationBufferMaxSize *= 2;
            *kPermutationBuffer = (uint64_t*)realloc(*kPermutationBuffer, *nPermutationBufferMaxSize * sizeof(uint64_t));
            assert(kOldBuffer == *kPermutationBuffer);
        }

        /* Copy the Key */
        char* pKey = &(*kKeyBuffer)[*nKeyBufferLength];
        strcpy(pKey, kPattern);

        /* Copy the Permutations */
        (*kPermutationBuffer)[*nPermutationBufferLength] = nPermutations;

        hashmap_put(kPatternHashmap, pKey, &((*kPermutationBuffer)[*nPermutationBufferLength]));

        /* Update the Buffer Lengths */
        *nKeyBufferLength          = nNewKeyBufferLength;
        *nPermutationBufferLength += 1;
    }
}

uint64_t possibleCombinations(const char* const kPattern, const size_t nPatternLength, const size_t nPatternOffset, const TowelType* const kTowels, const size_t nNumberOfTowels, const uint64_t nPermutations, PatternHashMapType* kSeen, char* kHashmapKeyBuffer, size_t* nHashmapKeyBufferLength, size_t* nHashmapKeyBufferMaxSize, uint64_t* kHashmapPermutationBuffer, size_t* nHashmapPermutationBufferLength, size_t* nHashmapPermutationBufferMaxSize)
{
    const uint64_t nStartPermutations = nPermutations;
    uint64_t       nLocalPermutations = nStartPermutations;
    size_t         nTowel;
 
    /* If the Pattern has been Seen Before... return the known delta */
    uint64_t* pExistingPermutations = hashmap_get(kSeen, &kPattern[nPatternOffset]);
    if (pExistingPermutations)
    {
        return nLocalPermutations + *pExistingPermutations;
    }
 
    /* If the Pattern has been Exhausted, return the incremented permutations */
    if (nPatternOffset == nPatternLength)
    {
        return nLocalPermutations + 1;
    }
 
    /* Recurse through all possible sub-patterns from longest to shortest */
    for (size_t nTowel = 0; nTowel < nNumberOfTowels; ++nTowel)
    {
        if (strncmp(&kPattern[nPatternOffset], kTowels[nTowel].kTowel, kTowels[nTowel].nLength) == 0)
        {
            nLocalPermutations = possibleCombinations(kPattern, nPatternLength, nPatternOffset + kTowels[nTowel].nLength, kTowels, nNumberOfTowels, nLocalPermutations, kSeen, kHashmapKeyBuffer, nHashmapKeyBufferLength, nHashmapKeyBufferMaxSize, kHashmapPermutationBuffer, nHashmapPermutationBufferLength, nHashmapPermutationBufferMaxSize);
        }
    }
 
    /* Add this to the Seen List */
    AddPattern(kSeen, &kPattern[nPatternOffset], nLocalPermutations - nStartPermutations, &kHashmapKeyBuffer, nHashmapKeyBufferLength, nHashmapKeyBufferMaxSize, &kHashmapPermutationBuffer, nHashmapPermutationBufferLength, nHashmapPermutationBufferMaxSize);
 
    /* Return the Known Permutation Count */
    return nLocalPermutations;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*              kBuffer;
        char**             kLines;
        size_t             nLineCount;
        size_t             nLongestLine;

        char*              kHashmapKeyBuffer;
        size_t             nHashmapKeyBufferLength          = 0u;
        size_t             nHashmapKeyBufferMaxSize         = 0x100000;

        uint64_t*          kHashmapPermutationBuffer;
        size_t             nHashmapPermutationBufferLength  = 0u;
        size_t             nHashmapPermutationBufferMaxSize = 0x10000;

        TowelType*         kTowels;
        size_t             nTowelCount = 0u;
        char*              pTowel;

        uint64_t           nPossibleCount        = 0u;
        uint64_t           nPossibleCombinations = 0u;

        size_t             nPattern;

        PatternHashMapType kPatternMap;
        hashmap_init(&kPatternMap, hashmap_hash_string, strcmp);

        /* Allocate the Hashmap Key/Permutation Buffers */
        kHashmapKeyBuffer         = (char*)malloc(nHashmapKeyBufferMaxSize * sizeof(char));
        kHashmapPermutationBuffer = (uint64_t*)malloc(nHashmapPermutationBufferMaxSize * sizeof(uint64_t));

        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nLongestLine, 0);
        fclose(pData);

        /* Worst Case is multiple 1 character entries which are broudly in the format: "X, " */
        kTowels = (TowelType*)malloc(((nLongestLine + 2) / 2) * sizeof(TowelType));

        /* Store the Towels sorted longest to shortest... */
        pTowel = kLines[0];
        while (pTowel)
        {
            char* pNextComma = strchr(pTowel, ',');
            kTowels[nTowelCount].kTowel  = pTowel;
            if (pNextComma)
            {
                kTowels[nTowelCount].nLength = pNextComma - pTowel;
                *pNextComma = '\0';
                pTowel      = pNextComma + 2;
            }
            else
            {
                kTowels[nTowelCount].nLength = strlen(pTowel);
                pTowel = NULL;
            }
            ++nTowelCount;
        }
        qsort(kTowels, nTowelCount, sizeof(TowelType), compare);

        /* For each pattern, find whether it can be made from the towels, and if so, how many ways it can be made... */
        for (nPattern = 2; nPattern < nLineCount; ++nPattern)
        {
            const uint64_t nPermutations = possibleCombinations(kLines[nPattern], strlen(kLines[nPattern]), 0, kTowels, nTowelCount, 0, &kPatternMap, kHashmapKeyBuffer, &nHashmapKeyBufferLength, &nHashmapKeyBufferMaxSize, kHashmapPermutationBuffer, &nHashmapPermutationBufferLength, &nHashmapPermutationBufferMaxSize);
            if (nPermutations > 0)
            {
                nPossibleCount        += 1;
                nPossibleCombinations += nPermutations;
            }
        }

        printf("Part 1: %" PRIu64 "\n", nPossibleCount);
        printf("Part 2: %" PRIu64 "\n", nPossibleCombinations);

        hashmap_cleanup(&kPatternMap);
 
        free(kBuffer);
        free(kLines);
        free(kHashmapKeyBuffer);
        free(kHashmapPermutationBuffer);
    }
 
    return 0;
}