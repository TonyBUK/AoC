#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

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

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const size_t            nASCIISize  = 1 << (sizeof(char) * 8);
        char*                   kBuffer;
        char**                  kLines;

        size_t                  nLineCount;
        size_t**                kASCIIFrequencies;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        if (nLineCount > 0)
        {
            const size_t nLineLength = strlen(kLines[0]);
            char*        kMinString;
            char*        kMaxString;
            size_t       i;

            /* Allocate the Frequency Buffer */
            kASCIIFrequencies = (size_t**)malloc(nLineLength * sizeof(size_t*));
            for (i = 0; i < nLineLength; ++i)
            {
                kASCIIFrequencies[i] = (size_t*)malloc(nASCIISize * sizeof(size_t));
                memset(kASCIIFrequencies[i], 0, nASCIISize * sizeof(size_t));
            }

            kMinString = (char*)malloc((nLineLength+1) * sizeof(char));
            kMaxString = (char*)malloc((nLineLength+1) * sizeof(char));

            strcpy(kMinString, "");
            strcpy(kMaxString, "");

            /* Calculate the ASCII Frequencies */
            for (i = 0; i < nLineCount; ++i)
            {
                size_t j;
                assert(strlen(kLines[i]) == nLineLength);

                for (j = 0; j < nLineLength; ++j)
                {
                    const unsigned char C = (unsigned char)kLines[i][j];
                    ++kASCIIFrequencies[j][C];
                }
            }

            /* Extract the Min/Max Frequencies */
            for (i = 0; i < nLineLength; ++i)
            {
                size_t nMinFrequency = (size_t)-1;
                size_t nMaxFrequency = 0;
                char   kMinChar;
                char   kMaxChar;
                size_t j;

                for (j = 0; j < nASCIISize; ++j)
                {
                    if (0 == kASCIIFrequencies[i][j])
                    {
                        continue;
                    }

                    if (kASCIIFrequencies[i][j] < nMinFrequency)
                    {
                        nMinFrequency = kASCIIFrequencies[i][j];
                        kMinChar      = (char)j;
                    }

                    if (kASCIIFrequencies[i][j] > nMaxFrequency)
                    {
                        nMaxFrequency = kASCIIFrequencies[i][j];
                        kMaxChar      = (char)j;
                    }
                }

                strncat(kMinString, &kMinChar, 1);
                strncat(kMaxString, &kMaxChar, 1);
            }

            printf("Part 1: %s\n", kMaxString);
            printf("Part 2: %s\n", kMinString);

            free(kMinString);
            free(kMaxString);

            /* Free the Frequency Buffer */
            for (i = 0; i < nLineLength; ++i)
            {
                free(kASCIIFrequencies[i]);
            }
            free(kASCIIFrequencies);
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);
    }

    return 0;
}
