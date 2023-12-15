#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

typedef struct tSlotType
{
    unsigned bSlotUsed;
    char*    kSlotLabel;
    int64_t  nSlotValue;
} tSlotType;

typedef struct tBoxType
{
    size_t      nSlotCount;
    size_t      nSlotCapacity;
    tSlotType*  kSlots;
} tBoxType;

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

uint8_t calculateHash(const char* pString)
{
    uint16_t nHash = 0;
    while (*pString)
    {
        nHash += (uint16_t)*pString;
        nHash *= 17;
        nHash %= 256;
        ++pString;
    }

    return (uint8_t)nHash;
}

size_t findSlot(const tBoxType* kSlots, const char* kLabel)
{
    size_t nSlot           = 0;

    for (nSlot = 0; nSlot < kSlots->nSlotCount; ++nSlot)
    {
        if (kSlots->kSlots[nSlot].bSlotUsed)
        {
            if (0 == strcmp(kSlots->kSlots[nSlot].kSlotLabel, kLabel))
            {
                return nSlot;
            }
        }
    }

    return kSlots->nSlotCount;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nMaxLineLength;

        size_t                  nWorstCaseItemsPerBox;
        tBoxType                kBoxes[256];
        size_t                  nBox;
        char*                   pEntry;

        uint64_t                nPartOneHash = 0;
        uint64_t                nPartTwoHash = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength, 0);
        fclose(pData);

        assert(nLineCount == 1);

        /* a=1 - Single Letter Label, single digit value... */
        nWorstCaseItemsPerBox = (nMaxLineLength / 3) + 1;

        /* Allocate the Boxes */
        for (nBox = 0; nBox < (sizeof(kBoxes) / sizeof(kBoxes[0])); ++nBox)
        {
            kBoxes[nBox].nSlotCapacity = nWorstCaseItemsPerBox;
            kBoxes[nBox].nSlotCount    = 0;
            kBoxes[nBox].kSlots        = (tSlotType*)malloc(nWorstCaseItemsPerBox * sizeof(tSlotType));
        }

        /* Process the Hashes for the CSV */
        pEntry = strtok(kBuffer, ",");
        while(pEntry)
        {
            char* pType;
            nPartOneHash += (uint64_t)calculateHash(pEntry);

            if ((pType = strstr(pEntry, "=")))
            {
                int64_t nValue;
                size_t  nSlot;

                /* Make the Label Separately Hashable */
                *pType = '\0';
                ++pType;

                nValue = strtoll(pType, NULL, 10);

                /* Calculate the Box Hash */
                nBox = calculateHash(pEntry);

                /* Determine if this already exists */
                nSlot = findSlot(&kBoxes[nBox], pEntry);
                if (nSlot >= kBoxes[nBox].nSlotCapacity)
                {
                    /* Reallocate the Box */
                    kBoxes[nBox].nSlotCapacity += nWorstCaseItemsPerBox;
                    kBoxes[nBox].kSlots = (tSlotType*)realloc(kBoxes[nBox].kSlots, kBoxes[nBox].nSlotCapacity * sizeof(tSlotType));
                }

                if (AOC_FALSE == kBoxes[nBox].kSlots[nSlot].bSlotUsed)
                {
                    kBoxes[nBox].kSlots[nSlot].bSlotUsed       = AOC_TRUE;
                    kBoxes[nBox].kSlots[nSlot].kSlotLabel      = (char*)malloc(strlen(pEntry) + 1 * sizeof(char));
                    assert(kBoxes[nBox].kSlots[nSlot].kSlotLabel);
                    strcpy(kBoxes[nBox].kSlots[nSlot].kSlotLabel, pEntry);
                    ++kBoxes[nBox].nSlotCount;
                }

                kBoxes[nBox].kSlots[nSlot].nSlotValue = nValue;
            }
            else if ((pType = strstr(pEntry, "-")))
            {
                size_t  nSlot;

                /* Make the Label Separately Hashable */
                *pType = '\0';

                /* Calculate the Box Hash */
                nBox = calculateHash(pEntry);

                /* Find the Slot */
                nSlot = findSlot(&kBoxes[nBox], pEntry);

                if (kBoxes[nBox].kSlots[nSlot].bSlotUsed)
                {
                    kBoxes[nBox].kSlots[nSlot].bSlotUsed = AOC_FALSE;
                    free(kBoxes[nBox].kSlots[nSlot].kSlotLabel);
                }
            }
            else
            {
                assert(0);
            }

            pEntry = strtok(NULL, ",");
        }

        printf("Part 1: %llu\n", nPartOneHash);

        /* Free the Boxes and Calculate Part 2 */
        for (nBox = 0; nBox < (sizeof(kBoxes) / sizeof(kBoxes[0])); ++nBox)
        {
            size_t nSlot       = 0;
            size_t nActualSlot = 0;
            for (nSlot = 0; nSlot < kBoxes[nBox].nSlotCount; ++nSlot)
            {
                if (kBoxes[nBox].kSlots[nSlot].bSlotUsed)
                {
                    nPartTwoHash += ((uint64_t)nBox + 1) * ((uint64_t)nActualSlot + 1) * kBoxes[nBox].kSlots[nSlot].nSlotValue;
                    free(kBoxes[nBox].kSlots[nSlot].kSlotLabel);

                    ++nActualSlot;
                }
            }

            free(kBoxes[nBox].kSlots);
        }

        printf("Part 2: %llu\n", nPartTwoHash);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
    }
 
    return 0;
}