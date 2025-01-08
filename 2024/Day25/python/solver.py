import time
import sys

def main() :

    kLocks  = []
    kKeys   = []
    nHeight = 0
    with open("../input.txt", "r") as inputFile:

        kItems = [k.split("\n") for k in inputFile.read().split("\n\n")]
        for kItem in kItems :

            # Store the Max Height
            nHeight = max(nHeight, len(kItem) - 2)

            # Convert the Item to a Height Map
            kHeights = [nHeight - (sum(kItem[y][x] == "." for y in range(len(kItem))) - 1) for x in range(len(kItem[0]))]

            # Decide if this is a Key or a Lock
            if kItem[0].count("#") == 0 :
                # Key
                assert(kItem[-1].count("#") == 5)
                kKeys.append(kHeights)
            else :
                # Lock
                assert(kItem[0].count("#") == 5)
                kLocks.append(kHeights)
            #end

        #end

    #end

    # Calculate the Unique Combinations which are valid
    nUniqueCombinations = sum(
        all(a + b <= nHeight for a, b in zip(kLock, kKey))
            for kLock in kLocks
                for kKey in kKeys
    )

    # Ho Ho Ho!
    print(f"Part 1: {nUniqueCombinations}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end