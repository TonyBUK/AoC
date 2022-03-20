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

typedef struct KioskType
{
    char**   kEncryptedData;
    size_t   nEncryptedDataCount;
    uint64_t nSectorId;
    char*    kChecksum;
} KioskType;

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

int sizeCompare(const void* a, const void* b)
{
   return ( (int64_t)*(size_t*)a - (int64_t)*(size_t*)b );
}

int charCompare(const void* a, const void* b)
{
   return ( (int16_t)*(int8_t*)a - (int16_t)*(int8_t*)b );
}

char* Checksum(char** kEncryptedData, const size_t nEncryptedDataCount, char* kChecksum)
{
    typedef struct StringMetricType
    {
        char    kEntry;
        size_t  nCount;
    } StringMetricType;

    typedef struct StringMetricByFrequencyType
    {
        size_t  nCount;
        char*   kEntries;
    } StringMetricByFrequencyType;

    size_t                          nMaxCount       = 0;
    size_t                          nCount          = 0;
    size_t                          nMetricsCount   = 0;
    size_t                          i;
    StringMetricType*               kStringMetrics;
    StringMetricByFrequencyType*    kStringMetricsByFrequency;
    size_t*                         kFrequencies;

    for (i = 0; i < nEncryptedDataCount; ++i)
    {
        nMaxCount += strlen(kEncryptedData[i]);
    }

    /* Collect the String Metrics */
    kStringMetrics            = (StringMetricType*)           malloc(nMaxCount * sizeof(StringMetricType));
    kStringMetricsByFrequency = (StringMetricByFrequencyType*)malloc(nMaxCount * sizeof(StringMetricByFrequencyType));
    kFrequencies              = (size_t*)                     malloc(nMaxCount * sizeof(size_t));
    for (i = 0; i < nEncryptedDataCount; ++i)
    {
        size_t j;

        for (j = 0; j < strlen(kEncryptedData[i]); ++j)
        {
            bool bFound = false;
            size_t k;
            for (k = 0; (false == bFound) && (k < nCount); ++k)
            {
                if (kEncryptedData[i][j] == kStringMetrics[k].kEntry)
                {
                    bFound = true;
                    break;
                }
            }

            if (true == bFound)
            {
                ++kStringMetrics[k].nCount;
            }
            else
            {
                kStringMetrics[nCount].kEntry = kEncryptedData[i][j];
                kStringMetrics[nCount].nCount = 1;
                ++nCount;
            }
        }
    }

    for (i = 0; i < nCount; ++i)
    {
        size_t j;
        bool   bFound = false;
        for (j = 0; (false == bFound) && (j < nMetricsCount); ++j)
        {
            if (kStringMetrics[i].nCount == kStringMetricsByFrequency[j].nCount)
            {
                bFound = true;
                strncat(kStringMetricsByFrequency[j].kEntries, &kStringMetrics[i].kEntry, 1);
                qsort(kStringMetricsByFrequency[j].kEntries, strlen(kStringMetricsByFrequency[j].kEntries), sizeof(char), &charCompare);
                break;
            }
        }

        if (false == bFound)
        {
            kStringMetricsByFrequency[nMetricsCount].nCount   = kStringMetrics[i].nCount;
            kStringMetricsByFrequency[nMetricsCount].kEntries = (char*)malloc(nMaxCount * sizeof(char));
            strcpy(kStringMetricsByFrequency[nMetricsCount].kEntries, "");
            strncat(kStringMetricsByFrequency[nMetricsCount].kEntries, &kStringMetrics[i].kEntry, 1);
            kFrequencies[nMetricsCount]                       = kStringMetrics[i].nCount;
            ++nMetricsCount;

            qsort(kFrequencies, nMetricsCount, sizeof(size_t), &sizeCompare);
        }
    }

    free(kStringMetrics);

    strcpy(kChecksum, "");
    for (i = 0; (strlen(kChecksum) < 5) && (i < nMetricsCount); ++i)
    {
        const size_t nFrequency = kFrequencies[nMetricsCount - i - 1];
        size_t j;

        for (j = 0; (strlen(kChecksum) < 5) && (j < nMetricsCount); ++j)
        {
            if (nFrequency == kStringMetricsByFrequency[j].nCount)
            {
                size_t k;
                for (k = 0; (strlen(kChecksum) < 5) && (k < strlen(kStringMetricsByFrequency[j].kEntries)); ++k)
                {
                    strncat(kChecksum, &kStringMetricsByFrequency[j].kEntries[k], 1);
                }
                break;
            }
        }
    }

    for (i = 0; i < nMetricsCount; ++i)
    {
        free(kStringMetricsByFrequency[i].kEntries);
    }
    free(kStringMetricsByFrequency);
    free(kFrequencies);

    return kChecksum;
}

void Decrypt(char* kEncrypted, size_t nSectorId)
{
    size_t i;
    for (i = 0; i < strlen(kEncrypted); ++i)
    {
        kEncrypted[i] = (char)(((size_t)(kEncrypted[i]-'a') + nSectorId) % 26) + 'a';
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  i;
        size_t                  j;
        size_t                  nLineCount;
        char                    kChecksum[6];

        size_t                  nSectorSum          = 0;
        uint64_t                nNorthPoleSector    = 0;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        for (i = 0; i < nLineCount; ++i)
        {
            char* p;

            /* Sanitize the String */
            do
            {
                p = strstr(kLines[i], "]");
                if (p)
                {
                    *p = '\0';
                }
            } while (p != NULL);
            
            do
            {
                p = strstr(kLines[i], "[");
                if (p)
                {
                    *p = '-';
                }
            } while (p != NULL);

            /* Store all Data associated with the Entry */
            KioskType kEntry;
            j = 0;

            kEntry.kEncryptedData      = (char**)malloc(strlen(kLines[i]) * sizeof(char*));

            kEntry.nEncryptedDataCount = 0;
            p = strtok(kLines[i], "-");
            while (p)
            {
                kEntry.kEncryptedData[j] = p;

                ++j;
                p = strtok(NULL, "-");
            }

            assert(j >= 2);
            kEntry.nEncryptedDataCount = j - 2;
            kEntry.kChecksum           = kEntry.kEncryptedData[j-1];
            kEntry.nSectorId           = strtoull(kEntry.kEncryptedData[j-2], NULL, 10);

            if (0 == strcmp(Checksum(kEntry.kEncryptedData, kEntry.nEncryptedDataCount, kChecksum), kEntry.kChecksum))
            {
                nSectorSum += kEntry.nSectorId;
                for (j = 0; j < kEntry.nEncryptedDataCount; ++j)
                {
                    Decrypt(kEntry.kEncryptedData[j], kEntry.nSectorId);
                    if (0 == strcmp(kEntry.kEncryptedData[j], "northpole"))
                    {
                        assert(0 == nNorthPoleSector);
                        nNorthPoleSector = kEntry.nSectorId;
                    }
                }
            }

            /* Free the current Entry */
            free(kEntry.kEncryptedData);
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %llu\n", (uint64_t)nSectorSum);
        printf("Part 2: %llu\n", nNorthPoleSector);
    }

    return 0;
}
