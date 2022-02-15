#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef struct happinessType
{
    char*          kPerson;
    int64_t        nHappiness;
} happinessType;

typedef struct ruleType
{
    char*          kPerson;
    size_t         nHappinessCount;
    happinessType* kHappiness;
} ruleType;

#define calculateHighestHappinessDefault(kPeople, nPeopleCount) calculateHighestHappiness(kPeople, nPeopleCount, NULL, NULL, 0, 0, 0, 0)

int64_t calculateHighestHappiness(const ruleType* kPeople, const size_t nPeopleCount, const char** kSeatingPlan, size_t* kCurrentSeatingPlan, const size_t nSeatingPlanSize, const size_t nMask, const size_t nCompletedMask, const int64_t nScore)
{
    int64_t nLocalScore = nScore;

    if (0 == nSeatingPlanSize)
    {
        const size_t nFinalMask             = (1 << nPeopleCount) - 1;
        const char** kSeatingPlanLookup     = (const char**) malloc(nPeopleCount * sizeof(char*));
        size_t*      kCurrentSeatingPlan    = (size_t*)malloc(nPeopleCount * sizeof(size_t));
        size_t       i;

        for (i = 0; i < nPeopleCount; ++i)
        {
            kSeatingPlanLookup[i] = kPeople[i].kPerson;
        }
        kCurrentSeatingPlan[0] = 0;

        nLocalScore = calculateHighestHappiness(kPeople, nPeopleCount, kSeatingPlanLookup, kCurrentSeatingPlan, 1, 1, nFinalMask, nLocalScore);

        free(kSeatingPlanLookup);
        free(kCurrentSeatingPlan);
    }
    else
    {
        if (nMask == nCompletedMask)
        {
            int64_t nCalculatedScore = 0;
            size_t  i;
            for (size_t i = 0; i < nSeatingPlanSize; ++i)
            {
                const size_t  nPerson = kCurrentSeatingPlan[i];
                const size_t  nLeft   = kCurrentSeatingPlan[(i+nSeatingPlanSize - 1) % nSeatingPlanSize];
                const size_t  nRight  = kCurrentSeatingPlan[(i+                 + 1) % nSeatingPlanSize];
                const char*   kLeft   = kSeatingPlan[nLeft];
                const char*   kRight  = kSeatingPlan[nRight];
                size_t        j;

                /* Find the Happiness Score for the Left Side */
                for (j = 0; j < kPeople[nPerson].nHappinessCount; ++j)
                {
                    if (0 == strcmp(kLeft, kPeople[nPerson].kHappiness[j].kPerson))
                    {
                        nCalculatedScore += kPeople[nPerson].kHappiness[j].nHappiness;
                    }
                }

                /* Find the Happiness Score for the Right Side */
                for (j = 0; j < kPeople[nPerson].nHappinessCount; ++j)
                {
                    if (0 == strcmp(kRight, kPeople[nPerson].kHappiness[j].kPerson))
                    {
                        nCalculatedScore += kPeople[nPerson].kHappiness[j].nHappiness;
                    }
                }
            }

            if (nCalculatedScore > nLocalScore)
            {
                nLocalScore = nCalculatedScore;
            }
        }
        else
        {
            size_t i;
            for (i = 1; i < nPeopleCount; ++i)
            {
                const size_t nCurrentMask = 1 << i;
                if (0 == (nCurrentMask & nMask))
                {
                    kCurrentSeatingPlan[nSeatingPlanSize] = i;
                    nLocalScore = calculateHighestHappiness(kPeople, nPeopleCount, kSeatingPlan, kCurrentSeatingPlan, nSeatingPlanSize + 1, nMask | nCurrentMask, nCompletedMask, nLocalScore);
                }
            }
        }
    }

    return nLocalScore;
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

        ruleType*           kRules;
        size_t              nRuleCount                = 0;
        size_t              nPeopleCount              = 0;
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

            kLines[nRuleCount] = pLine;
            ++nRuleCount;

            pLine = strtok(NULL, "\n");
        }

        /* Allocate the Rules */
        kRules = (ruleType*)malloc((nRuleCount+1) * sizeof(ruleType));

        for (i = 0; i < nRuleCount; ++i)
        {
            kRules[i].kHappiness      = (happinessType*)malloc(nRuleCount * sizeof(happinessType));
            kRules[i].nHappinessCount = 0;
        }

        /* Copy the Rule Data */
        for (i = 0; i < nRuleCount; ++i)
        {
            char*     pToken  = strtok(kLines[i], " ");
            ruleType* pRule   = NULL;
            size_t    j       = 0;
            BOOL      bNegate = FALSE;
            int64_t   nValue;

            while (pToken)
            {
                size_t k;

                if (0 == j)
                {
                    for (k = 0; (k < nPeopleCount) && (NULL == pRule); ++k)
                    {
                        if (0 == strcmp(kRules[k].kPerson, pToken))
                        {
                            pRule = &kRules[k];
                        }
                    }

                    if (NULL == pRule)
                    {
                        pRule = &kRules[nPeopleCount];
                        pRule->kPerson = pToken;
                        ++nPeopleCount;
                    }
                }
                else if (2 == j)
                {
                    assert(pRule != NULL);
                    if (0 == strcmp("lose", pToken))
                    {
                        bNegate = TRUE;
                    }
                }
                else if (3 == j)
                {
                    nValue = ((TRUE == bNegate) ? -1 : 1) * strtoll(pToken, NULL, 10);
                }
                else if (10 == j)
                {
                    pRule->kHappiness[pRule->nHappinessCount].kPerson    = pToken;
                    pRule->kHappiness[pRule->nHappinessCount].nHappiness = nValue;
                    ++pRule->nHappinessCount;
                }

                pToken = strtok(NULL, " ");
                ++j;
            }
        }

        printf("Part 1: %lli\n", calculateHighestHappinessDefault(kRules, nPeopleCount));
        kRules[nPeopleCount].kPerson = "";
        ++nPeopleCount;
        printf("Part 2: %lli\n", calculateHighestHappinessDefault(kRules, nPeopleCount));

        /* De-allocate the Rules */
        for (i = 0; i < nRuleCount; ++i)
        {
            free(kRules[i].kHappiness);
        }

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);
        free(kLines);
        free(kRules);
   }

    return 0;
}
