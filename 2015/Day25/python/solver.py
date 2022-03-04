import time

ALGORITHMICALLY = 0
HYBRID          = 1
PURE_MATH       = 2

def main(solveType) :

    # Read/Clean the Input File to extract the Row/Columns
    targetCoords = []
    with open("../input.txt", "r") as inputFile:

        def extractTarget(tokens) :
            assert(len(tokens) == 18)
            return {"col" : int(tokens[17]), "row" : int(tokens[15])}
        #end

        targetCoords = [extractTarget(line.replace(",","").replace(".","").replace("  ", " ").strip().split(" ")) for line in inputFile.readlines()]

    #end

    def triangleNumber(n) :
        return (n * (n+1)) // 2
    #end

    for targetCoord in targetCoords :

        current = 20151125

        if ALGORITHMICALLY == solveType :

            col     = 1
            row     = 1

            while (row != targetCoord["row"]) or (col != targetCoord["col"]) :

                if 1 == row :
                    row = col + 1
                    col = 1
                else :
                    row -= 1
                    col += 1
                #end
                current = (current * 252533) % 33554393
            #end

            print(f"Part 1: {current}")

        elif HYBRID == solveType :

            iterations = triangleNumber(targetCoord["row"] + targetCoord["col"] - 2) + targetCoord["col"] - 1
            for _ in range(iterations) :
                current = (current * 252533) % 33554393
            #end

            print(f"Part 1: {current}")
        
        elif PURE_MATH == solveType :

            iterations = triangleNumber(targetCoord["row"] + targetCoord["col"] - 2) + targetCoord["col"] - 1
            current = (pow(252533, iterations, 33554393) * current) % 33554393

            print(f"Part 1: {current}")

        #end

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main(ALGORITHMICALLY)
    print(f"{(time.perf_counter() - startTime)}s")

    startTime      = time.perf_counter()
    main(HYBRID)
    print(f"{(time.perf_counter() - startTime)}s")

    startTime      = time.perf_counter()
    main(PURE_MATH)
    print(f"{(time.perf_counter() - startTime)}s")

    print("Ho ho ho!")

#end
