import time
import sys

DIRECTIONS = {
    "U" : (-1,  0),
    "R" : ( 0,  1),
    "D" : ( 1,  0),
    "L" : ( 0, -1)
}

PART_TWO_DIRECTIONS = {
    0 : "R",
    1 : "D",
    2 : "L",
    3 : "U"
}

def main() :

    kDigInstructions = []
    with open("../input.txt", "r") as inputFile:

        for kLine in inputFile.readlines() :

            kTokens    = kLine.replace("\n", "").strip().split(" ")
            kDirPart1  = kTokens[0]
            nDistPart1 = int(kTokens[1])
            nDataPart2 = int(kTokens[2].replace("#","").replace("(", "").replace(")", ""), 16)
            nDirPart2  = PART_TWO_DIRECTIONS[nDataPart2 & 0xF]
            nDistPart2 = nDataPart2 >> 4

            kDigInstructions.append([kDirPart1, nDistPart1, nDirPart2, nDistPart2])

        #end

    #end

    # Thanks Google!
    # https://www.omnicalculator.com/math/irregular-polygon-area
    def shoeLaceFormula(kPoints):

        nNumPoints = len(kPoints)
        nArea      = 0

        for i in range(1, nNumPoints):

            # Extract our Y1/Y2 Terms
            y1, x1 = kPoints[i - 1]
            y2, x2 = kPoints[i]

            # Add to our Area
            nArea += (x1 * y2) - (x2 * y1)

        #end

        # Multiple the terms by 0.5
        return abs(nArea) // 2

    #end

    # Dig the Trenches
    kPointsPartOne    = [[0, 0]]
    kPointsPartTwo    = [[0, 0]]

    # Curiously, all the online calculators I've tried, and my own implementation seem
    # to be off by the Perimeter / 2.
    # I'm sure there's a very maths explanation as to why, but it works, so I'll figure
    # out why after the fact...
    nPerimeterPartOne = 0
    nPerimeterPartTwo = 0

    for kDigInstruction in kDigInstructions :
        kDirPartOne, nDistPartOne, kDirPartTwo, nDistPartTwo = kDigInstruction

        # Part One
        kPointsPartOne.append([kPointsPartOne[-1][0] + (DIRECTIONS[kDirPartOne][0] * nDistPartOne), kPointsPartOne[-1][1] + (DIRECTIONS[kDirPartOne][1] * nDistPartOne)])
        nPerimeterPartOne += nDistPartOne

        # Part Two
        kPointsPartTwo.append([kPointsPartTwo[-1][0] + (DIRECTIONS[kDirPartTwo][0] * nDistPartTwo), kPointsPartTwo[-1][1] + (DIRECTIONS[kDirPartTwo][1] * nDistPartTwo)])
        nPerimeterPartTwo += nDistPartTwo

    #end

    print(f"Part 1: {shoeLaceFormula(kPointsPartOne) + (nPerimeterPartOne // 2) + 1}")
    print(f"Part 2: {shoeLaceFormula(kPointsPartTwo) + (nPerimeterPartTwo // 2) + 1}")

#end

if __name__ == "__main__" :
   startTime = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end