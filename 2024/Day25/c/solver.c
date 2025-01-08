#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

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
        char*                       kInputBuffer;
        char**                      kInputLines;

        size_t                      nNumInputLines;
        size_t                      nLockWidth;
        size_t                      nLockHeight             = (size_t)-1;

        size_t*                     kLocks;
        size_t                      nNumLocks               = 0;

        size_t*                     kKeys;
        size_t                      nNumKeys                = 0;

        size_t*                     kCurrent;

        size_t                      i;

        unsigned                    bIdentifyingType        = AOC_TRUE;
        size_t                      nCurrentHeight          = (size_t)-1;

        size_t                      nPossibleCombinations   = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nNumInputLines, NULL, &nLockWidth, 0);
        fclose(pData);

        /* Allocate memory for the locks and keys */
        kLocks = (size_t*)malloc(nNumInputLines * nLockWidth * sizeof(size_t));
        kKeys  = (size_t*)malloc(nNumInputLines * nLockWidth * sizeof(size_t));

        for (i = 0; i < nNumInputLines; ++i)
        {
            const size_t nLineLength = strlen(kInputLines[i]);
            size_t       j;

            if (0 == nLineLength)
            {
                bIdentifyingType = AOC_TRUE;
                nLockHeight      = MIN(nLockHeight, nCurrentHeight);
                continue;
            }

            assert(nLineLength == nLockWidth);

            if (AOC_TRUE == bIdentifyingType)
            {
                /* The first row will either be all dots or hashes depending on whether it's a lock or a key */
                if (kInputLines[i][0] == '.')
                {
                    kCurrent = &kKeys[nNumKeys++ * nLockWidth];
                }
                else if (kInputLines[i][0] == '#')
                {
                    kCurrent = &kLocks[nNumLocks++ * nLockWidth];
                }
                else
                {
                    assert(0);
                }

                memset(kCurrent, 0, nLockWidth * sizeof(size_t));
                bIdentifyingType = AOC_FALSE;
                nCurrentHeight   = 0;
            }

            for (j = 0; j < nLockWidth; ++j)
            {
                if (kInputLines[i][j] == '#')
                {
                    ++kCurrent[j];
                }
            }
            ++nCurrentHeight;
        }

        /* Check each lock against each key... */
        for (i = 0; i < nNumLocks; ++i)
        {
            const size_t* kCurentLock = &kLocks[i * nLockWidth];
            size_t j;

            for (j = 0; j < nNumKeys; ++j)
            {
                const size_t* kCurentKey        = &kKeys[j * nLockWidth];
                unsigned      bValidCombination = AOC_TRUE;
                size_t k;

                /*
                 * Note: We consider a key/lock combination valid if the key
                 *       physically fits, not if the key and the lock are a
                 *       perfect match.
                 */
                for (k = 0; k < nLockWidth; ++k)
                {
                    if ((kCurentLock[k] + kCurentKey[k]) > nLockHeight)
                    {
                        bValidCombination = AOC_FALSE;
                        break;
                    }
                }

                if (AOC_TRUE == bValidCombination)
                {
                    ++nPossibleCombinations;
                }
            }
        }

        /* Ho Ho Ho */
        printf("Part 1: %zu\n", nPossibleCombinations);

        /* Cleanup */
        free(kInputLines);
        free(kInputBuffer);
        free(kLocks);
        free(kKeys);
    }
 
    return 0;
}