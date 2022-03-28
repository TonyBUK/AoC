import time
import sys

def main() :

    # Read the Input File
    dimensions = []
    with open("../input.txt", "r") as inputFile:
        boxes = [[int(X) for X in line.strip().split("x")] for line in inputFile.readlines()]
    #end

    def calculateWrappingPaper(box) :
        L     = 0
        W     = 1
        H     = 2
        SIDES = [box[L] * box[W], box[W] * box[H], box[H] * box[L]]
        return min(SIDES) + sum([2 * SIDE for SIDE in SIDES])
    #end

    def calculateRibbon(box) :
        sortedBox = sorted(box)
        return (2 * sortedBox[0]) + (2 * sortedBox[1]) + (box[0] * box[1] * box[2])
    #end

    wrappingPaper = 0
    ribbon        = 0
    for box in boxes :
        wrappingPaper += calculateWrappingPaper(box)
        ribbon        += calculateRibbon(box)
    #end

    print(f"Part 1: {wrappingPaper}")
    print(f"Part 2: {ribbon}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
