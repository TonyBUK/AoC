#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

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

size_t getDecompressionLength(char* kCompressedText, const bool bRecurse)
{
    typedef enum DecompressorStateType {E_COPYING_TEXT, E_DECODING_EXPANSION, E_EXPANDING_TEXT} DecompressorStateType;

    /* String Length */
    const size_t            nStringLength       = strlen(kCompressedText);

    /* Decompression State */
    DecompressorStateType   eState              = E_COPYING_TEXT;

    /* Decompressed Length */
    size_t                  nDecompressedLength = 0;

    /* Expansion Command {LENGTH}x{FREQUENCY} */
    const char*             pExpansionCommandStart;
    size_t                  nRepeatLength       = 0;
    size_t                  nRepeatFrequency    = 0;

    /* Expanded Text Fragment */
    char*                   pExpansionTextStart;
    size_t                  nExpansionLength    = 0;

    size_t                  i;

    for (i = 0; i < nStringLength; ++i)
    {
        switch (eState)
        {
            case E_COPYING_TEXT:
            {
                /* Copying Text: Keep going until we reach a "(" */
                if ('(' == kCompressedText[i])
                {
                    eState = E_DECODING_EXPANSION;
                    pExpansionCommandStart = &kCompressedText[i+1];
                }
                else
                {
                    ++nDecompressedLength;
                }
            } break;

            case E_DECODING_EXPANSION:
            {
                /* Decoding Expansion: Keep going until we reach a ")" */
                if (')' == kCompressedText[i])
                {
                    const char* pExpansionCommandMid;
                    assert(strstr(pExpansionCommandStart, "x"));
                    pExpansionCommandMid = strstr(pExpansionCommandStart, "x") + 1;

                    /* We can abuse the fact strto* terminate on the first non-numeric character */
                    nRepeatLength       = strtoull(pExpansionCommandStart, NULL, 10);
                    nRepeatFrequency    = strtoull(pExpansionCommandMid, NULL, 10);

                    if (false == bRecurse)
                    {
                        /* We can skip the Expansion Phase by manipulating the loop index */
                        eState               = E_COPYING_TEXT;
                        nDecompressedLength += nRepeatLength * nRepeatFrequency;
                        i                   += nRepeatLength;
                    }
                    else
                    {
                        eState              = E_EXPANDING_TEXT;
                        pExpansionTextStart = &kCompressedText[i+1];
                        nExpansionLength    = 0;
                    }
                }
            } break;

            case E_EXPANDING_TEXT:
            {
                /* Expanding Text: Keep going until we've reached all possible characters in the */

                ++nExpansionLength;

                if (nExpansionLength == nRepeatLength)
                {
                    /* This temporarily corrupts the input buffer to place a false NULL terminator
                     * in place to allow this to act as a substring for expansion */
                    const char kBufferedChar = pExpansionTextStart[nExpansionLength];
                    pExpansionTextStart[nExpansionLength] = '\0';

                    eState = E_COPYING_TEXT;

                    nDecompressedLength += nRepeatFrequency * getDecompressionLength(pExpansionTextStart, bRecurse);

                    /* Replace the Fake NUL terminator with the real value */
                    pExpansionTextStart[nExpansionLength] = kBufferedChar;
                }
            } break;
        }
    }

    return nDecompressedLength;
}

char* getDecompressedText(char* kCompressedText, const bool bRecurse)
{
    typedef enum DecompressorStateType {E_COPYING_TEXT, E_DECODING_EXPANSION, E_EXPANDING_TEXT} DecompressorStateType;

    /* String Length */
    const size_t            nStringLength       = strlen(kCompressedText);

    /* Decompression State */
    DecompressorStateType   eState              = E_COPYING_TEXT;

    /* Decompressed Text */
    /* This is way too much like hard work to do inline, so cheat... as the alternative is to incrementally grow the buffer... */
    char*                   kDecompressedText   = (char*)malloc((getDecompressionLength(kCompressedText, bRecurse) + 1) * sizeof(char));

    /* Expansion Command {LENGTH}x{FREQUENCY} */
    const char*             pExpansionCommandStart;
    size_t                  nRepeatLength       = 0;
    size_t                  nRepeatFrequency    = 0;

    /* Expanded Text Fragment */
    char*                   pExpansionTextStart;
    size_t                  nExpansionLength    = 0;

    size_t                  i;

    strcpy(kDecompressedText, "");

    for (i = 0; i < nStringLength; ++i)
    {
        switch (eState)
        {
            case E_COPYING_TEXT:
            {
                /* Copying Text: Keep going until we reach a "(" */
                if ('(' == kCompressedText[i])
                {
                    eState = E_DECODING_EXPANSION;
                    pExpansionCommandStart = &kCompressedText[i+1];
                }
                else
                {
                    strncat(kDecompressedText, &kCompressedText[i], 1);
                }
            } break;

            case E_DECODING_EXPANSION:
            {
                /* Decoding Expansion: Keep going until we reach a ")" */
                if (')' == kCompressedText[i])
                {
                    const char* pExpansionCommandMid;
                    assert(strstr(pExpansionCommandStart, "x"));
                    pExpansionCommandMid = strstr(pExpansionCommandStart, "x") + 1;

                    /* We can abuse the fact strto* terminate on the first non-numeric character */
                    nRepeatLength       = strtoull(pExpansionCommandStart, NULL, 10);
                    nRepeatFrequency    = strtoull(pExpansionCommandMid, NULL, 10);

                    eState              = E_EXPANDING_TEXT;
                    pExpansionTextStart = &kCompressedText[i+1];
                    nExpansionLength    = 0;
                }
            } break;

            case E_EXPANDING_TEXT:
            {
                /* Expanding Text: Keep going until we've reached all possible characters in the */

                ++nExpansionLength;

                if (nExpansionLength == nRepeatLength)
                {
                    /* This temporarily corrupts the input buffer to place a false NULL terminator
                     * in place to allow this to act as a substring for expansion */
                    const char kBufferedChar = pExpansionTextStart[nExpansionLength];
                    pExpansionTextStart[nExpansionLength] = '\0';

                    eState = E_COPYING_TEXT;

                    if (false == bRecurse)
                    {
                        size_t j;
                        for (j = 0; j < nRepeatFrequency; ++j)
                        {
                            strcat(kDecompressedText, pExpansionTextStart);
                        }
                    }
                    else
                    {
                        char* kFragment = getDecompressedText(pExpansionTextStart, bRecurse);
                        size_t j;
                        for (j = 0; j < nRepeatFrequency; ++j)
                        {
                            strcat(kDecompressedText, kFragment);
                        }
                        free(kFragment);
                    }

                    /* Replace the Fake NUL terminator with the real value */
                    pExpansionTextStart[nExpansionLength] = kBufferedChar;
                }
            } break;
        }
    }

    return kDecompressedText;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        size_t                  nLineCount;
        size_t                  i;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        for (i = 0; i < nLineCount; ++i)
        {
            printf("Part 1: %llu\n", (uint64_t)getDecompressionLength(kLines[i], false));
            printf("Part 2: %llu\n", (uint64_t)getDecompressionLength(kLines[i], true));
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);
    }

    return 0;
}
