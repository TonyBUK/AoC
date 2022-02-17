#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#define MAX(a,b) (a>=b) ? a : b

typedef struct ingredientProperties
{
    int64_t nCapacity;
    int64_t nDurability;
    int64_t nFlavor;
    int64_t nTexture;
    int64_t nCalories;
} ingredientProperties;

typedef struct cookieScore
{
    int64_t nCookieScore;
    int64_t nCalories;
} cookieScore;

typedef struct bestCookieScore
{
    int64_t nBestScore;
    int64_t nBestDietScore;
} bestCookieScore;

/* Calculated the Cookie Score for a given ingredient list                                          */
/* Note: The size of the ingredient list can be less than the size of the number of ingredients,    */
/*       this is taken to mean the remaining ingredients all have a quantity of 0.                  */
cookieScore calculateCookieScore(const ingredientProperties* kIngredients, const size_t* kIngredientList, const size_t nIngredientListSize)
{
    cookieScore kScore;
    int64_t     nCapacity   = 0;
    int64_t     nDurability = 0;
    int64_t     nFlavor     = 0;
    int64_t     nTexture    = 0;
    int64_t     nCalories   = 0;
    size_t      i;

    /* Multiply each ingredient property by the ingredient count and update the total */
    for (i = 0; i < nIngredientListSize; ++i)
    {
        const int64_t nIngredientCount = (int64_t)kIngredientList[i];
    
        nCapacity   += nIngredientCount * kIngredients[i].nCapacity;
        nDurability += nIngredientCount * kIngredients[i].nDurability;
        nFlavor     += nIngredientCount * kIngredients[i].nFlavor;
        nTexture    += nIngredientCount * kIngredients[i].nTexture;
        nCalories   += nIngredientCount * kIngredients[i].nCalories;
    }

    /* Disallow any negative totals */
    if (nCapacity   < 0) nCapacity   = 0;
    if (nDurability < 0) nDurability = 0;
    if (nFlavor     < 0) nFlavor     = 0;
    if (nTexture    < 0) nTexture    = 0;
    if (nCalories   < 0) nCalories   = 0;

    /* Return the cookie score and the calorie count */
    kScore.nCookieScore = (nCapacity * nDurability * nFlavor * nTexture);
    kScore.nCalories    = nCalories;
    return kScore;
}

/* Recursive Function to find the best cookie score, and the best diet cookie score (cookie score when the calorie count = 500) */
#define calculateBestCookieScoreDefault(kIngredients, nIngredientsSize, nTargetCalories, kBestScore) \
    calculateBestCookieScore(kIngredients, nIngredientsSize, nTargetCalories, &kBestScore, NULL, 0)

size_t countIngredients(const size_t* kIngredientList, const size_t nIngredientListSize)
{
    size_t i                = 0;
    size_t nIngredientCount = 0;
    for (i = 0; i < nIngredientListSize; ++i)
    {
        nIngredientCount += kIngredientList[i];
    }
    return nIngredientCount;
}

void calculateBestCookieScore(const ingredientProperties* kIngredients, const size_t nIngredientsSize, const int64_t nTargetCalories, bestCookieScore* pBestScore, size_t* kIngredientList, const size_t nIngredientListSize)
{
    size_t* kIngredientListLocal     = (kIngredientList == NULL) ? ((size_t*)malloc(nIngredientsSize * sizeof(size_t))) : kIngredientList;
    size_t  nIngredientListSizeLocal = nIngredientListSize;
    size_t  nLoopStart               = 0;
    size_t  nLoopEnd                 = 100;
    size_t  i;

    // Initialise the Scores on first entry...
    if (nIngredientListSize == 0)
    {
        pBestScore->nBestScore     = 0;
        pBestScore->nBestDietScore = 0;
        kIngredientListLocal[0]   = 0;
        ++nIngredientListSizeLocal;
    }
    else if (nIngredientListSizeLocal == nIngredientsSize)
    {
        nLoopStart = 100 - countIngredients(kIngredientList, nIngredientListSizeLocal-1);
        nLoopEnd   = nLoopStart;
    }

    for (i = nLoopStart; i <= nLoopEnd; ++i)
    {
        kIngredientListLocal[nIngredientListSizeLocal-1] = i;

        // Simple guard to prevent the Cookie Score being calculated for a set of ingredient counts
        // that have already been checked
        if (i > 0)
        {
            if (100 == countIngredients(kIngredientListLocal, nIngredientListSizeLocal))
            {
                // Calculate the Cookie Score/Calorie Count
                const cookieScore kCookieScore = calculateCookieScore(kIngredients, kIngredientListLocal, nIngredientListSizeLocal);
                
                // Update the Best Score
                pBestScore->nBestScore = MAX(kCookieScore.nCookieScore, pBestScore->nBestScore);
                
                // Update the Best Diet Score
                if (kCookieScore.nCalories == nTargetCalories)
                {
                    pBestScore->nBestDietScore = MAX(kCookieScore.nCookieScore, pBestScore->nBestDietScore);
                }

                break; // No point iterating further as the ingredient count can only go up!
            }
        }

        // If there's still ingredient we can iterate on...
        if (nIngredientListSizeLocal < nIngredientsSize)
        {
            // Recursive to the next level to start updating the next ingredient
            kIngredientListLocal[nIngredientListSizeLocal] = 0;
            ++nIngredientListSizeLocal;
            calculateBestCookieScore(kIngredients, nIngredientsSize, nTargetCalories, pBestScore, kIngredientListLocal, nIngredientListSizeLocal);
            --nIngredientListSizeLocal;
        }
    }

    if (NULL == kIngredientList)
    {
        free(kIngredientListLocal);
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long     nStartPos                 = ftell(pInput);
        unsigned long           nEndPos;
        unsigned long           nFileSize;

        char*                   kBuffer;
        char**                  kLines;
        char*                   pLine;

        size_t                  nIngredientCount          = 0;
        ingredientProperties*   kIngredients;
        size_t                  i;

        bestCookieScore         kBestScore;

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

            kLines[nIngredientCount] = pLine;
            ++nIngredientCount;

            pLine = strtok(NULL, "\n");
        }

        /* Allocate the Ingredients */
        kIngredients = (ingredientProperties*)malloc(nIngredientCount * sizeof(ingredientProperties));

        /* Store the Ingredients */
        for (i = 0; i < nIngredientCount; ++i)
        {
            char*     pToken  = strtok(kLines[i], " ");
            size_t    j       = 0;

            while (pToken)
            {
                /* Note: C/C++ String -> Numeric Conversions terminate the moment a non-numerical character is found */
                /*       So no need to strip out trailing commas.                                                    */
                if (2 == j)
                {
                    kIngredients[i].nCapacity   = strtoll(pToken, NULL, 10);
                }
                else if (4 == j)
                {
                    kIngredients[i].nDurability = strtoll(pToken, NULL, 10);
                }
                else if (6 == j)
                {
                    kIngredients[i].nFlavor     = strtoll(pToken, NULL, 10);
                }
                else if (8 == j)
                {
                    kIngredients[i].nTexture    = strtoll(pToken, NULL, 10);
                }
                else if (10 == j)
                {
                    kIngredients[i].nCalories   = strtoll(pToken, NULL, 10);
                }

                pToken = strtok(NULL, " ");
                ++j;
            }
        }

        /* Free the Line Buffers since we've parsed all the data into the Ingredient Data */
        free(kBuffer);
        free(kLines);

        /* Calculate the best cookie score */
        calculateBestCookieScoreDefault(kIngredients, nIngredientCount, 500, kBestScore);

        printf("Part 1: %lli\n", kBestScore.nBestScore);
        printf("Part 2: %lli\n", kBestScore.nBestDietScore);

        /* Free the Ingredients... but not the flavour! */
        free(kIngredients);
   }

    return 0;
}
