#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount)
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

    *pnLineCount = nLineCount;
}

uint64_t triangleNumber(const uint64_t n)
{
    return (n * (n+1)) / 2;
}

uint64_t getCodeAlgorithmically(const uint64_t nRow, const uint64_t nCol, const uint64_t nInitial)
{
    uint64_t nCurrentRow = 1;
    uint64_t nCurrentCol = 1;
    uint64_t nCurrent    = nInitial;

    while ((nRow != nCurrentRow) || (nCol != nCurrentCol))
    {
        if (1 == nCurrentRow)
        {
            nCurrentRow = nCurrentCol + 1;
            nCurrentCol = 1;
        }
        else
        {
            --nCurrentRow;
            ++nCurrentCol;
        }
        nCurrent = (nCurrent * 252533) % 33554393;
    }
    return nCurrent;
}

uint64_t getCodeHybrid(const uint64_t nRow, const uint64_t nCol, const uint64_t nInitial)
{
    const uint64_t nIterations = triangleNumber(nRow + nCol - 2) + nCol - 1;
    uint64_t nCurrent    = nInitial;
    uint64_t i;
    for (i = 0; i < nIterations; ++i)
    {
        nCurrent = (nCurrent * 252533) % 33554393;
    }
    return nCurrent;
}

uint64_t modPow(uint64_t nBase, uint64_t nExp, uint64_t nModulus)
{
    uint64_t nResult = 1;
    nBase %= nModulus;
    while (nExp > 0)
    {
        if (nExp & 1)
        {
            nResult = (nResult * nBase) % nModulus;
        }
        nBase = (nBase * nBase) % nModulus;
        nExp >>= 1;
    }
    return nResult;
}

uint64_t getCodeMathematically(const uint64_t nRow, const uint64_t nCol, const uint64_t nInitial)
{
    const uint64_t nIterations = triangleNumber(nRow + nCol - 2) + nCol - 1;
    return (modPow(252533, nIterations, 33554393) * nInitial) % 33554393;
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
        readLines(&pInput, &kBuffer, &kLines, &nLineCount);
        fclose(pInput);

        /* Iterate through each input */
        for (i = 0; i < nLineCount; ++i)
        {
            char*  pToken = strtok(kLines[i], " ");
            size_t j      = 0;

            uint64_t nRow;
            uint64_t nCol;

            while (pToken)
            {
                if (15 == j)
                {
                    nRow = strtoull(pToken, NULL, 10);
                }
                else if (17 == j)
                {
                    nCol = strtoull(pToken, NULL, 10);
                }

                pToken = strtok(NULL, " ");
                ++j;
            }

            printf("Part 1 (Algo): %llu\n", getCodeAlgorithmically(nRow, nCol, 20151125));
            printf("Part 1 (Hyb) : %llu\n", getCodeHybrid         (nRow, nCol, 20151125));
            printf("Part 1 (Math): %llu\n", getCodeMathematically (nRow, nCol, 20151125));
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);
    }

    return 0;
}
