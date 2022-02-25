import time
import math

def main() :

    # Read/Clean the Input File to extract the Lines
    presentTotals = []
    with open("../input.txt", "r") as inputFile:
        presentTotals = [int(line.strip()) for line in inputFile.readlines()]
    #end

    def findLowestHouseNumber(targetPresents, presentsPerHouse, housesPerElf = math.inf) :

        if targetPresents <= presentsPerHouse :
            return 1
        #end

        normalisedTargetPresents = math.ceil(targetPresents / presentsPerHouse)
        totalCounts              = [presentsPerHouse for i in range(normalisedTargetPresents+1)]

        # The Basic Algorithm to process all elves in turn, and increment the present count
        # for all possible houses they can deliver to
        for elf in range(2, normalisedTargetPresents + 1) :

            # Cache the Present Count
            elfPresents = elf * presentsPerHouse

            # For each house the elf can visit
            for house in range(1, min(normalisedTargetPresents // elf, housesPerElf) + 1) :

                # Increment the Presents for the current house
                totalCounts[house * elf] += elfPresents
                if totalCounts[elf] >= targetPresents :
                    return elf
                #end

            #end

        #end

        assert(False)

    #end

    for presentTotal in presentTotals :

        print(f"Part 1: {findLowestHouseNumber(presentTotal, 10)}")
        print(f"Part 2: {findLowestHouseNumber(presentTotal, 11, 50)}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
