import time
import sys
import math

def main() :

    def equationSolveable(kEquation : list[int], nIndex : int, nRunningTotal, bPartTwo : bool = False) :

        if nIndex == 0 :
            if nRunningTotal == 0 :
                return True
            #end
            return False
        elif nRunningTotal < 0 :
            return False
        #end

        # Note: The core premise here is to actually work backwards through the equation, starting from the
        #       target value and working back towards zero.
        #
        #       - Addition becomes subtraction.
        #       - Multiplication becomes division (with a simple modulo check to determine if its valid or not).
        #       - Concatenation becomes a base 10 shift (with a simple modulo check to determine if its valid or not).
        #
        #       What this allows is quicker discards for Multiply/Concatenation using modulo, as anywhere with a
        #       remainder will never be valid, since when working forwards, it would have been impossible to
        #       have a fractional value.

        # Shared between "Addition" and "Concatention"
        nSubtractedFromTotal = nRunningTotal - kEquation[nIndex]

        # Perform the Concatenation first for Part Two since we know the solution
        # requires it.
        if bPartTwo :

            # Denominator for Module Check
            nDenominator = (10 ** (1 + int(math.log10(kEquation[nIndex]))))

            if 0 == (nSubtractedFromTotal % nDenominator) :
                if equationSolveable(kEquation, nIndex - 1, nSubtractedFromTotal // nDenominator, bPartTwo=bPartTwo) :
                    return True
                #end
            #end

        #end

        # Add
        bFound = equationSolveable(kEquation, nIndex - 1, nSubtractedFromTotal, bPartTwo=bPartTwo)
        if bFound :
            return True
        #end

        # Multiply
        if 0 == (nRunningTotal % kEquation[nIndex]) :
            if equationSolveable(kEquation, nIndex - 1, nRunningTotal // kEquation[nIndex], bPartTwo=bPartTwo) :
                return True
            #end
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
    kSolveableEquations = [equationSolveable(k, len(k) - 1, k[0]) for k in kCalibrationEquations]
    nPartOne            = sum([k[0] for b,k in zip(kSolveableEquations, kCalibrationEquations) if b])
    print(f"Part 1: {nPartOne}")

    # Solve Part Two (any unsolved from part one now allowing the concat operator)
    kSolveableEquations = [equationSolveable(k, len(k) - 1, k[0], bPartTwo=True) if not b else True for b,k in zip(kSolveableEquations, kCalibrationEquations)]
    nPartTwo            = sum([k[0] for b,k in zip(kSolveableEquations, kCalibrationEquations) if b])
    print(f"Part 2: {nPartTwo}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end