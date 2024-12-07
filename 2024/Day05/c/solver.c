#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define MAX_PAGES (256)

typedef struct OrderingRules
{
    uint8_t  kPageAfter[MAX_PAGES];
    unsigned kPageAfterSet[MAX_PAGES];
    size_t   nNumRules;
} OrderingRules;

OrderingRules* g_kRules;

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

    *pkFileBuffer   = (char*) malloc((nFileSize+2)         * sizeof(char));
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
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';

        if (0 == bProcessUnix)
        {
            if (nFileSize >= 1)
            {
                if ((*pkFileBuffer)[nFileSize-1] != '\r')
                {
                    (*pkFileBuffer)[nFileSize-1]   = '\0';
                }
            }
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

unsigned validPageOrdering(const uint8_t* kUpdates, const size_t nNumUpdates, uint8_t* kSeenCache)
{
    size_t nUpdate;

    /* Clear the Cache */
    memset(kSeenCache, 0, MAX_PAGES * sizeof(uint8_t));

    /* For each page */
    for (nUpdate = 0; nUpdate < nNumUpdates; ++nUpdate)
    {
        /*
         * If the page is in the ordering rules, make sure we've not seen any of
         * the pages that must come before it.
         */
        if (g_kRules[kUpdates[nUpdate]].nNumRules > 0)
        {
            size_t nRule;

            for (nRule = 0; nRule < g_kRules[kUpdates[nUpdate]].nNumRules; ++nRule)
            {
                const uint8_t nPageAfter = g_kRules[kUpdates[nUpdate]].kPageAfter[nRule];

                if (kSeenCache[nPageAfter] == AOC_TRUE)
                {
                    return AOC_FALSE;
                }
            }
        }

        /* Add to the list of seen pages */
        kSeenCache[kUpdates[nUpdate]] = AOC_TRUE;
    }

    return AOC_TRUE;
}

int compare(const void *a, const void *b)
{
    return g_kRules[*(const uint8_t*)a].kPageAfterSet[*(const uint8_t*)b] ? 1 : -1;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kBuffer;
        char**                      kLines;
        size_t                      nNumLines;
        size_t                      nMaxLineWidth;
        size_t                      nLine;

        uint8_t*                    kUpdates;
        size_t                      nNumUpdates;

        uint8_t*                    kSeenCache;

        unsigned                    bProcessingRules   = AOC_TRUE;

        uint64_t                    nPartOneValidCount = 0;
        uint64_t                    nPartTwoValidCount = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nNumLines, NULL, &nMaxLineWidth, 0);
        fclose(pData);

        /* Allocate Memory for the Rules/Updates */
        g_kRules   = (OrderingRules*)calloc(MAX_PAGES, sizeof(OrderingRules));
        kUpdates   = (uint8_t*)malloc(nMaxLineWidth * sizeof(uint8_t));
        kSeenCache = (uint8_t*)malloc(MAX_PAGES * sizeof(uint8_t));

        for (nLine = 0; nLine < nNumLines; ++nLine)
        {
            if (bProcessingRules)
            {
                if (strlen(kLines[nLine]) == 0)
                {
                    bProcessingRules = AOC_FALSE;
                }
                else
                {
                    const uint64_t nPageBefore = strtoull(kLines[nLine], NULL, 10);
                    const char*    kAfter      = strstr(kLines[nLine], "|") + 1;
                    const uint64_t nPageAfter  = strtoull(kAfter, NULL, 10);

                    assert(nPageBefore < MAX_PAGES);
                    assert(nPageAfter < MAX_PAGES);

                    g_kRules[nPageBefore].kPageAfter[g_kRules[nPageBefore].nNumRules++] = (uint8_t)nPageAfter;
                    g_kRules[nPageBefore].kPageAfterSet[nPageAfter] = AOC_TRUE;
                }
            }
            else
            {
                const char* pUpdate = kLines[nLine] - 1;
                nNumUpdates = 0;

                while (pUpdate)
                {
                    const uint64_t nPage = strtoull(pUpdate + 1, NULL, 10);

                    assert(nPage < MAX_PAGES);

                    kUpdates[nNumUpdates++] = (uint8_t)nPage;

                    pUpdate = strstr(pUpdate + 1, ",");
                }

                if (validPageOrdering(kUpdates, nNumUpdates, kSeenCache))
                {
                    nPartOneValidCount += (uint64_t)kUpdates[nNumUpdates / 2];
                }
                else
                {
                    qsort(kUpdates, nNumUpdates, sizeof(uint8_t), compare);
                    nPartTwoValidCount += (uint64_t)kUpdates[nNumUpdates / 2];
                }
            }
        }

        printf("Part 1: %llu\n", nPartOneValidCount);
        printf("Part 2: %llu\n", nPartTwoValidCount);

        /* Free any Allocated Memory */
        free(kLines);
        free(kBuffer);
        free(g_kRules);
        free(kUpdates);
        free(kSeenCache);
     }
 
    return 0;
}