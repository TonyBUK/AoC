import time
import sys

def main() :

    WIDTH  = 101
    HEIGHT = 103

    # Parse the Robots
    kRobots = []
    with open("../input.txt", "r") as inputFile:
        for kLine in inputFile.readlines() :
            kLine = kLine.strip().replace(" ", "").replace("p", "").replace("v", "")
            kTokens = kLine.split("=")
            assert(len(kTokens) == 3)
            x, y = (int(k) for k in kTokens[1].split(","))
            vX, vY = (int(k) for k in kTokens[2].split(","))
            kRobots.append([(x, y), (vX, vY)])
        #end
    #end

    def calculateNewPosition(kRobots : list[tuple[int, int], tuple[int, int]], nRounds : int, WIDTH : int, HEIGHT : int) -> list[tuple[int, int]] :
        # Since they teleport back in this is just a simply multiply/modulo.
        kNewRobots = []
        for kRobot in kRobots :
            x, y    = kRobot[0]
            vX, vY  = kRobot[1]
            x      += nRounds * vX
            y      += nRounds * vY
            kNewRobots.append((x % WIDTH, y % HEIGHT))
        #end
        return kNewRobots
    #end

    def getQuadrantSafety(kRobots : list[tuple[int, int], tuple[int, int]], WIDTH : int, HEIGHT : int) -> int :

        QUADRANTS = [
            [[           0, WIDTH // 2], [            0, HEIGHT//2]], # Top Left
            [[(WIDTH+1)//2, WIDTH     ], [            0, HEIGHT//2]], # Top Right
            [[           0, WIDTH // 2], [(HEIGHT+1)//2, HEIGHT   ]], # Bottom Left
            [[(WIDTH+1)//2, WIDTH     ], [(HEIGHT+1)//2, HEIGHT   ]]  # Bottom Right
        ]

        # Count all the Robots in each Quadrant
        kRobotsInQuadrant = [0, 0, 0, 0]
        for kRobot in kRobots :
            x, y = kRobot
            for i, kQuadrant in enumerate(QUADRANTS) :
                if x in range(kQuadrant[0][0], kQuadrant[0][1]) and y in range(kQuadrant[1][0], kQuadrant[1][1]) :
                    kRobotsInQuadrant[i] += 1
                    break
                #end
            #end
        #end

        # Multiply the Quadrants
        nProduct = 1
        for nCount in kRobotsInQuadrant :
            nProduct *= nCount
        #end
        return nProduct

    #end

    def printRobots(kRobots, WIDTH, HEIGHT) :
        kGrid = [["." for _ in range(WIDTH)] for _ in range(HEIGHT)]
        for kRobot in kRobots :
            x, y = kRobot
            kGrid[y][x] = "#"
        #end
        for kLine in kGrid :
            print("".join(kLine))
        #end
    #end

    def isTreeCheat(kRobots : list[tuple[int, int], tuple[int, int]]) :

        # I can't believe this actually worked, this was just a first pass
        # to let me visually assess the number of robots versus grid size
        # to get an idea as to how big the tree might be.
        return len(kRobots) == len(set(kRobots))

    #end

    def isTreeHonest(kRobots : list[tuple[int, int], tuple[int, int]]) :

        def getMaxCluster(kRobot : tuple[int, int], kRobots : list[tuple[int, int], tuple[int, int]]) -> int :

            NEIGHBOURS = [
                [-1,  0], # Left
                [ 1,  0], # Right
                [ 0, -1], # Up
                [ 0,  1]  # Down
            ]

            kQueue = [kRobot]
            kSeen  = set()

            while kQueue :

                kCurrent = kQueue.pop()
                
                if kCurrent in kSeen :
                    continue
                #end
                kSeen.add(kCurrent)

                for kNeighbour in NEIGHBOURS :

                    kNextRobot = (kCurrent[0] + kNeighbour[0], kCurrent[1] + kNeighbour[1])

                    if kNextRobot in kRobots :

                        if kNextRobot not in kSeen and kNextRobot in kRobots :
                            kQueue.append(kNextRobot)
                        #end

                    #end

                #end

            #end

            return len(kSeen), kSeen

        #end

        # This calculates an average position, and computes the number of
        # robots that are within a 5 unit radius of that average position.
        #
        # The theory being that any image should have a centre of mass around
        # where the image is drawn, and that we would encounter a high number
        # of robots in that area.
        #
        # Because the puzzle ill defines the tree itself, we have to use some
        # arbitrary heuristic, working backwards from the answer itself, and
        # this seems adequate.
        #
        # 15 units was chosen as a radius because, for my input, it resulted
        # in the trigger point being comfortably above the number, with the
        # next closest being significantly below.
        #
        # As such, we use this as a filter for a much better test, neighbouring
        # clusters, which is a much more robust test for the tree as it checks
        # for a lurge number of robots adjacent to each other, however this is
        # far slower, so we can use the above heuristic to filter out testing
        # this unnecessarily.
        #
        # The thresholds are:
        #
        # Filter: 20% of robots are within a 15 unit radius of the calculated
        #         average position.
        #
        # Cluster: 10% of robots are adjacent to each other.

        # Calculate the Filter Value (20% of the Robot Count)
        nFilterThreshold = len(kRobots) // 5

        # Calculate the Average Position
        kAveragePosition = [0, 0]
        for kRobot in kRobots :
            kAveragePosition[0] += kRobot[0]
            kAveragePosition[1] += kRobot[1]
        #end
        kAveragePosition[0] /= len(kRobots)
        kAveragePosition[1] /= len(kRobots)

        # Count the number of Robots within a 15 unit radius of the average position
        #
        # Note : We can cheat distances by not calculate the square root, and comparing to the
        #        square of the distance we're interested in, which saves computational time.
        nCloseCount = 0
        for kRobot in kRobots :
            nDistance = ((kRobot[0] - kAveragePosition[0])**2) + ((kRobot[1] - kAveragePosition[1])**2)
            if nDistance <= 225 : # (within a 15 unit radius)
                nCloseCount += 1
            #end
        #end

        # If 20% of the Robots are within a 15 unit radius of the average position
        # then we can check for clusters.
        if nCloseCount >= nFilterThreshold :

            # Cluster Threshold (10% of the Robot Count)
            nClusterThreshold = nFilterThreshold // 2

            # Check for Clusters
            nMaxCluster       = 0
            kSeen             = set()

            for kRobot in kRobots :

                # Skip Robots we've already seen
                if kRobot in kSeen :
                    continue
                #end

                # Update the Cluster Size and Robots we've seen
                nSize, kSeenThisTime = getMaxCluster(kRobot=kRobot, kRobots=kRobots)
                kSeen               |= kSeenThisTime
                nMaxCluster          = max(nMaxCluster, nSize)

            #end

            # Return True if we have a large Cluster
            return nMaxCluster >= nClusterThreshold

        #end
        
        return False

    #end

    # Part 1: Quadrant Safety
    kRound100 = calculateNewPosition(kRobots=kRobots, nRounds=100, WIDTH=WIDTH, HEIGHT=HEIGHT)
    print(f"Part 1: {getQuadrantSafety(kRobots=kRound100, WIDTH=WIDTH, HEIGHT=HEIGHT)}")

    # Part 2: Tree Shape
    nRound = 0
    while True :
#        if isTreeHonest(kRobots=calculateNewPosition(kRobots=kRobots, nRounds=i, WIDTH=WIDTH, HEIGHT=HEIGHT)) :
        if isTreeCheat(kRobots=calculateNewPosition(kRobots=kRobots, nRounds=nRound, WIDTH=WIDTH, HEIGHT=HEIGHT)) :
            print(f"Part 2: {nRound}")
            break
        #end
        nRound += 1
    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end