import time
import math
from functools import cmp_to_key

def main() :

    # Read/Clean the Input File to extract the Byte Code Sequence
    packages = []
    with open("../input.txt", "r") as inputFile:
        packages = [int(line.strip()) for line in inputFile.readlines()]
    #end

    def getPermutationWithLowestQE(packages, compartmentCount) :

        def calculateQEFromMask(packages, mask) :
            product = 1
            for i in range(len(packages)) :
                if ((1 << i) & mask) != 0 :
                    product *= packages[i]
                #end
            #end
            return product
        #end

        def calculateCountFromMask(mask) :
            length = 0
            while mask > 0 :
                if (mask & 1) == 1 :
                    length += 1
                #end
                mask = mask // 2
            #end
            return length
        #end

        def getPermutationIndexes(packages, targetWeight, permutations, currentIndex = 0, currentWeight = 0, currentMask = 0) :

            for i in range(currentIndex, len(packages)) :

                deltaMask = 1 << i

                if (currentWeight + packages[i]) < targetWeight :

                    # Iterate...
                    getPermutationIndexes(packages, targetWeight, permutations, i + 1, currentWeight + packages[i], currentMask | deltaMask)

                elif (currentWeight + packages[i]) == targetWeight :

                    newMask = currentMask | deltaMask

                    # Store the Quantity, Quantum Entanglement and Mask
                    # This allows for the packages to be sorted, which means the first viable solution
                    # found when iterating will *always* have the best package in terms of Quantity/QE.
                    permutations.append([calculateCountFromMask(newMask), calculateQEFromMask(packages, newMask), newMask])

                    # The Weights are sorted so there's no more possible solutions at this depth
                    break

                else :
                    # The Weights are sorted so there's no more possible solutions at this depth
                    break
                #end
            #end

        #end

        def getLowestQE(packages, permutations, compartmentCount, lowestQE = math.inf, currentIndex = 0, combinedMask = 0, depth = 1, stack = None) :

            if None == stack :
                stack = []
            #end

            for i in range(currentIndex, len(permutations)) :

                # An easy way of telling whether two masks have no bits in common is to compare the OR to the ADD.
                # If there's no bits in common, the result should be equal, if not, then they share bits.
                deltaMask = permutations[i][2] | combinedMask
                if (deltaMask) == (permutations[i][2] + combinedMask) :

                    if depth == compartmentCount :
                        return stack[0]
                    else :
                        lowestQE = getLowestQE(packages, permutations, compartmentCount, lowestQE, i+1, deltaMask, depth + 1, stack + [permutations[i][1]])
                        if lowestQE != math.inf :
                            return lowestQE
                        #end
                    #end

                #end

            #end

            return lowestQE

        #end

        groupTarget             = sum(packages) // compartmentCount
        permutations            = []

        # As an optimsation for getPermutationIndexes, if the packages are sorted,
        # any time the weight for a permutation is too high, we can perform an
        # early termination
        packages.sort()

        # Get all the possible ways of making the target weight
        # This is stored as a mask, where each bit represents an index value
        # (since AoC was nice enough to provide < 64 entries), otherwise the
        # permutations would need to store the weights themselves which is
        # very inefficient.
        getPermutationIndexes(packages, groupTarget, permutations)

        # Sort the Permutations, this way the getLowestQE function can
        # return the first element of the first solution where all the
        # weights are balanced.  The data in Permutations is deliberately
        # structured such that it's sorted by Quantity, then Quantum Entanglement
        # in order to trivialise the final solve...
        permutations.sort()

        # Find the first combination that has no overlap, and return its QE
        return getLowestQE(packages, permutations, compartmentCount)

    #end

    print(f"Part 1: {getPermutationWithLowestQE(packages, 3)}")
    print(f"Part 2: {getPermutationWithLowestQE(packages, 4)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
