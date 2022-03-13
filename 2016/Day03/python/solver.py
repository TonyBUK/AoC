import time

def main() :

    # Read the Triangle Coordinates
    triangles = []
    with open("../input.txt", "r") as inputFile:
        triangles = [[int(x) for x in " ".join(line.strip().split()).split(" ")] for line in inputFile.readlines()]
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
    print(f"{(time.perf_counter() - startTime)}s")
#end
