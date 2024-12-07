#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
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
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);

    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }

    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';

        if (0 == bProcessUnix)
        {
            if (nFileSize >= 1)
            {
                if ((*pkFileBuffer)[nFileSize-1] != '\r')
                {
                    (*pkFileBuffer)[nFileSize-1]   = '\0';
                }
            }
        }
    }

    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);

        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;

            assert(pLine < &(*pkFileBuffer)[nFileSize]);

            (*pkLines)[nLineCount++] = pLine;

            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';

            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }

    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
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

unsigned equationSolveable(const uint64_t nTarget, const uint64_t* kEquation, const size_t nEquationSize, const size_t nIndex, const uint64_t nRunningTotal, const unsigned bPartTwo)
{
    unsigned bFound;

    if (nIndex == nEquationSize)
    {
        if (nRunningTotal == nTarget)
        {
            return AOC_TRUE;
        }

        return AOC_FALSE;
    }
    else if (nRunningTotal > nTarget)
    {
        return AOC_FALSE;
    }

    /*
     * Perform the Concatenation first for Part Two since we know the solution
     * requires it.
     */
    if (bPartTwo)
    {
        char kBuffer[32];
        sprintf(kBuffer, "%" PRIu64 "%" PRIu64, nRunningTotal, kEquation[nIndex]);
        bFound = equationSolveable(nTarget, kEquation, nEquationSize, nIndex + 1, strtoull(kBuffer, NULL, 10), bPartTwo);
        if (bFound)
        {
            return AOC_TRUE;
        }
    }

    /* Add */
    bFound = equationSolveable(nTarget, kEquation, nEquationSize, nIndex + 1, nRunningTotal + kEquation[nIndex], bPartTwo);
    if (bFound)
    {
        return AOC_TRUE;
    }

    /* Multiply */
    bFound = equationSolveable(nTarget, kEquation, nEquationSize, nIndex + 1, nRunningTotal * kEquation[nIndex], bPartTwo);
    if (bFound)
    {
        return AOC_TRUE;
    }

    return AOC_FALSE;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                       kBuffer;
        char**                      kRawEquations;
        size_t                      nNumEquations;
        size_t                      nMaxLineWidth;
        size_t                      nEquation;

        uint64_t*                   kEquationOperands;
        size_t                      nEquationOperandCount;

        uint64_t                    nPartOneSum      = 0;
        uint64_t                    nPartTwoSum      = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kRawEquations, &nNumEquations, NULL, &nMaxLineWidth, 0);
        fclose(pData);

        /* Allocate Memory for the Equation Operands */
        kEquationOperands = (uint64_t*)malloc(nMaxLineWidth * sizeof(uint64_t));

        for (nEquation = 0; nEquation < nNumEquations; ++ nEquation)
        {
            const uint64_t nTarget  = strtoull(kRawEquations[nEquation], NULL, 10);
            const char*    pOperand = strstr(kRawEquations[nEquation], ": ") + 1;

            nEquationOperandCount = 0;
            while (pOperand)
            {
                const uint64_t nOperand = strtoull(pOperand+1, NULL, 10);
                kEquationOperands[nEquationOperandCount++] = nOperand;
                pOperand = strstr(pOperand+1, " ");
            }

            if (equationSolveable(nTarget, kEquationOperands, nEquationOperandCount, 0, 0, AOC_FALSE))
            {
                nPartOneSum += nTarget;
                nPartTwoSum += nTarget;
            }
            else if (equationSolveable(nTarget, kEquationOperands, nEquationOperandCount, 0, 0, AOC_TRUE))
            {
                nPartTwoSum += nTarget;
            }
        }

        printf("Part 1: %" PRIu64 "\n", nPartOneSum);
        printf("Part 2: %" PRIu64 "\n", nPartTwoSum);

        /* Free any Allocated Memory */
        free(kRawEquations);
        free(kBuffer);
        free(kEquationOperands);
    }

    return 0;
}