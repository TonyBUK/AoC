import time
import sys
import math
import copy
 
def main() :

    kSeeds    = []
    kMaps     = {}
    kMappings = {}
    with open("../input.txt", "r") as inputFile:

        kLastMap = None

        for kLine in inputFile :

            kLine = kLine.replace("\n", "")
            if kLine.endswith(" map:") :

                kMapTokens = tuple(kLine.split(" map:")[0].split("-to-"))
                assert(len(kMapTokens) == 2)

                assert(kMapTokens[0] not in kMappings)
                kMappings[kMapTokens[0]] = kMapTokens[1]
                kMaps[kMapTokens]        = []

                kLastMap                 = kMapTokens

            elif kLine.startswith("seeds: ") :

                kSeeds   = [int(kSeed) for kSeed in kLine.split("seeds: ")[1].split(" ")]
                kLastMap = (None, None)

            elif None != kLastMap :

                if len(kLine) == 0 :
                    kLastMap = None
                    continue
                #end

                kValues = [int(kValue) for kValue in kLine.split(" ")]
                assert(len(kValues) == 3)

                kMaps[kLastMap].append({"DEST_RANGE_START"   : kValues[0],
                                        "SOURCE_RANGE_START" : kValues[1],
                                        "RANGE_LENGTH"       : kValues[2]})
            
            else :

                assert(False)

            #end

        #end

    #end

    # Sanity check the basics
    assert("seed"     in kMappings.keys())
    assert("location" in kMappings.values())

    nLowestLocation = math.inf

    for nSeed in kSeeds :

        kCurrentMapSource = "seed"
        kCurrentMapDest   = None
        nCurrentValue     = nSeed

        while kCurrentMapDest != "location" :

            kCurrentMapDest = kMappings[kCurrentMapSource]
            kCurrentMap     = (kCurrentMapSource, kCurrentMapDest)

            assert(kCurrentMap in kMaps.keys())

            for kMapping in kMaps[kCurrentMap] :

                if nCurrentValue in range(kMapping["SOURCE_RANGE_START"], kMapping["SOURCE_RANGE_START"] + kMapping["RANGE_LENGTH"]) :
                    nCurrentValue = kMapping["DEST_RANGE_START"] + (nCurrentValue - kMapping["SOURCE_RANGE_START"])
                    break
                #end

            #end

            # Move onto the next map in the sequence
            kCurrentMapSource = kCurrentMapDest

        #end

        # Store the Lowest Location so far
        if nCurrentValue < nLowestLocation :
            nLowestLocation = nCurrentValue
        #end

    #end

    print(f"Part 1: {nLowestLocation}")

    # Part 2 - Seeds are now start/ranges

    def findIntersection(kSourceRangeStart, kSourceRangeLength, kTestRangeStart, kTestRangeLength) :

        return range(max(kSourceRangeStart, kTestRangeStart), min(kSourceRangeStart  + kSourceRangeLength,
                                                                  kTestRangeStart    + kTestRangeLength))

    #end

    def decimateIntersectino(kSourceRange, kTestRangeStart, kTestRangeLength) :

        kRange = findIntersection(kSourceRange["START"], kSourceRange["LENGTH"], kTestRangeStart, kTestRangeLength)

        if len(kRange) == 0 :

            # No Overlap, keep all of the data
            return [kSourceRange]

        else :

            # Some Overlap, remove the overlapped part
            kRanges = []

            # If we have data before the overlap, preserve the pre-data
            if kRange[0] > kSourceRange["START"] :
                kRanges.append({"START" : kSourceRange["START"], "LENGTH" : kRange[0] - kSourceRange["START"] - 1})
            #end

            # If we have data after the overlap, preserve the post-data
            if (kRange[-1] + 1) < (kSourceRange["START"] + kSourceRange["LENGTH"]) :
                kRanges.append({"START" : kRange[-1] + 1, "LENGTH" : (kSourceRange["START"] + kSourceRange["LENGTH"]) - (kRange[-1] + 1)})
            #end

            # Debug Code - Do not run on real data!!!
            """
            print("Old Range     = ", end="")
            for i in range(kSourceRange["START"], kSourceRange["START"] + kSourceRange["LENGTH"] + 1) :
                print(f"{i} ", end=" ")
            #end
            print("")

            print("Test Range    = ", end="")
            for i in range(kTestRangeStart, kTestRangeStart + kTestRangeLength + 1) :
                print(f"{i} ", end=" ")
            #end
            print("")

            print("Overlap Range = ", end="")
            for i in kRange :
                print(f"{i} ", end=" ")
            #end
            print("")

            print("New Range     = ", end="")
            for k in kRanges :
                for i in range(k["START"], k["START"] + k["LENGTH"] + 1) :
                    print(f"{i} ", end=" ")
                #end
            #end
            print("")
            print("")
            """

            return kRanges

        #end

    #end

    assert((len(kSeeds) % 2) == 0)
    nLowestLocation = math.inf

    # Re-interpret Seeds as start/ranges
    kSeeds          = [{"START" : kSeeds[nIndex * 2], "LENGTH" : kSeeds[(nIndex * 2) + 1]} for nIndex in range(int(len(kSeeds) / 2))]

    for kSeed in kSeeds :

        kCurrentMapSource = "seed"
        kCurrentMapDest   = None
        kCurrentRanges    = [kSeed]

        # This copy is meant to store the seeds have yet to be found within the original list
        kRangessDecimated = copy.deepcopy(kCurrentRanges)

        while kCurrentMapDest != "location" :

            kCurrentMapDest = kMappings[kCurrentMapSource]
            kCurrentMap     = (kCurrentMapSource, kCurrentMapDest)

            assert(kCurrentMap in kMaps.keys())

            kRangessDecimated = copy.deepcopy(kCurrentRanges)
            kNewRanges        = []

            for kMapping in kMaps[kCurrentMap] :

                for kCurrentRange in kCurrentRanges :

                    # Calculate the intersecting range
                    kRange = findIntersection(kCurrentRange["START"], kCurrentRange["LENGTH"], kMapping["SOURCE_RANGE_START"], kMapping["RANGE_LENGTH"])

                    # If there is an intersection, then create a new range based on the destination
                    if len(kRange) > 0 :

                        kNewRanges.append({"START" : kMapping["DEST_RANGE_START"] + (kRange[0] - kMapping["SOURCE_RANGE_START"]),
                                           "LENGTH" : len(kRange)})

                    #end

                #end

                # Decimate the ranges to leave behind any non-intersected ranges
                for i in reversed(range(len(kRangessDecimated))) :
                    kTestRange = kRangessDecimated[i]
                    kRangessDecimated.pop(i)
                    kRangessDecimated.extend(decimateIntersectino(kTestRange, kMapping["SOURCE_RANGE_START"], kMapping["RANGE_LENGTH"]))
                #end

            #end

            # Combine the new overlapped ranges with the non-overlapped ranges
            kCurrentRanges = kNewRanges + kRangessDecimated

            # Move onto the next map in the sequence
            kCurrentMapSource = kCurrentMapDest

        #end

        # Store the Lowest Location so far
        for kCurrentRange in kCurrentRanges :
            if kCurrentRange["START"] < nLowestLocation :
                nLowestLocation = kCurrentRange["START"]
            #end
        #end

    #end

    print(f"Part 2: {nLowestLocation}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end