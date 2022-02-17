#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

typedef struct reindeerProfile
{
    size_t nFlightTime;
    size_t nFlightDistance;
    size_t nRestTime;
} reindeerProfile;

typedef enum reindeerCurrentState {E_REINDEER_RACING, E_REINDEER_RESTING} reindeerCurrentState;

typedef struct reindeerState
{
    reindeerProfile*        pProfile;
    reindeerCurrentState    eState;
    size_t                  nStateRemainingTime;
    size_t                  nDistance;
    size_t                  nPoints;
} reindeerState;

size_t winningDistance(const reindeerProfile* kReindeerStats, const size_t nReindeerCount, const size_t nTargetTime)
{
    size_t nBestDistance = 0;
    size_t i;

    for (i = 0; i < nReindeerCount; ++i)
    {
        int64_t                 nCurrentTime        = (int64_t)nTargetTime;
        size_t                  nCurrentDistance    = 0;
        while (nCurrentTime > 0)
        {
            size_t nDuration = kReindeerStats[i].nFlightTime;
            if (nDuration > nCurrentTime)
            {
                nDuration = nCurrentTime;
            }
            nCurrentDistance += (nDuration * kReindeerStats[i].nFlightDistance);
            nCurrentTime     -= (int64_t)(nDuration + kReindeerStats[i].nRestTime);
        }
        if (nCurrentDistance > nBestDistance)
        {
            nBestDistance = nCurrentDistance;
        }
    }

    return nBestDistance;
}

size_t winningPoints(reindeerState* kReindeerStates, const size_t nReindeerCount, const size_t nTargetTime)
{
    size_t nBestPoints = 0;
    size_t i;

    for (i = 0; i < nTargetTime; ++i)
    {
        /* Update the Distances and Compute the Best Distance */
        size_t nBestDistance = 0;
        size_t j;
        for (j = 0; j < nReindeerCount; ++j)
        {
            switch (kReindeerStates[j].eState)
            {
                case E_REINDEER_RACING:
                {
                    kReindeerStates[j].nDistance += kReindeerStates[j].pProfile->nFlightDistance;
                    --kReindeerStates[j].nStateRemainingTime;
                    if (0 == kReindeerStates[j].nStateRemainingTime)
                    {
                        kReindeerStates[j].eState              = E_REINDEER_RESTING;
                        kReindeerStates[j].nStateRemainingTime = kReindeerStates[j].pProfile->nRestTime;
                    }
                } break;

                case E_REINDEER_RESTING:
                {
                    --kReindeerStates[j].nStateRemainingTime;
                    if (0 == kReindeerStates[j].nStateRemainingTime)
                    {
                        kReindeerStates[j].eState              = E_REINDEER_RACING;
                        kReindeerStates[j].nStateRemainingTime = kReindeerStates[j].pProfile->nFlightTime;
                    }
                } break;

                default:
                {
                    assert(0);
                }
            }

            if (kReindeerStates[j].nDistance > nBestDistance)
            {
                nBestDistance = kReindeerStates[j].nDistance;
            }
        }

        /* Update the Points and update the Best Points */
        for (j = 0; j < nReindeerCount; ++j)
        {
            assert(kReindeerStates[j].nDistance <= nBestDistance);
            if (kReindeerStates[j].nDistance == nBestDistance)
            {
                ++kReindeerStates[j].nPoints;
                if (kReindeerStates[j].nPoints > nBestPoints)
                {
                    nBestPoints = kReindeerStates[j].nPoints;
                }
            }
        }
    }

    return nBestPoints;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char**              kLines;
        char*               pLine;

        reindeerProfile*    kReindeerProfiles;
        reindeerState*      kReindeerStates;
        size_t              nReindeerCount            = 0;
        size_t              i;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*) malloc((nFileSize+1)  * sizeof(char));
        kLines             = (char**)malloc((nFileSize)    * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");
        while (pLine)
        {
            const size_t nLineLength = strlen(pLine);
            if (nLineLength > 0)
            {
                if (pLine[nLineLength-1] == '.')
                {
                    pLine[nLineLength-1] = '\0';
                }
            }

            kLines[nReindeerCount] = pLine;
            ++nReindeerCount;

            pLine = strtok(NULL, "\n");
        }

        /* Allocate the Rules */
        kReindeerProfiles   = (reindeerProfile*)malloc(nReindeerCount * sizeof(reindeerProfile));
        kReindeerStates     = (reindeerState*)  malloc(nReindeerCount * sizeof(reindeerState));

        /* Copy the Reindeer Data */
        for (i = 0; i < nReindeerCount; ++i)
        {
            char*     pToken  = strtok(kLines[i], " ");
            size_t    j       = 0;

            while (pToken)
            {
                size_t k;

                if (3 == j)
                {
                    kReindeerProfiles[i].nFlightDistance = strtoull(pToken, NULL, 10);
                }
                else if (6 == j)
                {
                    kReindeerProfiles[i].nFlightTime     = strtoull(pToken, NULL, 10);
                }
                else if (13 == j)
                {
                    kReindeerProfiles[i].nRestTime       = strtoull(pToken, NULL, 10);
                }
                pToken = strtok(NULL, " ");
                ++j;
            }

            kReindeerStates[i].pProfile            = &kReindeerProfiles[i];
            kReindeerStates[i].nStateRemainingTime = kReindeerProfiles[i].nFlightTime;
            kReindeerStates[i].nDistance           = 0;
            kReindeerStates[i].nPoints             = 0;
        }

        /* Free the Line Buffers since we've parsed all the data into the Reindeer Data */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %zu\n", winningDistance(kReindeerProfiles, nReindeerCount, 2503));
        printf("Part 2: %zu\n", winningPoints  (kReindeerStates,   nReindeerCount, 2503));

        /* Free the Reindeer Data */
        free(kReindeerProfiles);
        free(kReindeerStates);
   }

    return 0;
}
