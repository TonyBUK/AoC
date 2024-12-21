import time
import sys
import math

def main() :

    def findShortestRoutes(kWalls : set[int,int], START : tuple[int,int], END : tuple[int, int]) :

        # Basically prevents me having to sort the queue...
        def manageDoubleQueue(kPositionQueue, kSecondaryQueue) :
            if len(kPositionQueue) == 0 :
                kPositionQueue  = kSecondaryQueue
                kSecondaryQueue = []
            #end
            return kPositionQueue, kSecondaryQueue
        #end

        NEIGHBOURS = [[-1, 0], # Left
                      [ 0,-1], # Up
                      [ 1, 0], # Right
                      [ 0, 1]  # Down
        ]

        kPositionQueue  = [[0, START, [START]]]
        kSecondaryQueue = []
        nLowestSteps    = math.inf
        kSeen           = {}
        kFewestSteps    = []

        # BFS Search to find the Position...

        while len(kPositionQueue) > 0 :

            # Get the next possible move
            nSteps, kPosition, kPath = kPositionQueue.pop()

            # If it's already the same or worse than the current known best move...
            if nSteps >= nLowestSteps :
                kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                continue
            #end

            # If this is the end position, test if we've encountered our best one yet
            if kPosition == END :
                if nSteps < nLowestSteps :
                    kFewestSteps = [kPath]
                    nLowestSteps = nSteps
                elif nSteps == nLowestSteps :
                    kFewestSteps.append(kPath)
                #end
                kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                continue
            #end

            # If we've already seen this position and at a lower step count...
            if kPosition in kSeen :
                if kSeen[kPosition] <= nSteps :
                    kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                    continue
                #end
            #end
            kSeen[kPosition] = nSteps

            # For each Neighbour
            for kNeighbour in NEIGHBOURS :

                # In the Grid?
                kNeighbourPosition = (kPosition[0] + kNeighbour[0], kPosition[1] + kNeighbour[1])
                if kNeighbourPosition[0] not in range(WIDTH) or kNeighbourPosition[1] not in range(HEIGHT) :
                    continue
                #end

                # Collided with wall?
                if kNeighbourPosition not in kWalls :
                    kSecondaryQueue.append([nSteps+1, kNeighbourPosition, kPath + [kNeighbourPosition]])
                #end

            #end

            kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)

        #end

        return nLowestSteps, kFewestSteps

    #end

    def updateDistances(kShortestPaths : list[list[int, int]], kShortestDistances : dict[tuple[int, int], int], kShortestPathUniquePoints : set[int, int] = None) :

        for kShortestPath in kShortestPaths :
            nShortestPathLength = len(kShortestPath)
            for i, kPosition in enumerate(kShortestPath) :
                nDistanceFromEnd = nShortestPathLength - i - 1
                kShortestDistances[kPosition] = nDistanceFromEnd
                if None != kShortestPathUniquePoints :
                    if 0 != nDistanceFromEnd :
                        kShortestPathUniquePoints.add(kPosition)
                    #end
                #end
            #end
        #end

    #end

    def manhattanDistance(kPosition1 : tuple[int, int], kPosition2 : tuple[int, int]) -> int :
        return abs(kPosition1[0] - kPosition2[0]) + abs(kPosition1[1] - kPosition2[1])
    #end

    def calculateTimeSave(kShortestPathUniquePoints : set[int, int], kShortestDistances : dict[tuple[int, int], int], nMaxDistance : int, nTarget : int, kWalls : set[int, int], nEnd : tuple[int, int], kUnusableRoutes : set[int, int]) -> int :

        nSavedTime        = 0

        # For Each Position
        for kPosition in kShortestPathUniquePoints :

            # We essentially have a Diamond Search Pattern
            # Note: Constrain this to the inner part of the grid.
            for nNeighbourY in range(max(1, kPosition[1] - nMaxDistance), min(HEIGHT - 1, kPosition[1] + nMaxDistance + 1)) :

                # Compute X based on Y to keep the Diamond Shape
                nXDistance = nMaxDistance - abs(kPosition[1] - nNeighbourY)
                for nNeighbourX in range(max(1, kPosition[0] - nXDistance), min(WIDTH - 1, kPosition[0] + nXDistance + 1)) :

                    # Test the Neighbour...
                    kNeighbourPosition = (nNeighbourX, nNeighbourY)

                    # Is this a known position?
                    if kNeighbourPosition in kShortestDistances :

                        # Coarsley Reject the Value before we apply Manhattan
                        nDelta = kShortestDistances[kPosition] - kShortestDistances[kNeighbourPosition]
                        if nDelta > (nTarget + 1) :

                            # Apply Manhattan to the Delta and Retry
                            nDelta -= manhattanDistance(kPosition1=kPosition, kPosition2=kNeighbourPosition)
                            nSavedTime += nDelta >= nTarget

                        #end

                    ############################################
                    # Uncomment this to handle closed off routes
                    ############################################

                    # Is this a position that has no valid route?
#                    elif kNeighbourPosition not in kUnusableRoutes :
#
#                        # Is this a Wall?
#                        if kNeighbourPosition not in kWalls :
#
#                            # If we don't, find the shortest distance from the Neighbour Position
#                            _, kShortestPathsFromStart = findShortestRoutes(kWalls, kNeighbourPosition, nEnd)
#
#                            # If there's no possible route, flag it as such
#                            if len(kShortestPathsFromStart) == 0 :
#                                kUnusableRoutes.add(kNeighbourPosition)
#                                continue
#                            #end
#
#                            # Update the Distances
#                            updateDistances(kShortestPaths=kShortestPathsFromStart, kShortestDistances=kShortestDistances)
#
#                            # Coarsley Reject the Value before we apply Manhattan
#                            nDelta = kShortestDistances[kPosition] - kShortestDistances[kNeighbourPosition]
#                            if nDelta > (nTarget + 1) :
#
#                                # Apply Manhattan to the Delta and Retry
#                                nDelta -= manhattanDistance(kPosition1=kPosition, kPosition2=kNeighbourPosition)
#                                nSavedTime += nDelta >= nTarget
#
#                            #end
#
#                        #end

                    #end

                #end

            #end

        #end

        return nSavedTime

    #end

    kMaze  = []
    kWalls = set()
    WIDTH  = 0
    HEIGHT = 0
    START  = None
    END    = None

    with open("../input.txt", "r") as inputFile:

        for inputLine in inputFile:

            kLine = inputLine.strip()
            WIDTH = max(len(kLine), WIDTH)

            if "S" in kLine :
                START = (kLine.index("S"), len(kMaze))
            #end

            if "E" in kLine :
                END = (kLine.index("E"), len(kMaze))
            #end

            kMaze.append(list(inputLine.strip()))
        #end

    #end
    HEIGHT = len(kMaze)

    # Add the Walls/Start/End
    kWalls.update([(x, y) for x in range(WIDTH) for y in range(HEIGHT) if kMaze[y][x] == '#'])

    # Calculate the Shortest Route from the Start
    kShortestDistances         = {}
    kShortestPathUniquePoints  = set()
    _, kShortestPathsFromStart = findShortestRoutes(kWalls, START, END)

    # Update the Distances
    updateDistances(kShortestPaths=kShortestPathsFromStart, kShortestDistances=kShortestDistances, kShortestPathUniquePoints=kShortestPathUniquePoints)

    kUnusableRoutes = set()
    print(f"Part 1: {calculateTimeSave(kShortestPathUniquePoints=kShortestPathUniquePoints, kShortestDistances=kShortestDistances, nMaxDistance= 2, nTarget=100, kWalls=kWalls, nEnd=END, kUnusableRoutes=kUnusableRoutes)}")
    print(f"Part 2: {calculateTimeSave(kShortestPathUniquePoints=kShortestPathUniquePoints, kShortestDistances=kShortestDistances, nMaxDistance=20, nTarget=100, kWalls=kWalls, nEnd=END, kUnusableRoutes=kUnusableRoutes)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end