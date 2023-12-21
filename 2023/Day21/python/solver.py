import time
import sys

DIRECTIONS         = [(0, -1), (1, 0), (0, 1), (-1, 0)]
MAX_STEPS_PART_ONE = 64
MAX_STEPS_PART_TWO = 26501365

def main() :

    kGarden = []
    with open("../input.txt", "r") as inputFile:
        kGarden = [kline.replace("\n", "") for kline in inputFile.readlines()]
    #end

    # Part 1 - Find the Starting Position
    kStart = None
    kRocks = set()
    for Y in range(len(kGarden)):
        for X in range(len(kGarden[Y])):
            if kGarden[Y][X] == "S":
                assert(kStart is None)
                kStart = (Y, X)
            elif kGarden[Y][X] == "#":
                kRocks.add((Y, X))
            #end
        #end
    #end
    assert(kStart is not None)

    def finalPositionCount(nIterations, kStart, kRocks, nGridWidth, nGridHeight, bExtend = False, bPrint = False) :

        kQueue              = [tuple([0, kStart])]
        kSeen               = set()
        nFinalPositionCount = 0
        kFinalPosition      = set()

        while (len(kQueue) > 0) :

            # Pop the Current Entry
            kQueueEntry = kQueue.pop()
            nSteps      = kQueueEntry[0]
            kPosition   = kQueueEntry[1]

            # Mark as Seen
            kSeen.add(kQueueEntry)

            if nSteps == nIterations :
                nFinalPositionCount += 1
                kFinalPosition.add(kPosition)
                continue
            #end

            # Check the Neighbours
            for kDirection in DIRECTIONS :

                kNextPosition      = (kPosition[0] + kDirection[0], kPosition[1] + kDirection[1])
                kNextPositionQueue = tuple([nSteps + 1, kNextPosition])
                if bExtend :
                    kNextPositionMainGrid = (kNextPosition[0] % nGridHeight, kNextPosition[1] % nGridWidth)
                else :
                    kNextPositionMainGrid = kNextPosition
                    if kNextPosition[0] < 0 or kNextPosition[0] >= nGridHeight or kNextPosition[1] < 0 or kNextPosition[1] >= nGridWidth :
                        continue
                    #end
                #end

                if kNextPositionQueue in kSeen :
                    continue
                elif kNextPositionMainGrid in kRocks :
                    continue
                else :
                    kQueue.append(kNextPositionQueue)
                #end

            #end

        #end

        if bPrint :

            if bExtend :

                nMinAxisY = min([k[0] for k in kFinalPosition])
                nMaxAxisY = max([k[0] for k in kFinalPosition])
                nMinAxisX = min([k[1] for k in kFinalPosition])
                nMaxAxisX = max([k[1] for k in kFinalPosition])

                if nMinAxisY < 0 :
                    if nMinAxisY % nGridHeight != 0 :
                        nMinAxisY = ((nMinAxisY // nGridHeight)) * nGridHeight
                    #end
                else :
                    nMinAxisY = 0
                #end

                if nMinAxisX < 0 :
                    if nMinAxisX % nGridWidth != 0 :
                        nMinAxisX = ((nMinAxisX // nGridWidth)) * nGridWidth
                    #end
                else :
                    nMinAxisX = 0
                #end

                if nMaxAxisY > nGridHeight :
                    if nMaxAxisY % nGridHeight != 0 :
                        nMaxAxisY = (((nMaxAxisY // nGridHeight) + 1) * nGridHeight) - 1
                    #end
                else :
                    nMaxAxisY = nGridHeight - 1
                #end

                if nMaxAxisX > nGridWidth :
                    if nMaxAxisX % nGridWidth != 0 :
                        nMaxAxisX = (((nMaxAxisX // nGridWidth) + 1) * nGridWidth) - 1
                    #end
                else :
                    nMaxAxisX = nGridWidth - 1
                #end

            else :

                nMinAxisX = 0
                nMaxAxisX = nGridWidth - 1
                nMinAxisY = 0
                nMaxAxisY = nGridHeight - 1

            #end

            for Y in range(nMinAxisY, nMaxAxisY+1):
                for X in range(nMinAxisX, nMaxAxisX+1):
                    if kGarden[Y%nGridHeight][X%nGridWidth] == "#":
                        print("#", end="")
                    elif (Y, X) in kFinalPosition :
                        print("O", end="")
                    else :
                        print(".", end="")
                    #end
                #end
                print("")
            #end
        #end

        return nFinalPositionCount

    #end

    print(f"Part 1: {finalPositionCount(MAX_STEPS_PART_ONE, kStart, kRocks, len(kGarden[0]), len(kGarden))}")

    # Part 2 - Process this as a Rhombus... but it's too big, so we need to subdivide it into the smaller
    #          components that make up the Rhombus.
    #
    # This will consist of:
    #
    # - Whole Grids, which will have odd/even patterns of steps in an alternating pattern.
    # - Diagonals, which will have a pattern of steps that will repeat in two patterns for each diagonal.
    # - Corners, which will have a pattern of steps that will unique for each corner.
    nWidth        = len(kGarden[0])
    nHeight       = len(kGarden)
    assert(nWidth == nHeight)

    # Calculate the Number of Grids in the Rhombus
    nGridSize = (MAX_STEPS_PART_TWO // nHeight) - 1

    # Calculate the Number of Whole Grids in the Rhombus
    nOddGrids   = (((nGridSize + 0) // 2) * 2) + 1
    nOddGrids  *= nOddGrids
    nEvenGrids  = ((nGridSize + 1) // 2) * 2
    nEvenGrids *= nEvenGrids

    # Calculate the Odds/Evens in the Grid
    nOdds  = finalPositionCount((nWidth * 2) + 1, kStart, kRocks, nWidth, nHeight)
    nEvens = finalPositionCount((nWidth * 2) + 0, kStart, kRocks, nWidth, nHeight)

    # Calculate the Number of Wholey Contained Grids
    nVisitedCount  = (nOddGrids * nOdds) + (nEvenGrids * nEvens)

    # Handle the Diagonals, these repeat in two patterns, and will of course differ on direction.
    nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth // 2) - 1,       (nWidth - 1, nHeight - 1), kRocks, nWidth, nHeight) # Top Left (Starting Right Middle)
    nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) // 2) - 1, (nWidth - 1, nHeight - 1), kRocks, nWidth, nHeight) # Top Left (Ending at Left Middle)
    nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth // 2) - 1,       (nWidth - 1, 0),           kRocks, nWidth, nHeight) # Top Right (starting Left Middle)
    nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) // 2) - 1, (nWidth - 1, 0),           kRocks, nWidth, nHeight) # Top Right (Ending at Right Middle)
    nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth // 2) - 1,       (0,          0),           kRocks, nWidth, nHeight) # Bottom Right (Starting Left Middle)
    nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) // 2) - 1, (0,          0),           kRocks, nWidth, nHeight) # Bottom Right (Ending at Right Middle)
    nVisitedCount += (nGridSize + 1) * finalPositionCount((nWidth // 2) - 1,       (0,          nHeight - 1), kRocks, nWidth, nHeight) # Bottom Left (Starting Right Middle)
    nVisitedCount += (nGridSize + 0) * finalPositionCount(((nWidth * 3) // 2) - 1, (0,          nHeight - 1), kRocks, nWidth, nHeight) # Bottom Left (Ending at Left Middle)

    # Now Handle the Edges
    nVisitedCount += finalPositionCount(nHeight - 1, (nHeight - 1, kStart[1]),  kRocks, nWidth, nHeight) # Top
    nVisitedCount += finalPositionCount(nWidth  - 1, (kStart[0],   0),          kRocks, nWidth, nHeight) # Right
    nVisitedCount += finalPositionCount(nHeight - 1, (0,           kStart[1]),  kRocks, nWidth, nHeight) # Bottom
    nVisitedCount += finalPositionCount(nWidth  - 1, (kStart[0],   nWidth - 1), kRocks, nWidth, nHeight) # Left

    print(f"Part 2: {nVisitedCount}")

#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end