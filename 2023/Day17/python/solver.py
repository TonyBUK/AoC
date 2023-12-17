import time
import sys
import math

DIRECTION_UP    = 0
DIRECTION_RIGHT = 1
DIRECTION_DOWN  = 2
DIRECTION_LEFT  = 3

TURN = {
    DIRECTION_UP    : [DIRECTION_LEFT,  DIRECTION_RIGHT],
    DIRECTION_RIGHT : [DIRECTION_UP,    DIRECTION_DOWN],
    DIRECTION_DOWN  : [DIRECTION_RIGHT, DIRECTION_LEFT],
    DIRECTION_LEFT  : [DIRECTION_DOWN,  DIRECTION_UP]
}

DIRECTIONS = {
    DIRECTION_UP    : (-1,  0),
    DIRECTION_RIGHT : ( 0,  1),
    DIRECTION_DOWN  : ( 1,  0),
    DIRECTION_LEFT  : ( 0, -1)
}

QUEUE_TOTAL_HEAT         = 0
QUEUE_CACHE_DATA         = 1
QUEUE_Y                  = 0
QUEUE_X                  = 1
QUEUE_DIRECTION          = 2
QUEUE_CONSECTUVIVE_MOVES = 3

def main() :

    kHeatLossMap = []
    with open("../input.txt", "r") as inputFile:
        kHeatLossMap = [[int(n) for n in k.replace("\n", "").strip()] for k in inputFile.readlines()]
    #end

    def processCrucible(kHeatLossMap, MIN_MOVES_BEFORE_TURNING, MAX_MOVES_BEFORE_TURNING) :

        START_X         = 0
        START_Y         = 0

        kCache          = {}
        kQueue          = [[0, (START_Y, START_X, DIRECTION_RIGHT, 0)],
                           [0, (START_Y, START_X, DIRECTION_DOWN,  0)]]
        kSecondaryQueue = []
        nBestHeatLoss   = math.inf

        TARGET_VALID_RANGE_Y = range(len(kHeatLossMap))
        TARGET_VALID_RANGE_X = range(len(kHeatLossMap[0]))
        TARGET_Y             = TARGET_VALID_RANGE_Y.stop - 1
        TARGET_X             = TARGET_VALID_RANGE_X.stop - 1

        while (len(kQueue) > 0) :

            # Get the next item in the queue
            kCurrent = kQueue.pop()

            while True :

                # Detect if we're now worse than the best solution
                if kCurrent[QUEUE_TOTAL_HEAT] >= nBestHeatLoss :
                    # In theory because the queue is sorted, it isn't going to get any better...
                    kQueue.clear()
                    break
                #end

                # Detect if this is the best solution
                if (kCurrent[QUEUE_CACHE_DATA][QUEUE_Y] == TARGET_Y) and (kCurrent[QUEUE_CACHE_DATA][QUEUE_X] == TARGET_X) :
                    if kCurrent[QUEUE_CACHE_DATA][QUEUE_CONSECTUVIVE_MOVES] >= MIN_MOVES_BEFORE_TURNING :
                        nBestHeatLoss = min(nBestHeatLoss, kCurrent[QUEUE_TOTAL_HEAT])
                        break
                    #end
                #end

                # Detect if we've seen this before...
                if kCurrent[QUEUE_CACHE_DATA] not in kCache :
                    kCache[kCurrent[QUEUE_CACHE_DATA]] = kCurrent[QUEUE_TOTAL_HEAT]
                elif kCache[kCurrent[QUEUE_CACHE_DATA]] > kCurrent[QUEUE_TOTAL_HEAT] :
                    kCache[kCurrent[QUEUE_CACHE_DATA]] = kCurrent[QUEUE_TOTAL_HEAT]
                else :
                    break
                #end

                # Add the Permutations to the Queue

                # Try to keep moving in the same direction if allowed
                if kCurrent[QUEUE_CACHE_DATA][QUEUE_CONSECTUVIVE_MOVES] < MAX_MOVES_BEFORE_TURNING :

                    # Try to move forward
                    nNextY = kCurrent[QUEUE_CACHE_DATA][QUEUE_Y] + DIRECTIONS[kCurrent[QUEUE_CACHE_DATA][QUEUE_DIRECTION]][0]
                    nNextX = kCurrent[QUEUE_CACHE_DATA][QUEUE_X] + DIRECTIONS[kCurrent[QUEUE_CACHE_DATA][QUEUE_DIRECTION]][1]

                    # Detect if we're still in the map
                    if (nNextY in TARGET_VALID_RANGE_Y) and (nNextX in TARGET_VALID_RANGE_X) :

                        # Add the next move to the queue
                        kSecondaryQueue.append([kCurrent[QUEUE_TOTAL_HEAT] + kHeatLossMap[nNextY][nNextX], (nNextY, nNextX, kCurrent[QUEUE_CACHE_DATA][QUEUE_DIRECTION], kCurrent[QUEUE_CACHE_DATA][QUEUE_CONSECTUVIVE_MOVES] + 1)])

                    #end

                #end

                # Try to turn if allowed
                if kCurrent[QUEUE_CACHE_DATA][QUEUE_CONSECTUVIVE_MOVES] >= MIN_MOVES_BEFORE_TURNING :

                    for kDirection in TURN[kCurrent[QUEUE_CACHE_DATA][QUEUE_DIRECTION]] :

                        # Try to Turn
                        nNextY = kCurrent[QUEUE_CACHE_DATA][QUEUE_Y] + DIRECTIONS[kDirection][0]
                        nNextX = kCurrent[QUEUE_CACHE_DATA][QUEUE_X] + DIRECTIONS[kDirection][1]

                        # Detect if we're still in the map
                        if (nNextY in TARGET_VALID_RANGE_Y) and (nNextX in TARGET_VALID_RANGE_X) :

                            # Add the next move to the queue
                            kSecondaryQueue.append([kCurrent[QUEUE_TOTAL_HEAT] + kHeatLossMap[nNextY][nNextX], (nNextY, nNextX, kDirection, 1)])

                        #end

                    #end

                #end
              
                break

            #end

            if len(kQueue) == 0 :
                kQueue = sorted(kSecondaryQueue, reverse=True)
                kSecondaryQueue = []
            #end

        #end

        return nBestHeatLoss

    #end
              
    print(f"Part 1: {processCrucible(kHeatLossMap, 1, 3)}")
    print(f"Part 2: {processCrucible(kHeatLossMap, 4, 10)}")

#end

if __name__ == "__main__" :
   startTime = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end