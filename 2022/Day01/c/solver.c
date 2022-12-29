#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
/* Convert a line of text into an unsigned long, use 0xFFFFFFFF as a bodge to indicate no number
 * has been parsed.
 */
unsigned long GetMeal(FILE** pData)
{
    unsigned long nCalorieCount = 0u;
 
    while (!feof(*pData))
    {
        char kData = fgetc(*pData);
 
        if (('\n' == kData) || !!feof(*pData))
        {
            if (0 == nCalorieCount)
            {
                return 0xFFFFFFFFu;
            }
            else
            {
                return nCalorieCount;
            }
        }
       
        assert((kData >= '0') && (kData <= '9'));
        nCalorieCount = (nCalorieCount * 10u) + (kData - '0');
    }
 
    return 0xFFFFFFFFu;
}
 
int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        /* The nature of the puzzle means we don't need *all* of the meals, just the
         * top 3, so no need to buffer anything more.
         */
 
        unsigned long kTopMeals[3]   = {0u};
        unsigned long nCandidateMeal;
 
        while (!feof(pData))
        {
            /* Add up the Current Meal */
            unsigned long nCalorieCount;
            nCandidateMeal = 0u;
            while (0xFFFFFFFFu != (nCalorieCount = GetMeal(&pData)))
            {
                nCandidateMeal += nCalorieCount;
            }
 
            /* If the meal has more calories than the current 3rd place... */
            if (nCandidateMeal > kTopMeals[0])
            {
                if (nCandidateMeal > kTopMeals[2])
                {
                    kTopMeals[0] = kTopMeals[1];
                    kTopMeals[1] = kTopMeals[2];
                    kTopMeals[2] = nCandidateMeal;
                }
                else if (nCandidateMeal > kTopMeals[1])
                {
                    kTopMeals[0] = kTopMeals[1];
                    kTopMeals[1] = nCandidateMeal;
                }
                else
                {
                    kTopMeals[0]  = nCandidateMeal;
                }
            }
        }
 
        printf("Part 1: %lu\n", kTopMeals[2]);
        printf("Part 2: %lu\n", kTopMeals[0] + kTopMeals[1] + kTopMeals[2]);
 
        fclose(pData);
    }
 
    return 0;
}