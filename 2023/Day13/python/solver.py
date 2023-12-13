import time
import sys
 
def main() :
 
    kValleys = []
    with open("../input.txt", "r") as inputFile:
        kValleys = [([list(x) for x in k.split("\n")]) for k in inputFile.read().split("\n\n")]
    #end
 
    def findVerticalReflection(kValley, nIgnore = -1) :
 
        for i in range(0, len(kValley) - 1) :
 
            if (i+1) == nIgnore : continue
 
            nTop      = i
            nBottom   = i + 1
            bAllMatch = True
 
            while nTop >= 0 and nBottom <= (len(kValley) - 1) :
 
                if kValley[nTop] != kValley[nBottom] :
                    bAllMatch = False
                    break
                #end
 
                nTop   -= 1
                nBottom += 1
 
            #end
 
            if bAllMatch :
                return i + 1
            #end
 
        #end
 
        return 0
 
    #end
 
    def findHorizontalReflection(kValley, nIgnore = -1) :
 
        # Transpose the Table to simplify mirroring and treat as a Vertical Reflection
        return findVerticalReflection([k for k in zip(*kValley)], nIgnore)
 
    #end
 
    # Part One - Reflection Test
 
    # Find all of the Reflections, and Buffer the Old Ones for Part Two
    kOldReflections = []
    nPatternSummary = 0
    for kValley in kValleys :
        kOldReflections.append([findHorizontalReflection(kValley), findVerticalReflection(kValley)])
        nPatternSummary      += kOldReflections[-1][0] + (kOldReflections[-1][1] * 100)
    #end
 
    print(f"Part 1: {nPatternSummary}")
 
    # Part Two - Reflection Test with Bitflips and Old Result Filtering
 
    OPPOSITE = {
        "#" : ".",
        "." : "#"
    }
 
    nPatternSummary = 0
    for kValley, kOldReflection in zip(kValleys, kOldReflections) :
 
        bFound = False
 
        for Y in range(len(kValley)) :
 
            for X in range(len(kValley[Y])):
 
                # Flip the Tile
                kValley[Y][X] = OPPOSITE[kValley[Y][X]]
 
                # Test Reflections, but filter the old Reflection
                kReflections = [findHorizontalReflection(kValley, kOldReflection[0]), findVerticalReflection(kValley, kOldReflection[1])]
 
                # Add any Horizontal Reflections
                if kReflections[0] > 0 :
                    nPatternSummary += kReflections[0]
                    bFound           = True
                #end
 
                # Add any Vertical Reflections
                if kReflections[1] > 0 :
                    nPatternSummary += kReflections[1] * 100
                    bFound           = True
                #end
 
                # Unflip the Tile
                kValley[Y][X] = OPPOSITE[kValley[Y][X]]
 
                if bFound :
                    break
                #end
 
            #end
 
            # I keep forgetting how to cascade breaks...
            if bFound :
                break
            #end
 
        #end
 
        assert (bFound)
 
    #end
 
    print(f"Part 2: {nPatternSummary}")
 
#end
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end