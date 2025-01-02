import time
import sys

def main() :

    kSecretNumbers = []
    with open("../input.txt", "r") as inputFile:
        kSecretNumbers = [int(k) for k in inputFile.read().split("\n")]
    #end

    def calcateSecretNumber(nNumber : int, nIterations : int) -> list[int] :

        kSecretNumbers = [nNumber]
        nSecretNumber  = nNumber

        for _ in range(nIterations) :
            nSecretNumber = ((nSecretNumber <<  6) ^ nSecretNumber) & 0xFFFFFF # Equivalent to ((x  *   64) ^ x) % 0x1000000
            nSecretNumber = ((nSecretNumber >>  5) ^ nSecretNumber) & 0xFFFFFF # Equivalent to ((x //   32) ^ x) % 0x1000000
            nSecretNumber = ((nSecretNumber << 11) ^ nSecretNumber) & 0xFFFFFF # Equivalent to ((x  * 2048) ^ x) % 0x1000000
            kSecretNumbers.append(nSecretNumber)
        #end

        return kSecretNumbers

    #end

    # Part One
    kSecretNumberSequences = [calcateSecretNumber(x, 2000) for x in kSecretNumbers]
    print(f"Part 1: {sum([x[-1] for x in kSecretNumberSequences])}")

    # Part Two

    # Extract the unit digit in each sequence, and calculate as a delta
    kSecretDigitSequences  = [[x % 10 for x in kSecretNumberSequence] for kSecretNumberSequence in kSecretNumberSequences]
    kSecretChangeSequences = [[y - x for x,y in zip(kSecretDigitSequence, kSecretDigitSequence[1:])] for kSecretDigitSequence in kSecretDigitSequences]

    # Cost for all possible sequences
    kBananaCost = {}

    # For each change sequence...
    for i,kChangeSequence in enumerate(kSecretChangeSequences) :

        # Ignore all but the first seen sequence
        kSeenSequence = set()

        # For each sequence of 4 digits...
        for j,(a,b,c,d) in enumerate(zip(kChangeSequence, kChangeSequence[1:], kChangeSequence[2:], kChangeSequence[3:])) :

            # If it's the first encounter, adjust the banana cost
            kSequence = (a,b,c,d)
            if kSequence not in kSeenSequence :
                kSeenSequence.add(kSequence)
                kBananaCost[kSequence] = kBananaCost.get(kSequence, 0) + kSecretDigitSequences[i][j+4]
            #end

        #end

    #end

    # Find the maximum cost
    print(f"Part 2: {max(kBananaCost.values())}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end