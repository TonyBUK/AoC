import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        def normalisedDifference(n) :
            if   n > 0 : return 1
            elif n < 0 : return -1
            else       : return 0
        #end

        def processSandFlow(kObstacles, kEntryPoint, bEndlessVoid = True) :

            # Constants of Interest
            nFloor   = max([V[1] for V in kObstacles]) + 2
            kSand    = set()
            kFalling = set()
            kQueue   = set([tuple(kEntryPoint)])

            # Process until Solved
            while len(kQueue) > 0 :

                kItem = kQueue.pop()

                # If a Queue Position points to Sand, ignore it
                if kItem in kSand :
                    continue
                #end

                # Indicate the Item is Falling
                kFalling.add(kItem)

                # Limit based on the Endless Void
                if bEndlessVoid :
                    if kItem[1] >= nFloor :
                        continue
                    #end
                #end

                # Try to Move Down
                kDown = tuple([kItem[0], kItem[1] + 1])
                if (kDown not in kObstacles) and (kDown not in kSand) and ((kDown[1] < nFloor) or bEndlessVoid) :
                    kQueue.add(kDown)
                    continue
                #end

                # Try to Move Down Left
                kDownLeft = tuple([kDown[0] - 1, kDown[1]])
                if (kDownLeft not in kObstacles) and (kDownLeft not in kSand) and ((kDownLeft[1] < nFloor) or bEndlessVoid) :
                    kQueue.add(kDownLeft)
                    continue
                #end

                # Try to Move Down Right
                kDownRight = tuple([kDown[0] + 1, kDown[1]])
                if (kDownRight not in kObstacles) and (kDownRight not in kSand) and ((kDownRight[1] < nFloor) or bEndlessVoid) :
                    kQueue.add(kDownRight)
                    continue
                #end

                # Handle either the Endless Void or Settling on the Infinite Floor
                if bEndlessVoid :
                    if kItem[1] < nFloor :
                        kSand.add(kItem)
                        kFalling.remove(kItem)
                    #end
                else :
                    kSand.add(kItem)
                    kFalling.remove(kItem)
                #end

                # Add any Falling Neighbours back into the Queue
                # Note: We only care about above
                for X in range(-1, 2) :
                    kPosition = tuple([kItem[0] + X, kItem[1] - 1])
                    if kPosition in kFalling :
                        kQueue.add(kPosition)
                    #end
                #end

            #end

            return len(kSand)

        #end

        # Convert the Data Input into Pairs of Ranges
        # Note: It's at point like this I think I can safely say the benefits of list comprehension
        #       start to get outweighed by the legebility.
        kInputCoordinates = [[[[int(kInt) for kInt in kEntry1.split(",")],
                               [int(kInt) for kInt in kEntry2.split(",")]]
                                for kEntry1,kEntry2 in zip(kLine.strip().split(" -> "), kLine.strip().split(" -> ")[1:])]
                                    for kLine in inputFile.readlines()]

        # Generate all of the Collideable Objects as Tuples
        kObstacles = set()
        for kCoordinates in kInputCoordinates :

            for kCoordinatePair in kCoordinates :

                # The Data is provided as ranges
                xDiff = normalisedDifference(kCoordinatePair[1][0] - kCoordinatePair[0][0])
                yDiff = normalisedDifference(kCoordinatePair[1][1] - kCoordinatePair[0][1])
                assert((xDiff == 0 and yDiff != 0) or (xDiff != 0 and yDiff == 0))

                # Create an entry in the Collideable Object Set based on each position in the range
                kPosition = [X for X in kCoordinatePair[0]]
                while True :

                    kObstacles.add(tuple(kPosition))

                    if kPosition == kCoordinatePair[1] :
                        break
                    #end

                    kPosition[0] += xDiff
                    kPosition[1] += yDiff

                #end

            #end

        #end

        print(f"Part 1: {processSandFlow(kObstacles, [500, 0], True)}")
        print(f"Part 2: {processSandFlow(kObstacles, [500, 0], False)}")

    #end
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end