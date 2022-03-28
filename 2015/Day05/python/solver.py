import time
import sys

def main() :

    # Read the Input File
    santasList = []
    with open("../input.txt", "r") as inputFile:
        santasList = [line.strip() for line in inputFile.readlines()]
    #end

    def isNicePart1(name) :
        VOWELS     = "aeiou"
        BAD_WORDS  = ["ab", "cd", "pq", "xy"]
        for BADWORD in BAD_WORDS :
            if BADWORD in name : return False
        #end
        return ((sum([1 if C in VOWELS else 0 for C   in name]) >= 3) and
                (sum([1 if C==D        else 0 for C,D in zip(name[0:-1],name[1:])]) >= 1))
    #end

    def isNicePart2(name) :

        # Repeated Words
        found = False
        for i in range(len(name)-3) :
            if any([(name[i] == C and name[i+1] == D) for C,D in zip(name[i+2:-1], name[i+3:])]) :
                found = True
                break
            #end
        #end

        # Sandwiched Letter
        if found :
            found = False
            for i in range(1, len(name)-1) :
                if name[i-1] == name[i+1] :
                    found = True
                    break
                #end
            #end
        #end

        return found

    #end

    nicePart1 = 0
    nicePart2 = 0

    for name in santasList :
        if isNicePart1(name) : nicePart1 += 1
        if isNicePart2(name) : nicePart2 += 1
    #end

    print(f"Part 1: {nicePart1}")
    print(f"Part 2: {nicePart2}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
