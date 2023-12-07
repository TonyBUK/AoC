import time
import sys
 
def main() :
 
    kRacesPartOne = []
    kRacesPartTwo = []

    with open("../input.txt", "r") as inputFile:
 
        kLinesPartOne = [" ".join(k.split(":")[1].strip().split()).split(" ") for k in inputFile.readlines()]
        kLinesPartTwo = ["".join(x) for x in kLinesPartOne]
        assert(len(kLinesPartOne) == 2)
        assert(len(kLinesPartTwo) == 2)
        kRacesPartOne = [(int(x[0]), int(x[1])) for x in zip(kLinesPartOne[0], kLinesPartOne[1])]
        kRacesPartTwo = [int(kLinesPartTwo[0]), int(kLinesPartTwo[1])]
 
    #end
 
    def findWinnerBruteForce(kRace) :
        return sum([1 for x in zip(range(1, kRace[0]), range(kRace[0] - 1, 1, -1)) if (x[0] * x[1]) > kRace[1]])
    #end
 
    def findWinnersSearch(kRace) :
 
        def isWinner(nValue, nRaceLength, nRaceRecord) :
            return (nValue * (nRaceLength - nValue)) > nRaceRecord
        #end
 
        # Use a Binary Search to find the "tipping" point where we start winning
        nRaceMidPoint = kRace[0] // 2
        nRaceStart    = 1
        nRaceEnd      = nRaceMidPoint
 
        assert(isWinner(nRaceMidPoint, kRace[0], kRace[1]))
 
        while True :
 
            nRaceMidPoint = nRaceStart + ((nRaceEnd - nRaceStart) // 2)
            bCurrentWinner = isWinner(nRaceMidPoint, kRace[0], kRace[1])
 
            if bCurrentWinner :
 
                bPreviousWinner = isWinner(nRaceMidPoint - 1, kRace[0], kRace[1])
 
                if False == bPreviousWinner:
                    break
                #end
 
                nRaceEnd   -= (nRaceEnd - nRaceStart) // 2
 
            else :
 
                nRaceStart  += (nRaceEnd - nRaceStart) // 2
 
            #end
 
        #end    
 
        return kRace[0] - (nRaceMidPoint * 2) + 1
 
    #end
 
    nProduct = 1
    [(nProduct := nProduct * findWinnersSearch(x)) for x in kRacesPartOne]
 
    print(f"Part 1: {nProduct}")
    print(f"Part 2: {findWinnersSearch(kRacesPartTwo)}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end