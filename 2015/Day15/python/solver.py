import time
import sys

def main() :

    def extractIngredients(tokens) :
        assert(len(tokens) == 11)
        return {"capacity" : int(tokens[2]), "durability" : int(tokens[4]), "flavor" : int(tokens[6]), "texture" : int(tokens[8]), "calories" : int(tokens[10])}
    #end

    # Read/Clean the Input File to extract the Ingredient Properties
    ingredients = []
    with open("../input.txt", "r") as inputFile:
        ingredients = [extractIngredients(line.replace(",", "").replace("\n", "").split(" ")) for line in inputFile.readlines()]
    #end

    # Calculated the Cookie Score for a given ingredient list
    # Note: The size of the ingredient list can be less than the size of the number of ingredients,
    #       this is taken to mean the remaining ingredients all have a quantity of 0.
    def calculateCookieScore(ingredients, ingredientList) :

        capacity   = 0
        durability = 0
        flavor     = 0
        texture    = 0
        calories   = 0

        # Multiply each ingredient property by the ingredient count and update the total
        for currentIngredientCount,currentIngredient in zip(ingredientList, ingredients) :
            capacity    += currentIngredientCount * currentIngredient["capacity"]
            durability  += currentIngredientCount * currentIngredient["durability"]
            flavor      += currentIngredientCount * currentIngredient["flavor"]
            texture     += currentIngredientCount * currentIngredient["texture"]
            calories    += currentIngredientCount * currentIngredient["calories"]
        #end

        # Disallow any negative totals
        if capacity   < 0 : capacity   = 0
        if durability < 0 : durability = 0
        if flavor     < 0 : flavor     = 0
        if texture    < 0 : texture    = 0
        if calories   < 0 : calories   = 0

        # Return the cookie score and the calorie count
        return (capacity * durability * flavor * texture), calories

    #end

    # Recursive Function to find the best cookie score, and the best diet cookie score (cookie score when the calorie count = 500)
    def calculateBestCookieScore(ingredients, targetCalories, bestScore = 0, bestDietScore = 0, ingredientList = None) :

        # As ever, never initialise a list/dictionary/set in the default parameter list!
        if ingredientList == None :
            ingredientList = [0]
        #end
        
        if len(ingredientList) == len(ingredients) :
            XStart = 100 - sum(ingredientList)
            XEnd   = XStart + 1
        else :
            XStart = 0
            XEnd   = 100+1
        #end

        for X in range(XStart, XEnd) :

            ingredientList[-1] = X

            # Simple guard to prevent the Cookie Score being calculated for a set of ingredient counts
            # that have already been checked
            if X > 0 :

                # If the Ingredient Count is exactly 100
                if sum(ingredientList) == 100 :
                    
                    # Calculate the Cookie Score/Calorie Count
                    cookieScore,calories = calculateCookieScore(ingredients, ingredientList)
                    
                    # Update the Best Score
                    bestScore = max(cookieScore, bestScore)
                    
                    # Update the Best Diet Score
                    if calories == targetCalories :
                        bestDietScore = max(cookieScore, bestDietScore)
                    #end

                    break # No point iterating further as the ingredient count can only go up!

                #end

            #end

            # If there's still ingredient we can iterate on...
            if len(ingredientList) < len(ingredients) :

                # Recursive to the next level to start updating the next ingredient
                ingredientList.append(0)
                bestScore, bestDietScore = calculateBestCookieScore(ingredients, targetCalories, bestScore, bestDietScore, ingredientList)
                ingredientList.pop()

            #end

        #end

        return bestScore, bestDietScore

    #end

    # Get the Best Score/Diet Score in a single pass to only calculate the permutations once
    bestScore, bestDietScore = calculateBestCookieScore(ingredients, 500)

    print(f"Part 1: {bestScore}")
    print(f"Part 2: {bestDietScore}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
