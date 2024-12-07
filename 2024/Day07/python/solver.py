import time
import sys
import math

def main() :

    def equationSolveablePartOne(kEquation : list[int], nLength : int, nIndex : int = 1, nRunningTotal : int = 0, kPartOneProducts : list[int] = None) -> bool : 

        if nIndex == nLength :
            if nRunningTotal == kEquation[0] :
                return True
            #end
            return False
        elif nRunningTotal > kEquation[0] :
            return False
        #end

        # Perform the Concatenation first for Part Two since we know the solution
        # requires it.
        if nRunningTotal > 0 :

            # Quick Discard Case
            # Multiplying the Data does not reach the appropriate order of magnitude
            nWorstCaseProduct = nRunningTotal * kPartOneProducts[nIndex-1]
            if nWorstCaseProduct < kEquation[0] :
                return False
            #end

        #end

        # Add
        if equationSolveablePartOne(kEquation, nLength, nIndex + 1, nRunningTotal + kEquation[nIndex], kPartOneProducts=kPartOneProducts) :
            return True
        #end

        # Multiply
        if equationSolveablePartOne(kEquation, nLength, nIndex + 1, nRunningTotal * kEquation[nIndex], kPartOneProducts=kPartOneProducts) :
            return True
        #end

        return False

    #end

    def equationSolveablePartTwo(kEquation : list[int], nLength : int, nIndex : int = 1, nRunningTotal : int = 0, kPartTwoConcatenations : list[int] = None) -> bool : 

        if nIndex == nLength :
            if nRunningTotal == kEquation[0] :
                return True
            #end
            return False
        elif nRunningTotal > kEquation[0] :
            return False
        #end

        # Perform the Concatenation first for Part Two since we know the solution
        # requires it.

        # Quick Discard Case
        # Concatenating all remaining data does not reach the appropriate order of magnitude
        nWorstCaseConcatenation = (nRunningTotal * (10 ** (1 + int(math.log10(kPartTwoConcatenations[nIndex-1]))))) + kPartTwoConcatenations[nIndex-1]
        if nWorstCaseConcatenation < kEquation[0] :
            return False
        #end

        # Equivalent to int(f"{nRunningTotal}{kEquation[nIndex]}")
        if equationSolveablePartTwo(kEquation, nLength, nIndex + 1, (nRunningTotal * (10 ** (1 + int(math.log10(kEquation[nIndex]))))) + kEquation[nIndex], kPartTwoConcatenations=kPartTwoConcatenations) :
            return True
        #end

        # Add
        if equationSolveablePartTwo(kEquation, nLength, nIndex + 1, nRunningTotal + kEquation[nIndex], kPartTwoConcatenations=kPartTwoConcatenations) :
            return True
        #end

        # Multiply
        if equationSolveablePartTwo(kEquation, nLength, nIndex + 1, nRunningTotal * kEquation[nIndex], kPartTwoConcatenations=kPartTwoConcatenations) :
            return True
        #end

        return False

    #end

    kCalibrationEquations  = []
    kPartOneProducts       = []
    kPartTwoConcatenations = []

    with open("../input.txt", "r") as inputFile:

        for kLine in inputFile.readlines() :

            kTokens    = kLine.split(": ")
            kSubTokens = kTokens[1].replace("\n", "").split(" ")
            nLength    = len(kSubTokens)

            # Equations for both parts
            kCalibrationEquations.append([int(kTokens[0])] + [int(k) for k in kSubTokens])

            # For the Part One Quick Case Discard, we can pre-compute the worst case magitudes
            # Part Two adds a concatenation operator, we can precompute the worst case
            # substrings for the quick discard cases.
            kPartOneProducts.append([1] * len(kSubTokens))
            kPartTwoConcatenations.append([])

            for i in range(nLength) :

                for j in range(i, nLength) :

                    nValue = int(kSubTokens[j])

#                    # Times Zero should be skipped, just incase
#                    if nValue == 0 :
#                        continue
                    # Times One is better as an Addition
                    if nValue == 1 :
                        kPartOneProducts[-1][i] += 1
                    else :
                        kPartOneProducts[-1][i] *= nValue
                    #end

                #end

                kPartTwoConcatenations[-1].append(int("".join(kSubTokens[i:])))

            #end

        #end

    #end

    # Solve Part One (+ and * only)
    kSolveableEquations = [equationSolveablePartOne(k, len(k), kPartOneProducts=p) for k,p in zip(kCalibrationEquations, kPartOneProducts)]
    nPartOne            = sum([k[0] for b,k in zip(kSolveableEquations, kCalibrationEquations) if b])
    print(f"Part 1: {nPartOne}")

    # Solve Part Two (any unsolved from part one now allowing the concat operator)
    kSolveableEquations = [equationSolveablePartTwo(k, len(k), kPartTwoConcatenations=c) if not b else True for b,k,c in zip(kSolveableEquations, kCalibrationEquations, kPartTwoConcatenations)]
    nPartTwo            = sum([k[0] for b,k in zip(kSolveableEquations, kCalibrationEquations,) if b])
    print(f"Part 2: {nPartTwo}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end