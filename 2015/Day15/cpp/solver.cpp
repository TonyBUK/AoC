#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <cstdint>
#include <assert.h>

std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

struct ingredientProperties
{
    int64_t nCapacity;
    int64_t nDurability;
    int64_t nFlavor;
    int64_t nTexture;
    int64_t nCalories;
};

struct cookieScore
{
    int64_t nCookieScore;
    int64_t nCalories;
};

struct bestCookieScore
{
    int64_t nBestScore;
    int64_t nBestDietScore;
};

// Calculated the Cookie Score for a given ingredient list
// Note: The size of the ingredient list can be less than the size of the number of ingredients,
//       this is taken to mean the remaining ingredients all have a quantity of 0.
cookieScore calculateCookieScore(const std::vector<ingredientProperties>& kIngredients, const std::vector<std::size_t>& kIngredientList)
{
    cookieScore kScore;
    int64_t     nCapacity   = 0;
    int64_t     nDurability = 0;
    int64_t     nFlavor     = 0;
    int64_t     nTexture    = 0;
    int64_t     nCalories   = 0;

    // Multiply each ingredient property by the ingredient count and update the total
    for (std::size_t i = 0; i < kIngredientList.size(); ++i)
    {
        const ingredientProperties& kIngredient      = kIngredients.at(i);
        const int64_t               nIngredientCount = static_cast<int64_t>(kIngredientList.at(i));
    
        nCapacity   += nIngredientCount * kIngredient.nCapacity;
        nDurability += nIngredientCount * kIngredient.nDurability;
        nFlavor     += nIngredientCount * kIngredient.nFlavor;
        nTexture    += nIngredientCount * kIngredient.nTexture;
        nCalories   += nIngredientCount * kIngredient.nCalories;
    }

    // Disallow any negative totals
    if (nCapacity   < 0) nCapacity   = 0;
    if (nDurability < 0) nDurability = 0;
    if (nFlavor     < 0) nFlavor     = 0;
    if (nTexture    < 0) nTexture    = 0;
    if (nCalories   < 0) nCalories   = 0;

    // Return the cookie score and the calorie count    
    kScore.nCookieScore = (nCapacity * nDurability * nFlavor * nTexture);
    kScore.nCalories    = nCalories;
    return kScore;
}

// Recursive Function to find the best cookie score, and the best diet cookie score (cookie score when the calorie count = 500)
void calculateBestCookieScore(std::vector<ingredientProperties>& kIngredients, int64_t nTargetCalories, bestCookieScore& kBestScore, std::vector<std::size_t> kIngredientList = std::vector<std::size_t>())
{
    std::size_t nLoopStart = 0;
    std::size_t nLoopEnd   = 100;

    // Initialise the Scores on first entry...
    if (kIngredientList.size() == 0)
    {
        kBestScore.nBestScore     = 0;
        kBestScore.nBestDietScore = 0;
        kIngredientList.push_back(0);
    }
    else if (kIngredientList.size() == kIngredients.size())
    {
        nLoopStart = static_cast<std::size_t>(100 - std::accumulate(kIngredientList.begin(), kIngredientList.end(), 0ll));
        nLoopEnd   = nLoopStart;
    }

    for (std::size_t i = nLoopStart; i <= nLoopEnd; ++i)
    {
        kIngredientList.back() = i;

        // Simple guard to prevent the Cookie Score being calculated for a set of ingredient counts
        // that have already been checked
        if (i > 0)
        {
            if (100 == std::accumulate(kIngredientList.begin(), kIngredientList.end(), 0ll))
            {
                // Calculate the Cookie Score/Calorie Count
                const cookieScore kCookieScore = calculateCookieScore(kIngredients, kIngredientList);
                
                // Update the Best Score
                kBestScore.nBestScore = std::max(kCookieScore.nCookieScore, kBestScore.nBestScore);
                
                // Update the Best Diet Score
                if (kCookieScore.nCalories == nTargetCalories)
                {
                    kBestScore.nBestDietScore = std::max(kCookieScore.nCookieScore, kBestScore.nBestDietScore);
                }

                break; // No point iterating further as the ingredient count can only go up!
            }
        }

        // If there's still ingredient we can iterate on...
        if (kIngredientList.size() < kIngredients.size())
        {
            // Recursive to the next level to start updating the next ingredient
            kIngredientList.push_back(0);
            calculateBestCookieScore(kIngredients, nTargetCalories, kBestScore, kIngredientList);
            kIngredientList.pop_back();
        }
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<ingredientProperties> kIngredients;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);
            while (kLine.back() == '.')
            {
                kLine.pop_back();
            }

            // Extract the Ingredient Properties
            const std::vector<std::string> kProperties = split(kLine, ' ');
            assert(kProperties.size() == 11);

            // Note: C/C++ String -> Numeric Conversions terminate the moment a non-numerical character is found
            //       So no need to strip out trailing commas.
            ingredientProperties kIngredient;
            kIngredient.nCapacity   = std::stoll(kProperties.at(2));
            kIngredient.nDurability = std::stoll(kProperties.at(4));
            kIngredient.nFlavor     = std::stoll(kProperties.at(6));
            kIngredient.nTexture    = std::stoll(kProperties.at(8));
            kIngredient.nCalories   = std::stoll(kProperties.at(10));

            // Add the Current Ingredient Property to the list...
            kIngredients.push_back(kIngredient);
        }

        bestCookieScore kBestScore;
        calculateBestCookieScore(kIngredients, 500, kBestScore);

        std::cout << "Part 1: " << kBestScore.nBestScore     << std::endl;
        std::cout << "Part 2: " << kBestScore.nBestDietScore << std::endl;
    }

    return 0;
}
