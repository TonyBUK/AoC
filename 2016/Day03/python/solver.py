import time
import sys

def main() :

    # Read the Triangle Lengths
    triangles = []
    with open("../input.txt", "r") as inputFile:
        triangles = [[int(x) for x in " ".join(line.strip().split()).split(" ")] for line in inputFile.readlines()]

        # Equivalent functionality without List Comprehension

        # for line in inputFile.readlines() :
        #     cleanedLine = line.strip()
        #     while "  " in cleanedLine :
        #         cleanedLine = cleanedLine.replace("  ", " ")
        #     #end
        #     triangle = []
        #     for X in cleanedLine.split(" ") :
        #         triangle.append(int(X))
        #     #end
        #     triangles.append(triangle)
        # #end
    #end

    def countValidTriangles(triangles) :
        valid = 0
        for triangle in triangles :
            triangle = sorted(triangle)
            if triangle[2] < (triangle[0] + triangle[1]) :
                valid += 1
            #end
        #end
        return valid
    #end

    print(f"Part 1: {countValidTriangles(triangles)}")

    assert((len(triangles) % 3) == 0)

    # Re-arrange the triangles by columns
    rearrangedTriangles = []
    for Y in range(0, len(triangles), 3) :
        for X in range(len(triangles[Y])) :
            rearrangedTriangles.append([triangles[Y][X], triangles[Y+1][X], triangles[Y+2][X]])
        #end
    #end

    print(f"Part 2: {countValidTriangles(rearrangedTriangles)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
