#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

typedef struct IP7
{
    char**  kIPAddress;
    size_t  nIPAddressSize;
    char**  kHypernetAddress;
    size_t  nHypernetAddressSize;
} IP7;

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

void extractIPAddress(const char* kIPAddress, IP7* pIP7Address)
{
    const size_t nMaxSize              = strlen(kIPAddress) + 1;
    size_t       nIPAddressCount       = ('[' == kIPAddress[0]) ? 0 : 1;
    size_t       nHypernetAddressCount = 0;
    bool         bIsHypernet           = false;
    bool         bAllocated            = false;
    char*        p;
    size_t       i;

    /* Count the Number of each Address Type */
    for (i = 0; i < strlen(kIPAddress); ++i)
    {
        if ('[' == kIPAddress[i])
        {
            nHypernetAddressCount += 1;
        }
        else if (']' == kIPAddress[i])
        {
            nIPAddressCount += 1;
        }
    }

    /* Pre-allocate the Buffers */
    pIP7Address->kIPAddress           = (char**)malloc(nIPAddressCount       * sizeof(char*));
    pIP7Address->nIPAddressSize       = 0;
    pIP7Address->kHypernetAddress     = (char**)malloc(nHypernetAddressCount * sizeof(char*));
    pIP7Address->nHypernetAddressSize = 0;

    /* Parse the Data */
    for (i = 0; i < strlen(kIPAddress); ++i)
    {
        if ('[' == kIPAddress[i])
        {
            bIsHypernet = true;
            bAllocated  = false;
        }
        else if (']' == kIPAddress[i])
        {
            bIsHypernet = false;
            bAllocated  = false;
        }
        else
        {
            if (false == bAllocated)
            {
                if (true == bIsHypernet)
                {
                    pIP7Address->kHypernetAddress[pIP7Address->nHypernetAddressSize] = (char*)malloc(nMaxSize * sizeof(char));
                    p = pIP7Address->kHypernetAddress[pIP7Address->nHypernetAddressSize];
                    ++pIP7Address->nHypernetAddressSize;
                }
                else
                {
                    pIP7Address->kIPAddress[pIP7Address->nIPAddressSize] = (char*)malloc(nMaxSize * sizeof(char));
                    p = pIP7Address->kIPAddress[pIP7Address->nIPAddressSize];
                    ++pIP7Address->nIPAddressSize;
                }
                bAllocated = true;
                strcpy(p, "");
            }

            strncat(p, &kIPAddress[i], 1);
        }
    }
}

bool containsABBA(const char* kString)
{
    size_t i;
    for (i = 0; (i + 3) < strlen(kString); ++i)
    {
        const wchar_t AA[] = {kString[i],   kString[i+3]};
        const wchar_t BB[] = {kString[i+1], kString[i+2]};

        if (AA[0] == BB[0]) continue;
        if (AA[0] != AA[1]) continue;
        if (BB[0] != BB[1]) continue;

        return true;
    }

    return false;
}

bool supportsTLS(const IP7* pIP7Address)
{
    size_t i;

    /* If the Hypernet Sequences contain the ABBA sequence, then
     * this can't support TLS */
    for (i = 0; i < pIP7Address->nHypernetAddressSize; ++i)
    {
        if (containsABBA(pIP7Address->kHypernetAddress[i]))
        {
            return false;
        }
    }

    /* Now we've checked Hypernet, check all address parts for TLS support */
    for (i = 0; i < pIP7Address->nIPAddressSize; ++i)
    {
        if (containsABBA(pIP7Address->kIPAddress[i]))
        {
            return true;
        }
    }

    return false;
}

bool getInverseSequence(const char* kString, char* kInverseABASequence, size_t* pOffset)
{
    size_t i;

    for (i = 0; (i + 2) < strlen(kString); ++i)
    {
        const wchar_t AA[] = {kString[i],   kString[i+2]};
        const wchar_t B    = kString[i+1];

        if (AA[0] == B)     continue;
        if (AA[0] != AA[1]) continue;

        kInverseABASequence[0] = B;
        kInverseABASequence[1] = AA[0];
        kInverseABASequence[2] = B;

        *pOffset += i + 1;

        return true;
    }

    return false;
}

bool supportsSSL(const IP7* pIP7Address)
{
    char   kInverseABASequence[4] = {'\0','\0','\0','\0'};
    size_t i;

    for (i = 0; i < pIP7Address->nIPAddressSize; ++i)
    {
        /* Retrieve the next possible Inverse Sequence */
        size_t j = 0;

        while (getInverseSequence(&pIP7Address->kIPAddress[i][j], kInverseABASequence, &j))
        {
            size_t k;

            /* Search the Hypernet */
            for (k = 0; k < pIP7Address->nHypernetAddressSize; ++k)
            {
                if (strstr(pIP7Address->kHypernetAddress[k], kInverseABASequence))
                {
                    return true;
                }
            }
        }
    }

    return false;
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

        size_t                  nSupportsTLS    = 0;
        size_t                  nSupportsSSL    = 0;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        for (i = 0; i < nLineCount; ++i)
        {
            IP7    kIP7Address;
            size_t j;

            /* Allocate and Extract the IP Address */
            extractIPAddress(kLines[i], &kIP7Address);

            /* Determine if TLS/SSL is supported */
            nSupportsTLS += supportsTLS(&kIP7Address) ? 1 : 0;
            nSupportsSSL += supportsSSL(&kIP7Address) ? 1 : 0;

            /* Free the IP Component */
            for (j = 0; j < kIP7Address.nIPAddressSize; ++j)
            {
                free(kIP7Address.kIPAddress[j]);
            }
            free(kIP7Address.kIPAddress);

            /* Free the Hypernet Component */
            for (j = 0; j < kIP7Address.nHypernetAddressSize; ++j)
            {
                free(kIP7Address.kHypernetAddress[j]);
            }
            free(kIP7Address.kHypernetAddress);
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %u\n", (uint32_t)nSupportsTLS);
        printf("Part 2: %u\n", (uint32_t)nSupportsSSL);
    }

    return 0;
}
