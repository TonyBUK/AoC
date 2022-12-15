import time
import sys
import math

SENSOR_X  = 0
SENSOR_Y  = 1
BEACON_X  = 2
BEACON_Y  = 3
TGT_ROW   = 2000000
TGT_RANGE = range(0, 4000000 + 1)

def main() :

    with open("../input.txt", "r") as inputFile:

        def calculateManhattenDistance(kItem) :
            return abs(kItem[SENSOR_X] - kItem[BEACON_X]) + abs(kItem[SENSOR_Y] - kItem[BEACON_Y])
        #end

        def getManhattenXBoundsForBeacon(kItem, nRow) :

            nManhattenDistance = calculateManhattenDistance(kItem)
            if abs(kItem[SENSOR_Y] - nRow) > nManhattenDistance :
                return []
            else :
                nX = nManhattenDistance - abs(kItem[SENSOR_Y] - nRow)
                return [-nX + kItem[SENSOR_X], nX + kItem[SENSOR_X]]
            #end

        #end

        def getRow(nTargetRow, kBeacons) :

            # Get Bounds for the Row
            kBounds         = [getManhattenXBoundsForBeacon(kItem, nTargetRow) for kItem in kBeacons]
            kBounds         = [tuple(kBound) for kBound in kBounds if kBound != []]
            kCombinedBounds = set([kBound for kBound in kBounds])

            # Combine the Bounds
            while True :
                bMerged = False
                for i, kBound1 in enumerate(kBounds) :
                    for kBound2 in kBounds[i+1:] :

                        if min(kBound1[-1], kBound2[-1]) >= max(kBound1[0], kBound2[0]) :
                            kNewBound = tuple([min(kBound1[0], kBound2[0]), max(kBound1[1], kBound2[1])])
                            if kBound1 in kCombinedBounds : kCombinedBounds.remove(kBound1)
                            if kBound2 in kCombinedBounds : kCombinedBounds.remove(kBound2)
                            if kNewBound not in kCombinedBounds :
                                kCombinedBounds.add(kNewBound)
                                bMerged = True
                            #end
                        #end
                    #end
                #end
                if bMerged :
                    kBounds         = list(kCombinedBounds)
                    kCombinedBounds = set([kBound for kBound in kBounds])
                    continue
                #end
                break
            #end

            return kBounds

        #end

        def getYIntersections(kBeaconPlusManhatten1, kBeaconPlusManhatten2) :

            def lineIntersection(line1, line2):

                xdiff = (line1[0][0] - line1[1][0], line2[0][0] - line2[1][0])
                ydiff = (line1[0][1] - line1[1][1], line2[0][1] - line2[1][1])

                def det(a, b):
                    return a[0] * b[1] - a[1] * b[0]
                #end

                div = det(xdiff, ydiff)
                if div == 0:
                    return []
                #end

                d = (det(*line1), det(*line2))
                x = det(d, xdiff) / div
                y = det(d, ydiff) / div
                return [x, y]

            #end

            kIntersections         = set()
            kAdjacentIntersections = set()
            kBeacon1Lines  = [
                              [[kBeaconPlusManhatten1[0][SENSOR_X]                           , kBeaconPlusManhatten1[0][SENSOR_Y] + kBeaconPlusManhatten1[1]],
                               [kBeaconPlusManhatten1[0][SENSOR_X] + kBeaconPlusManhatten1[1], kBeaconPlusManhatten1[0][SENSOR_Y]                           ]],

                              [[kBeaconPlusManhatten1[0][SENSOR_X] + kBeaconPlusManhatten1[1], kBeaconPlusManhatten1[0][SENSOR_Y]                           ],
                               [kBeaconPlusManhatten1[0][SENSOR_X]                           , kBeaconPlusManhatten1[0][SENSOR_Y] - kBeaconPlusManhatten1[1]]],

                              [[kBeaconPlusManhatten1[0][SENSOR_X]                           , kBeaconPlusManhatten1[0][SENSOR_Y] - kBeaconPlusManhatten1[1]],
                               [kBeaconPlusManhatten1[0][SENSOR_X] - kBeaconPlusManhatten1[1], kBeaconPlusManhatten1[0][SENSOR_Y]                           ]],

                              [[kBeaconPlusManhatten1[0][SENSOR_X] - kBeaconPlusManhatten1[1], kBeaconPlusManhatten1[0][SENSOR_Y]                           ],
                               [kBeaconPlusManhatten1[0][SENSOR_X]                           , kBeaconPlusManhatten1[0][SENSOR_Y] + kBeaconPlusManhatten1[1]]]
                             ]

            kBeacon2Lines  = [
                              [[kBeaconPlusManhatten2[0][SENSOR_X]                           , kBeaconPlusManhatten2[0][SENSOR_Y] + kBeaconPlusManhatten2[1]],
                               [kBeaconPlusManhatten2[0][SENSOR_X] + kBeaconPlusManhatten2[1], kBeaconPlusManhatten2[0][SENSOR_Y]                           ]],

                              [[kBeaconPlusManhatten2[0][SENSOR_X] + kBeaconPlusManhatten2[1], kBeaconPlusManhatten2[0][SENSOR_Y]                           ],
                               [kBeaconPlusManhatten2[0][SENSOR_X]                           , kBeaconPlusManhatten2[0][SENSOR_Y] - kBeaconPlusManhatten2[1]]],

                              [[kBeaconPlusManhatten2[0][SENSOR_X]                           , kBeaconPlusManhatten2[0][SENSOR_Y] - kBeaconPlusManhatten2[1]],
                               [kBeaconPlusManhatten2[0][SENSOR_X] - kBeaconPlusManhatten2[1], kBeaconPlusManhatten2[0][SENSOR_Y]                           ]],

                              [[kBeaconPlusManhatten2[0][SENSOR_X] - kBeaconPlusManhatten2[1], kBeaconPlusManhatten2[0][SENSOR_Y]                           ],
                               [kBeaconPlusManhatten2[0][SENSOR_X]                           , kBeaconPlusManhatten2[0][SENSOR_Y] + kBeaconPlusManhatten2[1]]]
                             ]

            for kLine1 in kBeacon1Lines :
                for kLine2 in kBeacon2Lines :
                    kCurrentIntersection = lineIntersection(kLine1, kLine2)
                    if len(kCurrentIntersection) > 0 :
                        if 0 != (kCurrentIntersection[1] % 1) :
                            nFloor                        = int(math.floor(kCurrentIntersection[1]))
                            kIntersectionRange            = set()
                            kAdjacentIntegersectionRange  = set()
                            kAdjacentIntegersectionRange.add(nFloor + 1)
                            kAdjacentIntegersectionRange.add(nFloor    )
                            kAdjacentIntegersectionRange.add(nFloor + 2)
                            kAdjacentIntegersectionRange.add(nFloor - 1)
                        else :
                            nIntersectionRange            = int(kCurrentIntersection[1])
                            kIntersectionRange            = set([nIntersectionRange])
                            kAdjacentIntegersectionRange  = set()
                            kAdjacentIntegersectionRange.add(nIntersectionRange + 1)
                            kAdjacentIntegersectionRange.add(nIntersectionRange - 1)
                        #end
                        kIntersections         |= kIntersectionRange
                        kAdjacentIntersections |= kAdjacentIntegersectionRange
                    #end
                #end
            #end

            return kIntersections, kAdjacentIntersections

        #end

        # Why....
        kBeacons = [[int(kItem) for i,kItem in enumerate(kLine.strip().replace(" ", "").replace("=",",").replace(":",",").split(",")) if i in [1,3,5,7]] for kLine in inputFile.readlines()]

        # Store the co-ordinates of all of the beacons in that range
        kBounds       = getRow(TGT_ROW, kBeacons)
        kRanges       = [range(x[0], x[1] + 1) for x in kBounds]
        kBeaconsOnRow = set([X[BEACON_X] for X in kBeacons if X[BEACON_Y] == TGT_ROW])
        nCount        = sum(kBound[1] - kBound[0] + 1 for kBound in kBounds)

        for nBeacon in kBeaconsOnRow :
            for kRange in kRanges :
                if nBeacon in kRange :
                    nCount -= 1
                #end
            #end
        #end

        print(f"Part 1: {nCount}")

        # Determine Y Ranges of Interest

        # First grab the Peaks
        kBeaconsPlusManhatten = [[kBeacon, calculateManhattenDistance(kBeacon)] for kBeacon in kBeacons]
        kYRowsOfInterest      = [[kBeaconPlusManhatten[0][SENSOR_Y] + kBeaconPlusManhatten[1] - 1,
                                  kBeaconPlusManhatten[0][SENSOR_Y] + kBeaconPlusManhatten[1],
                                  kBeaconPlusManhatten[0][SENSOR_Y] + kBeaconPlusManhatten[1] + 1,
                                  kBeaconPlusManhatten[0][SENSOR_Y] - kBeaconPlusManhatten[1] - 1,
                                  kBeaconPlusManhatten[0][SENSOR_Y] - kBeaconPlusManhatten[1],
                                  kBeaconPlusManhatten[0][SENSOR_Y] - kBeaconPlusManhatten[1] + 1,
                                  kBeaconPlusManhatten[0][SENSOR_Y] - 1,
                                  kBeaconPlusManhatten[0][SENSOR_Y],
                                  kBeaconPlusManhatten[0][SENSOR_Y] + 1] for kBeaconPlusManhatten in kBeaconsPlusManhatten]
        kPriorityQueue  = set()
        kSecondaryQueue = set([kSub for kEntry in kYRowsOfInterest for kSub in kEntry if kSub in TGT_RANGE])

        # Next Grab the Intersections
        for i, kBeaconPlusManhatten1 in enumerate(kBeaconsPlusManhatten) :
            for kBeaconPlusManhatten2 in kBeaconsPlusManhatten[i+1:] :
                kIntersections, kAdjacentIntersections = getYIntersections(kBeaconPlusManhatten1, kBeaconPlusManhatten2)
                kPriorityQueue  |= set([nIntersection for nIntersection in kIntersections         if nIntersection in TGT_RANGE])
                kSecondaryQueue |= set([nIntersection for nIntersection in kAdjacentIntersections if nIntersection in TGT_RANGE])
            #end
        #end

        bFound = False
        for Y in kPriorityQueue :

            kBounds = getRow(Y, kBeacons)
            if len(kBounds) > 1 :
                kBounds = sorted(kBounds)
                assert(abs(kBounds[0][1] - kBounds[1][0]) == 2)
                assert(kBounds[0][1] in TGT_RANGE)
                bFound = True
                print(f"Part 2: {((kBounds[0][1] + 1) * 4000000) + Y}")
                break
            #end

        #end

        if False == bFound :
            for Y in kSecondaryQueue :

                kBounds = getRow(Y, kBeacons)
                if len(kBounds) > 1 :
                    kBounds = sorted(kBounds)
                    assert(abs(kBounds[0][1] - kBounds[1][0]) == 2)
                    assert(kBounds[0][1] in TGT_RANGE)
                    bFound = True
                    print(f"Part 2: {((kBounds[0][1] + 1) * 4000000) + Y}")
                    break
                #end

            #end
        #end

        assert(bFound)

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
