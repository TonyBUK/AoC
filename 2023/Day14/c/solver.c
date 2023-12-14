#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

typedef struct tGridLookup
{
    unsigned  bPresent;
    size_t    nCount;
    size_t    nMaxCount;
    char**    kGridBuffer;
    uint64_t* kCycle;
} tGridLookup;


/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;

    tGridLookup*            kGridLookup    = NULL;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
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
        char* pBufferedLine;
 
        assert(pLine < &(*pkFileBuffer)[nReadCount]);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;
 
        pBufferedLine = pLine;

        pLine = strtok(NULL, (bProcessUnix ? "\n" : "\r\n"));

        /* Fix long standing bug where duplicate empty lines are skipped */
        if (pLine)
        {
            if((pLine - pBufferedLine) != nLineLength + 1)
            {
                if (bProcessUnix)
                {
                    char* pPreviousLine = pLine - 1;
                    while (*pPreviousLine == '\n')
                    {
                        (*pkLines)[nLineCount] = pPreviousLine;
                        ++nLineCount;

                        *pPreviousLine = '\0';
                        --pPreviousLine;
                    }
                }
                else
                {
                    char* pPreviousLine = pLine - 2;
                    while (*pPreviousLine == '\r')
                    {
                        (*pkLines)[nLineCount] = pPreviousLine;
                        ++nLineCount;

                        pPreviousLine[0] = '\0';
                        pPreviousLine[1] = '\0';
                        --pPreviousLine;
                        --pPreviousLine;
                    }
                }
            }
        }
  
        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nReadCount+1];
        ++nLineCount;
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

uint64_t countLoad(const char** kGrid, const size_t nWidth, const size_t nHeight)
{
    uint64_t nLoad = 0;
    size_t   X, Y;

    for (Y = 0; Y < nHeight; ++Y)
    {
        for (X = 0; X < nWidth; ++X)
        {
            if (kGrid[Y][X] == 'O')
            {
                nLoad += nHeight - Y;
            }
        }
    }

    return nLoad;
}

void moveNorth(char** kGrid, const size_t nWidth, const size_t nHeight)
{
    size_t Y;
    size_t X;
    size_t nLastUnobstructedPos;

    for (X = 0; X < nWidth; ++X)
    {
        nLastUnobstructedPos = 0;

        for (Y = 0; Y < nHeight; ++Y)
        {
            if (kGrid[Y][X] == 'O')
            {
                if (Y > nLastUnobstructedPos)
                {
                    kGrid[nLastUnobstructedPos][X] = kGrid[Y][X];
                    kGrid[Y][X] = '.';

                    ++nLastUnobstructedPos;

                    continue;
                }
            }

            if (kGrid[Y][X] != '.')
            {
                nLastUnobstructedPos = Y + 1;
            }
        }
    }
}

void moveSouth(char** kGrid, const size_t nWidth, const size_t nHeight)
{
    size_t Y;
    size_t X;
    size_t nLastUnobstructedPos;

    for (X = 0; X < nWidth; ++X)
    {
        nLastUnobstructedPos = nHeight;

        for (Y = nHeight; Y > 0; --Y)
        {
            if (kGrid[Y - 1][X] == 'O')
            {
                if (Y < nLastUnobstructedPos)
                {
                    kGrid[nLastUnobstructedPos - 1][X] = kGrid[Y - 1][X];
                    kGrid[Y - 1][X] = '.';

                    --nLastUnobstructedPos;

                    continue;
                }
            }

            if (kGrid[Y - 1][X] != '.')
            {
                nLastUnobstructedPos = Y - 1;
            }
        }
    }
}

void moveWest(char** kGrid, const size_t nWidth, const size_t nHeight)
{
    size_t Y;
    size_t X;
    size_t nLastUnobstructedPos;

    for (Y = 0; Y < nHeight; ++Y)
    {
        nLastUnobstructedPos = 0;

        for (X = 0; X < nWidth; ++X)
        {
            if (kGrid[Y][X] == 'O')
            {
                if (X > nLastUnobstructedPos)
                {
                    kGrid[Y][nLastUnobstructedPos] = kGrid[Y][X];
                    kGrid[Y][X] = '.';

                    ++nLastUnobstructedPos;

                    continue;
                }
            }

            if (kGrid[Y][X] != '.')
            {
                nLastUnobstructedPos = X + 1;
            }
        }
    }
}

void moveEast(char** kGrid, const size_t nWidth, const size_t nHeight)
{
    size_t Y;
    size_t X;
    size_t nLastUnobstructedPos;

    for (Y = 0; Y < nHeight; ++Y)
    {
        nLastUnobstructedPos = nWidth;

        for (X = nWidth; X > 0; --X)
        {
            if (kGrid[Y][X - 1] == 'O')
            {
                if (X < nLastUnobstructedPos)
                {
                    kGrid[Y][nLastUnobstructedPos - 1] = kGrid[Y][X - 1];
                    kGrid[Y][X - 1] = '.';

                    --nLastUnobstructedPos;

                    continue;
                }
            }

            if (kGrid[Y][X - 1] != '.')
            {
                nLastUnobstructedPos = X - 1;
            }
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        const uint64_t          TOTAL_CYCLES        = 1000000000ull;

        char*                   kBuffer;
        char*                   kBufferPartTwo;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nFileLength;
        size_t                  nMaxLineLength;

        size_t                  nCacheEntry;
        size_t                  nCacheSize          = 200000;
        tGridLookup*            kGridCache;

        size_t                  nLoadHistorySize    = 100000;
        uint64_t*               kLoadHistory;

        uint64_t                nCycle              = 0;
        uint64_t                nLoopStart;
        uint64_t                nRemainingCycles;
        uint64_t                nBufferedLoad;
        unsigned                bLoopFound          = AOC_FALSE;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, &nFileLength, &nMaxLineLength, 0);
        fclose(pData);

        /* Buffer the Data for Part Two */
        kBufferPartTwo = (char*)malloc(nFileLength * sizeof(char));
        memcpy(kBufferPartTwo, kBuffer, nFileLength);

        /* Move North */
        moveNorth(kLines, nMaxLineLength, nLineCount);

        printf("Part 1: %llu\n", countLoad((const char**)kLines, nMaxLineLength, nLineCount));

        /* Restore the Data for Part Two */
        memcpy(kBuffer, kBufferPartTwo, nFileLength);

        /* Allocate the Cache for Loop Finding */
        kGridCache = (tGridLookup*)calloc(nCacheSize, sizeof(tGridLookup));
        assert(kGridCache);
        kLoadHistory = (uint64_t*)malloc(nLoadHistorySize * sizeof(uint64_t));

        while (nCycle < TOTAL_CYCLES)
        {
            uint64_t nLoad;

            moveNorth(kLines, nMaxLineLength, nLineCount);
            moveWest (kLines, nMaxLineLength, nLineCount);
            moveSouth(kLines, nMaxLineLength, nLineCount);
            moveEast (kLines, nMaxLineLength, nLineCount);

            if (AOC_FALSE == bLoopFound)
            {
                /* We use the load as our "fast hash" */
                nLoad = countLoad((const char**)kLines, nMaxLineLength, nLineCount);

                /* Cache manipulation nonsense... */
                if (nLoad >= nCacheSize)
                {
                    nCacheSize *= 2;
                    kGridCache = (tGridLookup*)realloc(kGridCache, nCacheSize * sizeof(tGridLookup));
                    assert(kGridCache);
                }

                /* Determine if the Lookup exists */
                if (AOC_FALSE == kGridCache[nLoad].bPresent)
                {
                    /* If not, allocate it... */
                    kGridCache[nLoad].bPresent    = AOC_TRUE;
                    kGridCache[nLoad].nCount      = 0;
                    kGridCache[nLoad].nMaxCount   = 100;
                    kGridCache[nLoad].kGridBuffer = (char**)malloc(kGridCache[nLoad].nMaxCount * sizeof(char*));
                    kGridCache[nLoad].kCycle      = (uint64_t*)malloc(kGridCache[nLoad].nMaxCount * sizeof(uint64_t));

                    assert(kGridCache[nLoad].kGridBuffer);
                    assert(kGridCache[nLoad].kCycle);
                }
                /* Or if we're about to get too big, re-allocated it... */
                else if (kGridCache[nLoad].nCount >= kGridCache[nLoad].nMaxCount)
                {
                    kGridCache[nLoad].nMaxCount *= 2;
                    kGridCache[nLoad].kGridBuffer = (char**)realloc(kGridCache[nLoad].kGridBuffer, kGridCache[nLoad].nMaxCount * sizeof(char*));
                    kGridCache[nLoad].kCycle      = (uint64_t*)realloc(kGridCache[nLoad].kCycle, kGridCache[nLoad].nMaxCount * sizeof(uint64_t));
                }

                /* Determine if the Cache Entry already exists */
                for (nCacheEntry = 0; nCacheEntry < kGridCache[nLoad].nCount; ++nCacheEntry)
                {
                    if (0 == memcmp(kGridCache[nLoad].kGridBuffer[nCacheEntry], kBuffer, nFileLength))
                    {
                        uint64_t nLoopLength;
                        /* If so, we've found a loop! */
                        bLoopFound       = AOC_TRUE;
                        nLoopStart       = kGridCache[nLoad].kCycle[nCacheEntry];
                        nLoopLength      = nCycle - kGridCache[nLoad].kCycle[nCacheEntry];
                        nRemainingCycles = (TOTAL_CYCLES - nCycle) % nLoopLength;
                        nCycle           = TOTAL_CYCLES - nRemainingCycles;
                        break;
                    }
                }

                if (bLoopFound)
                {
                    if ((nLoopStart + nRemainingCycles - 1) < nLoadHistorySize)
                    {
                        break;
                    }

                    ++nCycle;
                    continue;
                }
                else
                {
                    /* Store the Cache Entry */
                    nCacheEntry = kGridCache[nLoad].nCount;
                    kGridCache[nLoad].kGridBuffer[nCacheEntry] = (char*)malloc(nFileLength * sizeof(char));
                    assert(kGridCache[nLoad].kGridBuffer[nCacheEntry]);
                    memcpy(kGridCache[nLoad].kGridBuffer[nCacheEntry], kBuffer, nFileLength * sizeof(char));
                    kGridCache[nLoad].kCycle[nCacheEntry] = nCycle;

                    ++kGridCache[nLoad].nCount;

                    if (nCycle >= nLoadHistorySize)
                    {
                        nLoadHistorySize *= 2;
                        kLoadHistory = (uint64_t*)realloc(kLoadHistory, nLoadHistorySize * sizeof(uint64_t));
                        assert(kLoadHistory);
                    }
                    kLoadHistory[nCycle] = nLoad;
                }
            }

            ++nCycle;
        }

        if (nCycle == TOTAL_CYCLES)
        {
            printf("Part 2: %llu\n", countLoad((const char**)kLines, nMaxLineLength, nLineCount));
        }
        else
        {
            printf("Part 2: %llu\n", kLoadHistory[nLoopStart + nRemainingCycles - 1]);
        }

        /* Free the Grid Cache */
        for (nCacheEntry = 0; nCacheEntry < nCacheSize; ++nCacheEntry)
        {
            if (kGridCache[nCacheEntry].bPresent)
            {
                size_t nGridCacheEntry;

                for (nGridCacheEntry = 0; nCacheEntry < kGridCache[nCacheEntry].nCount; ++nGridCacheEntry)
                {
                    free(kGridCache[nCacheEntry].kGridBuffer[nGridCacheEntry]);
                }
                free(kGridCache[nCacheEntry].kGridBuffer);
                free(kGridCache[nCacheEntry].kCycle);
            }
        }
        free(kGridCache);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kBufferPartTwo);
    }
 
    return 0;
}