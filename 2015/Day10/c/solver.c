#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

void storeSeeSay(const size_t nRepetitionDigit, const size_t nRepetitionCount, size_t* pSeeSay, size_t* pSeeSaySize)
{
    if (nRepetitionCount > 9)
    {
                 long nPower        = (unsigned long)floor(log10((double)nRepetitionCount));
        unsigned long nRunningTotal = nRepetitionCount;
        while (nPower >= 0)
        {
            const unsigned long nCurrentDivisor = (unsigned long)pow(10.0, nPower);
            pSeeSay[*pSeeSaySize] = nRunningTotal / nCurrentDivisor;
            *pSeeSaySize          = *pSeeSaySize + 1;
            nRunningTotal         = nRunningTotal % nCurrentDivisor;
            --nPower;
        }

    }
    else
    {
        pSeeSay[*pSeeSaySize] = nRepetitionCount;
        *pSeeSaySize          = *pSeeSaySize + 1;
    }
    pSeeSay[*pSeeSaySize] = nRepetitionDigit;
    *pSeeSaySize          = *pSeeSaySize + 1;
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

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");
        while (pLine)
        {
            size_t i;
            size_t x;

            size_t  nSeeSaySize = strlen(pLine);
            size_t* kSeeSay = malloc(sizeof(size_t) * nSeeSaySize);
            for (i = 0; i < nSeeSaySize; ++i)
            {
                kSeeSay[i] = (size_t)(pLine[i] - '0');
            }

            for (x = 1; x <= 50; ++x)
            {
                /* Initialise Repetitions / Next See Say String */
                /* Note: The buffer can never be more than twice the size of the previous buffer */
                const size_t nCurrentSeeSaySize = nSeeSaySize;
                size_t       nRepetitionDigit   = kSeeSay[0];
                size_t       nRepetitionCount   = 1;
                size_t*      kNewSeeSay         = malloc(sizeof(size_t) * nCurrentSeeSaySize * 2);

                /* Skip the first digit as we already know the See/Say Number */
                nSeeSaySize = 0;
                for (i = 1; i < nCurrentSeeSaySize; ++i)
                {
                    if (nRepetitionDigit == kSeeSay[i])
                    {
                        /* Increment the Reptition Count */
                        ++nRepetitionCount;
                    }
                    else
                    {
                        /* Store the Current Repetition */
                        storeSeeSay(nRepetitionDigit, nRepetitionCount, kNewSeeSay, &nSeeSaySize);

                        /* Setup the next Repetition */
                        nRepetitionDigit = kSeeSay[i];
                        nRepetitionCount = 1;
                    }
                }

                /* Store the Current Repetition */
                storeSeeSay(nRepetitionDigit, nRepetitionCount, kNewSeeSay, &nSeeSaySize);

                if (40 == x)
                {
                    printf("Part 1: %lu\n", nSeeSaySize);
                }

                free(kSeeSay);
                kSeeSay = kNewSeeSay;
            }

            printf("Part 2: %lu\n", nSeeSaySize);

            /* Copy the repetition for the next cycle */
            free(kSeeSay);
            pLine = strtok(NULL, "\n");
        }

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);
   }

    return 0;
}
