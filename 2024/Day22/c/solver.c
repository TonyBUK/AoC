#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

/*
 * Note: The values we're storing are in the range -9 .. 9, which is we add 9 to the result gives as an
 *       values in the range 0 .. 18, meaning if we treat this as base 19, we can combine the values into
 *       a single integer in the range 0 .. 130320, or (19**4)-1
 */
#define SECRET_ITERATIONS (2000)

#define MIN_CHANGE_VALUE (-9)
#define MAX_CHANGE_VALUE (9)
#define MIN_VALUE_DELTA  (-(MIN_CHANGE_VALUE))
#define CHANGE_VALUE_RANGE (MAX_CHANGE_VALUE - MIN_CHANGE_VALUE + 1)
#define TO_CACHE(a,b,c,d) (((a) + MIN_VALUE_DELTA) + (((b) + MIN_VALUE_DELTA) * CHANGE_VALUE_RANGE) + (((c) + MIN_VALUE_DELTA) * CHANGE_VALUE_RANGE*CHANGE_VALUE_RANGE) + (((d) + MIN_VALUE_DELTA) * CHANGE_VALUE_RANGE*CHANGE_VALUE_RANGE*CHANGE_VALUE_RANGE))
#define CACHE_SIZE (CHANGE_VALUE_RANGE*CHANGE_VALUE_RANGE*CHANGE_VALUE_RANGE*CHANGE_VALUE_RANGE)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kSecretCodeBuffer;
        char**                      kSecretCodesRaw;

        size_t                      nNumSecretCodes;
        size_t                      i;

        uint64_t*                   kSecretNumbers;
        signed char*                kSecretNumberUnits;
        signed char*                kSecretNumberUnitChanges;
        uint64_t*                   kBananaCount;
        unsigned*                   kBananaCountSeen;
        size_t*                     kBananaCountSeenArray;
        size_t                      nBananaCountSeenCount;

        uint64_t                    nPartOne              = 0;
        uint64_t                    nPartTwo              = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kSecretCodeBuffer, &kSecretCodesRaw, &nNumSecretCodes, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Secret Numbers */
        kSecretNumbers           = (uint64_t*)malloc(nNumSecretCodes * (SECRET_ITERATIONS+1) * sizeof(uint64_t));
        kSecretNumberUnits       = (signed char*)malloc(nNumSecretCodes * (SECRET_ITERATIONS+1) * sizeof(signed char));
        kSecretNumberUnitChanges = (signed char*)malloc(nNumSecretCodes * SECRET_ITERATIONS * sizeof(signed char));
        kBananaCount             = (uint64_t*)calloc(CACHE_SIZE, sizeof(uint64_t));
        kBananaCountSeen         = (unsigned*)calloc(CACHE_SIZE, sizeof(unsigned));
        kBananaCountSeenArray    = (size_t*)malloc(CACHE_SIZE * sizeof(size_t));

        for (i = 0; i < nNumSecretCodes; ++i)
        {
            uint64_t     nSecretNumber                   = strtoull(kSecretCodesRaw[i], NULL, 10);
            uint64_t*    kCurrentSecretNumbers           = &kSecretNumbers[i*(SECRET_ITERATIONS+1)];
            signed char* kCurrentSecretNumberUnits       = &kSecretNumberUnits[i*(SECRET_ITERATIONS+1)];
            signed char* kCurrentSecretNumberUnitChanges = &kSecretNumberUnitChanges[i*SECRET_ITERATIONS];
            size_t j;

            /* Calculate the Secret Number Sequence */
            kCurrentSecretNumbers[0]     = nSecretNumber;
            kCurrentSecretNumberUnits[0] = (signed char)(nSecretNumber % 10);

            for (j = 1; j <= SECRET_ITERATIONS; ++j)
            {
                nSecretNumber = ((nSecretNumber <<  6) ^ nSecretNumber) & 0xFFFFFF; // Equivalent to ((x  *   64) ^ x) % 0x1000000
                nSecretNumber = ((nSecretNumber >>  5) ^ nSecretNumber) & 0xFFFFFF; // Equivalent to ((x //   32) ^ x) % 0x1000000
                nSecretNumber = ((nSecretNumber << 11) ^ nSecretNumber) & 0xFFFFFF; // Equivalent to ((x  * 2048) ^ x) % 0x1000000

                kCurrentSecretNumbers[j]             = nSecretNumber;
                kCurrentSecretNumberUnits[j]         = (signed char)(nSecretNumber % 10);
                kCurrentSecretNumberUnitChanges[j-1] = kCurrentSecretNumberUnits[j] - kCurrentSecretNumberUnits[j-1];
            }

            /* Part 1 - Sum of each of the 2000'th iterations */
            nPartOne += kCurrentSecretNumbers[SECRET_ITERATIONS];

            /* Part 2 - Permutations */
            nBananaCountSeenCount = 0;
            for (j = 3; j < SECRET_ITERATIONS; ++j)
            {
                const size_t nCacheIndex = TO_CACHE(kCurrentSecretNumberUnitChanges[j-3], kCurrentSecretNumberUnitChanges[j-2], kCurrentSecretNumberUnitChanges[j-1], kCurrentSecretNumberUnitChanges[j]);

                if (AOC_FALSE == kBananaCountSeen[nCacheIndex])
                {
                    kBananaCountSeen[nBananaCountSeenCount++] = nCacheIndex;
                    kBananaCountSeen[nCacheIndex]             = AOC_TRUE;
                    kBananaCount[nCacheIndex]                += kCurrentSecretNumberUnits[j+1];

                    /* Update the Maximum Sold number of Bananas */
                    nPartTwo                                  = MAX(nPartTwo, kBananaCount[nCacheIndex]);
                }
            }

            /* Cleanup the Seen Cache */
            for (j = 0; j < nBananaCountSeenCount; ++j)
            {
                kBananaCountSeen[kBananaCountSeen[j]] = AOC_FALSE;
            }
        }

        /* Output the Results */
        printf("Part 1: %" PRIu64 "\n", nPartOne);
        printf("Part 2: %" PRIu64 "\n", nPartTwo);

        /* Cleanup */
        free(kSecretCodesRaw);
        free(kSecretCodeBuffer);
        free(kSecretNumbers);
        free(kSecretNumberUnits);
        free(kSecretNumberUnitChanges);
        free(kBananaCount);
        free(kBananaCountSeen);
    }
 
    return 0;
}