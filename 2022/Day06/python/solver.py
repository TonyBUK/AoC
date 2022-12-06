import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        # Read the Input Data
        kInputData = inputFile.read().strip()

        # Find the first 4 unique characters
        nUniqueHeader = 0
        for i,kData in enumerate(zip(kInputData[0:], kInputData[1:], kInputData[2:], kInputData[3:])) :
            if len(set(kData)) == 4 :
                nUniqueHeader = i + 4
                break
            #end
        #end

        # Find the first 14 unique characters
        nUniquePacket = 0
        for i in range(nUniqueHeader+4, len(kInputData)) :
            if len(set(kInputData[i:i+14])) == 14 :
                nUniquePacket = i + 14
                break
            #end
        #end

        print(f"Part 1: {nUniqueHeader}")
        print(f"Part 2: {nUniquePacket}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
