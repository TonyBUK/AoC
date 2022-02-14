#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

const char* Parse(const char* pJSONText, char* pScratch, const char* pFilter, int64_t* pValue, BOOL* bFiltered)
{
    const char*   p              = pJSONText;
    const int64_t nCurrent       = *pValue;

    if ('[' == *p)
    {
        ++p;

        while (']' != *p)
        {
            p = Parse(p, pScratch, pFilter, pValue, NULL);

            if (',' == *p)
            {
                ++p;
            }
        }
        ++p;
    }
    else if ('{' == *p)
    {
        BOOL    bFilteredLocal = FALSE;
        ++p;

        while ('}' != *p)
        {
            /* Skip the Key */
            while (':' != *p)
            {
                ++p;
            }
            ++p;
            p = Parse(p, pScratch, pFilter, pValue, &bFilteredLocal);

            if (',' == *p)
            {
                ++p;
            }
        }
        ++p;

        /* Rollback if this data was filtered */
        if (TRUE == bFilteredLocal)
        {
            *pValue = nCurrent;
        }
    }
    else if ('\"' == *p)
    {
        pScratch[0] = '\0';
        ++p;
        do
        {
            strncat(pScratch, p, 1);
            ++p;
        } while ('\"' != *p);
        ++p;

        if (bFiltered)
        {
            if (strlen(pFilter) > 0)
            {
                if (0 == strcmp(pScratch, pFilter))
                {
                    *bFiltered = TRUE;
                }
            }
        }
    }
    else if ('\0' != *p)
    {
        const char* kNumeric = "-0123456789";
        pScratch[0] = '\0';
        while (strchr(kNumeric, *p))
        {
            strncat(pScratch, p, 1);
            ++p;
        }

        *pValue += strtol(pScratch, NULL, 10);
    }

    return p;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                   = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char*               kScratch;
        char*               pLine;

        int64_t             nPart1Value = 0;
        int64_t             nPart2Value = 0;


        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;
        kBuffer            = (char*) malloc((nFileSize+1)  * sizeof(char));
        kScratch           = (char*) malloc((nFileSize+1)  * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");
        while (pLine)
        {
            Parse(pLine, kScratch, "",    &nPart1Value, NULL);
            Parse(pLine, kScratch, "red", &nPart2Value, NULL);
            pLine = strtok(NULL, "\n");
        }

        printf("Part 1: %lli\n", nPart1Value);
        printf("Part 2: %lli\n", nPart2Value);

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);
        free(kScratch);
   }

    return 0;
}
