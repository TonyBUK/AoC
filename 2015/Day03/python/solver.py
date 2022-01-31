import time

def main() :

    # Read the Input File
    dimensions = []
    with open("../input.txt", "r") as inputFile:
        directionsList = [line.strip() for line in inputFile.readlines()]
    #end

    for directions in directionsList :

        santaPos        = [0,0]
        combinedPos     = [[0,0],[0,0]]
        currentTurn     = 0

        santaDeliveries = set([tuple(santaPos)])
        totalDeliveries = set([tuple(combinedPos[currentTurn])])

        for direction in directions :
            if   '<' == direction :
                combinedPos[currentTurn][0] -= 1
                santaPos[0]                 -= 1
            elif '^' == direction :
                combinedPos[currentTurn][1] += 1
                santaPos[1]                 += 1
            elif '>' == direction :
                combinedPos[currentTurn][0] += 1
                santaPos[0]                 += 1
            elif 'v' == direction :
                combinedPos[currentTurn][1] -= 1
                santaPos[1]                 -= 1
            #end

            santaDeliveries.add(tuple(santaPos))
            totalDeliveries.add(tuple(combinedPos[currentTurn]))
            currentTurn = 1 - currentTurn

        #end

        print(f"Part 1: {len(santaDeliveries)}")
        print(f"Part 2: {len(totalDeliveries)}")

    #end


#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
