import time
import sys

def main() :

    kPartsRawGrid = []
    with open("../input.txt", "r") as inputFile:
        kPartsRawGrid = [k.replace("\n", "") for k in inputFile.readlines()]
    #end

    # Find all the Part ID's, Parts and Gears
    kParts           = set()
    kPartIDs         = []
    kPartIDsIndexed  = {}
    nPartIDCount     = 0
    kGears           = set()

    # Extract the Relevant Data for Part 1
    for y, kLine in enumerate(kPartsRawGrid) :

        kPartId = ""

        # Note: To prevent repeating the test for when a Part ID has completed, we pad the line
        #       with an additional "." at the end.
        for x, kToken in enumerate(kLine + ".") :

            if kToken.isdigit() :

                kPartId += kToken
                kPartIDsIndexed[(y, x)] = nPartIDCount

            else :

                if kPartId != "" :
                    kPartIDs.append({"Y" : y, "X_START" : x - len(kPartId), "X_END" : x, "VALUE" : int(kPartId)})
                    kPartId = ""
                    nPartIDCount += 1
                #end

                if kToken == "." : continue
                if kToken == "*" : kGears.add((y, x))
                kParts.add((y, x))

            #end

        #end

    #end

    # Part 1: Sum all the Part ID's
    nPartOneSum = 0
    for kPartId in kPartIDs :
        bFound = False
        for Y in range(kPartId["Y"] - 1, kPartId["Y"] + 2, 1) :
            for X in range(kPartId["X_START"] - 1, kPartId["X_END"] + 1, 1) :
                if (Y, X) in kParts :
                    nPartOneSum += kPartId["VALUE"]
                    bFound       = True
                    break
                #end
            else :
                if bFound :
                    break
            #end
        #end
    #end

    print(f"Part 1: {nPartOneSum}")

    # Part 2: Calculate the Sum of all the Gear Ratios
    nPart2GearRatioSum = 0
    for kGear in kGears :

        kUniqueParts = set()
        for Y in range(kGear[0] - 1, kGear[0] + 2, 1) :
            for X in range(kGear[1] - 1, kGear[1] + 2, 1) :
                if (Y, X) in kPartIDsIndexed :
                    kUniqueParts.add(kPartIDsIndexed[(Y, X)])
                #end
            #end
        #end

        if len(kUniqueParts) == 2 :
            nPart2GearRatioSum += kPartIDs[kUniqueParts.pop()]["VALUE"] * kPartIDs[kUniqueParts.pop()]["VALUE"]
        #end

    #end

    print(f"Part 2: {nPart2GearRatioSum}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
