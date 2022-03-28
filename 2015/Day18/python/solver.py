import time
import sys

def main() :

    # Read/Clean the Input File to extract the Buckets
    conwayGrid = []
    with open("../input.txt", "r") as inputFile:
        conwayGrid = [[1 if C == "#" else 0 for C in line.strip()] for line in inputFile.readlines()]
    #end

    # Debug Function
    def printConwayFromGrid(conwayGrid) :
        for row in conwayGrid :
            for col in row :
                print("#" if col == 1 else " ", end="")
            #end
            print("")
        #end
    #end

    # Debug Function
    def printConwayFromLiveConways(liveConways, WIDTH, HEIGHT) :
        for Y in range(HEIGHT) :
            for X in range(WIDTH) :
                pos = tuple([Y,X])
                print("#" if pos in liveConways else " ", end="")
            #end
            print("")
        #end
    #end

    def processConways(conwayGrid, iterations, forceCorners = False) :

        HEIGHT = len(conwayGrid)
        WIDTH  = len(conwayGrid[0])

        def initialiseConwayCandidates(liveConways) :

            conwayCandidates = {}

            for liveConway in liveConways :

                # Add/Nop the Current Conway Candidate as we need the position to exist, even if it's Neighbour Count is 0...
                conwayCandidates[liveConway] = conwayCandidates.get(liveConway, 0)

                # Extract X/Y
                Y = liveConway[0]
                X = liveConway[1]

                # Set the Y Bounds
                tYStart = Y - 1 if Y > 1            else 0
                tYEnd   = Y + 1 if Y < (HEIGHT - 2) else (HEIGHT - 1)

                # Set the X Bounds
                tXStart = X - 1 if X > 1            else 0
                tXEnd   = X + 1 if X < (WIDTH  - 2) else (WIDTH  - 1)

                for tY in range(tYStart, tYEnd + 1) :
                    for tX in range(tXStart, tXEnd + 1) :
                        if tY != Y or tX != X :
                            conwayTuple                   = tuple([tY,tX])
                            conwayCandidates[conwayTuple] = conwayCandidates.get(conwayTuple, 0) + 1
                        #end
                    #end
                #end
            #end

            return conwayCandidates

        #end

        liveConways = set()
        for Y in range(len(conwayGrid)) :
            for X in range(len(conwayGrid)) :
                if conwayGrid[Y][X] :
                    liveConways.add(tuple([Y,X]))
                #end
            #end
        #end

        conwayCorners = set()
        if forceCorners :
            conwayCorners.add(tuple([       0,       0]))
            conwayCorners.add(tuple([HEIGHT-1,       0]))
            conwayCorners.add(tuple([       0, WIDTH-1]))
            conwayCorners.add(tuple([HEIGHT-1, WIDTH-1]))
            liveConways |= conwayCorners
        #end

        for _ in range(iterations) :

            conwayCandidates = initialiseConwayCandidates(liveConways)

            for conwayPos,conwayCount in conwayCandidates.items() :
                if conwayPos in liveConways :
                    if conwayCount not in [2,3] :
                        liveConways.remove(conwayPos)
                    #end
                else :
                    if conwayCount == 3 :
                        liveConways.add(conwayPos)
                    #end
                #end
            #end

            liveConways |= conwayCorners

        #end

        return len(liveConways)

    #end

    print(f"Part 1: {processConways(conwayGrid, 100)}")
    print(f"Part 2: {processConways(conwayGrid, 100, True)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
