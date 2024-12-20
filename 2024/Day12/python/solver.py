import time
import sys

def main() :
 
    # Bad!
    sys.setrecursionlimit(10000)

    def calculateCropRegionBFS(kFarm : list[list[str]], kCrop : str, X : int, Y : int) -> tuple[int, int] :

        NEIGHBOURS = [[0, -1], [0, 1], [-1, 0], [1, 0]]

        nPerimeter = 0
        nArea      = 0
        kSeen      = set()
        kPerimeter = set()

        kPositionQueue = [(X, Y)]

        while len(kPositionQueue) > 0 :

            kPositionKey = kPositionQueue.pop()

            if kPositionKey in kSeen :
                continue
            #eend
            kSeen.add(kPositionKey)

            nArea += 1

            for kNeighbour in NEIGHBOURS :
    
                nX = kPositionKey[0] + kNeighbour[0]
                nY = kPositionKey[1] + kNeighbour[1]

                if (nX, nY) in kSeen :
                    continue
                #end
    
                if nY in range(len(kFarm)) :
                    if nX in range(len(kFarm[nY])) :
                        if kFarm[nY][nX] == kCrop :
                            kPositionQueue.append((nX, nY))
                        else :
                            nPerimeter += 1
                            kPerimeter.add((nX, nY))
                        #end
                    else :
                        nPerimeter += 1
                        kPerimeter.add((nX, nY))
                    #end
                else :
                    nPerimeter += 1
                    kPerimeter.add((nX, nY))
                #end
            #end

        #end

        return nArea, nPerimeter, kSeen, kPerimeter

    #end

    def calculateCropRegionDFS(kFarm : list[list[str]], kCrop : str, X : int, Y : int, nPerimeter : int = 0, nArea : int = 0, kSeen : set[int,int] = None, kPerimeter : set[int, int] = None) -> tuple[int, int] :
 
        if kSeen is None :
            kSeen = set()
        #end
        if kPerimeter is None :
            kPerimeter = set()
        #end
 
        kPositionKey = (X, Y)
        if kPositionKey in kSeen :
            return nArea, nPerimeter, kSeen, kPerimeter
        #eend
        kSeen.add(kPositionKey)
 
        nArea += 1
 
        NEIGHBOURS = [[0, -1], [0, 1], [-1, 0], [1, 0]]
 
        for kNeighbour in NEIGHBOURS :
 
            nX = X + kNeighbour[0]
            nY = Y + kNeighbour[1]
 
            if nY in range(len(kFarm)) :
                if nX in range(len(kFarm[nY])) :
                    if kFarm[nY][nX] == kCrop :
                        nArea, nPerimeter, kSeen, kPerimeter = calculateCropRegionDFS(kFarm, kCrop, nX, nY, nPerimeter, nArea, kSeen, kPerimeter)
                    else :
                        nPerimeter += 1
                        kPerimeter.add((nX, nY))
                    #end
                else :
                    nPerimeter += 1
                    kPerimeter.add((nX, nY))
                #end
            else :
                nPerimeter += 1
                kPerimeter.add((nX, nY))
            #end
        #end
 
        return nArea, nPerimeter, kSeen, kPerimeter
 
    #end
 
    # Parse the Input File
    kFarm    = []
    kCrops   = []
    kSeen    = set()
    with open("../input.txt", "r") as inputFile:
        kFarm = [list(k.strip()) for k in inputFile.readlines()]
    #end

    # Use the BFS Solutution
    calculateCropRegion = calculateCropRegionBFS
 
    # Find the Regions/Areas
    for Y, kRow in enumerate(kFarm) :
        for X, kCell in enumerate(kRow) :
            if not (X, Y) in kSeen :
                nArea, nPerimeter, kSeenThisCrop, _ = calculateCropRegion(kFarm, kCell, X, Y)
                kCrops.append((nArea, nPerimeter))
                kSeen |= kSeenThisCrop
            #en
        #end
    #end
 
    print(f"Part 1: {sum(v[0] * v[1] for v in kCrops)}")

    # One of the key issues is permiters really exist between tiles,
    # and you edge cases to consider... or just waste a ton of RAM
    # and enlarge the farm by a factor of 3 to ensure permiters can
    # be given grid coordinates without the need for edge case checks,
    # just simple overlap checks of straight sides as everything will
    # now have a line of at least three cells for the perimeter.
    #
    # For example consider
    #
    # EEEEE
    # EXXXX
    # EEEEE
    # EXXXX
    # EEEEE
    #
    # If we explode it by 3 we can represent the perimiter in grid
    #
    # EEEEEEEEEEEEEEE
    # EEEEEEEEEEEEEEE
    # EEEEEEEEEEEEEEE
    # EEEXXXXXXXXXXXX
    # EEEXXXXXXXXXXXX
    # EEEXXXXXXXXXXXX
    # EEEEEEEEEEEEEEE
    # EEEEEEEEEEEEEEE
    # EEEEEEEEEEEEEEE
    # EEEXXXXXXXXXXXX
    # EEEXXXXXXXXXXXX
    # EEEXXXXXXXXXXXX
    # EEEEEEEEEEEEEEE
    # EEEEEEEEEEEEEEE
    # EEEEEEEEEEEEEEE
    #
    # For the E we have:
    #
    #  .###############.
    #  #EEEEEEEEEEEEEEE#
    #  #EEEEEEEEEEEEEEE#
    #  #EEEEEEEEEEEEEEE#
    #  #EEE############.
    #  #EEE#............
    #  #EEE############.
    #  #EEEEEEEEEEEEEEE#
    #  #EEEEEEEEEEEEEEE#
    #  #EEEEEEEEEEEEEEE#
    #  #EEE############.
    #  #EEE#............
    #  #EEE############.
    #  #EEEEEEEEEEEEEEE#
    #  #EEEEEEEEEEEEEEE#
    #  #EEEEEEEEEEEEEEE#
    #  .###############.
    #
    # And for the X Regions:
    #
    # ....############.
    # ...#XXXXXXXXXXXX#
    # ...#XXXXXXXXXXXX#
    # ...#XXXXXXXXXXXX#
    # ....############.


    kFarmExploded = []

    for kRow in kFarm :
        kFarmExploded.append([])
        kFarmExploded.append([])
        kFarmExploded.append([])
        for kCell in kRow :
            kFarmExploded[-1].extend([kCell] * 3)
            kFarmExploded[-2].extend([kCell] * 3)
            kFarmExploded[-3].extend([kCell] * 3)
        #end
    #end

    # Find the Perimeters (again)
    kSeen         = set()
    kCropsPartTwo = []
    for Y, kRow in enumerate(kFarmExploded) :
        for X, kCell in enumerate(kRow) :
            if not (X, Y) in kSeen :
                _, _, kSeenThisCrop, kPerimeter = calculateCropRegion(kFarmExploded, kCell, X, Y)
                kCropsPartTwo.append(kPerimeter)
                kSeen |= kSeenThisCrop
            #en
        #end
    #end

    # Now just follow the sides
    HORIZONTAL = [1, 0]
    VERTICAL   = [0, 1]
    nPartTwo   = 0
    kSeen      = set()

    def followSide(kPerimeter, X, Y, kDirection, kPointsInSide = None) :

        if (X, Y) in kPointsInSide :
            return
        #end
        kPointsInSide.add((X, Y))

        nNextX = X + kDirection[0]
        nNextY = Y + kDirection[1]
        if (nNextX, nNextY) in kPerimeter :
            followSide(kPerimeter, nNextX, nNextY, kDirection, kPointsInSide)
        #end

        nNextX = X - kDirection[0]
        nNextY = Y - kDirection[1]
        if (nNextX, nNextY) in kPerimeter :
            followSide(kPerimeter, nNextX, nNextY, kDirection, kPointsInSide)
        #end

    #end

    for i,kCrop in enumerate(kCropsPartTwo) :

        nSides = 0

        # Find all Sides
        kSeen = set()
        for kPoint in kCrop :

            if kPoint in kSeen :
                continue
            #end

            # This is now just a function of finding any point and following adjacent points
            # in a single axis, if there's more than 1, it's a side.
            kHorizontalSide = set()
            kVerticalSide   = set()
            followSide(kCrop, kPoint[0], kPoint[1], HORIZONTAL, kHorizontalSide)
            followSide(kCrop, kPoint[0], kPoint[1], VERTICAL, kVerticalSide)

            if len(kHorizontalSide) > 1 :
                nSides += 1
            #end
            if len(kVerticalSide) > 1 :
                nSides += 1
            #end

            kSeen |= kHorizontalSide
            kSeen |= kVerticalSide
        #end

        nPartTwo += kCrops[i][0] * nSides

    #end

    print(f"Part 2: {nPartTwo}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end