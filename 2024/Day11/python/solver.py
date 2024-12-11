import time
import sys
 
def main() :
 
    # Parse the Input File
    with open("../input.txt", "r") as inputFile:
 
         for kLine in inputFile.readlines() :
            kStones = {}
            for i, kStone in enumerate(kLine.strip().split(" ")) :
                nStone = int(kStone)
                kStones[nStone] = kStones.get(nStone, 0) + 1
            #end
 
            for i in range(75) :
 
                # We don't want to modify the dictionary while iterating over it
                # so we store a replacement dictionary
                kStonesNew = {}
 
                for nStone, nCount in kStones.items() :
 
                    # 0 => 1
                    if nStone == 0 :
                        kStonesNew[1]      = kStonesNew.get(1, 0) + nCount
                    else :
                        # Even => Split String
                        kStoneString  = str(nStone)
                        nStringLength = len(kStoneString)
                        if 0 == (nStringLength % 2) :
                            nLeft  = int(kStoneString[:nStringLength // 2])
                            nRight = int(kStoneString[nStringLength // 2:])
                            kStonesNew[nLeft]     = kStonesNew.get(nLeft, 0) + nCount
                            kStonesNew[nRight]    = kStonesNew.get(nRight, 0) + nCount
                        # Anything else multiply by 2024
                        else :
                            kStonesNew[nStone * 2024] = kStonesNew.get(nStone * 2024, 0) + nCount
                        #end
                    #end
                #end
 
                # Update the Original Stones dictionary
                kStones = kStonesNew
 
                if 25 == (i+1) :
                    print(f"Part 1: {sum(kStones.values())}")
                #end
 
            #end
 
            print(f"Part 2: {sum(kStones.values())}")
 
        #end
 
    #end
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end