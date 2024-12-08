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

unsigned equationSolveable(const uint64_t* kEquation, const size_t nEquationSize, const int64_t nIndex, const int64_t nRunningTotal, const unsigned bPartTwo)
{
    int64_t  nSubtractFromTotal;

    if (nIndex < 0)
    {
        if (nRunningTotal == 0)
        {
            return AOC_TRUE;
        }

        return AOC_FALSE;
    }
    else if (nRunningTotal < 0)
    {
        return AOC_FALSE;
    }

    /*
     * Note: The core premise here is to actually work backwards through the equation, starting from the
     *       target value and working back towards zero.
     *
     *       - Addition becomes subtraction.
     *       - Multiplication becomes division (with a simple modulo check to determine if its valid or not).
     *       - Concatenation becomes a base 10 shift (with a simple modulo check to determine if its valid or not).
     *
     *       What this allows is quicker discards for Multiply/Concatenation using modulo, as anywhere with a
     *       remainder will never be valid, since when working forwards, it would have been impossible to
     *       have a fractional value.
     */

    /* Shared between "Addition" and "Concatention" */
    nSubtractFromTotal = nRunningTotal - kEquation[nIndex];

    /*
     * Perform the Concatenation first for Part Two since we know the solution
     * requires it.
     */
    if (bPartTwo)
    {
        /* I'm avoid the maths library as I don't want to work in floating point...
         * so we can do a simple base 10 shift to determine the magnitude of the
         * next value to understand how it would have impacted the running total
         * if we were working left to right.
         */
        int64_t nDemoninator = 1;
        int64_t nNextValue   = kEquation[nIndex];
        while (nNextValue > 0)
        {
            nDemoninator *= 10;
            nNextValue /= 10;
        }

        if (0 == (nSubtractFromTotal % nDemoninator))
        {
            if (equationSolveable(kEquation, nEquationSize, nIndex - 1, nSubtractFromTotal / nDemoninator, bPartTwo))
            {
                return AOC_TRUE;
            }
        }
    }

    /* Add */
    if (equationSolveable(kEquation, nEquationSize, nIndex - 1, nSubtractFromTotal, bPartTwo))
    {
        return AOC_TRUE;
    }

    /* Multiply */
    if (0 == (nRunningTotal % kEquation[nIndex]))
    {
        if (equationSolveable(kEquation, nEquationSize, nIndex - 1, nRunningTotal / kEquation[nIndex], bPartTwo))
        {
            return AOC_TRUE;
        }
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

            if (equationSolveable(kEquationOperands, nEquationOperandCount, nEquationOperandCount - 1, nTarget, AOC_FALSE))
            {
                nPartOneSum += nTarget;
                nPartTwoSum += nTarget;
            }
            else if (equationSolveable(kEquationOperands, nEquationOperandCount, nEquationOperandCount - 1, nTarget, AOC_TRUE))
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