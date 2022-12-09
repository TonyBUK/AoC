import time
import sys
 
def main() :
 
    with open("../input.txt", "r") as inputFile:

        VECTORS = {
            "R" : [ 1,  0],
            "L" : [-1,  0],
            "U" : [ 0, -1],
            "D" : [ 0,  1]}

        def PrintUniquePositions(kUniquePositions) :

            MINX = min([POS[0] for POS in kUniquePositions])
            MAXX = max([POS[0] for POS in kUniquePositions])
            MINY = min([POS[1] for POS in kUniquePositions])
            MAXY = max([POS[1] for POS in kUniquePositions])

            for Y in range(MINY, MAXY + 1) :
                kString = ""
                for X in range(MINX, MAXX+1) :
                    if tuple([X, Y]) in kUniquePositions :
                        kString += "#"
                    else :
                        kString += "."
                #end
                print(kString)
            #end

        #end

        GLOBAL_MINX = 0
        GLOBAL_MAXX = 0
        GLOBAL_MINY = 0
        GLOBAL_MAXY = 0

        def PrintRope(kPositions) :

            nonlocal GLOBAL_MINX
            nonlocal GLOBAL_MAXX
            nonlocal GLOBAL_MINY
            nonlocal GLOBAL_MAXY

            MINX = min(GLOBAL_MINX, min([POS[0] for POS in kPositions]))
            MAXX = max(GLOBAL_MAXX, max([POS[0] for POS in kPositions]))
            MINY = min(GLOBAL_MINY, min([POS[1] for POS in kPositions]))
            MAXY = max(GLOBAL_MAXY, max([POS[1] for POS in kPositions]))

            GLOBAL_MINX = min(GLOBAL_MINX, MINX)
            GLOBAL_MAXX = max(GLOBAL_MAXX, MAXX)
            GLOBAL_MINY = min(GLOBAL_MINY, MINY)
            GLOBAL_MAXY = max(GLOBAL_MAXY, MAXY)

            for Y in range(MINY, MAXY + 1) :
                kString = ""
                for X in range(MINX, MAXX+1) :
                    if [X, Y] in kPositions :
                        kString += f"{kPositions.index([X, Y])}"
                    elif [0, 0] == [X, Y]:
                        kString += "s"
                    else :
                        kString += "."
                #end
                print(kString)
            #end

        #end

        def Adjacent(kHead, kTail) :
            if ((abs(kHead[0] - kTail[0]) <= 1) and
                (abs(kHead[1] - kTail[1]) <= 1)) :
                return True
            #end
            return False
        #end

        def SplitMove(kMove) :
            return [kMove[0], int(kMove[1])]
        #end

        # Extract the Moves
        kMoves              = [SplitMove(kMove.strip().split(" ")) for kMove in inputFile.readlines()]

        ##########
        # Part 1 #
        ##########

        kHead = [0, 0]
        kTail = [0, 0]

        kUniquePositionsPart1 = set()
        kUniquePositionsPart1.add(tuple(kTail))

        for kMove in kMoves :

            kVector = VECTORS[kMove[0]]

            for _ in range(kMove[1]) :

                kHead[0] += kVector[0]
                kHead[1] += kVector[1]

                if True == Adjacent(kHead, kTail) : continue

                # Calculate the Direction of Travel Needed between the Head and Tail
                # We get this by computer a vector limited to 1 in each axis in the direction of
                # the delta.
                kTailVector = [min(1, max(-1, kHead[0] - kTail[0])),
                               min(1, max(-1, kHead[1] - kTail[1]))]

                # Move the Tail
                kTail[0] += kTailVector[0]
                kTail[1] += kTailVector[1]

                kUniquePositionsPart1.add(tuple(kTail))

            #end

        #end

        print(f"Part 1: {len(kUniquePositionsPart1)}")

        ##########
        # Part 2 #
        ##########

        # This will store the Head as well
        kKnots = [[0, 0] for _ in range(10)]

        kUniquePositionsPart2 = set()
        kUniquePositionsPart2.add(tuple(kKnots[-1]))

        for kMove in kMoves :

            kVector     = VECTORS[kMove[0]]

            for _ in range(kMove[1]) :

                # Move the Head
                kKnots[0][0] += kVector[0]
                kKnots[0][1] += kVector[1]

                for i in range(1, len(kKnots)) :

                    if Adjacent(kKnots[i-1], kKnots[i]) : continue

                    # Calculate the Direction of Travel Needed between the Current and Previous Knot
                    # We get this by computer a vector limited to 1 in each axis in the direction of
                    # the delta.
                    kTailVector = [min(1, max(-1, kKnots[i-1][0] - kKnots[i][0])),
                                   min(1, max(-1, kKnots[i-1][1] - kKnots[i][1]))]

                    # Move the Knot
                    kKnots[i][0] += kTailVector[0]
                    kKnots[i][1] += kTailVector[1]

                #end

                kUniquePositionsPart2.add(tuple(kKnots[-1]))

            #end

        #end

#        PrintUniquePositions(kUniquePositionsPart2)
        print(f"Part 2: {len(kUniquePositionsPart2)}")

    #end
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end