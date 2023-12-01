import time
import sys

def main() :

    kLines = []
    with open("../input.txt", "r") as inputFile:
        kLines = inputFile.readlines()
    #end

    kCalibrationValuesPartOne = 0
    kCalibrationValuesPartTwo = 0

    DIGIT_WORDS = ["one", "two", "three", "four", "five", "six", "seven", "eight", "nine"]

    for kLine in kLines :

        # Part 1
        # Extract digits and add to sum
        kDigits = [k for k in kLine if k.isdigit()]
        kCalibrationValuesPartOne += int(kDigits[0] + kDigits[-1])

        # Part 2

        # Substitute digit words with digits
        kPartTwoLine = ""
        for i in range(len(kLine)) :
            kPositions = [kLine[i:].find(k) for k in DIGIT_WORDS]
            if 0 in kPositions :
                kPartTwoLine += str(kPositions.index(0) + 1)
            else :
                kPartTwoLine += kLine[i]
            #end
        #end

        # Extract digits and add to sum
        kDigits = [k for k in kPartTwoLine if k.isdigit()]
        kCalibrationValuesPartTwo += int(kDigits[0] + kDigits[-1])

    #end

    print(f"Part 1: {kCalibrationValuesPartOne}")
    print(f"Part 2: {kCalibrationValuesPartTwo}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
