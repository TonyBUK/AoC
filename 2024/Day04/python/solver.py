import time
import sys
 
def main() :
 
    kWordSearch = []
    with open("../input.txt", "r") as inputFile:
        kWordSearch = [list(kInputLine.strip()) for kInputLine in inputFile.readlines()]
    #end
 
    def getSearchArea(kWordSearch : list[list[str]], X : int, Y : int, SEARCH_AREA : list[list[int]], WIDTH : int, HEIGHT : int) -> str :
 
        kOutput = []
        for kCoordinate in SEARCH_AREA :
            if X + kCoordinate[0] in range(WIDTH) :
                if Y + kCoordinate[1] in range(HEIGHT) :
                    kOutput.append(kWordSearch[Y + kCoordinate[1]][X + kCoordinate[0]])
                #end
            #end
        #end
       
        return "".join(kOutput)
 
    #end
 
    HEIGHT = len(kWordSearch)
    WIDTH  = len(kWordSearch[0])
 
    SEARCH_AREAS = [
        [[ 0, 0], [ 1, 0], [ 2, 0], [ 3, 0]], # Across
        [[ 0, 0], [ 0, 1], [ 0, 2], [ 0, 3]], # Down
        [[ 0, 0], [-1,-1], [-2,-2], [-3,-3]], # Diagonal Backward
        [[ 0, 0], [ 1,-1], [ 2,-2], [ 3,-3]]  # Diagonal Forward
    ]
    VALID_SET = set(["M", "S"])
 
    nPartOne = 0
    nPartTwo = 0
 
    for Y in range(HEIGHT) :
 
        for X in range(WIDTH) :
 
            # Part One
            # ========
 
            # Only Search if this is the Start or End of XMAS
            if kWordSearch[Y][X] in ["X", "S"] :
                for kSearchArea in SEARCH_AREAS :
                    kWord = getSearchArea(kWordSearch=kWordSearch, X=X, Y=Y, SEARCH_AREA=kSearchArea, WIDTH=WIDTH, HEIGHT=HEIGHT)
                    # Check for either Condition
                    if kWord in ["XMAS", "SAMX"] :
                        nPartOne += 1
                    #end
                #end
            #end
 
            # Part Two
            # ========
 
            # We can constrain the overall search range to guarentee no out of bounds reads
            # since a valid "A" will never be on the edges.
            if Y in range(1, HEIGHT - 1) :
 
                if X in range(1, WIDTH - 1) :
 
                    # If this is a Middle "A"...
                    if kWordSearch[Y][X] == "A" :
                        # We're now interest in the diagonal pairs
                        kPair1 = set([kWordSearch[Y-1][X-1], kWordSearch[Y+1][X+1]])
                        kPair2 = set([kWordSearch[Y-1][X+1], kWordSearch[Y+1][X-1]])
                        if kPair1 == VALID_SET and kPair2 == VALID_SET :
                            nPartTwo += 1
                        #end
                    #end
                #end
            #end
        #end
 
    #end
 
    print(f"Part 1: {nPartOne}")
    print(f"Part 2: {nPartTwo}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end