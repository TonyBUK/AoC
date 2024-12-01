import time
import sys

def main() :

    # Read the Input File as two Sorted Lists of Numbers for Each Column
    kLists = []
    with open("../input.txt", "r") as inputFile:
        kLists = [sorted(k) for k in zip(*[[int(k) for k in kLine.split()] for kLine in inputFile])]
    #end

    # Calculate the Distances for Part 1
    print(f"Part 1: {sum([abs(a - b) for a,b in zip(kLists[0], kLists[1])])}")

    # Calculate the Similarity for Part 2
    print(f"Part 2: {sum([x * kLists[1].count(x) for x in kLists[0]])}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
