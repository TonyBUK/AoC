#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* Perform the Escape Encoding */
void escape(const char* kInput, char* kOutput)
{
    const char* CHARACTERS_TO_ESCAPE = "\\\"";
    size_t i;
    kOutput[0] = '\0';
    strcat(kOutput, "\"");
    for (i = 0; i < strlen(kInput); ++i)
    {
        if (strchr(CHARACTERS_TO_ESCAPE, kInput[i]))
        {
            strcat(kOutput, "\\");
        }
        strncat(kOutput, &kInput[i], 1);
    }
    strcat(kOutput, "\"");
}

/* Perform the Escape Decoding */
void unescape(const char* kInput, char* kOutput)
{
    size_t nStartAdjust    = 0;
    size_t nEndAdjust      = 0;
    BOOL   bIsEscape       = FALSE;
    size_t i;
    kOutput[0] = '\0';

    /* Handle if the Text is embedded within Quoutes */
    if (('\"' == kInput[0]) && ('\"' == kInput[strlen(kInput)-1]))
    {
        nStartAdjust    = 1;
        nEndAdjust      = 1;
    }

    for (i = nStartAdjust; i < (strlen(kInput) - nEndAdjust); ++i)
    {
        if (!bIsEscape)
        {
            if ('\\' == kInput[i])
            {
                bIsEscape = TRUE;
            }
            else
            {
                strncat(kOutput, &kInput[i], 1);
            }
        }
        else
        {
            if ('x' == kInput[i])
            {
                const char kHex[]   = {kInput[i+1], kInput[i+2], '\0'};
                const char kDecoded = (char)strtoul(kHex, NULL, 16);
                strncat(kOutput, &kDecoded, 1);
                i += 2;
            }
            else
            {
                strncat(kOutput, &kInput[i], 1);
            }
            bIsEscape = FALSE;
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char*               kDecodedBuffer            = NULL;
        size_t              nDecodedBufferLength      = 0;
        char*               kEncodedBuffer            = NULL;
        size_t              nEncodedBufferLength      = 0;
        char*               pLine;

        unsigned long       nRawLength                = 0;
        unsigned long       nDecodedLength            = 0;
        unsigned long       nEncodedLength            = 0;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*) malloc((nFileSize+1)  * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");

        if (pLine)
        {
            nDecodedBufferLength = strlen(pLine) + 1;
            nEncodedBufferLength = nDecodedBufferLength * 2;
            kDecodedBuffer       = (char*)malloc(nDecodedBufferLength);
            kEncodedBuffer       = (char*)malloc(nEncodedBufferLength);;
        }
        while (pLine)
        {
            const size_t nNeededLength = strlen(pLine) + 1;
            if (nNeededLength > nDecodedBufferLength)
            {
                free(kDecodedBuffer);
                free(kEncodedBuffer);
                nDecodedBufferLength = strlen(pLine) + 1;
                nEncodedBufferLength = nDecodedBufferLength * 2;
                kDecodedBuffer       = (char*)malloc(nDecodedBufferLength);
                kEncodedBuffer       = (char*)malloc(nEncodedBufferLength);;
            }

            /* Update the Total Raw Data Length (note: Remove the NUL terminator) */
            nRawLength += nNeededLength - 1;

            /* Perform the Unescape */
            unescape(pLine, kDecodedBuffer);
            nDecodedLength += strlen(kDecodedBuffer);
            
            /* Perform the Escape */
            escape(pLine, kEncodedBuffer);
            nEncodedLength += strlen(kEncodedBuffer);

            pLine = strtok(NULL, "\n");
        }
        free(kDecodedBuffer);
        free(kEncodedBuffer);

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);

        printf("Part 1: %lu\n", nRawLength     - nDecodedLength);
        printf("Part 2: %lu\n", nEncodedLength - nRawLength);
   }

    return 0;
}
