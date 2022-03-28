import time
import sys

def main() :

    def decodeCommand(kTokens) :
        if kTokens[0] == "toggle" :
            return [kTokens[0], [int(X) for X in kTokens[1].split(",")], [int(X) for X in kTokens[3].split(",")]]
        else :
            return [kTokens[1], [int(X) for X in kTokens[2].split(",")], [int(X) for X in kTokens[4].split(",")]]
        #end
    #end

    # Read the Input File
    lightCommands = []
    with open("../input.txt", "r") as inputFile:
        lightCommands = [decodeCommand(line.strip().split()) for line in inputFile.readlines()]
    #end

    WIDTH        = 1000
    HEIGHT       = 1000
    lights       = [[0 for X in range(WIDTH)] for Y in range(HEIGHT)]
    lightsDimmer = [[0 for X in range(WIDTH)] for Y in range(HEIGHT)]

    for lightCommand in lightCommands :
        for Y in range(lightCommand[1][1], lightCommand[2][1] + 1) :
            for X in range(lightCommand[1][0], lightCommand[2][0] + 1) :
                if "toggle" == lightCommand[0] :
                    lights[Y][X]        = 1 - lights[Y][X]
                    lightsDimmer[Y][X] += 2
                elif "off"  == lightCommand[0] :
                    lights[Y][X]        = 0
                    if lightsDimmer[Y][X] > 0  : lightsDimmer[Y][X] -= 1
                elif "on"   == lightCommand[0] :
                    lights[Y][X]        = 1
                    lightsDimmer[Y][X] += 1
                #end
            #end
        #end
    #end

    print(f"Part 1: {sum([sum(lightRow) for lightRow in lights])}")
    print(f"Part 2: {sum([sum(lightRow) for lightRow in lightsDimmer])}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
