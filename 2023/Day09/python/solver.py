import time
import sys
 
def main() :

    kSandHistory = []
    with open("../input.txt", "r") as inputFile:

        kSandHistory = [[int(j) for j in k.split()] for k in inputFile.readlines()]

    #end

    def getExtrapolatedValue(kSandHistorySample) :

        # Recursing this makes my head hurt...
        kSandHistory = [kSandHistorySample]

        # Calculate the Layers
        while not all([0 == x for x in kSandHistory[-1]]) :

            assert(len(kSandHistory[-1]) > 0)
            
            # Add the next Layer
            kSandHistory.append([x[1] - x[0] for x in zip(kSandHistory[-1][0:], kSandHistory[-1][1:])])

        #end

        # We now have enough data to extrapolate the new values
        kExtrapolatedValues = [kSandHistory[-2][0], kSandHistory[-2][-1]]

        for i in range(len(kSandHistory) - 3, -1, -1) :
            kExtrapolatedValues[0] = kSandHistory[i][0]  - kExtrapolatedValues[0]
            kExtrapolatedValues[1] = kSandHistory[i][-1] + kExtrapolatedValues[1]
        #end

        return kExtrapolatedValues

    #end    

    kExtrapolatedValues =[getExtrapolatedValue(kSandHistorySample) for kSandHistorySample in kSandHistory]
    print(f"Part 1: {sum([k[1] for k in kExtrapolatedValues])}")
    print(f"Part 2: {sum([k[0] for k in kExtrapolatedValues])}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end