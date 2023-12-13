#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define PART_TWO_FOLD_SIZE  (5)
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

#define SIZE_GEAR_COUNT    (7)
#define SIZE_GEAR_INDEX    (7)
#define SIZE_PATTERN_INDEX (7)

#define MAX_GEAR_COUNT    (1 << SIZE_GEAR_COUNT)
#define MAX_GEAR_INDEX    (1 << SIZE_GEAR_INDEX)
#define MAX_PATTERN_INDEX (1 << SIZE_PATTERN_INDEX)

#define POS_GEAR_COUNT    (0)
#define POS_GEAR_INDEX    (POS_GEAR_COUNT + SIZE_GEAR_COUNT)
#define POS_PATTERN_INDEX (POS_GEAR_INDEX + SIZE_GEAR_INDEX)

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, sizeof(char), nFileSize, *pFile);
    assert(nReadCount > 0);
    assert(nReadCount <= nFileSize);
 
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        bProcessUnix = 0;
    }
 
    if ((*pkFileBuffer)[nReadCount] != '\n')
    {
        (*pkFileBuffer)[nReadCount]   = '\n';
        (*pkFileBuffer)[nReadCount+1] = '\0';
 
        if (0 == bProcessUnix)
        {
            if (nReadCount >= 1)
            {
                if ((*pkFileBuffer)[nReadCount-1] != '\r')
                {
                    (*pkFileBuffer)[nReadCount-1]   = '\0';
                }
            }
        }
    }
 
    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, (bProcessUnix ? "\n" : "\r\n"));
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
 
        assert(pLine < &(*pkFileBuffer)[nReadCount]);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;
 
        pLine = strtok(NULL, (bProcessUnix ? "\n" : "\r\n"));
 
        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nReadCount;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

size_t encodeKey(const uint64_t nCurrentDamagedGearsCount, const size_t nRecordStatusIndex, const size_t nDamagedGearsIndex)
{
    assert(nCurrentDamagedGearsCount < MAX_GEAR_COUNT);
    assert(nDamagedGearsIndex < MAX_GEAR_INDEX);
    assert(nRecordStatusIndex < MAX_PATTERN_INDEX);

    return (nCurrentDamagedGearsCount << POS_GEAR_COUNT) | (nDamagedGearsIndex << POS_GEAR_INDEX) | (nRecordStatusIndex << POS_PATTERN_INDEX);
}

uint64_t getPermutationCount(const char* kRecordStatus, const size_t nRecordStatusSize, const uint64_t* kDamagedGears, const uint64_t nDamagedGearsSize, const uint64_t nCurrentDamagedGearsCount, const size_t nRecordStatusIndex, const size_t nDamagedGearsIndex, uint64_t* kCache, unsigned* kCacheValid, const unsigned bFirst)
{
    const char* kHashSubstitution = "#?";
    const char* kDotSubstitution  = ".?";

    size_t   nKey;
    uint64_t nResult;
    char     kLookup[2] = {'\0', '\0'};

    /* Initialise the Cache */
    if (bFirst)
    {
        memset(kCacheValid, 0, MAX_GEAR_COUNT * MAX_GEAR_INDEX * MAX_PATTERN_INDEX * sizeof(unsigned));
    }

    /* Create the Key for the Current State and Check if it's in the Cache */
    nKey = encodeKey(nCurrentDamagedGearsCount, nRecordStatusIndex, nDamagedGearsIndex);
    if (kCacheValid[nKey])
    {
        return kCache[nKey];
    }

    /* Have we reached a possible solution? */
    if (nRecordStatusSize == nRecordStatusIndex)
    {
        /* Have we processed all the gears? */
        if (nDamagedGearsSize == nDamagedGearsIndex)
        {
            /* We have a solution! */
            return 1;
        }
        return 0;
    }

    /* Result for this state since we couldn't use the cache */
    nResult = 0;
    kLookup[0] = kRecordStatus[nRecordStatusIndex];

    /* Process the Hash / Hash Substitution */
    if (strstr(kHashSubstitution, kLookup))
    {
        /* For a Hash case we incrememnt the number of damaged gears we've seen and then process the next character. */
        if (nDamagedGearsIndex < nDamagedGearsSize)
        {
            /* Just for break... */
            while (1)
            {
                /* Check to see if this is the last item, and if so, check to see if we've processed the correct number of gears.
                 * Otherwise we need to do a rather convoluted check elsewhere...
                 */
                if (nRecordStatusSize == (nRecordStatusIndex + 1))
                {
                    if ((nCurrentDamagedGearsCount + 1) == kDamagedGears[nDamagedGearsIndex])
                    {
                        nResult += getPermutationCount(kRecordStatus, nRecordStatusSize, kDamagedGears, nDamagedGearsSize, 0, nRecordStatusIndex + 1, nDamagedGearsIndex + 1, kCache, kCacheValid, AOC_FALSE);
                        break;
                    }
                }

                /* Increment the Gear Count */
                if (nCurrentDamagedGearsCount < kDamagedGears[nDamagedGearsIndex])
                {
                    nResult += getPermutationCount(kRecordStatus, nRecordStatusSize, kDamagedGears, nDamagedGearsSize, nCurrentDamagedGearsCount + 1, nRecordStatusIndex + 1, nDamagedGearsIndex, kCache, kCacheValid, AOC_FALSE);
                }

                break;
            }
        }
    }

    /* Process the Dot / Dot Substitution */
    if (strstr(kDotSubstitution, kLookup))
    {
        /* For a Dot or Dot substitution there's really only two things we care about.
         *
         * Are we not processing a gear, in which case move on to the next character.
         * Are we processing a gear, in which we can only move onto the next gear if we've processed the correct number of gears.
         * Otherwise do nothing...
         */

        /* Not processing a Damaged Gear, so no state change here... */
        if (0 == nCurrentDamagedGearsCount)
        {
            nResult += getPermutationCount(kRecordStatus, nRecordStatusSize, kDamagedGears, nDamagedGearsSize, nCurrentDamagedGearsCount, nRecordStatusIndex + 1, nDamagedGearsIndex, kCache, kCacheValid, AOC_FALSE);
        }
        else if ((nDamagedGearsIndex < nDamagedGearsSize) && (nCurrentDamagedGearsCount == kDamagedGears[nDamagedGearsIndex]))
        {
            /* We've processed the correct number of gears, so we can move onto the next gear. */
            nResult += getPermutationCount(kRecordStatus, nRecordStatusSize, kDamagedGears, nDamagedGearsSize, 0, nRecordStatusIndex + 1, nDamagedGearsIndex + 1, kCache, kCacheValid, AOC_FALSE);
        }
    }

    /* Cache the Result so we don't repeat the work */
    kCache[nKey]      = nResult;
    kCacheValid[nKey] = AOC_TRUE;

    return nResult;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;
        size_t                  nMaxLineLength;
        uint64_t*               kDamagedGears;
        size_t                  nDamagedGearCount;

        uint64_t                nGearPermutationsPartOne = 0;
        uint64_t                nGearPermutationsPartTwo = 0;

        char*                   kUnfoldedLine;
        uint64_t*               kUnfoldedGears;

        uint64_t*               kCache;
        unsigned*               kCacheValid;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength);
        fclose(pData);

        kDamagedGears  = (uint64_t*)malloc(nMaxLineLength * sizeof(uint64_t));
        kUnfoldedLine  = (char*)malloc(nMaxLineLength * PART_TWO_FOLD_SIZE * sizeof(char));
        kUnfoldedGears = (uint64_t*)malloc(nMaxLineLength * PART_TWO_FOLD_SIZE * sizeof(uint64_t));

        /* We'll take a stab at this... */
        kCache         = (uint64_t*)malloc(MAX_GEAR_COUNT * MAX_GEAR_INDEX * MAX_PATTERN_INDEX * sizeof(uint64_t));
        kCacheValid    = (unsigned*)malloc(MAX_GEAR_COUNT * MAX_GEAR_INDEX * MAX_PATTERN_INDEX * sizeof(unsigned));

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            char* pValue = strstr(kLines[nLine], " ");
            int i;
            assert(pValue);

            /* Put a hard split for the Gears */
            *pValue = '\0';

            ++pValue;
            nDamagedGearCount = 0;
            do
            {
                kDamagedGears[nDamagedGearCount]  = strtoull(pValue, NULL, 10);
                ++nDamagedGearCount;
                pValue = strstr(pValue, ",");
                if (pValue)
                {
                    ++pValue;
                }
            } while (pValue);

            assert(nDamagedGearCount < nMaxLineLength);

            for (i = 0; i < PART_TWO_FOLD_SIZE; ++i)
            {
                int j;
                if (0 == i)
                {
                    strcpy(kUnfoldedLine, kLines[nLine]);
                }
                else
                {
                    strcat(kUnfoldedLine, "?");
                    strcat(kUnfoldedLine, kLines[nLine]);
                }

                for (j = 0; j < nDamagedGearCount; ++j)
                {
                    kUnfoldedGears[j + (i * nDamagedGearCount)] = kDamagedGears[j];
                }
            }

            nGearPermutationsPartOne += getPermutationCount(kLines[nLine], strlen(kLines[nLine]), kDamagedGears,  nDamagedGearCount,                      0, 0, 0, kCache, kCacheValid, AOC_TRUE);
            nGearPermutationsPartTwo += getPermutationCount(kUnfoldedLine, strlen(kUnfoldedLine), kUnfoldedGears, nDamagedGearCount * PART_TWO_FOLD_SIZE, 0, 0, 0, kCache, kCacheValid, AOC_TRUE);
        }

        printf("Part 1: %llu\n", nGearPermutationsPartOne);
        printf("Part 2: %llu\n", nGearPermutationsPartTwo);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kDamagedGears);
        free(kUnfoldedLine);
        free(kUnfoldedGears);
        free(kCache);
        free(kCacheValid);
    }
 
    return 0;
}