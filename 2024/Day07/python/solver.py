import time
import sys
import math

def main() :

    def equationSolveable(kEquation : list[int], nIndex : int = 1, nRunningTotal : int = 0, bPartTwo : bool = False) :

        if nIndex == len(kEquation) :
            if nRunningTotal == kEquation[0] :
                return True
            #end
            return False
        elif nRunningTotal > kEquation[0] :
            return False
        #end

        # Perform the Concatenation first for Part Two since we know the solution
        # requires it.
        if bPartTwo :
            # Equivalent to int(f"{nRunningTotal}{kEquation[nIndex]}")
            bFound = equationSolveable(kEquation, nIndex + 1, (nRunningTotal * (10 ** (1 + int(math.log10(kEquation[nIndex]))))) + kEquation[nIndex], bPartTwo=bPartTwo)
            if bFound :
                return True
            #end
        #end

        # Add
        bFound = equationSolveable(kEquation, nIndex + 1, nRunningTotal + kEquation[nIndex], bPartTwo=bPartTwo)
        if bFound :
            return True
        #end

        # Multiply
        bFound = equationSolveable(kEquation, nIndex + 1, nRunningTotal * kEquation[nIndex], bPartTwo=bPartTwo)
        if bFound :
            return True
        #end

        return False

    #end

    kCalibrationEquations = []
    with open("../input.txt", "r") as inputFile:

        kCalibrationEquations = [
            [int(kTokens[0])] + [int(k) for k in kTokens[1].split(" ")]
               for line in inputFile.readlines()
                  for kTokens in [line.split(": ")]
        ]
#        for line in inputFile.readlines() :
#            kTokens = line.split(": ")
#            kCalibrationEquations.append([int(kTokens[0])] + [int(k) for k in kTokens[1].split(" ")])
#        #end

    #end

    # Solve Part One (+ and * only)
    kSolveableEquations = [equationSolveable(k) for k in kCalibrationEquations]
    nPartOne            = sum([k[0] for b,k in zip(kSolveableEquations, kCalibrationEquations) if b])
    print(f"Part 1: {nPartOne}")

    # Solve Part Two (any unsolved from part one now allowing the concat operator)
    kSolveableEquations = [equationSolveable(k, bPartTwo=True) if not b else True for b,k in zip(kSolveableEquations, kCalibrationEquations)]
    nPartTwo            = sum([k[0] for b,k in zip(kSolveableEquations, kCalibrationEquations) if b])
    print(f"Part 2: {nPartTwo}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end