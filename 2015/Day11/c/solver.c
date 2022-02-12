#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static char kNextChar[256];

void initialiseAllowedDigits()
{
    const char* kAllowedDigits = "abcdefghjkmnpqrstuvwxyz";

    size_t i;
    for (i = 0; i < (sizeof(kNextChar)/sizeof(char)); ++i)
    {
        const char kCurr[2] = {(char)i, '\0'};
        if (strstr(kAllowedDigits, kCurr))
        {
            char kNext[2] = {(kCurr[0] == 'z') ? 'a' : ((char)(i+1)), '\0'};
            while (NULL == strstr(kAllowedDigits, kNext))
            {
                ++kNext[0];
            }
            kNextChar[i] = kNext[0];
        }
        else
        {
            kNextChar[i] = '\0';
        }
    }
}

void incrementPassword(char* kPassword, size_t nPasswordLength)
{
    size_t i;

    /* Ready for the dirty secret... characters are just numbers, and strings are just     */
    /* character arrays, meaning strings are just number arrays... rather than converting  */
    /* bases etc., because we're only incrementing by 1 each time, this is really just an  */
    /* array rotation, so using a specially pre-processed array that not only rotates, but */
    /* also skips, we effectively mimick what the base 23 functionality provided in a      */
    /* cheaper, but less readable way.                                                     */
    for (i = nPasswordLength; i > 0; --i)
    {
        kPassword[i-1] = kNextChar[kPassword[i-1]];
        if ('a' != kPassword[i-1])
        {
            break;
        }
    }
}

/* Rule 1: Passwords must include one increasing straight of at least three letters, */
/*         like abc, bcd, cde, and so on, up to xyz. They cannot skip letters; abd   */
/*         doesn't count.                                                            */

/*         But there's a big gotcha here... because we've stripped i, o and l, that  */
/*         means technically hjk now passes this rule, which doesn't seem right,     */
/*         so I've re-indexed the values back into base 26 first before doing the    */
/*         comparison, that way hjk would now no longer be consecutive.              */
BOOL rule1(const char* kPassword, size_t nPasswordLength)
{
    size_t i;
    for (i = 2; i < nPasswordLength; ++i)
    {
        if ((kPassword[i-2] == (kPassword[i-1]-1)) && (kPassword[i-1] == (kPassword[i]-1)))
        {
            return TRUE;
        }
    }

    return FALSE;
}

// Rule 3 : Passwords must contain at least two different, non-overlapping pairs of
//          letters, like aa, bb, or zz.
//
// Note: Because of the "different" part, it's impossible to overlap, i.e. aa can't
//       overlap bb.  If the different part wasn't a rule, then we'd need to consdier
//       aaa and ensure it's not detected as two pairs, but since it is, we don't.
BOOL rule3(const char* kPassword, size_t nPasswordLength)
{
    int    nUniquePair = -1;
    size_t i;
    for (i = 1; i < nPasswordLength; ++i)
    {
        if (kPassword[i-1] == kPassword[i])
        {
            if (-1 == nUniquePair)
            {
                nUniquePair = (int)kPassword[i-1];
            }
            else
            {
                if (nUniquePair != (int)kPassword[i-1])
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

char* solvePassword(char* kPassword)
{
    const size_t nPasswordLength = strlen(kPassword);

    /* Force a single increment to get things moving... */
    incrementPassword(kPassword, nPasswordLength);

    /* The core principle here is to keep incrementing the password until    */
    /* we pass a given rule, then move onto the next rule and repeat.  We've */
    /* found our password when all the rules passed without incrementing the */
    /* password.                                                             */
    while (1)
    {
        BOOL bAnyIncrements = FALSE;

        /* Validate the Current Password */

        /* Rule 1: See above */
        while (FALSE == rule1(kPassword, nPasswordLength))
        {
            incrementPassword(kPassword, nPasswordLength);
            bAnyIncrements = TRUE;
        }

        /* Rule 2: Passwords may not contain the letters i, o, or l, as these letters can be     */
        /*         mistaken for other characters and are therefore confusing.                    */

        /* This is impossible to achieve because of how we've already stripped out these letters */
        /* when moving to Base 23, meaning we always pass this rule                              */

        // Rule 3 : See above
        while (FALSE == rule3(kPassword, nPasswordLength))
        {
            incrementPassword(kPassword, nPasswordLength);
            bAnyIncrements = TRUE;
        }

        if (FALSE == bAnyIncrements) break;
    }

    return kPassword;
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
        char*               pLine;

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

        /* Perform the required initialisation for the Solver */
        initialiseAllowedDigits();

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");
        while (pLine)
        {
            /* Part 1 - First Password */
            printf("Part 1: %s\n", solvePassword(pLine));
            
            /* Part 2 - Second Password */
            printf("Part 2: %s\n", solvePassword(pLine));

            pLine = strtok(NULL, "\n");
        }

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);
   }

    return 0;
}
