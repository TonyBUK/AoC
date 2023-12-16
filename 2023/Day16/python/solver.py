import time
import sys

DIRECTION_UP    = 1
DIRECTION_RIGHT = 2
DIRECTION_DOWN  = 4
DIRECTION_LEFT  = 8

DIRECTIONS = {
    DIRECTION_UP    : (-1, 0),
    DIRECTION_RIGHT : (0, 1),
    DIRECTION_DOWN  : (1, 0),
    DIRECTION_LEFT  : (0, -1)
}

DIRECTION_Y = 0
DIRECTION_X = 1

ACTION_UP               = DIRECTION_UP
ACTION_RIGHT            = DIRECTION_RIGHT
ACTION_DOWN             = DIRECTION_DOWN
ACTION_LEFT             = DIRECTION_LEFT
ACTION_SPLIT_UP_DOWN    = 16
ACTION_SPLIT_LEFT_RIGHT = 32

DIRECTION_ACTION = {
    "\\" : {DIRECTION_UP : ACTION_LEFT,             DIRECTION_RIGHT : ACTION_DOWN,          DIRECTION_DOWN : ACTION_RIGHT,              DIRECTION_LEFT : ACTION_UP},
    "/"  : {DIRECTION_UP : ACTION_RIGHT,            DIRECTION_RIGHT : ACTION_UP,            DIRECTION_DOWN : ACTION_LEFT,               DIRECTION_LEFT : ACTION_DOWN},
    "|"  : {DIRECTION_UP : ACTION_UP,               DIRECTION_RIGHT : ACTION_SPLIT_UP_DOWN, DIRECTION_DOWN : ACTION_DOWN,               DIRECTION_LEFT : ACTION_SPLIT_UP_DOWN},
    "-"  : {DIRECTION_UP : ACTION_SPLIT_LEFT_RIGHT, DIRECTION_RIGHT : ACTION_RIGHT,         DIRECTION_DOWN : ACTION_SPLIT_LEFT_RIGHT,   DIRECTION_LEFT : ACTION_LEFT}
}

QUEUE_Y         = 0
QUEUE_X         = 1
QUEUE_DIRECTION = 2

def main() :

    kMirrorGrid = []
    with open("../input.txt", "r") as inputFile:
        kMirrorGrid = [list(k.replace("\n", "").strip()) for k in inputFile.readlines()]
    #end

    def processMirrors(kMirrorGrid, kInitialLightBeam = None, bVisualise = False) :

        # Cache the Mirror Dimensions
        MIRROR_HEIGHT = len(kMirrorGrid)
        MIRROR_WIDTH  = len(kMirrorGrid[0])

        # Validate/Add Light Beams as Requested...
        def addBeamForProcessing(nLightBeamY, nLightBeamX, nLightBeamDirection, kLightBeams, kCache) :

            # Check if the Light Beam is in the Grid
            if (nLightBeamY >= 0) and (nLightBeamY < MIRROR_HEIGHT) and (nLightBeamX >= 0) and (nLightBeamX < MIRROR_WIDTH) :

                # Check if the Light Beam is in the Cache
                # If so, then we've hit a loop, and can stop
                if 0 == (kCache[nLightBeamY][nLightBeamX] & nLightBeamDirection) :

                    # Add the Light Beam to the List of mirrors to Process
                    kLightBeams.append((nLightBeamY, nLightBeamX, nLightBeamDirection))

                #end

            #end

        #end

        # Initialise the Initial Light Beam if not Specified
        if None == kInitialLightBeam :
            kInitialLightBeam = (0, 0, DIRECTION_RIGHT)
        #end

        # Initialise the Cache and the Light Beams to Process
        kCache      = [[0 for _ in range(len(kMirrorGrid[0]))] for _ in range(len(kMirrorGrid))]
        kLightBeams = [kInitialLightBeam]

        # Initialise the Energised Grid
        kEnergisedGrid = [[0 for _ in range(len(kMirrorGrid[0]))] for _ in range(len(kMirrorGrid))]

        # While there are Light Beams to Process
        while len(kLightBeams) > 0 :

            # Get the Light Beam
            kLightBeam = kLightBeams.pop()

            # Indicate the Grid is now Energised
            kEnergisedGrid[kLightBeam[QUEUE_Y]][kLightBeam[QUEUE_X]] = 1

            # Add the Light Beam to the Cache
            kCache[kLightBeam[QUEUE_Y]][kLightBeam[QUEUE_X]] |= kLightBeam[QUEUE_DIRECTION]

            # Calculate the Next Position
            kMirror = kMirrorGrid[kLightBeam[QUEUE_Y]][kLightBeam[QUEUE_X]]
            if kMirror != "." :

                # Get the Mirror Action (based on the Mirror Type and the Light Beam Direction)
                kMirrorAction = DIRECTION_ACTION[kMirror][kLightBeam[QUEUE_DIRECTION]]

                # Check if the Mirror Action is a simple Direction Change
                if kMirrorAction not in [ACTION_SPLIT_UP_DOWN, ACTION_SPLIT_LEFT_RIGHT] :

                    # Calculate the Next Position and Add to the Beam List for Processing
                    addBeamForProcessing(kLightBeam[QUEUE_Y] + DIRECTIONS[kMirrorAction][DIRECTION_Y], kLightBeam[QUEUE_X] + DIRECTIONS[kMirrorAction][DIRECTION_X], kMirrorAction, kLightBeams, kCache)

                # Otherwise it's a Split
                elif kMirrorAction == ACTION_SPLIT_UP_DOWN :

                    # Calculate the Next Position and Add to the Beam List for Processing
                    addBeamForProcessing(kLightBeam[QUEUE_Y] + DIRECTIONS[ACTION_UP][DIRECTION_Y],   kLightBeam[QUEUE_X] + DIRECTIONS[ACTION_UP][DIRECTION_X],   ACTION_UP,   kLightBeams, kCache)
                    addBeamForProcessing(kLightBeam[QUEUE_Y] + DIRECTIONS[ACTION_DOWN][DIRECTION_Y], kLightBeam[QUEUE_X] + DIRECTIONS[ACTION_DOWN][DIRECTION_X], ACTION_DOWN, kLightBeams, kCache)

                else :

                    # Calculate the Next Position and Add to the Beam List for Processing
                    addBeamForProcessing(kLightBeam[QUEUE_Y] + DIRECTIONS[ACTION_LEFT][DIRECTION_Y],  kLightBeam[QUEUE_X] + DIRECTIONS[ACTION_LEFT][DIRECTION_X],  ACTION_LEFT,  kLightBeams, kCache)
                    addBeamForProcessing(kLightBeam[QUEUE_Y] + DIRECTIONS[ACTION_RIGHT][DIRECTION_Y], kLightBeam[QUEUE_X] + DIRECTIONS[ACTION_RIGHT][DIRECTION_X], ACTION_RIGHT, kLightBeams, kCache)

                #end

            else :

                # Keep Moving in the Current Direction
                addBeamForProcessing(kLightBeam[QUEUE_Y] + DIRECTIONS[kLightBeam[QUEUE_DIRECTION]][DIRECTION_Y], kLightBeam[QUEUE_X] + DIRECTIONS[kLightBeam[QUEUE_DIRECTION]][DIRECTION_X], kLightBeam[QUEUE_DIRECTION], kLightBeams, kCache)

            #end

        #end

        # Optionaly Visualiser
        if bVisualise :

            for nRow in range(len(kEnergisedGrid)) :

                for nCol in range(len(kEnergisedGrid[nRow])) :

                    # If it's a Mirror, print the Mirror
                    if kMirrorGrid[nRow][nCol] != "." :
                        print(kMirrorGrid[nRow][nCol], end="")
                        continue
                    #end

                    # If it's Energised, determine the Direction(s)
                    kCacheMatches = []
                    for kDirection in [DIRECTION_UP, DIRECTION_RIGHT, DIRECTION_DOWN, DIRECTION_LEFT] :
                        if 0 != (kCache[nRow][nCol] & kDirection) :
                            kCacheMatches.append(kDirection)
                        #end
                    #end

                    # If there's more than one direction, print the number of different directions                            
                    if len(kCacheMatches) > 1 :
                        # Shouldn't be more than 4, as it's direction based, not
                        # frequency.
                        print(f"{len(kCacheMatches)}", end="")
                    # Otherwise print the Direction
                    elif len(kCacheMatches) == 1 :
                        DIRECITONS_CHAR = {
                            DIRECTION_UP    : "^",
                            DIRECTION_RIGHT : ">",
                            DIRECTION_DOWN  : "v",
                            DIRECTION_LEFT  : "<"
                        }
                        print(f"{DIRECITONS_CHAR[kCacheMatches[0]]}", end="")
                    # Otherwise, it's npt energised, so print a dot
                    else :
                        print(".", end="")
                    #end

                #end
                        
                print("")

            #end

        #end
                
        return sum([sum(kEnergisedGrid[nRow]) for nRow in range(len(kEnergisedGrid))])

    #end

    # Part 1: Top Left Moving Right
    nPartOneEnergy = processMirrors(kMirrorGrid)
    print(f"Part 1: {nPartOneEnergy}")

    # Part 2: Traverse the Edges and find the Best Position
    nMaxEnergy = nPartOneEnergy
    for nRow in range(len(kMirrorGrid)) :
        for kDirection,nCol in zip([DIRECTION_RIGHT, DIRECTION_LEFT], [0, len(kMirrorGrid[nRow]) - 1]) :

            # Skip Part One's Solution
            if kDirection == DIRECTION_RIGHT and nRow == 0 and nCol == 0 : continue

            nEnergy = processMirrors(kMirrorGrid, [nRow, nCol, kDirection])
            if nEnergy > nMaxEnergy :
                nMaxEnergy    = nEnergy
            #end
        #end
    #end

    for nCol in range(len(kMirrorGrid[0])) :
        for kDirection,nRow in zip([DIRECTION_DOWN, DIRECTION_UP], [0, len(kMirrorGrid) - 1]) :
            nEnergy = processMirrors(kMirrorGrid, [nRow, nCol, kDirection])
            if nEnergy > nMaxEnergy :
                nMaxEnergy = nEnergy
            #end
        #end

    print(f"Part 2: {nMaxEnergy}")

#end

if __name__ == "__main__" :
   startTime = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end