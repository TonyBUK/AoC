import time
import sys
import math

def main() :

    kBytes          = []
    WIDTH           = 71
    HEIGHT          = 71

    with open("../input.txt", "r") as inputFile:
        kBytes = [(int(tokens[0]), int(tokens[1])) for line in inputFile for tokens in [line.strip().split(",")]]
    #end

    def findShortestRoute(kFallenBytes : set[int,int], bAnyPath : bool = False) :

        # Basically prevents me having to sort the queue...
        def manageDoubleQueue(kPositionQueue, kSecondaryQueue) :
            if len(kPositionQueue) == 0 :
                kPositionQueue  = kSecondaryQueue
                kSecondaryQueue = []
            #end
            return kPositionQueue, kSecondaryQueue
        #end

        NEIGHBOURS = [[-1, 0], # Left
                      [ 0,-1], # Up
                      [ 1, 0], # Right
                      [ 0, 1]  # Down
        ]

        kPositionQueue  = [[0, (0,0)]]
        kSecondaryQueue = []
        nLowestSteps    = math.inf
        kSeen           = {}

        # BFS Search to find the Position...

        while len(kPositionQueue) > 0 :

            # Get the next possible move
            nSteps, kPosition = kPositionQueue.pop()

            # If it's already the same or worse than the current known best move...
            if nSteps >= nLowestSteps :
                kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                continue
            #end

            # If this is the end position, test if we've encountered our best one yet
            if kPosition == (WIDTH-1, HEIGHT-1) :
                if bAnyPath :
                    return nSteps
                #end
                nLowestSteps = min(nLowestSteps, nSteps)
                kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                continue
            #end

            # If we've already seen this position and at a lower step count...
            if kPosition in kSeen :
                if kSeen[kPosition] <= nSteps :
                    kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)
                    continue
                #end
            #end
            kSeen[kPosition] = nSteps

            # For each Neighbour
            for kNeighbour in NEIGHBOURS :

                # In the Grid?
                kNeighbourPosition = (kPosition[0] + kNeighbour[0], kPosition[1] + kNeighbour[1])
                if kNeighbourPosition[0] not in range(WIDTH) or kNeighbourPosition[1] not in range(HEIGHT) :
                    continue
                #end

                # Collided with bytes?
                if kNeighbourPosition not in kFallenBytes :
                    kSecondaryQueue.append([nSteps+1, kNeighbourPosition])
                #end

            #end

            kPositionQueue, kSecondaryQueue = manageDoubleQueue(kPositionQueue, kSecondaryQueue)

        #end

        return nLowestSteps

    #end

    # Part 1: Find the Shortest Route for 1024 falled bytes
    print(f"Part 1: {findShortestRoute(set(kBytes[:1024]))}")

    # Part 2: Find the first maze that can't be solved
    nMin = 1025
    nMax = len(kBytes)
    while nMin < nMax :

        # Calculate the Mid Point
        nMid = ((nMax - nMin) // 2) + nMin

        # In this scenario, Min/Max are one apart and we know Max must be inf
        if nMid == nMin :
            break
        #end

        if math.isinf(findShortestRoute(set(kBytes[:nMid]), True)) :
            nMax = nMid
        else :
            nMin = nMid
        #end

    #end

    # Note: The answer is actually nMax-1, where nMax will be one higher than the index
    #       due to array splicing.
    #
    #       i.e. array       = [(0,0),(0,1)]
    #            spliceFirst = array[:1] = [(0,0)]
    #
    #            In this instance we slice the array up to (but not including) element 1, which returns element 0 only.
    #
    #       We splice on nMax, meaning nMax-1 is the last element in the array, which is also nMid.
    print(f"Part 2: {kBytes[nMid][0]},{kBytes[nMid][1]}")

    return

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end