import time
import sys

def main() :

    WIDTH  = 50
    HEIGHT = 6

    def decodeCommand(kTokens) :

        assert(len(kTokens) >= 2)

        kCommand = {}
        kCommand["parameters"] = []

        if "rect" == kTokens[0] :
            assert(len(kTokens) == 2)
            kCommand["parameters"] = [int(X) for X in kTokens[1].split("x")]
        elif "rotate" == kTokens[0] :
            assert(("row" == kTokens[1]) or ("column" == kTokens[1]))
            assert(len(kTokens) == 5)
            kCommand["parameters"].append(kTokens[1])

            if ("row" == kTokens[1]) :
                assert(kTokens[2].find("y=") == 0)
            else :
                assert(kTokens[2].find("x=") == 0)
            #end

            kCommand["parameters"].append(int(kTokens[2][2:]))
            kCommand["parameters"].append(int(kTokens[4]))

        else :
            assert(False)
        #end

        kCommand["command"] = kTokens[0]

        return kCommand

    #end

    # Read and Decode the Commands
    kCommands = []
    with open("../input.txt", "r") as inputFile:
        kCommands = [decodeCommand(line.strip().split(" ")) for line in inputFile.readlines()]
    #end

    # Initialse a Blank Grid
    kGrid  = [[" " for X in range(WIDTH)] for Y in range(HEIGHT)]

    # Process each Command
    for kCommand in kCommands :

        # Rectangle
        if "rect" == kCommand["command"] :

            # Put a blocky character in the rectangle area
            for Y in range(kCommand["parameters"][1]) :
                for X in range(kCommand["parameters"][0]) :
                    kGrid[Y][X] = "#"
                #end
            #end

        # Rotate
        elif "rotate" == kCommand["command"] :

            # Rotate Row
            if "row" == kCommand["parameters"][0] :

                # Buffer the Row Data
                nRow         = kCommand["parameters"][1]
                nShift       = kCommand["parameters"][2]
                kBufferedRow = [C for C in kGrid[nRow]]
                
                # Rotate the Row right by the specified amount
                for X in range(WIDTH) :
                    kGrid[nRow][(X + nShift) % WIDTH] = kBufferedRow[X]
                #end

            # Rotate Column
            elif "column" == kCommand["parameters"][0] :

                # Buffer the Column Data
                nColumn         = kCommand["parameters"][1]
                nShift          = kCommand["parameters"][2]
                kBufferedColumn = [kRow[nColumn] for kRow in kGrid]

                # Rotate the Column down by the specified amount
                for Y in range(HEIGHT) :
                    kGrid[(Y + nShift) % HEIGHT][nColumn] = kBufferedColumn[Y]
                #end
            #end

        #end

    #end

    print(f"Part 1: {sum([sum([0 if C == ' ' else 1 for C in kRow]) for kRow in kGrid])}")
    print("Part 2:")
    for kRow in kGrid :
        print("".join(kRow))
    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)*1000}ms", file=sys.stderr)
#end
