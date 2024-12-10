import time
import sys
 
def main() :
 
    def countRoutes(kHikingTrial : list[list[int]]) -> int :
 
        def countCompleteRoutes(kHikingTrial : list[list[int]], X : int, Y : int, nHeight : int, nDistinctRouteScore : int, kUniqueEndPoints : set[tuple[int,int]]) -> int :
 
            # If we've reached height 9, add to the list of unique end points, and distinct routes
            if nHeight == 9 :
                kUniqueEndPoints.add((X,Y))
                return kUniqueEndPoints, nDistinctRouteScore + 1
            #end
 
            NEIGHBOURS = [( 0,-1), # Up
                          ( 1, 0), # Right
                          ( 0, 1), # Down
                          (-1, 0)  # Left
                        ]
 
            # Use DFS to find all the distinct routes
            for kNeighbour in NEIGHBOURS :
 
                nX = X + kNeighbour[0]
                nY = Y + kNeighbour[1]
 
                if nY in range(len(kHikingTrial)) :
                    if nX in range(len(kHikingTrial[nY])) :
                        if kHikingTrial[nY][nX] == (nHeight + 1) :
                            _, nDistinctRouteScore = countCompleteRoutes(kHikingTrial=kHikingTrial, X=nX, Y=nY, nHeight=nHeight + 1, nDistinctRouteScore=nDistinctRouteScore, kUniqueEndPoints=kUniqueEndPoints)
                        #end
                    #end
                #end
 
            #end
 
            return len(kUniqueEndPoints), nDistinctRouteScore
 
        #end

        # Find all the Zero Height Trail Heads
        nTrailHeadScore     = 0
        nDistinctRouteScore = 0
 
        for Y in range(len(kHikingTrial)) :
            for X in range(len(kHikingTrial[Y])) :
                if kHikingTrial[Y][X] == 0 :
                    nTrailHeadScoreForRoute, nDistinctRouteScore = countCompleteRoutes(kHikingTrial=kHikingTrial, X=X, Y=Y, nHeight=0, nDistinctRouteScore=nDistinctRouteScore, kUniqueEndPoints=set())
                    nTrailHeadScore += nTrailHeadScoreForRoute
                #end
            #end
        #end
 
        return nTrailHeadScore, nDistinctRouteScore
 
    #end
 
    kHikingTrial = []
 
    # Parse the Input File
    with open("../input.txt", "r") as inputFile:
         for kLine in inputFile.readlines() :
            kHikingTrial.append([int(k) for k in kLine.strip()])
        #end
    #end

    # Solve Both Parts
    nPartOne, nPartTwo = countRoutes(kHikingTrial=kHikingTrial)

    print(f"Part 1: {nPartOne}")
    print(f"Part 2: {nPartTwo}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end