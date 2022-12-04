import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        def getRange(pair) :
            return range(int(pair[0]), int(pair[1]) + 1)
        #end

        # First Extract the Pairs
        kPairs = [[set(getRange(pair.split("-"))) for pair in line.strip().split(",")] for line in inputFile.readlines()]

        # For each Pair
        nDuplicates = 0
        nIntersections = 0

        for kPair in kPairs :

            # Part 1: One Pair is a Subset of the Other
            if kPair[0].issubset(kPair[1]) or kPair[1].issubset(kPair[0]) :
                nDuplicates += 1
            #end

            # Part 2: An intersection of any size exists between the pairs
            if len(kPair[0].intersection(kPair[1])) > 0 :
                nIntersections += 1
            #end
        #end

        print(f"Part 1: {nDuplicates}")
        print(f"Part 2: {nIntersections}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
