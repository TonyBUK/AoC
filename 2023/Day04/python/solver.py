import time
import sys
 
def main() :
 
    def extractCard(kLine) :
         return [set(int(m) for m in l.split(" ") if m != "") for l in [k for k in kLine.split(":")[1].split("|")]]
    #end
 
    kWinningCardCount = []
    with open("../input.txt", "r") as inputFile:
        kWinningCardCount = [[len(k[0] & k[1]), 1] for k in (extractCard(kLine.replace("\n", "")) for kLine in inputFile.readlines())]

        # Or for the one-liner fans... aka bad people.
        # kWinningCardCount = [[len(k[0] & k[1]), 1] for k in ([set(int(m) for m in l.split(" ") if m != "") for l in [k for k in kLine.split(":")[1].split("|")]] for kLine in inputFile.readlines())]
    #end
 
    print(f"Part 1: {sum([2 ** (k[0] - 1) for k in kWinningCardCount if k[0] > 0])}")
 
    for i, kWinningNumbers in enumerate(kWinningCardCount) :
        for j in range(i+1, i+kWinningNumbers[0]+1) :
            if j < len(kWinningCardCount) :
                kWinningCardCount[j][1] += kWinningNumbers[1]
            #end
        #end
    #end
 
    print(f"Part 2: {sum([k[1] for k in kWinningCardCount])}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end