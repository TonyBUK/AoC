import time
import sys
 
def main() :
 
    with open("../input.txt", "r") as inputFile:
 
        kTreeGrid           = [[int(kTree) for kTree in kTrees.strip()] for kTrees in inputFile.readlines()]
 
        def IsVisible(kTrees, nHeight) :
            return max(kTrees) < nHeight
        #end
 
        # Visibility: The Outer Ring is always Visible
        nVisibleCount = (len(kTreeGrid) * 2) + (len(kTreeGrid[-1] * 2)) - 4 # The Corners are common, so remove on set of them
        for Y in range (1, len(kTreeGrid) - 1) :
 
            for X in range (1, len(kTreeGrid[Y]) - 1) :
 
                bIsVisible                              = IsVisible(kTreeGrid[Y][0:X],                                          kTreeGrid[Y][X]) # Left
                if False == bIsVisible : bIsVisible     = IsVisible(kTreeGrid[Y][X+1:],                                         kTreeGrid[Y][X]) # Right
                if False == bIsVisible : bIsVisible     = IsVisible([kTreeGrid[tY][X] for tY in range(Y-1, -1, -1)],            kTreeGrid[Y][X]) # Up
                if False == bIsVisible : bIsVisible     = IsVisible([kTreeGrid[tY][X] for tY in range(Y+1, len(kTreeGrid), 1)], kTreeGrid[Y][X]) # Down
                if True  == bIsVisible : nVisibleCount += 1
 
            #end
 
        #end
 
        print(f"Part 1: {nVisibleCount}")
 
        def UnobstructedViewSize(kTrees, nHeight) :
            nSize = 1
            while kTrees[-1] < nHeight :
                nSize += 1
                kTrees.pop()
                if len(kTrees) == 0 :
                    nSize -= 1
                    break
                #end
            #end
            return nSize
        #end

        nMaxScenicScore = 0
        for Y in range (1, len(kTreeGrid) - 1) :
 
            for X in range (1, len(kTreeGrid[Y]) - 1) :
 
                # We want to arrange the slice under test to ensure adjacent tree's are always the *last* item in the aray
                nScenicScore  = UnobstructedViewSize(kTreeGrid[Y][0:X],                                           kTreeGrid[Y][X]) # Left
                nScenicScore *= UnobstructedViewSize(kTreeGrid[Y][len(kTreeGrid[Y])-1:X:-1],                      kTreeGrid[Y][X]) # Right
                nScenicScore *= UnobstructedViewSize([kTreeGrid[tY][X] for tY in range(0, Y, 1)],                 kTreeGrid[Y][X]) # Up
                nScenicScore *= UnobstructedViewSize([kTreeGrid[tY][X] for tY in range(len(kTreeGrid)-1, Y, -1)], kTreeGrid[Y][X]) # Down
 
                if nScenicScore > nMaxScenicScore : nMaxScenicScore = nScenicScore
 
            #end
 
        #end
 
        print(f"Part 2: {nMaxScenicScore}")
 
    #end
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end