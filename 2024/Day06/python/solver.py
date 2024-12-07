import time
import sys

def main() :

    class GuardDirection :

        def __init__(self) :
            self.nDirection = self.UP
        #end

        def turnRight(self) :
            self.nDirection = (self.nDirection + 1) % 4
        #end

        def direction(self) -> list[int,int]:
            return self.DIRECTIONS[self.nDirection]
        #end

        UP    = 0
        RIGHT = 1
        DOWN  = 2
        LEFT  = 3

        DIRECTIONS = [
            [ 0, -1], # UP
            [ 1,  0], # RIGHT
            [ 0,  1], # DOWN
            [-1,  0]  # LEFT
        ]

        nDirection = UP

    #end

    kMaze           = []
    kGuardPos       = None

    with open("input.txt", "r") as inputFile:

        kMaze      = [list(kInputLine.strip()) for kInputLine in inputFile.readlines()]
        kObstacles = set()

        for nRow, kRow in enumerate(kMaze) :

            if "^" in kRow :
                kGuardPos = (kRow.index("^"), nRow)
            #end

            if "#" in kRow :
                nIndex = -1
                while True :
                    try :
                        nIndex  = kRow.index("#", nIndex + 1)
                    except ValueError :
                        break
                    #end
                    kObstacles.add((nIndex, nRow))
                #end
            #end
        #end

    #end

    assert(kGuardPos is not None)

    def getRoute(kMaze : list[list[str]], kGuardPos : tuple[int,int], kGuardDirection : GuardDirection) -> list[tuple[int,int]] :

        kGuardVisit = set()
        kGuardVisit.add(kGuardPos)

        while True :

            kCurrentDirection = kGuardDirection.direction()
            kNextPos          = (kGuardPos[0] + kCurrentDirection[0], kGuardPos[1] + kCurrentDirection[1])

            if kNextPos[1] in range(len(kMaze)) and kNextPos[0] in range(len(kMaze[0])) :
                if kMaze[kNextPos[1]][kNextPos[0]] == "#" :
                    kGuardDirection.turnRight()
                    continue
                else :
                    kGuardPos = kNextPos
                    kGuardVisit.add(kGuardPos)
                #end
            else :
                break
            #end

        #end

        return kGuardVisit

    #end

    kGuardVisit = getRoute(kMaze, kGuardPos, GuardDirection())
    print(f"Part 1: {len(kGuardVisit)}")

    # Part Two... make loops.

    # First part, build a node list of all the points around each obstacle.
    kGuardCatchNodes = {}
    kGuardMoveNodes = {}

    def AddGuardNodes(kGuardNodes : dict, kMaze : list[list[str]], kGuardPos : tuple[int,int], nGuardDirection : int) :

        bInBounds = True

        # Create a Key for the direction
        kGuardKey = tuple([kGuardPos, nGuardDirection])
        if kGuardKey in kGuardNodes :
            return
        #end

        # Walk this direction until we hit a wall or leave the grid
        kGuardDirection = GuardDirection.DIRECTIONS[nGuardDirection]
        while True :

            kNextGuardPos = (kGuardPos[0] + kGuardDirection[0], kGuardPos[1] + kGuardDirection[1])

            if kNextGuardPos[1] in range(len(kMaze[0])) and kNextGuardPos[0] in range(len(kMaze)) :
                if kMaze[kNextGuardPos[1]][kNextGuardPos[0]] == "#" :
                    break
                #end
            else :
                bInBounds = False
                break
            #end

            kGuardPos = kNextGuardPos

        #end

        kGuardNodes[kGuardKey] = (kGuardPos, bInBounds)

    #end

    for kObstacle in kObstacles :

        nCol, nRow = kObstacle
        kWalkableNeighbours = []
        for kNeighbourDirection in [GuardDirection.UP, GuardDirection.RIGHT, GuardDirection.DOWN, GuardDirection.LEFT]:

            kNeighbour = GuardDirection.DIRECTIONS[kNeighbourDirection]

            nX = nCol + kNeighbour[0]
            nY = nRow + kNeighbour[1]

            if nY in range(len(kMaze)) and nX in range(len(kMaze[0])) :

                if kMaze[nY][nX] != "#" :

                    # Store the "Catch" Node (movement towards the obstruction) and the right turn from the catch node
                    nNeighbourDirectionReverse = (kNeighbourDirection + 2) % 4
                    kWalkableNeighbours.append([(nX, nY), [nNeighbourDirectionReverse, (nNeighbourDirectionReverse + 1) % 4]])

                #end

            #end

        #end

        # For each Direction
        for kWalkable in kWalkableNeighbours :

            assert(len(kWalkable[1]) == 2)
            AddGuardNodes(kGuardCatchNodes, kMaze, kWalkable[0], kWalkable[1][0])
            AddGuardNodes(kGuardMoveNodes, kMaze, kWalkable[0], kWalkable[1][1])

        #end

    #end

    # Get the first Colliding Node for the Start Position...
    # Note: Adding the start position to the actual collision nodes
    #       just introduces a world of pain!
    kStartNode = {}
    AddGuardNodes(kStartNode, kMaze, kGuardPos, GuardDirection.UP)
    kStartNode = list(kStartNode.values())[0]

    def manualMove(kGuardPos, kGuardDirection, kGuardCatchNodes) :

        # Turn Right, and keep moving until we're back in the Node List or we leave
        # the grid.
        kGuardDirection.turnRight()
        kGuardMove = kGuardDirection.direction()

        while True :

            kGuardKey = tuple([kGuardPos, kGuardDirection.nDirection])
            kNextPos = (kGuardPos[0] + kGuardMove[0], kGuardPos[1] + kGuardMove[1])

            if not kNextPos[0] in range(len(kMaze[0])) or not kNextPos[1] in range(len(kMaze)) :
                return False, None
            #end

            # Is this a valid key...
            if kGuardKey in kGuardCatchNodes :
                break
            #end

            kGuardPos = kNextPos

        #end

        kGuardDirection.turnRight()
        return True, kGuardPos

    #end

    def getLoop(kFirstNode : tuple[tuple[int,int],int], kNewObstacle : tuple[int,int], kGuardCatchNodes : dict, kGuardMoveNodes : dict, kGuardPos : tuple[int,int], kGuardDirection : GuardDirection) -> bool :

        kSeen      = set()
        bFirstNode = True

        while True :

            kGuardKey         = tuple([kGuardPos, kGuardDirection.nDirection])

            if kGuardKey in kSeen :
                return True
            #end
            kSeen.add(kGuardKey)

            kNextPos, bInBounds = kGuardMoveNodes[kGuardKey] if not bFirstNode else kFirstNode
            bFirstNode = False

            # Does the next position intersect with the new obstacle?
            # Note: One of the axis must match for an intersection, so we can use this to quickly discard
            #       obvious non-collisions with the new obstacle to help throughput.
            if kNewObstacle[0] in [kGuardPos[0], kNextPos[0]] or kNewObstacle[1] in [kGuardPos[1], kNextPos[1]] :

                if ((kNewObstacle[0] in range(min(kGuardPos[0], kNextPos[0]), max(kGuardPos[0], kNextPos[0]) + 1)) and
                    (kNewObstacle[1] in range(min(kGuardPos[1], kNextPos[1]), max(kGuardPos[1], kNextPos[1]) + 1))) :

                    # If we intersect, do a manual move until we're back in the known node lists or if we've left
                    # the grid.

                    # Set our position to adjacent to the new obstacle
                    kGuardMove = kGuardDirection.direction()
                    kGuardPos  = (kNewObstacle[0] - kGuardMove[0], kNewObstacle[1] - kGuardMove[1])

                    bInBounds, kGuardPos = manualMove(kGuardPos, kGuardDirection, kGuardCatchNodes)
                    if bInBounds is False :
                        return False
                    #end

                    continue

                #end

            #end

            if bInBounds is False :
                return False
            #end

            kGuardPos = kNextPos
            kGuardDirection.turnRight()

        #end

    #end

    # We only need to consider points in the route that the guard visited in part one, unless that point is
    # the start point.
    nLoopCount = 0

    for kNewObstacle in kGuardVisit :

        if kNewObstacle != kGuardPos :

            bLoop = getLoop(kStartNode, kNewObstacle, kGuardCatchNodes, kGuardMoveNodes, kGuardPos, GuardDirection())
            if bLoop :
                nLoopCount += 1
            #end

        #end

    #end

    print(f"Part 2: {nLoopCount}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end