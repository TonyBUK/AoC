import time
import sys

def main() :

    # Read/Clean the Input File to extract the Moves
    moves = []
    with open("../input.txt", "r") as inputFile:
        moves = [line.strip() for line in inputFile.readlines()]
    #end

    # Standard Keypad
    # Note: All positions are valid
    STANDARD_KEYPAD = [[1,2,3],
                       [4,5,6],
                       [7,8,9]]

    # Bathroom Keypad
    # Note: None is used to represent invalid positions
    BATHROOM_KEYPAD = [[None, None,    1, None, None],
                       [None,    2,    3,    4, None],
                       [   5,    6,    7,    8,    9],
                       [None,  'A',  'B',  'C', None],
                       [None, None,  'D', None, None]]

    # Map a Movement to a Position change
    # Note: -1 is up because positive changes to the "Y" axis in an array go downwards
    DIRECTION_VECTORS = {"U" : [ 0, -1],
                         "D" : [ 0,  1],
                         "L" : [-1,  0],
                         "R" : [ 1,  0]}

    def getPasscode(keypad, moves, start) :

        position = [0,0]

        # Find the Start Position, and extract the Height/Width of the Keypad
        found    = False
        HEIGHT   = len(keypad)
        for Y,ROW in enumerate(keypad) :
            WIDTH = len(ROW)
            for X,C in enumerate(ROW) :
                if start == C :
                    position = [Y,X]
                    found    = True
                    break
                #end
            #end
            if found : break
        #end

        # Make sure we actually found the start position...
        assert(found)

        # Initialise the Passcode
        passcode = ""

        # For each set of moves...
        for currentMoveset in moves :

            # Process all moves in the current set
            for currentMove in currentMoveset :

                # Store the potential new position
                candidatePosition = [N+M for N,M in zip(position, DIRECTION_VECTORS[currentMove])]
                
                # Ensure the new position is on the Keypad Grid
                candidatePosition[0] = max(min(WIDTH-1,  candidatePosition[0]), 0)
                candidatePosition[1] = max(min(HEIGHT-1, candidatePosition[1]), 0)
                
                # Ensure the new position is on the Keypad Itself
                if keypad[candidatePosition[1]][candidatePosition[0]] != None :
                    position = candidatePosition
                #end

            #end

            # Add the current keypad value to the Passcode
            passcode += str(keypad[position[1]][position[0]])

        #end

        return passcode

    #end

    print(f"Part 1: {getPasscode(STANDARD_KEYPAD, moves, 5)}")
    print(f"Part 2: {getPasscode(BATHROOM_KEYPAD, moves, 5)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
