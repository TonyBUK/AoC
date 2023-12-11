import time
import sys
 
def main() :

    kGalaxyUnexpanded = []
    with open("../input.txt", "r") as inputFile:
        kGalaxyUnexpanded = [list(k.replace("\n", "")) for k in inputFile.readlines()]
    #end

    def calculateExpandedPoint(nPointX, nPointY, kGalaxyEmptyRows, kGalaxyEmptyCols, nScaling) :
        return (nPointY + (sum(kGalaxyEmptyRows[:nPointY]) * (nScaling-1)), nPointX + (sum(kGalaxyEmptyCols[:nPointX]) * (nScaling-1)))
    #end

    def sumManhattens(kGalaxyPointsExpanded ) :

        def calculateManhattenDistance(kPointA, kPointB) :
            return abs(kPointA[0] - kPointB[0]) + abs(kPointA[1] - kPointB[1])
        #end

        # Convert to a list for indexing
        kGalaxyPointsExpandedList = list(kGalaxyPointsExpanded)

        # Sum all the Manhattens
        nManhattenSum = 0
        for i, kGalaxyA in enumerate(kGalaxyPointsExpandedList) :
            for kGalaxyB in kGalaxyPointsExpandedList[i+1:] :
                nManhattenSum += calculateManhattenDistance(kGalaxyA, kGalaxyB)
            #end
        #end

        return nManhattenSum

    #end

    # Pre-calculate empty rows and columns
    kGalaxyEmptyRows = ["".join(kRow).find("#") == -1 for kRow in kGalaxyUnexpanded]
    kGalaxyEmptyCols = ["".join(kCol).find("#") == -1 for kCol in zip(*kGalaxyUnexpanded)]

    # Create a set of all points in the galaxy (expanded)
    kGalaxyPointsExpandedPartOne = set()
    kGalaxyPointsExpandedPartTwo = set()
    for Y in range(len(kGalaxyUnexpanded)) :
        for X in range(len(kGalaxyUnexpanded[Y])) :
            if kGalaxyUnexpanded[Y][X] == "#" :
                kGalaxyPointsExpandedPartOne.add(calculateExpandedPoint(X, Y, kGalaxyEmptyRows, kGalaxyEmptyCols, 2))
                kGalaxyPointsExpandedPartTwo.add(calculateExpandedPoint(X, Y, kGalaxyEmptyRows, kGalaxyEmptyCols, 1000000))
            #end
        #end
    #end

    print(f"Part 1: {sumManhattens(kGalaxyPointsExpandedPartOne)}")
    print(f"Part 2: {sumManhattens(kGalaxyPointsExpandedPartTwo)}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end