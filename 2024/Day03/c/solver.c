#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

/* Determines if this is one of the allowed start tokens */
unsigned IsStartToken(const char* const pData, const char** kAllowedTokens, const size_t* ALLOWED_TOKEN_LENGTHS, const size_t nAllowedTokenCount, size_t* nFoundTokenLength, size_t* nFoundToken)
{
    for (size_t j = 0; j < nAllowedTokenCount; ++j)
    {
        if (strncmp(pData, kAllowedTokens[j], ALLOWED_TOKEN_LENGTHS[j]) == 0)
        {
            *nFoundTokenLength = ALLOWED_TOKEN_LENGTHS[j];
            if (NULL != nFoundToken)
            {
                *nFoundToken = j;
            }
            return AOC_TRUE;
        }
    }

    return AOC_FALSE;
}

const char* FindNextToken(const char* const pData, const size_t nLength, const char** kAllowedTokens, const size_t* ALLOWED_TOKEN_LENGTHS, const size_t nAllowedTokenCount, size_t* nTokenLength)
{
    const char* pFoundToken       = NULL;
    size_t      nFoundTokenLength = 0u;
    unsigned    bFound            = AOC_FALSE;

    /* Find the Start of the next Token */
    for (size_t i = 0; i < nLength; ++i)
    {
        size_t nFoundToken;
        if (IsStartToken(pData + i, kAllowedTokens, ALLOWED_TOKEN_LENGTHS, nAllowedTokenCount, &nFoundTokenLength, &nFoundToken))
        {
            pFoundToken = pData + i;

            /*
             * No point searching for the end of a token if the start token ended in a brace, since it's already complete.
             * Note: This neatly removes the need to test inside the braces for do/don't.
             */
            if (kAllowedTokens[nFoundToken][nFoundTokenLength - 1] == ')')
            {
                *nTokenLength = nFoundTokenLength;
                return pFoundToken;
            }
            break;
        }
    }

    /*
     * Find the End of the Token
     * Note: There's one annoying edge case where the token is
     *       never actually terminated, but a new token starts
     *       afterwards, so repeat the process using that as the
     *       new start point.
     */
    if (pFoundToken)
    {
        /* Search for the End Terminator */
        for (size_t i = pFoundToken - pData + nFoundTokenLength; i < nLength; ++i)
        {
            if (pData[i] == ')')
            {
                *nTokenLength = (size_t) (&pData[i] - pFoundToken) + 1;
                return pFoundToken;
            }
            if (IsStartToken(pData + i, kAllowedTokens, ALLOWED_TOKEN_LENGTHS, nAllowedTokenCount, &nFoundTokenLength, NULL))
            {
                return FindNextToken(pData + i, nLength - i, kAllowedTokens, ALLOWED_TOKEN_LENGTHS, nAllowedTokenCount, nTokenLength);
            }
        }
    }

    return NULL;
}

unsigned AllCharsValid(const char* const pData, const size_t nLength, const char* const kAllowedChars, const size_t nAllowedChars)
{
    for (size_t i = 0; i < nLength; ++i)
    {
        unsigned bValid = AOC_FALSE;
        for (size_t j = 0; j < nAllowedChars; ++j)
        {
            if (pData[i] == kAllowedChars[j])
            {
                bValid = AOC_TRUE;
                break;
            }
        }

        if (!bValid)
        {
            return AOC_FALSE;
        }
    }

    return AOC_TRUE;
}

uint64_t GetNextValue(const char* const pData, const size_t nLength, const size_t nMaxDigitLength, size_t* nOffset)
{
    size_t   nLocalOffset = *nOffset;
    size_t   nDigitCount  = 0;
    uint64_t nValue       = 0u;

    while (nLocalOffset < nLength)
    {
        if (isdigit(pData[nLocalOffset]))
        {
            nValue = (nValue * 10u) + (pData[nLocalOffset] - '0');
            ++nDigitCount;

            if (nDigitCount > nMaxDigitLength)
            {
                *nOffset = nLocalOffset;
                return 0u;
            }
        }
        else
        {
            *nOffset = nLocalOffset;
            return nValue;
        }

        ++nLocalOffset;
    }

    return nValue;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        /* Allowed Start Tokens for Both Parts */
        const char* ALLOWED_TOKENS[] =
        {
            "mul(",
            "don't()",
            "do()"
        };
        const size_t ALLOWED_TOKEN_LENGTHS[] =
        {
            strlen(ALLOWED_TOKENS[0]),
            strlen(ALLOWED_TOKENS[1]),
            strlen(ALLOWED_TOKENS[2])
        };
        const char*  VALID_CHARS_PART_ONE        = "0123456789,";
        const size_t VALID_CHARS_PART_ONE_LENGTH = strlen(VALID_CHARS_PART_ONE);

        uint64_t nPartOne  = 0u;
        uint64_t nPartTwo  = 0u;
        unsigned bDo       = AOC_TRUE;

        char*    kData     = NULL;

        while (!feof(pData))
        {
            size_t nSize    = 0u;
            size_t nLength;

            /* Solve both Parts... */
            while ((nLength = getline(&kData, &nSize, pData)) != -1)
            {
                const char* kDataPtr = kData;

                /* For Each Token... */
                do
                {                
                    size_t nTokenLength;
                    kDataPtr = FindNextToken(kDataPtr, nLength - (kDataPtr-kData), ALLOWED_TOKENS, ALLOWED_TOKEN_LENGTHS, sizeof(ALLOWED_TOKENS)/sizeof(ALLOWED_TOKENS[0]), &nTokenLength);
                    if (kDataPtr)
                    {
                        /* By Default they're valid... */
                        unsigned bValidPartOne = AOC_TRUE;
                        unsigned bValidPartTwo = AOC_TRUE;

                        /* Ensure there's no invalid characters within the braces (Part One) */
                        if (!AllCharsValid(kDataPtr + ALLOWED_TOKEN_LENGTHS[0], nTokenLength - ALLOWED_TOKEN_LENGTHS[0] - 1, VALID_CHARS_PART_ONE, VALID_CHARS_PART_ONE_LENGTH))
                        {
                            /* Part One */
                            bValidPartOne = AOC_FALSE;
                        }
                        /*
                         * We know it starts with mul( and ends with )
                         * so it's just the inner data for Part One.
                         */
                        else if (!strnstr(kDataPtr, ",", nTokenLength))
                        {
                            /* Part One */
                            bValidPartOne = AOC_FALSE;
                        }

                        /* Note: Part Two can never have invalid characters based on how the tokenizer works */

                        if (bValidPartOne)
                        {
                            /*
                             * Get the Multiplication Values
                             * Note: The challenge has a rule that input digits will be in the range 1 .. 3.
                             */
                            size_t         nOffset = ALLOWED_TOKEN_LENGTHS[0];
                            const uint64_t nLeft   = GetNextValue(kDataPtr, nTokenLength, 3, &nOffset);
                            
                            /* The only way this wouldn't be a comma is if we abandoned the read or there was some
                             * weird garbling of the input data, so ignore this mul instruction. */
                            if (kDataPtr[nOffset] != ',')
                            {
                                continue;
                            }
                            ++nOffset;
                            const uint64_t nRight  = GetNextValue(kDataPtr, nTokenLength, 3, &nOffset);

                            nPartOne += nLeft * nRight;

                            if (bDo)
                            {
                                nPartTwo += nLeft * nRight;
                            }
                        }
                        /* Otherwise it's a do or a don't... */
                        else if (bValidPartTwo)
                        {
                            if (strncmp(kDataPtr, ALLOWED_TOKENS[1], ALLOWED_TOKEN_LENGTHS[1]) == 0)
                            {
                                bDo = AOC_FALSE;
                            }
                            else if (strncmp(kDataPtr, ALLOWED_TOKENS[2], ALLOWED_TOKEN_LENGTHS[2]) == 0)
                            {
                                bDo = AOC_TRUE;
                            }
                        }

                        kDataPtr += nTokenLength;
                    }
                } while (kDataPtr);
            }
        }

        free(kData);

        printf("Part 1: %llu\n", nPartOne);
        printf("Part 2: %llu\n", nPartTwo);
    }
 
    return 0;
}