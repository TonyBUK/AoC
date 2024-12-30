import time
import sys
import math

def main() :

    # Constants
    NORTH = 0
    EAST  = 1
    SOUTH = 2
    WEST  = 3
    NEIGHBOURS = [[ 0, -1], # North
                  [ 1,  0], # East
                  [ 0,  1], # South
                  [-1,  0]]

    NEIGHBOURS_DIRECTIONS = {
        NORTH : [NORTH, EAST, WEST],
        EAST  : [EAST, NORTH, SOUTH],
        SOUTH : [SOUTH, EAST, WEST],
        WEST  : [WEST, NORTH, SOUTH]
    }

    DIRECTION_SCORES = [[1, 1001, 2001, 1001], # North
                        [1001, 1, 1001, 2001], # East
                        [2001, 1001, 1, 1001], # South
                        [1001, 2001, 1001, 1]] # West

    def findNeighbourNodes(kNode, kMaze, kNodes) :

        kNeighbourNodesWithScores = {}

        kSeenPositions = {}
        kPositionQueue = [[0, (kNode[0], kNode[1])]]

        while len(kPositionQueue) > 0 :

            nScore, kPositionAndDirection = kPositionQueue.pop()
            kPosition, nDirection = kPositionAndDirection

            for nNeighbourDirection in NEIGHBOURS_DIRECTIONS[nDirection] :

                kNeighbour = NEIGHBOURS[nNeighbourDirection]
                nX         = kPosition[0] + kNeighbour[0]
                nY         = kPosition[1] + kNeighbour[1]

                if kMaze[nY][nX] == "#" :
                    continue
                #end

                # Score the Neighbour
                nNeighbourScore = nScore + DIRECTION_SCORES[nDirection][nNeighbourDirection]

                # If this isn't a key target (an intersection)...
                kNeighbourKey = ((nX,nY), nNeighbourDirection)
                if kNeighbourKey not in kNodes :

                    # This position isn't any better than the last time we got here...
                    if kNeighbourKey in kSeenPositions :
                        if nNeighbourScore >= kSeenPositions[kNeighbourKey] :
                            continue
                        #end
                    #end

                    # Score the Distance to this Node, and append to the Queue
                    kPositionQueue.append([nNeighbourScore, kNeighbourKey])

                # Store the Key Target (intersection)
                else :

                    # Score the Distance to this Node, but don't append to the Queue
                    if kNeighbourKey in kNeighbourNodesWithScores :
                        kNeighbourNodesWithScores[kNeighbourKey] = min(kNeighbourNodesWithScores[kNeighbourKey], nNeighbourScore)
                    else :
                        kNeighbourNodesWithScores[kNeighbourKey] = nNeighbourScore
                    #end

                #end

            #end

            if (kPosition, nDirection) in kSeenPositions :
                kSeenPositions[(kPosition, nDirection)] = min(kSeenPositions[(kPosition, nDirection)], nScore)
            else :
                kSeenPositions[(kPosition, nDirection)] = nScore
            #end

        #end

        return kNeighbourNodesWithScores

    #end

    def getAllPointsBetweenNodes(kNode1, kNode2, kNodes, kMaze) :

        kPaths = set()
        kSeen  = {}

        nTargetScore = kNodes[kNode1][kNode2]
        kPositionQueue = [[0, kNode1[0], kNode1[1], [kNode1[0]]]]

        while len(kPositionQueue) > 0 :

            nScore, kPosition, nDirection, kPath = kPositionQueue.pop()

            # If this is the end node, add the route.
            if kPosition == kNode2[0] :

                # Note: Weirdly we can actually get here with a lower score, as the "shortest" path
                #       to the node may actually be through another node.  But if that's the case,
                #       it'll be found when comparing those nodes.
                # assert(nScore >= nTargetScore)

                if nScore == nTargetScore :
                    kPaths |= set(kPath)
                #end
                continue

            #end

            # Have we been here already, if so, skip.
            if (kPosition, nDirection) in kSeen :
                if nScore > kSeen[(kPosition, nDirection)] :
                    continue
                #end
            #end
            kSeen[(kPosition, nDirection)] = nScore

            # For each possible neighbour.
            for nNeighbourDirection in NEIGHBOURS_DIRECTIONS[nDirection] :

                # Is this a Wall?
                kNeighbour = NEIGHBOURS[nNeighbourDirection]
                nX = kPosition[0] + kNeighbour[0]
                nY = kPosition[1] + kNeighbour[1]
                if kMaze[nY][nX] == "#" :
                    continue
                #end

                # Have we exceeded the target score?
                nNewScore = nScore + DIRECTION_SCORES[nDirection][nNeighbourDirection]
                if nNewScore > nTargetScore :
                    continue
                #end

                # Add the new position to the queue.
                kPositionQueue.append([nNewScore, (nX, nY), nNeighbourDirection, kPath + [(nX, nY)]])

            #end

        #end

        assert(len(kPaths) > 0)

        return kPaths

    #end

    def manageDoubleQueue(kQueue, kSecondaryQueue) :
        if len(kQueue) == 0 :
            kQueue = kSecondaryQueue
            kSecondaryQueue = []
        #end
        return kQueue, kSecondaryQueue
    #end

    # Maze
    kMaze  = []
    WIDTH  = 0
    HEIGHT = 0
    START  = None
    END    = None

    with open("../input.txt", "r") as inputFile:

        for Y, kLine in enumerate(inputFile.readlines()) :

            kLine = kLine.strip()
            kMaze.append(list(kLine))

            WIDTH = max(WIDTH, len(kLine))
            HEIGHT += 1

        #end

    #end

    # Compute the Maze as a Series of Nodes, a node being any point that is has three adjacent sides,
    # Or the Start/End Points
    kNodes = {}

    # The Maze is bound by a wall, we only need to process the innies.
    for Y in range(1,HEIGHT-1) :

        for X in range(1,WIDTH-1) :

            # Ignore the entry if it's a Wall
            if kMaze[Y][X] == "#" :
                continue
            #end

            # S/E
            if kMaze[Y][X] == "S" :
                START = (X,Y)
            #end

            if kMaze[Y][X] == "E" :
                END   = (X,Y)
            #end

            kFreeWalls = []
            for nDirection,kNeighbour in enumerate(NEIGHBOURS) :

                nX = X + kNeighbour[0]
                nY = Y + kNeighbour[1]
                if kMaze[nY][nX] != "#" or (X,Y) == END :
                    kFreeWalls.append(nDirection)
                #end

            #end

            # Add the Node, we'll evaluate their meta-data when we have
            # all the nodes declared
            if len(kFreeWalls) >= 3 or kMaze[Y][X] in "SE" :
                for nDirection in kFreeWalls :
                    kNodes[((X,Y),nDirection)] = None
                #end
            #end

        #end

    #end

    # Evaluate the Maze as a series of scores to the next Nodes
    for kNode in kNodes :
        if kNode[0] != END :
            kNodes[kNode] = findNeighbourNodes(kNode, kMaze, kNodes)
        #end
    #end

    # Part 1: Find the Lowest Score from the Start Node to the End Node

    kPositionQueue  = [[0, START, EAST, [(START, EAST)]]]
    kSecondaryQueue = []
    kSeenNodes      = {}
    nMinScore       = math.inf
    kBestPaths      = []

    while len(kPositionQueue) > 0 :
        
        nScore, kPosition, nDirection, kPath = kPositionQueue.pop()

        # If we're at the end, store the score and path if this is the best score
        # we've seen so far.
        if kPosition == END :

            if nScore < nMinScore :
                nMinScore = nScore
                kBestPaths = [kPath]
            elif nScore == nMinScore :
                kBestPaths.append(kPath)
            #end

            kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
            continue

        #end

        if (kPosition, nDirection) in kSeenNodes :
            # Note: For Part Two, it's necessary to allow "equal" scores to be re-evaluated
            #       since the path taken to reach here may differ, and we need all unique paths.
            if nScore > kSeenNodes[(kPosition, nDirection)] :
                kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                continue
            #end
        #end

        for kNeighbour,nNeighbourDeltascore in kNodes[(kPosition, nDirection)].items() :
            nNeighbourScore = nScore + nNeighbourDeltascore
            kNeighbourKey   = (kNeighbour[0], kNeighbour[1])
            if (kNeighbourKey) in kSeenNodes :
                if nNeighbourScore > kSeenNodes[(kNeighbour[0], kNeighbour[1])] :
                    continue
                #end
            #end
            kSecondaryQueue.append([nNeighbourScore, kNeighbour[0], kNeighbour[1], kPath + [kNeighbourKey]])
        #end

        if (kPosition, nDirection) in kSeenNodes :
            kSeenNodes[(kPosition, nDirection)] = min(kSeenNodes[(kPosition, nDirection)], nScore)
        else :
            kSeenNodes[(kPosition, nDirection)] = nScore
        #end

        # Manage the Queue
        # Note: Queue swapping is a very coarse method of sorting in so far as we'll have
        #       some form of oldest to newest, but with no real heuristics.
        kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)

    #end

    print(f"Part 1: {nMinScore}")

    # Part 2: We know which nodes we went through for each route, and even the score to get there,
    #         now we just need the actual points between the nodes.
    kOverlappingPaths = set()
    for kBestPath in kBestPaths :
        for kNode1,kNode2 in zip(kBestPath, kBestPath[1:]) :
            kOverlappingPaths = kOverlappingPaths | getAllPointsBetweenNodes(kNode1, kNode2, kNodes, kMaze)
        #end
    #end

    print(f"Part 2: {len(kOverlappingPaths)}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end