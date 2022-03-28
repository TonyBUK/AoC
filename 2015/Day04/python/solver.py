import time
import hashlib
import sys

def main() :

    # Read the Input File
    keys = []
    with open("../input.txt", "r") as inputFile:
        keys = [line.strip() for line in inputFile.readlines()]
    #end

    for key in keys :

        nPart1 = -1
        nPart2 = -1
        nCount = 1
        while nPart1 == -1 or nPart2 == -1 :

            hash = hashlib.md5((key + str(nCount)).encode()).hexdigest()

            if nPart1 == -1 :
                if hash[0:5] == "00000" :
                    nPart1 = nCount
                #end
            #end

            if nPart2 == -1 :
                if hash[0:6] == "000000" :
                    nPart2 = nCount
                #end
            #end

            nCount += 1
        #end

        print(f"Part 1: {nPart1}")
        print(f"Part 2: {nPart2}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
