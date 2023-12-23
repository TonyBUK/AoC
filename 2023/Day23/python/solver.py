import time
import sys

VALID_DIRECTIONS = {
    "." : [(-1, 0), (1, 0), (0, -1), (0, 1)],
    ">" : [(0, 1)],
    "<" : [(0, -1)],
    "^" : [(-1, 0)],
    "v" : [(1, 0)]
}

VALID_DIRECTIONS_PART_TWO = {
    "." : [(-1, 0), (1, 0), (0, -1), (0, 1)],
    ">" : [(-1, 0), (1, 0), (0, -1), (0, 1)],
    "<" : [(-1, 0), (1, 0), (0, -1), (0, 1)],
    "^" : [(-1, 0), (1, 0), (0, -1), (0, 1)],
    "v" : [(-1, 0), (1, 0), (0, -1), (0, 1)]
}

def main() :

    kMaze = []
    with open("../input.txt", "r") as inputFile:
        kMaze = [k.replace("\n", "") for k in inputFile.readlines()]
    #end

    START = (0, 1)
    END   = (len(kMaze) - 1, len(kMaze[0]) - 2)

    kValidPositions = set()
    for Y in range(len(kMaze)) :
        for X in range(len(kMaze[Y])) :
            if kMaze[Y][X] in VALID_DIRECTIONS :
                kValidPositions.add((Y, X))
            #end
        #end
    #end

    assert(kMaze[START[0]][START[1]] == ".")
    assert(kMaze[END[0]][END[1]] == ".")

    assert(START in kValidPositions)
    assert(END in kValidPositions)

    def generateNodes(kMaze, kValidPositions, kValidDirections) :

        kNodes = {}
        for Y in range(len(kMaze)) :

            for X in range(len(kMaze[Y])) :

                kPosition = (Y, X)

                if kPosition not in kValidPositions :
                    continue
                #end

                kNodes[kPosition] = []

                for kNeighbour in kValidDirections[kMaze[Y][X]] :
                    kNextPosition = (Y + kNeighbour[0], X + kNeighbour[1])
                    if kNextPosition not in kValidPositions :
                        continue
                    #end
                    kNextPositionWithCost = [(kNextPosition[0], kNextPosition[1]), 1]
                    kNodes[kPosition].append(kNextPositionWithCost)
                #end

            #end

        #end

        bNodesCulled = True
        while bNodesCulled :

            bNodesCulled = False
            kNodesToCull = set()

            for kNode,kNeighbours in kNodes.items() :

                if len(kNeighbours) == 2 :
                    kNodesToCull.add(kNode)
                #end

            #end

            for kNodeToCull in kNodesToCull :

                kNeighbours = kNodes[kNodeToCull]

                assert(len(kNeighbours) == 2)

                kNodeToCullWithCost = []
                for kNeighbour in kNeighbours :
                    for kNeighbourWithCost in kNodes[kNeighbour[0]] :
                        if kNeighbourWithCost[0] == kNodeToCull :
                            kNodeToCullWithCost.append(kNeighbourWithCost)
                        #end
                    #end
                #end

                # This may fail as some nodes may be one way traversals, won't be bidirectional
                if len(kNodeToCullWithCost) == 2 :

                    # Link the Neighbours
                    kNodes[kNeighbours[0][0]].remove(kNodeToCullWithCost[0])
                    kNodes[kNeighbours[0][0]].append([(kNeighbours[1][0][0], kNeighbours[1][0][1]), kNeighbours[0][1] + kNodeToCullWithCost[1][1]])
                    kNodes[kNeighbours[1][0]].remove(kNodeToCullWithCost[1])
                    kNodes[kNeighbours[1][0]].append([(kNeighbours[0][0][0], kNeighbours[0][0][1]), kNeighbours[1][1] + kNodeToCullWithCost[0][1]])

                    kNodes.pop(kNodeToCull)

                    bNodesCulled = True

                #end

            #end

        #end

        return kNodes

    #end

    def findLongestRoute(kPosition, kEnd, kNodes, nMaxSteps = 0, nSteps = 0, kRoute = None) :

        nLocalMaxSteps = nMaxSteps

        if kRoute is None :
            kRoute = set()
        #end

        if kPosition == kEnd :
            if nSteps > nMaxSteps :
                return nSteps
            else :
                return nMaxSteps
            #end
        #end

        for kNeighbour in kNodes[kPosition] :

            if kNeighbour[0] in kRoute :
                continue
            #end

            kRoute.add(kNeighbour[0])
            nLocalMaxSteps = findLongestRoute(kNeighbour[0], kEnd, kNodes, nLocalMaxSteps, nSteps + kNeighbour[1], kRoute)
            kRoute.remove(kNeighbour[0])

        #end

        return nLocalMaxSteps

    #end

    kPartOneNodes = generateNodes(kMaze, kValidPositions, VALID_DIRECTIONS)
    print(f"Part 1: {findLongestRoute(START, END, kPartOneNodes)}")
    kPartTwoNodes = generateNodes(kMaze, kValidPositions, VALID_DIRECTIONS_PART_TWO)
    print(f"Part 2: {findLongestRoute(START, END, kPartTwoNodes)}")

#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end