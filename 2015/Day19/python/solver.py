import time
import math

def main() :

    # Read/Clean the Input File to extract the Lines
    fileLines = []
    with open("../input.txt", "r") as inputFile:
        fileLines = [line.strip() for line in inputFile.readlines()]
    #end

    # Process the Data for Parsing
    assert(len(fileLines) > 2)
    moduleFabricationRules = [line.split(" => ") for line in fileLines[0:-2]]
    medicineMolecule       = fileLines[-1]

    def getPermutations(medicineMolecule, moduleFabricationRules) :

        uniquePermutations = set()
        
        # For each rule
        for moduleFabricationRule in moduleFabricationRules :

            # Replace all instances of the substring...
            newIndex   = medicineMolecule.find(moduleFabricationRule[0], 0)
            while (newIndex >= 0) :

                # Extract the Text either side of the current substition
                left  = medicineMolecule[0:newIndex]
                right = medicineMolecule[newIndex:].replace(moduleFabricationRule[0], moduleFabricationRule[1], 1)

                # Add the Substitution to the unique list of Substituted Strings
                uniquePermutations.add(left + right)

                # Attempt to find the next substitution...
                newIndex   = medicineMolecule.find(moduleFabricationRule[0], newIndex + 1)

            #end

        #end

        return uniquePermutations

    #end

    def findFavouriteMolecule(currentMolecules, moduleFabricationRules, favouriteMolecule, depth = 1, bestDepth = math.inf) :

        # Note: We could work our way forwards, parsing all possible sets at each level until
        #       we reach an answer, but this soon becomes a "heat death of the universe" style
        #       task where each new depth exponentially adds more permutations.
        #       Instead, if we start from the end and work backwards.  The fact the solutions are
        #       reductive helps reduce the overhead of permutations.

        if depth == 1:

            for i in range(len(moduleFabricationRules)) :

                temp = moduleFabricationRules[i][0]
                moduleFabricationRules[i][0] = moduleFabricationRules[i][1]
                moduleFabricationRules[i][1] = temp

                # This assumes that we are always reductive, so assert that theory
                assert(len(moduleFabricationRules[i][0]) >= len(moduleFabricationRules[i][1]))

            #end

        #end

        for currentMolecule in currentMolecules :

            newMolecules = getPermutations(currentMolecule, moduleFabricationRules)

            if favouriteMolecule in newMolecules :

                bestDepth = depth
                return bestDepth

            elif (depth+1) < bestDepth :

                # Rather than just randomly iterating through the solutions, sort them by length and parse
                # each group separately...

                sortedNewMolecules = sorted(list(newMolecules), key=len)
                listLen            = [len(x) for x in sortedNewMolecules]

                # Parse each group from shortest to longest
                for length in sorted(list(set(listLen))) :

                    newSet    = set([X for X,L in zip(sortedNewMolecules, listLen) if L == length])
                    bestDepth = findFavouriteMolecule(newSet, moduleFabricationRules, favouriteMolecule, depth + 1, bestDepth)

                    # Assume that once we've found any solution, it's the best one.
                    # Whether this stands up to scrutiny or not for other inputs is something
                    # I'd be interested to find out...
                    if bestDepth != math.inf :
                        return bestDepth
                    #end

                #end

            #end

            # Assume that once we've found any solution, it's the best one.
            # Whether this stands up to scrutiny or not for other inputs is something
            # I'd be interested to find out...
            if bestDepth != math.inf :
                return bestDepth
            #end

        #end

        return bestDepth

    #end

    print(f"Part 1: {len(getPermutations(medicineMolecule, moduleFabricationRules))}")
    print(f"Part 2: {findFavouriteMolecule(set([medicineMolecule]), moduleFabricationRules, 'e')}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
