#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>

typedef struct LinkedList
{
    unsigned long nLeft;
    int64_t       nValue;
    unsigned long nRight;
} TLinkedList;

int64_t getValue(FILE** pFile)
{
    char    kEntry;
    int64_t nSign = 1;
    int64_t nValue = 0;

    kEntry = fgetc(*pFile);
    if ('-' == kEntry)
    {
        nSign = -1;
        kEntry = fgetc(*pFile);
    }

    do
    {
        if (feof(*pFile))
        {
            break;
        }
        nValue = (nValue * 10ll) + (int64_t)(kEntry - '0');
    } while ('\n' != (kEntry = fgetc(*pFile)));

    return nValue * nSign;
}

void applyMixingSequence(const unsigned long nNumEntries, TLinkedList* kMixerSequence)
{
    const int64_t MODULO_SIZE     = (int64_t)nNumEntries - 1ll;
    const int64_t HALF_WAY        = MODULO_SIZE / 2ll;
    const int64_t MINUS_HALF_WAY  = -HALF_WAY;
    const int64_t CHANGE_LEFT     = -1ll - MODULO_SIZE;
    const int64_t CHANGE_RIGHT    = MODULO_SIZE + 1ll;
    unsigned long i;

    for (i = 0; i < nNumEntries; ++i)
    {
        int64_t nValue = kMixerSequence[i].nValue;

        /* Nothing to do for Zero */
        if (0ll != nValue)
        {
            const unsigned long nLeft       = kMixerSequence[i].nLeft;
            const unsigned long nRight      = kMixerSequence[i].nRight;
            unsigned long       nIndex      = i;
            unsigned long       nNewLeft;
            unsigned long       nNewRight;

            /* Limit the Range of the Value to avoid loop
             * but once going through the loop, we start wanting to adjust for "ourselves"
             * so the modulo is 1 less...
             */
            if (nValue > 0ll)
            {
                nValue %= MODULO_SIZE;

                /* If we're past the half way point move left instead */
                if (nValue > HALF_WAY)
                {
                    nValue += CHANGE_LEFT;
                }
            }
            else
            {
                nValue = -1 - (-nValue % MODULO_SIZE);

                /* If we're past the half way point move right instead */
                if (nValue < MINUS_HALF_WAY)
                {
                    nValue += CHANGE_RIGHT;
                }
            }


            if (nValue > 0)
            {
                int64_t j;

                /* Traverse Right through the Circular Buffer (the slow bit!) */
                for (j = 0ll; j < nValue; ++j)
                {
                    nIndex = kMixerSequence[nIndex].nRight;
                }
            }
            else
            {
                int64_t j;

                /* Traverse Left through the Circular Buffer (the slow bit!) */
                for (j = 0ll; j > nValue; --j)
                {
                    nIndex = kMixerSequence[nIndex].nLeft;
                }
            }

            /* Update the Left/Right of the Neighbour Pair we broke by moving */
            kMixerSequence[nLeft].nRight = nRight;
            kMixerSequence[nRight].nLeft = nLeft;

            /* Get the New Neighbour Pair */
            nNewRight = kMixerSequence[nIndex].nRight;
            nNewLeft  = nIndex;

            /* Link Ourselves Left/Right with the New Neighbours */
            kMixerSequence[nNewRight].nLeft = i,
            kMixerSequence[nNewLeft].nRight = i;

            /* Insert Ourselves */
            kMixerSequence[i].nLeft  = nNewLeft;
            kMixerSequence[i].nRight = nNewRight;
        }
    }
}

unsigned long generateIndirectLookup(const unsigned long nNumEntries, unsigned long* kMixerIndexes, TLinkedList* kMixerSequence)
{
    /* Generate the Indexes for a Faster Lookup
     * The actual start point is arbitrary.
     */
    unsigned long nIndex     = 0;
    unsigned long nZeroIndex = 0;
    unsigned long i;

    for (i = 0; i < nNumEntries; ++i)
    {
        kMixerIndexes[i] = nIndex;

        if (0ll == kMixerSequence[nIndex].nValue)
        {
            nZeroIndex = i;
        }
        nIndex = kMixerSequence[nIndex].nRight;
    }

    return nZeroIndex;
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const int64_t  DECRYPTION_KEY   = 811589153ll;

        unsigned long  nFileSize;
        unsigned long  nMaxEntries;
        TLinkedList*   kMixerSequencePart1;
        TLinkedList*   kMixerSequencePart2;
        unsigned long* kMixerIndexes;
        unsigned long  nNumEntries = 0;
        unsigned long  nZeroIndex;
        unsigned long  i;
        int64_t        nOutputs1, nOutputs2, nOutputs3;

        fseek(pFile, 0, SEEK_END);
        nMaxEntries = ftell(pFile) / 2;
        fseek(pFile, 0, SEEK_SET);

        kMixerSequencePart1 = (TLinkedList*)  malloc(nMaxEntries * sizeof(TLinkedList));
        kMixerSequencePart2 = (TLinkedList*)  malloc(nMaxEntries * sizeof(TLinkedList));
        kMixerIndexes       = (unsigned long*)malloc(nMaxEntries * sizeof(unsigned long));

        while (!feof(pFile))
        {
            const int64_t nValue = getValue(&pFile);

            /* Part 1 */
            kMixerSequencePart1[nNumEntries].nLeft  = nNumEntries - 1;
            kMixerSequencePart1[nNumEntries].nRight = nNumEntries + 1;
            kMixerSequencePart1[nNumEntries].nValue = nValue;

            /* Part 2 */
            kMixerSequencePart2[nNumEntries].nLeft  = nNumEntries - 1;
            kMixerSequencePart2[nNumEntries].nRight = nNumEntries + 1;
            kMixerSequencePart2[nNumEntries].nValue = nValue * DECRYPTION_KEY;

            ++nNumEntries;
        }

        /* Fix Left/Right */
        kMixerSequencePart1[0].nLeft              = nNumEntries - 1;
        kMixerSequencePart2[0].nLeft              = nNumEntries - 1;
        kMixerSequencePart1[nNumEntries-1].nRight = 0;
        kMixerSequencePart2[nNumEntries-1].nRight = 0;

        /* Close the Input File */
        fclose(pFile);

        /* Part 1 */

        /* Mix Once */
        applyMixingSequence(nNumEntries, kMixerSequencePart1);

        /* Generate the Indexes for a Faster Lookup */
        nZeroIndex = generateIndirectLookup(nNumEntries, kMixerIndexes, kMixerSequencePart1);

        /* Get the Grove Co-Ordinates */
        nOutputs1  = kMixerSequencePart1[kMixerIndexes[(1000 + nZeroIndex) % nNumEntries]].nValue;
        nOutputs2  = kMixerSequencePart1[kMixerIndexes[(2000 + nZeroIndex) % nNumEntries]].nValue;
        nOutputs3  = kMixerSequencePart1[kMixerIndexes[(3000 + nZeroIndex) % nNumEntries]].nValue;
        printf("Part 1: %lli\n", nOutputs1 + nOutputs2 + nOutputs3);

        /* Part 2 */

        /* Mix 10 Times */
        for (i = 0; i < 10; ++i)
        {
            applyMixingSequence(nNumEntries, kMixerSequencePart2);
        }

        /* Generate the Indexes for a Faster Lookup */
        nZeroIndex = generateIndirectLookup(nNumEntries, kMixerIndexes, kMixerSequencePart2);
        nOutputs1  = kMixerSequencePart2[kMixerIndexes[(1000 + nZeroIndex) % nNumEntries]].nValue;
        nOutputs2  = kMixerSequencePart2[kMixerIndexes[(2000 + nZeroIndex) % nNumEntries]].nValue;
        nOutputs3  = kMixerSequencePart2[kMixerIndexes[(3000 + nZeroIndex) % nNumEntries]].nValue;

        /* Get the Grove Co-Ordinates */
        printf("Part 2: %lli\n", nOutputs1 + nOutputs2 + nOutputs3);

        /* Cleanup */
        free(kMixerSequencePart1);
        free(kMixerSequencePart2);
        free(kMixerIndexes);
    }

    return 0;
}
