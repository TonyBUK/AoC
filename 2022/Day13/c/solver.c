#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

unsigned long getValue(const char* kLine, size_t* pLinePtr)
{
    unsigned long nValue = 0;
    while(isdigit(kLine[*pLinePtr]))
    {
        nValue = (nValue * 10) + (unsigned long)(kLine[*pLinePtr] - '0');
        *pLinePtr += 1;
    }
    return nValue;
}

typedef struct CompareType
{
    size_t  nLeftPtr;
    size_t  nRightPtr;
    int     nCompare;
} TCompareType;

TCompareType compareLists(const char* kLeft, const char* kRight, const unsigned bLeftForcePushed, const unsigned bRightForcePushed)
{
    const size_t nLeftLength    = strlen(kLeft);
    const size_t nRightLength   = strlen(kRight);
    TCompareType kCompare       = {0, 0, 0};

    while ((kCompare.nLeftPtr < nLeftLength) && (kCompare.nRightPtr < nRightLength))
    {
        if (('[' == kLeft[kCompare.nLeftPtr]) && (']' == kRight[kCompare.nRightPtr]) && !bRightForcePushed)
        {
            kCompare.nCompare = 1;
            return kCompare;
        }
        if ((']' == kLeft[kCompare.nLeftPtr]) && ('[' == kRight[kCompare.nRightPtr]) && !bLeftForcePushed)
        {
            kCompare.nCompare = -1;
            return kCompare;
        }

        /* Determine if we need a Depth Push */
        if (('[' == kLeft[kCompare.nLeftPtr]) && ('[' == kRight[kCompare.nRightPtr]))
        {
            TCompareType kLocalCompare = compareLists(&kLeft[kCompare.nLeftPtr   + 1],
                                                      &kRight[kCompare.nRightPtr + 1],
                                                      0,
                                                      0);

            kCompare.nLeftPtr     += kLocalCompare.nLeftPtr  + 1;
            kCompare.nRightPtr    += kLocalCompare.nRightPtr + 1;

            if (0 != kLocalCompare.nCompare)
            {
                kCompare.nCompare = kLocalCompare.nCompare;
                return kCompare;
            }
        }
        else if (('[' == kLeft[kCompare.nLeftPtr]) && ('[' != kRight[kCompare.nRightPtr]))
        {
            TCompareType kLocalCompare = compareLists(&kLeft[kCompare.nLeftPtr + 1],
                                                      &kRight[kCompare.nRightPtr],
                                                      0,
                                                      1);

            kCompare.nLeftPtr     += kLocalCompare.nLeftPtr + 1;
            kCompare.nRightPtr    += kLocalCompare.nRightPtr;

            if (0 != kLocalCompare.nCompare)
            {
                kCompare.nCompare = kLocalCompare.nCompare;
                return kCompare;
            }
        }
        else if (('[' != kLeft[kCompare.nLeftPtr]) && ('[' == kRight[kCompare.nRightPtr]))
        {
            TCompareType kLocalCompare = compareLists(&kLeft[kCompare.nLeftPtr],
                                                      &kRight[kCompare.nRightPtr + 1],
                                                      1,
                                                      0);

            kCompare.nLeftPtr  += kLocalCompare.nLeftPtr;
            kCompare.nRightPtr += kLocalCompare.nRightPtr + 1;

            if (0 != kLocalCompare.nCompare)
            {
                kCompare.nCompare = kLocalCompare.nCompare;
                return kCompare;
            }
        }

        /* Determine if we need a Depth Pop */
        else if ((']' == kLeft[kCompare.nLeftPtr]) && (']' == kRight[kCompare.nRightPtr]))
        {
            ++kCompare.nLeftPtr;
            ++kCompare.nRightPtr;
            return kCompare;
        }
        else if ((']' == kLeft[kCompare.nLeftPtr]) && (']' != kRight[kCompare.nRightPtr]))
        {
            ++kCompare.nLeftPtr;
            kCompare.nCompare = -1;
            return kCompare;
        }
        else if ((']' != kLeft[kCompare.nLeftPtr]) && (']' == kRight[kCompare.nRightPtr]))
        {
            ++kCompare.nRightPtr;
            kCompare.nCompare = 1;
            return kCompare;
        }

        /* Handle Lists */
        else if ((',' == kLeft[kCompare.nLeftPtr]) && (',' == kRight[kCompare.nRightPtr]))
        {
            ++kCompare.nLeftPtr;
            ++kCompare.nRightPtr;
        }
        else if ((',' != kLeft[kCompare.nLeftPtr]) && (',' == kRight[kCompare.nRightPtr]))
        {
            ++kCompare.nRightPtr;
            kCompare.nCompare = -1;
            return kCompare;
        }
        else if ((',' == kLeft[kCompare.nLeftPtr]) && (',' != kRight[kCompare.nRightPtr]))
        {
            ++kCompare.nRightPtr;
            kCompare.nCompare = 11;
            return kCompare;
        }
        else
        {
            const unsigned long nLeft  = getValue(kLeft, &kCompare.nLeftPtr);
            const unsigned long nRight = getValue(kRight, &kCompare.nRightPtr);

            /* Compare the Values */
            if (nLeft < nRight)
            {
                kCompare.nCompare = -1;
                return kCompare;
            }
            else if (nLeft > nRight)
            {
                kCompare.nCompare = 1;
                return kCompare;
            }
        }
    }

    return kCompare;
}

int compare(const void* pLeft, const void* pRight)
{
    const char*        kLeft        = *(const char**)pLeft;
    const char*        kRight       = *(const char**)pRight;
    const TCompareType kCompare     = compareLists(kLeft, kRight, 0, 0);
    return kCompare.nCompare;
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const char*             kDivisors[]     =
        {
            "[[2]]",
            "[[6]]"
        };
        const size_t            nDivisorSize    = sizeof(kDivisors) / sizeof(kDivisors[0]);

        char*                   kBuffer;
        char**                  kLines;
        const char**            kLinesPart2;
        size_t                  nLineCount;

        size_t                  i;
        size_t                  nInOrderCount   = 0;
        size_t                  nProduct        = 1;
        size_t                  nDivisorCount   = 0;

        /* Read the entire file into a line buffer. */
        readLines(&pFile, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pFile);

        /* Part 1: Verify LT behaviour... */

        assert(0 == (nLineCount % 2));
        for (i = 0; i < nLineCount; i += 2)
        {
            if (-1 == compare((const void*)&kLines[i], (const void*)&kLines[i+1]))
            {
                nInOrderCount += (i/2)+1;
            }
        }
        printf("Part 1: %zu\n", nInOrderCount);

        /* Part 2: Use the LT comparison to perform a sort... */

        /* Allocate a new buffer and append the Divisors */
        kLinesPart2 = (const char**)malloc((nLineCount+2) * sizeof(const char*));
        for (i = 0; i < nLineCount; ++i)
        {
            kLinesPart2[i] = kLines[i];
        }
        kLinesPart2[nLineCount + 0] = kDivisors[0];
        kLinesPart2[nLineCount + 1] = kDivisors[1];

        /* Sort the Buffer using the same Comparator as Part 1 */
        qsort(kLinesPart2, nLineCount + 2, sizeof(char*), compare);

        /* Find the Divisors */
        for (i = 0; (i < (nLineCount + 2)) && (nDivisorCount < nDivisorSize); ++i)
        {
            size_t j = 0;
            for (j = 0; j < nDivisorSize; ++j)
            {
                if (0 == strcmp(kDivisors[j], kLinesPart2[i]))
                {
                    nProduct *= i + 1;
                    ++nDivisorCount;
                    break;
                }
            }
        }

        printf("Part 2: %zu\n", nProduct);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kLinesPart2);
    }

    return 0;
}
