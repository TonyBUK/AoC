import time
import sys

def main() :

    def printMapPartTwo(kRobot : list[int,int], kBoxes : dict[tuple[int,int, tuple[int,int]]], kWalls : set[int,int], WIDTH : int, HEIGHT : int, kDeltas : set[int, int]) :
        for Y in range(HEIGHT) :
            for X in range(WIDTH) :
                if (X,Y) == (kRobot[0], kRobot[1]) :
                    print("\033[32m", end="")
                    print("@", end="")
                    print("\033[0m", end="")
                    continue
                #end
                if kDeltas is not None :
                    if (X, Y) in kDeltas :
                        print("\033[31m", end="")
                    #end
                #end
                if (X,Y) in kBoxes :
                    kBox1 = (X,Y)
                    kBox2 = kBoxes[kBox1]
                    if kBox1[0] < kBox2[0] :
                        print("[", end="")
                    else :
                        print("]", end="")
                    #end
                elif (X,Y) in kWalls :
                    print("#", end="")
                else :
                    print(".", end="")
                #end
                if kDeltas is not None :
                    if (X, Y) in kDeltas :
                        print("\033[0m", end="")
                    #end
                #end
            #end
            print()
        #end
        print("====================================")
    #end

    # Directions
    UP    = 0
    RIGHT = 1
    DOWN  = 2
    LEFT  = 3
    DIRECTIONS = [
        ( 0, -1), # UP
        ( 1,  0), # RIGHT
        ( 0,  1), # DOWN
        (-1,  0)  # LEFT
    ]
    GRID_TO_DIRECTIONS = {
        "^" : UP,
        ">" : RIGHT,
        "v" : DOWN,
        "<" : LEFT
    }

    # For Debugging Only
    DIRECTION_TO_GRID = {
        v : k for k,v in GRID_TO_DIRECTIONS.items()
    }

    # Part 1 Box Movement as DFS
    def moveBoxPartOne(nDirection : int, kBox : list[int,int], kBoxes : set[int, int], kWalls : set[int, int]) :

        # Calculate the Box Position
        kNextBoxPosition = (kBox[0] + DIRECTIONS[nDirection][0],
                            kBox[1] + DIRECTIONS[nDirection][1])

        # If it's collided with a Wall, ignore the move
        if kNextBoxPosition in kWalls :
            return False
        #end

        # If its collided with a box, see if the next box can move, otherwise ignore the move
        if kNextBoxPosition in kBoxes :
            if False == moveBoxPartOne(nDirection, kNextBoxPosition, kBoxes, kWalls) :
                return False
            #end
        #end

        # Move the Box
        kBoxes.remove(kBox)
        kBoxes.add(kNextBoxPosition)

        # Indicate to any other box moves that the move is legal
        return True

    #end

    def moveRobotPartOne(nDirection : int, kRobot : list[int,int], kBoxes : set[int, int], kWalls : set[int, int]) :

        # Calculate the Robot Position
        kNextRobotPosition = (kRobot[0] + DIRECTIONS[nDirection][0],
                              kRobot[1] + DIRECTIONS[nDirection][1])

        # If the Robot has collided with a wall, ignore the move
        if kNextRobotPosition in kWalls :
            return
        #end

        # If the Robot has collided with a box, see if the box can move, otherwise ignore the move
        if kNextRobotPosition in kBoxes :
            if False == moveBoxPartOne(nDirection, kNextRobotPosition, kBoxes, kWalls) :
                return
            #end
        #end

        # Move the Robot
        kRobot[0] = kNextRobotPosition[0]
        kRobot[1] = kNextRobotPosition[1]

    #end

    # Part 2 Box Movement as DFS
    def moveBoxPartTwo(nDirection : int, kBoxes : list[list[int,int]], kBoxPairs : dict[tuple[int,int], tuple[int,int]], kWalls : set[int, int], kDeltasToAdd : set[int, int], kDeltasToRemove : set[int, int]) :

        # We should always have two boxes, since each box as 2 units wide
        assert(len(kBoxes) == 2)

        # Calculate the next position(s) and sort by X so that we have them ordered left to right
        kNextBoxPositions = sorted([(kBox[0] + DIRECTIONS[nDirection][0],
                                     kBox[1] + DIRECTIONS[nDirection][1]) for kBox in kBoxes])

        # Walls are Impossible Moves
        for kNextBoxPosition in kNextBoxPositions :
            if kNextBoxPosition in kWalls :
                return False
            #end
        #end

        # If we're moving Left, find any adjacent boxes to the left side and move it first
        if nDirection == LEFT :
            if kNextBoxPositions[0] in kBoxPairs :
                if False == moveBoxPartTwo(nDirection, [kNextBoxPositions[0], kBoxPairs[kNextBoxPositions[0]]], kBoxPairs, kWalls, kDeltasToAdd, kDeltasToRemove) :
                    return False
                #end
            #end
        # If we're moving Left, find any adjacent boxes to the right side and move it first
        elif nDirection == RIGHT :
            if kNextBoxPositions[1] in kBoxPairs :
                if False == moveBoxPartTwo(nDirection, [kNextBoxPositions[1], kBoxPairs[kNextBoxPositions[1]]], kBoxPairs, kWalls, kDeltasToAdd, kDeltasToRemove) :
                    return False
                #end
            #end
        # Moving Up/Down
        else :
            # As the boxes are 2 units wide, we will either collide with:
            #
            # 1. A Wall
            # 2. One Box
            # 3. Two Boxes

            # The problem is, it cascades and eventually one of the boxes might be blocked, so initially, check *all* boxes can move, but don't
            # actually move them, just buffer the moves to apply if all boxes can move.
            for kNextBoxPosition in kNextBoxPositions :
                if kNextBoxPosition in kBoxPairs :
                    if False == moveBoxPartTwo(nDirection, [kNextBoxPosition, kBoxPairs[kNextBoxPosition]], kBoxPairs, kWalls, kDeltasToAdd, kDeltasToRemove) :
                        return False
                    #end
                #end
            #end

        #end

        # Buffer the possible changes to make if all the boxes are moveable
        kDeltasToRemove.add(kBoxes[0])
        kDeltasToRemove.add(kBoxes[1])
        kDeltasToAdd.add((kNextBoxPositions[0], kNextBoxPositions[1]))

        return True

    #end

    def moveRobotPartTwo(nDirection : int, kRobot : list[int,int], kBoxPairs : dict[tuple[int,int], tuple[int,int]], kWalls : set[int, int], kDeltas : set[int, int]) :

        kDeltasToAdd    = set()
        kDeltasToRemove = set()

        # Calculate the Robot Position
        kNextRobotPosition = (kRobot[0] + DIRECTIONS[nDirection][0],
                              kRobot[1] + DIRECTIONS[nDirection][1])

        # If the Robot has collided with a wall, ignore the move
        if kNextRobotPosition in kWalls :
            return
        #end

        # If the Robot has collided with a box, see if the box can move, otherwise ignore the move
        if kNextRobotPosition in kBoxPairs :
            kNextBoxPositions = [kNextRobotPosition, kBoxPairs[kNextRobotPosition]]
            if False == moveBoxPartTwo(nDirection, kNextBoxPositions, kBoxPairs, kWalls, kDeltasToAdd, kDeltasToRemove) :
                return
            #end
        #end

        # Move the Robot
        kRobot[0] = kNextRobotPosition[0]
        kRobot[1] = kNextRobotPosition[1]

        # Move the Boxes
        for kDeltaToRemove in kDeltasToRemove :
            kBoxPairs.pop(kDeltaToRemove)
            if kDeltas is not None :
                kDeltas.add(kDeltaToRemove)
            #end
        #end
        for kDeltaToAdd in kDeltasToAdd :
            kBoxPairs[kDeltaToAdd[0]] = kDeltaToAdd[1]
            kBoxPairs[kDeltaToAdd[1]] = kDeltaToAdd[0]
            if kDeltas is not None :
                kDeltas.add(kDeltaToAdd[0])
                kDeltas.add(kDeltaToAdd[1])
            #end
        #end

    #end

    # For Debugging Only
    WIDTH  = 0
    HEIGHT = 0

    # Grid / Moves
    kWallsPartOne        = set()
    kWallsPartTwo        = set()
    kBoxesPartOne        = set()
    kBoxPairsPartTwo     = {}
    kMovesPartOne        = []
    kRobotPartOne        = None
    kRobotPartTwo        = None
    with open("../input.txt", "r") as inputFile:

        kPuzzleInput = inputFile.read().split("\n\n")

        # Parse the Grid
        assert(len(kPuzzleInput) >= 2)

        for Y,kLine in enumerate(kPuzzleInput[0].split("\n")) :
            HEIGHT += 1
            for X,K in enumerate(kLine) :
                if K == "#" :
                    kWallsPartOne.add((X,Y))
                    kWallsPartTwo.add(((X*2),Y))
                    kWallsPartTwo.add(((X*2)+1,Y))
                elif K == "." :
                    pass
                elif K == "O" :
                    kBoxesPartOne.add((X,Y))
                    kBoxLeft  = ((X*2),  Y)
                    kBoxRight = ((X*2)+1,Y)
                    kBoxPairsPartTwo[kBoxLeft]  = kBoxRight
                    kBoxPairsPartTwo[kBoxRight] = kBoxLeft
                elif K == "@" :
                    kRobotPartOne = [X,Y]
                    kRobotPartTwo = [X*2,Y]
                else :
                    assert(False)
                #end
            #end
            WIDTH = max(WIDTH, len(kLine))
        #end

        for kLine in kPuzzleInput[1].split("\n") :
            kMovesPartOne.append([GRID_TO_DIRECTIONS[k] for k in kLine.strip()])
        #end

    #end

    # Part 1 : One unit wide boxes
    for kMoveSequence in kMovesPartOne :
        for kMove in kMoveSequence :
            moveRobotPartOne(kMove, kRobotPartOne, kBoxesPartOne, kWallsPartOne)
        #end
    #end

    nPartOne = 0
    for kBox in kBoxesPartOne :
        nPartOne += (100 * kBox[1]) + kBox[0]
    #end

    print(f"Part 1: {nPartOne}")

    # Part 2 : One unit wide boxes
    for kMoveSequence in kMovesPartOne :
        for kMove in kMoveSequence :
#            kDeltas = set()
            moveRobotPartTwo(kMove, kRobotPartTwo, kBoxPairsPartTwo, kWallsPartTwo, None) # kDeltas)
#            if (len(kDeltas) > 0) :
#                printMapPartTwo(kRobotPartTwo, kBoxPairsPartTwo, kWallsPartTwo, WIDTH*2, HEIGHT, kDeltas)
#            #end
        #end
    #end

    # Count the Box GPS Co-Ordinates  ensuring we only count each box once and only count the left
    # side of the box
    nPartTwo = 0
    kSeen = set()
    for kBox in kBoxPairsPartTwo :

        if kBox in kSeen :
            continue
        #end

        kBox1 = kBox
        kBox2 = kBoxPairsPartTwo[kBox]

        kBox = kBox1 if kBox1[0] < kBox2[0] else kBox2

        nPartTwo += (100 * kBox[1]) + kBox[0]

        kSeen.add(kBox1)
        kSeen.add(kBox2)

    #end

    print(f"Part 2: {nPartTwo}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end