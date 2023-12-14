import time
import sys

def main() :

   kDish = []
   with open("input.txt", "r") as inputFile:
       kDish = [list(k.replace("\n", "")) for k in inputFile.readlines()]
   #end

   # Convert the Dish into a list of Rock Positions, both Rounded and Cubed
   kRoundedRocks = set()
   kCubedRocks   = set()
   for Y in range(len(kDish)) :
       for X in range(len(kDish[Y])) :
           if kDish[Y][X] == "#" :
               kCubedRocks.add((Y,X))
           elif kDish[Y][X] == "O" :
               kRoundedRocks.add((Y,X))
           elif kDish[Y][X] != "." :
               assert(False)
           #end
       #end
   #end

   # Cardinal Direction Constants
   NORTH = 0
   EAST  = 1
   SOUTH = 2
   WEST  = 3

   def tiltDish(kDirections, kRoundedRocks, kCubedRocks, nDishWidth, nDishHeight) :

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

       kOriginalRoundedRocksReference = kRoundedRocks

       # For Each Direction to Tilt
       for kDirection in kDirections :

           # Buffer all Data that will be constant for this tilt direction
           nPrimaryAxis     = PRIMARY_AXIS[kDirection]
           nDirectionInAxis = DIRECTION_IN_AXIS[kDirection]
           nMaxSizeInAxis   = kAxis[nPrimaryAxis]

           # Keep cycling through all of the rocks until we can't move any more
           bAnyRocksMoved   = True
           while True == bAnyRocksMoved :

               kProcessedRocks = set()
               bAnyRocksMoved  = False

               # Cycle through each rock
               for kLocation in kRoundedRocks :

                   # Buffer the Location
                   kCheckedLocation  = list(kLocation)
                   kPreviousLocation = kLocation

                   # Keep moving the rock in the same direction until it collides
                   while True :

                       kCheckedLocation[nPrimaryAxis] += nDirectionInAxis
                       kCheckedLocationTuple           = tuple(kCheckedLocation)

                       # Check for Edges
                       if (kCheckedLocation[nPrimaryAxis]) < 0 or (kCheckedLocation[nPrimaryAxis] >= nMaxSizeInAxis) :

                           kProcessedRocks.add(kPreviousLocation)
                           break

                       # Cubed Rocks
                       elif kCheckedLocationTuple in kCubedRocks :

                           kProcessedRocks.add(kPreviousLocation)
                           break

                       # Already Moved Rocks
                       elif kCheckedLocationTuple in kProcessedRocks :

                           kProcessedRocks.add(kPreviousLocation)
                           break

                       # Unmoved Rocks
                       elif not (kCheckedLocationTuple in kRoundedRocks) :

                           # Keep Moving until we can't
                           bAnyRocksMoved    = True

                           # Buffer the Previous Location
                           kPreviousLocation = kCheckedLocationTuple

                       #end

                   #end

               #end

               kRoundedRocks   = kProcessedRocks

           #end

       #end

       kOriginalRoundedRocksReference.clear()
       kOriginalRoundedRocksReference.update(kProcessedRocks)

   #end

   def calculateLoad(kRoundedRocks, nDishHeight) :
       return sum([nDishHeight - k[0] for k in kRoundedRocks])
   #end

   DISH_HEIGHT = len(kDish)
   DISH_WIDTH  = len(kDish[0])

   kPartOneRoundedRocks = set(k for k in kRoundedRocks)
   tiltDish([NORTH], kPartOneRoundedRocks, kCubedRocks, DISH_WIDTH, DISH_HEIGHT)

   print(f"Part 1: {calculateLoad(kPartOneRoundedRocks, DISH_HEIGHT)}")

   TOTAL_CYCLES = 1000000000
   kStates      = []
   nCycle       = 0
   nLoopLength  = None
   bLoopFound   = False

   while nCycle < TOTAL_CYCLES :

       tiltDish([NORTH, WEST, SOUTH, EAST], kRoundedRocks, kCubedRocks, DISH_WIDTH, DISH_HEIGHT)

       if bLoopFound == False :

           if kRoundedRocks in kStates :

                # Calculate the Remaining Cycles
                nLoopStart          = kStates.index(kRoundedRocks)
                nLoopLength         = nCycle - nLoopStart
                nRemainingCycles    = (TOTAL_CYCLES - nCycle) % nLoopLength

                # Should always be true, but better safe...
                if (nLoopStart + nRemainingCycles - 1) < len(kStates) :
                    kRoundedRocks = kStates[nLoopStart + nRemainingCycles - 1]
                    break
                #end

                # If for some reason we're here, just play back the remaining cycles...
                bLoopFound       = True
                nCycle           = TOTAL_CYCLES - nRemainingCycles

           else :

               kStates.append(set(k for k in kRoundedRocks))

           #end

       #end

       nCycle += 1

   #end

   print(f"Part 2: {calculateLoad(kRoundedRocks, DISH_HEIGHT)}")

#end
if __name__ == "__main__" :
   startTime = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end