import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        def isAdjacentCubes(c1, c2) :

            nIdenticalCount = 0
            nAdjacentCount  = 0

            for i in range(3) :
                if   c1[i] == c2[i] :
                    nIdenticalCount += 1
                elif abs(c1[i] - c2[i]) == 1 :
                    nAdjacentCount += 1
                else :
                    return False
            #end
            return (nIdenticalCount == 2) and (nAdjacentCount == 1)

        #end

        def isConnectedCubes(c1, c2) :

            for i in range(3) :
                if abs(c1[i] - c2[i]) > 1 :
                    return False
                #end
            #end
            return True

        #end

        kCubes = [tuple([int(x) for x in kLine.strip().split(",")]) for kLine in inputFile.readlines()]

        # Part 1: Subtract any Adjacent Sides
        nTotalPotentialSufraceArea = len(kCubes) * 6
        for i,kCube1 in enumerate(kCubes) :
            for kCube2 in kCubes[i+1:] :
                if isAdjacentCubes(kCube1, kCube2) :
                    nTotalPotentialSufraceArea -= 2
                #end
            #end
        #end

        print(f"Part 1: {nTotalPotentialSufraceArea}")

        # Part 2:
        # We're going to be adding "water" cubes to the list starting from obviously outside.
        #
        # The rules for new cubes are:
        # 1. They must be adjacent to a "water" cube.
        # 2. They cannot be in the same location as a normal cube.
        # 3. They must be adjacent to a normal cube.
        #
        # Eventually we get to a point where we can't add any more water cubes, and the puzzle is solved
        # as the surface area is all the normal cubes sides which are adjacent to water cube sides.

        # Find the obvious starting points, being the extremes of each axis
        MINX = min([C[0] for C in kCubes])
        MAXX = max([C[0] for C in kCubes])
        MINY = min([C[1] for C in kCubes])
        MAXY = max([C[1] for C in kCubes])
        MINZ = min([C[2] for C in kCubes])
        MAXZ = max([C[2] for C in kCubes])

        kQueue       = []
        kWaterCubes  = set()
        kCubes       = set(kCubes)
        nSurfaceArea = 0

        for kCube in kCubes :

            if kCube[0] == MINX :
                kWaterCube = tuple([kCube[0] - 1, kCube[1], kCube[2]])
                kQueue.append(kWaterCube)
            #end

            if kCube[0] == MAXX :
                kWaterCube = tuple([kCube[0] + 1, kCube[1], kCube[2]])
                kQueue.append(kWaterCube)
            #end

            if kCube[1] == MINY :
                kWaterCube = tuple([kCube[0], kCube[1] - 1, kCube[2]])
                kQueue.append(kWaterCube)
            #end

            if kCube[1] == MAXY :
                kWaterCube = tuple([kCube[0], kCube[1] + 1, kCube[2]])
                kQueue.append(kWaterCube)
            #end

            if kCube[2] == MINZ :
                kWaterCube = tuple([kCube[0], kCube[1], kCube[2] - 1])
                kQueue.append(kWaterCube)
            #end

            if kCube[2] == MAXZ :
                kWaterCube = tuple([kCube[0], kCube[1], kCube[2] + 1])
                kQueue.append(kWaterCube)
            #end

        #end

        ADJACENT_CUBE_DELTAS = [
            [-1,  0,  0],
            [ 1,  0,  0],
            [ 0, -1,  0],
            [ 0,  1,  0],
            [ 0,  0, -1],
            [ 0,  0,  1]
        ]

        DIAGONAL_CUBE_DELTAS = [
            [-1, -1, -1],
            [ 1, -1, -1],
            [-1,  1, -1],
            [ 1,  1, -1],
            [-1,  0, -1],
            [ 1,  0, -1],
            [ 0, -1, -1],
            [ 0,  1, -1],

            [-1, -1,  0],
            [ 1, -1,  0],
            [-1,  1,  0],
            [ 1,  1,  0],

            [-1, -1,  1],
            [ 1, -1,  1],
            [-1,  1,  1],
            [ 1,  1,  1],
            [-1,  0,  1],
            [ 1,  0,  1],
            [ 0, -1,  1],
            [ 0,  1,  1]
        ]

        # Process the Queue until Empty
        while len(kQueue) > 0 :

            kWaterCube = kQueue.pop()
            
            # Already seen
            if kWaterCube in kWaterCubes : continue
            kWaterCubes.add(kWaterCube)

            # Create a Hypothetical Cube on each of the 6 faces
            kCandidates = []
            bAdjacent   = False
            for kAdjacentCubeDelta in ADJACENT_CUBE_DELTAS :

                kNewCube = tuple([A+B for A,B in zip(kWaterCube, kAdjacentCubeDelta)])

                if kNewCube in kCubes :
                    nSurfaceArea += 1
                    bAdjacent = True
                elif kNewCube not in kWaterCubes :
                    kCandidates.append(kNewCube)
                #end
            #end

            # If the Water Cube wasn't Adjacent, check if the Water Cube was Diagonal
            bDiagonal = False
            for kDiagonalCubeDelta in DIAGONAL_CUBE_DELTAS :
                if tuple([A+B for A,B in zip(kWaterCube, kDiagonalCubeDelta)]) in kCubes :
                    bDiagonal = True
                    break
                #end
            #end

            # Add the Candidates to the Queue if the Watercube was Diagonal or Adjacent
            if bAdjacent or bDiagonal :
                kQueue += kCandidates
            #end

        #end

        print(f"Part 2: {nSurfaceArea}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
