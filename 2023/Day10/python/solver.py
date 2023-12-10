import time
import sys
 
def main() :

    PIPES = {
        "|": set([(-1,  0), ( 1,  0)]),  # North, South
        "-": set([( 0,  1), ( 0, -1)]),  # East,  West
        "L": set([(-1,  0), ( 0,  1)]),  # North, East
        "J": set([(-1,  0), ( 0, -1)]),  # North, West
        "7": set([( 1,  0), ( 0, -1)]),  # South, West
        "F": set([( 1,  0), ( 0,  1)]),  # South, East
        ".": set()                       # Empty
    }

    NEIGHBOURS = set([(1,0), (-1,0), (0,1), (0,-1)])

    kRawMap = []
    with open("../input.txt", "r") as inputFile:
        kRawMap = [list(k.replace("\n", "")) for k in inputFile.readlines()]
    #end

    kStartNode = None
    kMapNodes  = {}

    # First find the Start Node
    for y in range(len(kRawMap)) :
        if "S"  in kRawMap[y] :
            kStartNode = (y, kRawMap[y].index("S"))
            break
        #end
    #end

    # Now build the Map
    # Note: Start is a Speacial Case in that it's unknown at this point in time, so our very
    #       first act is to replace it.
    kStartType = set()
    for kNeighbour in NEIGHBOURS :

        Y = kStartNode[0] + kNeighbour[0]
        X = kStartNode[1] + kNeighbour[1]

        # Note: This is the only time we should need this test, otherwise the input
        #       data set would be invalid.
        if Y >= 0 and Y < len(kRawMap) :

            if X >= 0 and X < len(kRawMap[Y])  :

                assert(kRawMap[Y][X] in PIPES)
                kReverseNeighbour = (-kNeighbour[0], -kNeighbour[1])
                if kReverseNeighbour in PIPES[kRawMap[Y][X]] :
                    kStartType.add(kNeighbour)
                #end

            #end

        #end

    #end

    for kType, kNeighbours in PIPES.items() :
        if kStartType == kNeighbours :
            kRawMap[kStartNode[0]][kStartNode[1]] = kType
            break
        #end
    #end

    assert(kRawMap[kStartNode[0]][kStartNode[1]] != "S")

    def convertRelativeNeighboursToAbsoluteNeighbours(kNode, kNeighbours) :
        return set((kNode[0] + kNeighbour[0], kNode[1] + kNeighbour[1]) for kNeighbour in kNeighbours)
    #end

    kMapNodes[kStartNode] = convertRelativeNeighboursToAbsoluteNeighbours(kStartNode, PIPES[kRawMap[kStartNode[0]][kStartNode[1]]])
    kNodesToProcess = [[0, kStartNode]]
    nMaxDistance    = 0

    while len(kNodesToProcess) > 0 :

        # Get the Current Node
        # Note: Because it's a loop, we want to use a FIFO approach, as we'll be evenly processing in
        #       either direction, this way we sacrifice array performance for having to keep tabs for
        #       revisited nodes with shorter paths.
        kNode = kNodesToProcess.pop(0)

        # Get the Node Neighbours
        for kNeighbour in kMapNodes[kNode[1]] :

            if kNeighbour in kMapNodes : continue

            # Add the Neighbour to the Map Nodes
            kMapNodes[kNeighbour] = convertRelativeNeighboursToAbsoluteNeighbours(kNeighbour, PIPES[kRawMap[kNeighbour[0]][kNeighbour[1]]])

            # Add the Neighbour to the list of Nodes to Process
            kNodesToProcess.append([kNode[0] + 1, kNeighbour])
            if kNode[0] + 1 > nMaxDistance :
                nMaxDistance = kNode[0] + 1
            #end

        #end

    #end

    print(f"Part 1: {nMaxDistance}")

    # Part 2: Count the Number of Tiles enclosed within the loop
    # This will be a fill pattern approach.  Use the circumference
    # of the loop and count any tiles that aren't part of the loop
    # as out of the loop tiles.

    # Simplify the problem, if you're not part of the loop, you're a blank tile
    kRawMap = [[kRawMap[nRow][nCol] if (nRow, nCol) in kMapNodes else "." for nCol in range(len(kRawMap[nRow]))] for nRow in range(len(kRawMap))]

    # Now the annoying bit, we actually have a maze within a maze... we can squeeze through pipe
    # gaps.  For my sanity... let's construct a larger map that 2x larger in size.
    #
    # Anything that's got an East exit gets padded with a "-" to the right
    # Anything that's got a South exit gets padded with a "|" below
    # Diagonals are assumed to be dots
    kEnlargedMap = []
    for y in range(len(kRawMap)) :

        kEnlargedMap.append([])

        # Extend Eastwards
        for x in range(len(kRawMap[0])) :

            kEnlargedMap[-1].append(kRawMap[y][x])

            if kRawMap[y][x] not in PIPES :
                kEnlargedMap[-1].append(kRawMap[y][x])
            else :
                if (0,1) in PIPES[kRawMap[y][x]] :
                    kEnlargedMap[-1].append("-")
                else :
                    kEnlargedMap[-1].append(".")
                #end
            #end

        #end

        # Extend Southwards
        kEnlargedMap.append([])
        for x in range(len(kRawMap[0])) :

            if kRawMap[y][x] not in PIPES :
                kEnlargedMap[-1].append(kRawMap[y][x])
            else :
                if (1,0) in PIPES[kRawMap[y][x]] :
                    kEnlargedMap[-1].append("|")
                else :
                    kEnlargedMap[-1].append(".")
                #end
            #end

            kEnlargedMap[-1].append(".")

        #end

    #end

    # Now we need to fill in the gaps
    kFillSet = set()

    # Initalise the Search Area with any unmapped nodes on the boarder of the map
    for y in [0, len(kEnlargedMap) - 1] :
        for x in range(len(kEnlargedMap[y])) :
            if kEnlargedMap[y][x] == "." :
                kEnlargedMap[y][x] = "#"
                kFillSet.add((y, x))
            #end
        #end
    #end

    for x in [0, len(kEnlargedMap[0]) - 1] :
        for y in range(len(kEnlargedMap)) :
            if kEnlargedMap[y][x] == "." :
                kEnlargedMap[y][x] = "#"
                kFillSet.add((y, x))
            #end
        #end
    #end

    while len(kFillSet) > 0 :

        kNode = kFillSet.pop()

        # Iterate through the neighbours in the cardinal directions
        for kNeighbour in NEIGHBOURS :

            Y = kNode[0] + kNeighbour[0]
            X = kNode[1] + kNeighbour[1]

            # Note: Part 2 made me a liar...
            if Y >= 0 and Y < len(kEnlargedMap) :

                if X >= 0 and X < len(kEnlargedMap[Y])  :

                    if (Y, X) in kFillSet        : continue
                    if kEnlargedMap[Y][X] != "." : continue

                    kEnlargedMap[Y][X] = "#"
                    kFillSet.add((Y, X))

                #end

            #end

        #end

    #end

    # Shrink the Enlarged Map
    kRawMap = [[kEnlargedMap[nRow*2][nCol*2] for nCol in range(len(kRawMap[nRow]))] for nRow in range(len(kRawMap))]

    print(f"Part 2: {sum(k.count('.') for k in kRawMap)}")

    if False :

        for Y in range(len(kRawMap)) :
            print("".join(kRawMap[Y]))
        #end

        for Y in range(len(kEnlargedMap)) :
            print("".join(kEnlargedMap[Y]))
        #end

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end