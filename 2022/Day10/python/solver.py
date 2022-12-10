import time
import sys
 
def main() :
 
    with open("../input.txt", "r") as inputFile:

        def parseRawOpcode(kPair) :
            if len(kPair) == 2 :
                return [kPair[0], int(kPair[1])]
            #end
            return [kPair[0]]
        #end

        def processOpcodes(kOpcodes, nCycleStart, nCycleFrequency, nWidth, nHeight) :

            CYCLES = {
                "addx" : 2,
                "noop" : 1
            }

            # Initial state, as a bodge, we buffer a NOP.
            nCycle           = 0
            x                = 1
            nResult          = 0

            # Initial Display State
            kDisplay         = ["" for _ in range(nHeight)]

            # Process all the Opcodes / Cycles
            for kOpcode in kOpcodes :

                nRemainingCycles = CYCLES[kOpcode[0]]

                while nRemainingCycles > 0 :

                    # Process the End of Cycle
                    nCycle           += 1
                    nRemainingCycles -= 1

                    # Part 1
                    # Sum of Cycles and Frequencies at specified iterations
                    if ((nCycle - nCycleStart) % nCycleFrequency) == 0 :
                        nResult += nCycle * x
                    #end

                    # Part 2
                    # Dumping based on Pixel Position
                    nRow           = (nCycle - 1)//nWidth
                    nPixelPosition = nCycle - (nRow * nWidth)
                    kDisplay[nRow] += "#" if (nPixelPosition in range(x, x + 3)) else " "

                #end

                # Process the Buffered Command once its timer elapses
                if kOpcode[0] == "addx" :

                    x += kOpcode[1]

                #end

            #end

            return nResult, kDisplay

        #end

        # Buffer the Opcodes
        kOpcodes = [parseRawOpcode(line.strip().split(" ")) for line in inputFile.readlines()]

        # Process Both Parts
        kResults = processOpcodes(kOpcodes, 20, 40, 40, 6)

        # Results
        print(f"Part 1: {kResults[0]}")
        print(f"Part 2:")
        for kLine in kResults[1] :
            print(kLine)
        #end

    #end
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end