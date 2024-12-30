import time
import sys
import math

def main() :

    # Opcodes
    ADV             = 0
    BXL             = 1
    BST             = 2
    JNZ             = 3
    BXC             = 4
    OUT             = 5
    BDV             = 6
    CDV             = 7

    def getComboOperand(nOperand : int, kRegisters : dict[str, int]) -> int :

        COMBO_LOOKUP = {
            4 : "A",
            5 : "B",
            6 : "C"
        }

        if nOperand <= 3 :
            return nOperand
        else :
            return kRegisters[COMBO_LOOKUP[nOperand]]
        #end

    #end

    # Opcode Execution as described by the puzzle
    def runProgram(kProgram : list[int], kRegisters : dict[str, int]) -> list[int] :

        kRegisters      = {k : v for k, v in kRegisters.items()}
        kOutput         = []
        nProgramCounter = 0
        while nProgramCounter < len(kProgram) :

            nOpcode  = kProgram[nProgramCounter]
            nOperand = kProgram[nProgramCounter + 1]

            if nOpcode == ADV :
                nNumerator      = kRegisters["A"]
                nDenominator    = 2**getComboOperand(nOperand=nOperand, kRegisters=kRegisters)
                kRegisters["A"] = (nNumerator // nDenominator)
            elif nOpcode == BXL :
                kRegisters["B"] = (kRegisters["B"] ^ nOperand) % 8
            elif nOpcode == BST :
                kRegisters["B"] = getComboOperand(nOperand=nOperand, kRegisters=kRegisters) % 8
            elif nOpcode == JNZ :
                if kRegisters["A"] != 0 :
                    nProgramCounter = nOperand
                    continue
                #end
            elif nOpcode == BXC :
                kRegisters["B"] = (kRegisters["B"] ^ kRegisters["C"]) % 8
            elif nOpcode == OUT :
                kOutput.append(getComboOperand(nOperand=nOperand, kRegisters=kRegisters) % 8)
            elif nOpcode == BDV :
                nNumerator      = kRegisters["A"]
                nDenominator    = 2**getComboOperand(nOperand=nOperand, kRegisters=kRegisters)
                kRegisters["B"] = (nNumerator // nDenominator) % 8
            elif nOpcode == CDV :
                nNumerator      = kRegisters["A"]
                nDenominator    = 2**getComboOperand(nOperand=nOperand, kRegisters=kRegisters)
                kRegisters["C"] = (nNumerator // nDenominator) % 8
            else :
                assert(False)
            #end

            nProgramCounter += 2

        #end

        return kOutput

    #end

    # Opcode Reverse Execution
    # - Divides become multiplies.
    # - XOR's remain unchanged.
    # - JNZ becomes an iteration flag.
    # - Output is when we test about returning the lowest A value if we've achieved the expected output depth, and therefore the expected output.
    def runProgramReversed(kProgram : list[int], kExpectedOutput : list[int], kRegisters : dict[str, int] = None, nProgramCounter : int = 0, nDepth = 0, nLowestA : int = math.inf) -> int :

        # Create an empty set of Registers and Program Counter on the first pass
        if kRegisters is None :
            kRegisters = {
                "A" : 0,
                "B" : 0,
                "C" : 0
            }
            nProgramCounter = len(kProgram) - 2
        else :
            # Create a Local "Deep" Copy of the Registers
            kRegisters = {k : v for k, v in kRegisters.items()}
        #end

        # None of these loops should occur more than once.
        nIterations = 0

        while True :

            nOpcode  = kProgram[nProgramCounter]
            nOperand = kProgram[nProgramCounter + 1]

            if nOpcode == ADV :
                nNumerator      = kRegisters["A"]
                nDenominator    = 2**getComboOperand(nOperand=nOperand, kRegisters=kRegisters)
                nRegisterA      = (nNumerator * nDenominator)
                for nOffset in range(8) :
                    kRegisters["A"] = nRegisterA + nOffset
                    if runProgram(kProgram, kRegisters) == kExpectedOutput[len(kExpectedOutput)-nDepth-1:] :
                        nLowestA = min(nLowestA, runProgramReversed(kProgram, kExpectedOutput, kRegisters, nProgramCounter - 2, nDepth + 1, nLowestA))
                    #end
                #end
            elif nOpcode == BXL :
                kRegisters["B"] = (kRegisters["B"] ^ nOperand) % 8
            elif nOpcode == BST :
                kRegisters["B"] = getComboOperand(nOperand=nOperand, kRegisters=kRegisters) % 8
            elif nOpcode == JNZ :
                nIterations += 1
            elif nOpcode == BXC :
                kRegisters["B"] = (kRegisters["B"] ^ kRegisters["C"]) % 8
            elif nOpcode == OUT :
                if (nDepth == len(kProgram)) :
                    return kRegisters["A"]
                elif (nDepth == 0) and (nIterations == 1) :
                    assert(nOperand in [4, 5, 6])
                    kRegisters["A" if nOperand == 4 else "B" if nOperand == 5 else "C"] = kExpectedOutput[-1]
                #end
            elif nOpcode == BDV :
                nNumerator      = kRegisters["A"]
                nDenominator    = 2**getComboOperand(nOperand=nOperand, kRegisters=kRegisters)
                kRegisters["B"] = (nNumerator * nDenominator) % 8
            elif nOpcode == CDV :
                nNumerator      = kRegisters["A"]
                nDenominator    = 2**(getComboOperand(nOperand=nOperand, kRegisters=kRegisters))
                kRegisters["C"] = (nNumerator * nDenominator) % 8
            else :
                assert(False)
            #end

            nProgramCounter -= 2

            if nProgramCounter < 0 :
                if nIterations > 1 :
                    return nLowestA
                else :
                    nProgramCounter = len(kProgram) - 2
                #end
            #end

        #end

    #end

    kRegisters      = {}
    kProgram        = []

    with open("../input.txt", "r") as inputFile:

        kTokens = inputFile.read().split("\n\n")
        assert(len(kTokens) == 2)

        # Registers
        kRegisterStringTokens = kTokens[0].split("\n")
        assert(len(kRegisterStringTokens) == 3)
        for kRegisterString in kRegisterStringTokens :
            kRegisterString = kRegisterString.replace("Register ", "").replace(":", "")
            kRegisterToken  = kRegisterString.split(" ")
            kRegisters[kRegisterToken[0]] = int(kRegisterToken[1])
        #end

        # Program
        kProgramStringTokens = kTokens[1].replace("Program: ", "").split(",")
        kProgram             = [int(k) for k in kProgramStringTokens]

    #end

    kOutput = runProgram(kProgram=kProgram, kRegisters=kRegisters)
    print(f"Part 1: {",".join([str(k) for k in kOutput])}")

    # Part 2:
    print(f"Part 2: {runProgramReversed(kProgram=kProgram, kExpectedOutput=kProgram)}")

#end

if __name__ == "__main__" :
   startTime      = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end