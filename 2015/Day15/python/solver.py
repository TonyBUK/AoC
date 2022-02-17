import time

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

    def calculateCookieScore(ingredients, ingredientList) :

        capacity   = 0
        durability = 0
        flavor     = 0
        texture    = 0
        calories   = 0

        for currentIngredientCount,currentIngredient in zip(ingredientList, ingredients) :
            capacity    += currentIngredientCount * currentIngredient["capacity"]
            durability  += currentIngredientCount * currentIngredient["durability"]
            flavor      += currentIngredientCount * currentIngredient["flavor"]
            texture     += currentIngredientCount * currentIngredient["texture"]
            calories    += currentIngredientCount * currentIngredient["calories"]
        #end

        if capacity   < 0 : capacity   = 0
        if durability < 0 : durability = 0
        if flavor     < 0 : flavor     = 0
        if texture    < 0 : texture    = 0
        if calories   < 0 : calories   = 0

        return (capacity * durability * flavor * texture), calories

    #end

    def calculateBestCookieScore(ingredients, targetCalories, bestScore = 0, bestDietScore = 0, ingredientList = None) :

        if ingredientList == None :
            ingredientList = [0]
        #end

        for X in range(100+1) :

            ingredientList[-1] = X

            if X > 0 :
                if sum(ingredientList) > 100 : break
                if sum(ingredientList) == 100 :
                    cookieScore,calories = calculateCookieScore(ingredients, ingredientList)
                    if cookieScore > bestScore : bestScore = cookieScore
                    if calories == targetCalories :
                        if cookieScore > bestDietScore : bestDietScore = cookieScore
                    #end
                #end
            #end

            if len(ingredientList) < len(ingredients) :
                ingredientList.append(0)
                bestScore, bestDietScore = calculateBestCookieScore(ingredients, targetCalories, bestScore, bestDietScore, ingredientList)
                ingredientList.pop()
            #end

        #end

        return bestScore, bestDietScore

    #end

    bestScore, bestDietScore = calculateBestCookieScore(ingredients, 500)

    print(f"Part 1: {bestScore}")
    print(f"Part 2: {bestDietScore}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
