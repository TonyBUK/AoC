import time
import sys

def main() :

    # Read the Input File
    lines = []
    with open("../input.txt", "r") as inputFile:
        lines = inputFile.readlines()
    #end

    # Iterate over the Puzzle Input
    # Note: This should just be one line...
    assert(len(lines) == 1)
    for line in lines :

        # Part 1: Subtract the number of ")" from the number of "(" to get the final floor
        print(f"Part 1: {line.count('(') - line.count(')')}")

        # Part 2: Manually apply the algorithm and print the index value
        # Note: The index value is to be output as 1 based, enumerate provides a 0 based index.
        count = 0
        for i,c in enumerate(line) :
            if   c == "(" : count += 1
            elif c == ")" : count -= 1
            if count == -1 :
                print(f"Part 2: {i+1}")
                break
            #end
        #end
    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
