#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';
    }
    else
    {
        (*pkFileBuffer)[nFileSize]   = '\0';
    }
 
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

        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
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

void getNextSample(const char* kBuffer, int64_t* kValueBuffer, int64_t* kLeftValueBuffer, int64_t* kRightValueBuffer, const size_t nValueBufferSize, int64_t* pnLeftColumnExtrapolation, int64_t* pnRightColumnExtrapolation)
{
    int64_t*    kValues1                = kValueBuffer;
    int64_t*    kValues2                = kValueBuffer + nValueBufferSize;
    int64_t*    kTemp;
    size_t      nDepthCount             = 0;
    size_t      nElementCount           = 0;

    int64_t     kExtrapolatedValues[2]  = {0, 0};

    const char* pBuffer                 = kBuffer;

    /* Step 1 - Extract the Top Layer of Values */
    while (pBuffer)
    {
        kValues1[nElementCount] = strtoll(pBuffer, NULL, 10);

        ++pBuffer;
        pBuffer = strstr(pBuffer, " ");

        ++nElementCount;
    }

    /* Keep going until we calculate the next layer down */
    while (1)
    {
        unsigned bAllZeroes = AOC_TRUE;
        size_t  nElement;

        assert(nElementCount > 0);

        /* Calculate the Next Row */
        for (nElement = 0; nElement < nElementCount - 1; ++nElement)
        {
            kValues2[nElement] = kValues1[nElement + 1] - kValues1[nElement];

            if (kValues2[nElement])
            {
                bAllZeroes = AOC_FALSE;
            }
        }

        /* Store the Left/Right Columns for Later */
        kLeftValueBuffer[nDepthCount]  = kValues1[0];
        kRightValueBuffer[nDepthCount] = kValues1[nElementCount - 1];
        ++nDepthCount;

        if (bAllZeroes)
        {
            break;
        }

        /* Decrement the Element Count, Swap the Buffers, effictively making the next row the current row */
        --nElementCount;
        kTemp    = kValues1;
        kValues1 = kValues2;
        kValues2 = kTemp;
    }

    kExtrapolatedValues[0] = kLeftValueBuffer[nDepthCount - 1];
    kExtrapolatedValues[1] = kRightValueBuffer[nDepthCount - 1];

    for (nElementCount = nDepthCount - 1; nElementCount > 0; --nElementCount)
    {
        kExtrapolatedValues[0] = kLeftValueBuffer[nElementCount - 1]  - kExtrapolatedValues[0];
        kExtrapolatedValues[1] = kRightValueBuffer[nElementCount - 1] + kExtrapolatedValues[1];
    }

    *pnLeftColumnExtrapolation  = kExtrapolatedValues[0];
    *pnRightColumnExtrapolation = kExtrapolatedValues[1];
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;
        size_t                  nMaxLineLength;

        /* These are buffers that will be used by getNextSample.
         * kValueBuffer will store the current/next line being proccesed.
         * kLeftValueBuffer will store the left column.
         * kRightValueBuffer will store the rights column.
         */
        int64_t*                kValueBuffer;
        int64_t*                kLeftValueBuffer;
        int64_t*                kRightValueBuffer;

        int64_t                 nPartOneResult      = 0;
        int64_t                 nPartTwoResult      = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength);
        fclose(pData);

        /* Allocate the Buffers */
        kValueBuffer       = (int64_t*)malloc(nMaxLineLength * 2 * sizeof(int64_t));
        kLeftValueBuffer   = (int64_t*)malloc(nMaxLineLength * sizeof(int64_t));
        kRightValueBuffer  = (int64_t*)malloc(nMaxLineLength * sizeof(int64_t));

        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            int64_t nLeftColumnExtrapolation  = 0;
            int64_t nRightColumnExtrapolation = 0;

            getNextSample(kLines[nLine], kValueBuffer, kLeftValueBuffer, kRightValueBuffer, nMaxLineLength, &nLeftColumnExtrapolation, &nRightColumnExtrapolation);

            nPartOneResult += nRightColumnExtrapolation;
            nPartTwoResult += nLeftColumnExtrapolation;
        }

        printf("Part 1: %lld\n", nPartOneResult);
        printf("Part 2: %lld\n", nPartTwoResult);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kValueBuffer);
        free(kLeftValueBuffer);
        free(kRightValueBuffer);
    }
 
    return 0;
}