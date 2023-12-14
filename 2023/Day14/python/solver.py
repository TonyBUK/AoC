import time
import sys

def main() :

    kDish = []
    with open("../input.txt", "r") as inputFile:
        kDish = [list(k.replace("\n", "")) for k in inputFile.readlines()]
    #end

    # Cardinal Direction Constants
    NORTH = 0
    EAST  = 1
    SOUTH = 2
    WEST  = 3

    def tiltDish(kDirections, kDish, nDishWidth, nDishHeight) :

        kAxis = [nDishHeight, nDishWidth]

        PRIMARY_AXIS = {
            NORTH : 0, # Y
            EAST  : 1, # X
            SOUTH : 0, # Y
            WEST  : 1  # X
        }

        DIRECTION_IN_AXIS = {
            NORTH : -1,
            EAST  : 1,
            SOUTH : 1,
            WEST  : -1
        }

        for kDirection in kDirections :
        
            bReversed               = DIRECTION_IN_AXIS[kDirection] == 1
            nPrimaryAxisReference   = PRIMARY_AXIS[kDirection]
            nSecondaryAxisReference = 1 - nPrimaryAxisReference
            nDirectionInAxis        = -DIRECTION_IN_AXIS[kDirection]

            for nSecondaryAxis in range(kAxis[nSecondaryAxisReference]) :

                kPrimaryAxis = range(kAxis[nPrimaryAxisReference])
                if bReversed :
                    kPrimaryAxis = reversed(kPrimaryAxis)
                #end

                if bReversed :
                    nLastUnobstructedPos = kAxis[nPrimaryAxisReference] - 1
                else :
                    nLastUnobstructedPos = 0
                #end

                for nPrimaryAxis in kPrimaryAxis :

                    kLocation = [0, 0]
                    kLocation[nPrimaryAxisReference]   = nPrimaryAxis
                    kLocation[nSecondaryAxisReference] = nSecondaryAxis

                    if kDish[kLocation[0]][kLocation[1]] == "O" :

                        if ((not bReversed and (nPrimaryAxis > nLastUnobstructedPos)) or
                            (    bReversed and (nPrimaryAxis < nLastUnobstructedPos))) :

                            kLastUnobstructedLocation = [0, 0]
                            kLastUnobstructedLocation[nPrimaryAxisReference]   = nLastUnobstructedPos
                            kLastUnobstructedLocation[nSecondaryAxisReference] = nSecondaryAxis

                            kDish[kLastUnobstructedLocation[0]][kLastUnobstructedLocation[1]] = "O"
                            kDish[kLocation[0]][kLocation[1]]                                 = "."
 
                            nLastUnobstructedPos += nDirectionInAxis

                            continue

                        #end
                    
                    #end

                    if kDish[kLocation[0]][kLocation[1]] != "." :

                        nLastUnobstructedPos = kLocation[nPrimaryAxisReference] + nDirectionInAxis

                    #end

                #end

            #end

        #end

    #end

    def calculateLoad(kDish, nDishHeight) :
        return sum(nDishHeight - Y for Y in range(nDishHeight) for X in range(len(kDish[Y])) if kDish[Y][X] == "O")
    #end

    DISH_HEIGHT = len(kDish)
    DISH_WIDTH  = len(kDish[0])

    kPartOneDish = [[kCol for kCol in kRow] for kRow in kDish]
    tiltDish([NORTH], kPartOneDish, DISH_WIDTH, DISH_HEIGHT)

    print(f"Part 1: {calculateLoad(kPartOneDish, DISH_HEIGHT)}")

    TOTAL_CYCLES = 1000000000
    kStates      = []
    nCycle       = 0
    nLoopLength  = None
    bLoopFound   = False

    while nCycle < TOTAL_CYCLES :

        tiltDish([NORTH, WEST, SOUTH, EAST], kDish, DISH_WIDTH, DISH_HEIGHT)

        if bLoopFound == False :

            if kDish in kStates :

                # Calculate the Remaining Cycles
                nLoopStart          = kStates.index(kDish)
                nLoopLength         = nCycle - nLoopStart
                nRemainingCycles    = (TOTAL_CYCLES - nCycle) % nLoopLength

                # Should always be true, but better safe...
                if (nLoopStart + nRemainingCycles - 1) < len(kStates) :
                    kDish = kStates[nLoopStart + nRemainingCycles - 1]
                    break
                #end

                # If for some reason we're here, just play back the remaining cycles...
                bLoopFound       = True
                nCycle           = TOTAL_CYCLES - nRemainingCycles

            else :

                kStates.append([[kCol for kCol in kRow] for kRow in kDish])

            #end

        #end

        nCycle += 1

    #end

    print(f"Part 2: {calculateLoad(kDish, DISH_HEIGHT)}")

#end
if __name__ == "__main__" :
   startTime = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end