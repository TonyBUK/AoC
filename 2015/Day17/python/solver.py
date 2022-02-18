import time

def main() :

    # Read/Clean the Input File to extract the Buckets
    containers = []
    with open("../input.txt", "r") as inputFile:
        containers = [int(line) for line in inputFile.readlines()]
    #end

    def calculatePermutations(containers, target, current = 0, count = 0, index = 0, permutations = None) :

        # Handle the first time through...
        if permutations is None :
            permutations = {}
        #end

        # Update the Permutation if we've reached our target
        if target == current :

            if count not in permutations :
                permutations[count] = 0
            #end
            permutations[count] += 1

            return permutations

        #end

        # Iterate through the remaining possible permutations
        for i in range(index, len(containers)) :
            if current + containers[i] <= target :
                permutations = calculatePermutations(containers, target, current + containers[i], count + 1, i+1, permutations)
            #end
        #end

        return permutations

    #end

    permutations = calculatePermutations(containers, 150)

    # Get the total of all permutatiosn for Part 1
    total        = sum([i for i in permutations.values()])

    # Get the minimum number of containers used for Part 2
    minSolution  = min(permutations.keys())

    print(f"Part 1: {total}")
    print(f"Part 2: {permutations[minSolution]}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
