#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE    (1u)
#define AOC_FALSE   (0u)

typedef struct MonkeyType
{
    unsigned        bLiteral;
    int64_t         nLiteral;
    size_t          nLeftMonkey;
    size_t          nRightMonkey;
    char            kOperator;
} TMonkeyType;

size_t encodeMonkey(const char* kMonkey)
{
    /* Note: This deliberately adds 1 in order to make 0 impossible as a result, allowing
     *       for an easy way of identifying an index that cannot be true, which is used
     *       as the inert state for ignoring monkeys in monkeyMath.
     */
    const size_t nIndex = ((size_t)(kMonkey[0] - 'a' + 1) << 15u) |
                          ((size_t)(kMonkey[1] - 'a' + 1) << 10u) |
                          ((size_t)(kMonkey[2] - 'a' + 1) <<  5u) |
                          ((size_t)(kMonkey[3] - 'a' + 1));

    assert((kMonkey[0] >= 'a') && (kMonkey[0] <= 'z'));
    assert((kMonkey[1] >= 'a') && (kMonkey[1] <= 'z'));
    assert((kMonkey[2] >= 'a') && (kMonkey[2] <= 'z'));
    assert((kMonkey[3] >= 'a') && (kMonkey[3] <= 'z'));

    return nIndex;
}

int64_t monkeyMath(TMonkeyType* kCurrentMonkey, TMonkeyType* kMonkeys, unsigned bUpdateMonkeys, const size_t nIgnoredMonkey, unsigned* bValid)
{
    unsigned bLocalValid;
    if (NULL == bValid)
    {
        bLocalValid = AOC_TRUE;
    }
    else
    {
        bLocalValid = *bValid;
    }

    if (kCurrentMonkey->bLiteral)
    {
        /* At this point bLocalValid can't have updated, therefore bValid retains its value... */
        return kCurrentMonkey->nLiteral;
    }
    else
    {
        unsigned bIgnored         = AOC_FALSE;
        unsigned bLocalValidLeft  = bLocalValid;
        unsigned bLocalValidRight = bLocalValid;
        int64_t  nLiteral;

        /* Determine if the Current Monkey is ignored.
         *
         * Note: This now means it's possible a Monkey isn't resolved, in which case we don't
         *       want any higher up Monkeys resolving either, which is why we push down a
         *       validity flag.
         *
         *       We do of course want to continue validating the Monkey on the other side...
         */
        if (nIgnoredMonkey == kCurrentMonkey->nLeftMonkey)
        {
            bLocalValidLeft = AOC_FALSE;
            bIgnored        = AOC_TRUE;
        }
        else if (nIgnoredMonkey == kCurrentMonkey->nRightMonkey)
        {
            bLocalValidRight = AOC_FALSE;
            bIgnored         = AOC_TRUE;
        }

        switch (kCurrentMonkey->kOperator)
        {
            case '+':
            {
                nLiteral = monkeyMath(&kMonkeys[kCurrentMonkey->nLeftMonkey],  kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidLeft) +
                           monkeyMath(&kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidRight);
            } break;

            case '-':
            {
                nLiteral = monkeyMath(&kMonkeys[kCurrentMonkey->nLeftMonkey],  kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidLeft) -
                           monkeyMath(&kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidRight);
            } break;

            case '*':
            {
                nLiteral = monkeyMath(&kMonkeys[kCurrentMonkey->nLeftMonkey],  kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidLeft) *
                           monkeyMath(&kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidRight);
            } break;

            case '/':
            {
                nLiteral = monkeyMath(&kMonkeys[kCurrentMonkey->nLeftMonkey],  kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidLeft) /
                           monkeyMath(&kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, bUpdateMonkeys, nIgnoredMonkey, &bLocalValidRight);
            } break;
        }

        /* Re-combine the Validities */
        bLocalValid = bLocalValidLeft && bLocalValidRight;

        /* Update the Monkeys if requested and if the Monkey Maths could be resolved */
        if (bUpdateMonkeys)
        {
            if (bLocalValid)
            {
                kCurrentMonkey->bLiteral = AOC_TRUE;
                kCurrentMonkey->nLiteral = nLiteral;
            }
            else if (AOC_FALSE == bIgnored)
            {
                assert((kMonkeys[kCurrentMonkey->nLeftMonkey].bLiteral  == AOC_FALSE) ||
                       (kMonkeys[kCurrentMonkey->nRightMonkey].bLiteral == AOC_FALSE));
            }
        }

        /* Pass the Validity back down the stack */
        if (NULL != bValid)
        {
            *bValid = bLocalValid;
        }

        return nLiteral;
    }
}

int64_t reverseMonkeyMath(const int64_t nStartValue, const TMonkeyType* kCurrentMonkey, const TMonkeyType* kMonkeys, const size_t nTargetMonkey)
{
    /* Determine if we've found the Monkey of Interest */
    if (&kMonkeys[nTargetMonkey] == kCurrentMonkey)
    {
        return nStartValue;
    }

    if ((kCurrentMonkey->nLeftMonkey != nTargetMonkey) && kMonkeys[kCurrentMonkey->nLeftMonkey].bLiteral)
    {
        const int64_t nMonkeyValue = kMonkeys[kCurrentMonkey->nLeftMonkey].nLiteral;

        if (kCurrentMonkey->nRightMonkey != nTargetMonkey)
        {
            assert(AOC_FALSE == kMonkeys[kCurrentMonkey->nRightMonkey].bLiteral);
        }

        switch (kCurrentMonkey->kOperator)
        {
            case '+':
            {
                /* Normal   : x = a + b */
                /* Reversed : b = x - a */
                return reverseMonkeyMath(nStartValue - nMonkeyValue, &kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, nTargetMonkey);
            } break;

            case '-':
            {
                /* Normal   : x = a - b */
                /* Reversed : b = a - x */
                return reverseMonkeyMath(nMonkeyValue - nStartValue, &kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, nTargetMonkey);
            } break;

            case '*':
            {
                /* Normal   : x = a * b */
                /* Reversed : b = x / a */
                return reverseMonkeyMath(nStartValue / nMonkeyValue, &kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, nTargetMonkey);
            } break;

            case '/':
            {
                /* Normal   : x = a / b */
                /* Reversed : b = a / x */
                return reverseMonkeyMath(nMonkeyValue / nStartValue, &kMonkeys[kCurrentMonkey->nRightMonkey], kMonkeys, nTargetMonkey);
            } break;
        }
    }
    else
    {
        const int64_t nMonkeyValue = kMonkeys[kCurrentMonkey->nRightMonkey].nLiteral;

        if (kCurrentMonkey->nLeftMonkey != nTargetMonkey)
        {
            assert(AOC_FALSE == kMonkeys[kCurrentMonkey->nLeftMonkey].bLiteral);
        }

        switch (kCurrentMonkey->kOperator)
        {
            case '+':
            {
                /* Normal   : x = a + b */
                /* Reversed : a = x - b */
                return reverseMonkeyMath(nStartValue - nMonkeyValue, &kMonkeys[kCurrentMonkey->nLeftMonkey], kMonkeys, nTargetMonkey);
            } break;

            case '-':
            {
                /* Normal   : x = a - b */
                /* Reversed : a = x + b */
                return reverseMonkeyMath(nMonkeyValue + nStartValue, &kMonkeys[kCurrentMonkey->nLeftMonkey], kMonkeys, nTargetMonkey);
            } break;

            case '*':
            {
                /* Normal   : x = a * b */
                /* Reversed : a = x / b */
                return reverseMonkeyMath(nStartValue / nMonkeyValue, &kMonkeys[kCurrentMonkey->nLeftMonkey], kMonkeys, nTargetMonkey);
            } break;

            case '/':
            {
                /* Normal   : x = a / b */
                /* Reversed : a = x * b */
                return reverseMonkeyMath(nMonkeyValue * nStartValue, &kMonkeys[kCurrentMonkey->nLeftMonkey], kMonkeys, nTargetMonkey);
            } break;
        }
    }

    assert(0);
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const size_t    nHumanIndex = encodeMonkey("humn");
        const size_t    nRootIndex  = encodeMonkey("root");

        TMonkeyType*    kMonkeys;
        size_t          i;

        /* If we assume each monkeys name is a unique 4 letter (lowercase) sequence, we
         * can pack the data into 20 bits, as each letter will be between 0 and 25 if we
         * remove the ASCII offset for 'a', meaning we can express each letter as 5 bits.
         * This means there will be 2,097,152 possible entries.
         */
        kMonkeys = (TMonkeyType*)malloc((1u << 20u) * sizeof(TMonkeyType));
        assert(kMonkeys);

        /* Read the Monkeys */
        while (!feof(pFile))
        {
            char   kMonkey[4];
            char   kRemainingData[15];
            size_t nMonkey;

            fread(kMonkey, sizeof(kMonkey), 1, pFile);
            nMonkey = encodeMonkey(kMonkey);

            i = 0u;
            do
            {
                kRemainingData[i] = fgetc(pFile);
                ++i;
            } while (!feof(pFile) && (kRemainingData[i-1] != '\n'));
            kRemainingData[i] = '\0';

            if (isdigit(kRemainingData[2]))
            {
                kMonkeys[nMonkey].bLiteral = AOC_TRUE;
                kMonkeys[nMonkey].nLiteral = (size_t)strtoull(&kRemainingData[2], NULL, 10);
            }
            else
            {
                kMonkeys[nMonkey].bLiteral      = AOC_FALSE;
                kMonkeys[nMonkey].nLeftMonkey   = encodeMonkey(&kRemainingData[2]);
                kMonkeys[nMonkey].nRightMonkey  = encodeMonkey(&kRemainingData[9]);

                switch(kRemainingData[7])
                {
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    {
                        kMonkeys[nMonkey].kOperator = kRemainingData[7];
                    } break;

                    default:
                    {
                        assert(0);
                    }
                }
            }
        }

        fclose(pFile);

        /* Perform an initial pass to pre-cache data for Part 2, and also partially solved Part 1 */
        monkeyMath(&kMonkeys[nRootIndex], kMonkeys, AOC_TRUE, nHumanIndex, NULL);

        /* Solve Part 1, but *don't* update the Monkeys themselves this time */
        printf("Part 1: %lli\n", monkeyMath(&kMonkeys[nRootIndex], kMonkeys, AOC_FALSE, 0, NULL));

        /* For Part 2, assuming humn only appears once, we should always walk up the stack with an
         * unsolved value and a solved value, meaning we can re-arrange each expression to solve
         * humn.
         */
        assert(AOC_FALSE == kMonkeys[nRootIndex].bLiteral);
        if (kMonkeys[kMonkeys[nRootIndex].nLeftMonkey].bLiteral)
        {
            assert(AOC_FALSE == kMonkeys[kMonkeys[nRootIndex].nRightMonkey].bLiteral);
            printf("Part 2: %lli\n", reverseMonkeyMath(kMonkeys[kMonkeys[nRootIndex].nLeftMonkey].nLiteral,
                                                       &kMonkeys[kMonkeys[nRootIndex].nRightMonkey],
                                                       kMonkeys,
                                                       nHumanIndex));
        }
        else
        {
            assert(AOC_FALSE == kMonkeys[kMonkeys[nRootIndex].nLeftMonkey].bLiteral);
            printf("Part 2: %lli\n", reverseMonkeyMath(kMonkeys[kMonkeys[nRootIndex].nRightMonkey].nLiteral,
                                                       &kMonkeys[kMonkeys[nRootIndex].nLeftMonkey],
                                                       kMonkeys,
                                                       nHumanIndex));
        }

        free(kMonkeys);
    }

    return 0;
}
