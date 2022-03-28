import time
import sys

def main() :

    # Read/Clean the Input File to extract the Moves
    moves = []
    with open("../input.txt", "r") as inputFile:

        def extractMove(token) :
            return {"turn" : token[0], "distance" : int(token[1:])}
        #end

        moves = [extractMove(token) for line in inputFile.readlines() for token in line.strip().split(", ")]

    #end

    # Map N/E/S/W and how Left/Right Turns change directions
    DIRECTION_CHANGE = {
        "N" : {"L" : "W", "R" : "E"},
        "E" : {"L" : "N", "R" : "S"},
        "S" : {"L" : "E", "R" : "W"},
        "W" : {"L" : "S", "R" : "N"}
    }

    # Map N/E/S/W to Vectors
    DIRECTION = {
        "N" : [ 0,  1],
        "E" : [ 1,  0],
        "S" : [ 0, -1],
        "W" : [-1,  0]
    }

    def manhattenDistance(position) :
        return sum([abs(x) for x in position])
    #end

    def processMoves(moves, finishOnFirstRepetition) :
        
        orientation = "N"
        position    = [0, 0]

        # If we're checking repetitions, we really only want to move one block at a time,
        # otherwise we're intersection testing, which is a bit much for Day 1
        if finishOnFirstRepetition :

            visited = set()
            visited.add(tuple(position))
            for move in moves :
                orientation   = DIRECTION_CHANGE[orientation][move["turn"]]
                for _ in range(move["distance"]) :
                    position = [N+M for N,M in zip(position, DIRECTION[orientation])]
                    positionTuple = tuple(position)
                    if positionTuple in visited :
                        return position
                    #end
                    visited.add(positionTuple)
                #end
            #end

        # Otherwise... we can be more efficient in our movement, moving multiple blocks at a time
        # since we only care where we end up, not where we've been
        else :

            for move in moves :
                orientation   = DIRECTION_CHANGE[orientation][move["turn"]]
                deltaPosition = [(N*move["distance"]) for N in DIRECTION[orientation]]
                position = [N+M for N,M in zip(position, deltaPosition)]
            #end
        #end

        return position

    #end

    print(f"Part 1: {manhattenDistance(processMoves(moves, False))}")
    print(f"Part 2: {manhattenDistance(processMoves(moves, True))}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
