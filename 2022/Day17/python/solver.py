import time
import sys

ROCKS = [
            [[True , True,  True , True]],  # ####

            [[False, True , False],         # .#.
             [True , True , True ],         # ###
             [False, True , False]],        # .#.

            [[False, False, True ],         # ..#
             [False, False, True ],         # ..#
             [True , True , True ]],        # ###

            [[True ],                       # #
             [True ],                       # #
             [True ],                       # #
             [True ]],                      # #

            [[True , True ],                # ##
             [True , True ]]                # ##
        ]

def main() :

    with open("../input.txt", "r") as inputFile:

        MINX = 0
        MAXX = 6

        def getTowerHeight(nRockTarget, kJetStreamPattern) :

            def getRow(Y, kCollideableAreas) :
                kRow = ""
                for X in range (MINX, MAXX + 1) :
                    if tuple([X, Y]) in kCollideableAreas :
                        kRow += "#"
                    else :
                        kRow += "."
                    #end
                #end
                return kRow
            #end

            def printCollisions(kCollideableAreas) :

                MINY = min([kPosition[1] for kPosition in kCollideableAreas])
                MAXY = max([kPosition[1] for kPosition in kCollideableAreas])

                for Y in range(MAXY, MINY - 1, -1) :
                    print("|", end="")
                    print(getRow(Y, kCollideableAreas), end="")
                    print("|")
                #end
                
                print("+-------+")
                print("")

            #end

            def hasCollided(nReferenceX, nReferenceY, kCollideableAreas, nRockType) :
                for Y,kCollisionRow in enumerate(ROCKS[nRockType]) :
                    for X,bCollision in enumerate(kCollisionRow) :
                        if bCollision :
                            kPosition = tuple([ X + nReferenceX,
                                               -Y + nReferenceY +
                                               len(ROCKS[nRockType]) - 1])
                            if kPosition in kCollideableAreas :
                                return True
                            #end
                        #end
                    #end
                #end
                return False
            #end

            def updateCollideablePattern(nReferenceX, nReferenceY, kCollideableAreas, nRockType, nHighestRockPoint) :
                for Y,kCollisionRow in enumerate(ROCKS[nRockType]) :
                    for X,bCollision in enumerate(kCollisionRow) :
                        if bCollision :
                            kPosition = tuple([ X + nReferenceX,
                                               -Y + nReferenceY +
                                               len(ROCKS[nRockType]) - 1])
                            kCollideableAreas.add(kPosition)
                            if (kPosition[1] + 1) > nHighestRockPoint :
                                nHighestRockPoint = (kPosition[1] + 1)
                            #end
                        #end
                    #end
                #end
                return nHighestRockPoint
            #end

            # Everything to do with Rock State
            nRockCount          = 0
            nCurrentJetStream   = -1
            kRockPosition       = None
            kCollideableAreas   = set()
            nRockType           = -1
            nHighestRockPoint   = 0
            bRockFalling        = False

            # These are for the Loop Intervals
            # In a nutshell, the Height Deltas stores how (if at all) each rock in turn modifies
            # the current height.  The Offset/Intervals can index into this to determine what
            # a given count would actually achieve.
            kHeightDeltas       = []
            bLoopFound          = False
            nFoundLoopOffset    = 0
            nFoundLoopInterval  = 0

            # This is for loop cacheing, specifically the Delta Peaks is the lynchpin of the
            # whole algorithm, as this, when combined with jet stream position and rock type
            # will uniquely describe a game state at a given time, which will then allow
            # an easy method to determine when a repeated state is encountered.  From this
            # we can determine offset/intervals.
            kPeaks             = [nHighestRockPoint-1 for _ in range(MINX, MAXX + 1)]
            kDeltaPeaks        = None
            kBetterCacheData   = {}
            bLoggingPeaks      = False

            # Process each rock until we find a loop or process all the rocks we were asked to.
            while nRockCount <= nRockTarget :

                # Star the Next Rock if needed...
                if False == bRockFalling :

                    # Update the Peak Heights.  For speed, we store absolute Y positions
                    # to limit how much needs to be checked with respect to collisions.
                    for X in range(len(kPeaks)) :
                        for Y in range(kPeaks[X], nHighestRockPoint + 1) :
                            kPosition = tuple([X,Y])
                            if kPosition in kCollideableAreas :
                                kPeaks[X] = Y
                            #end
                        #end
                    #end

                    # From the Peaks we can derive the Peaks as Deltas.
                    # Initially, not all of the floor will have a rock, so skip calculating
                    # delta's until that happens, otherwise we might get weird false positives.
                    if False == bLoggingPeaks :
                        bLoggingPeaks = all([nPeak >= 0 for nPeak in kPeaks])
                    #end
                    if True == bLoggingPeaks :
                        kDeltaPeaks = [nHighestRockPoint - nPeak for nPeak in kPeaks]
                    #end

                    # Start the next rock in the appropriate position, and adjust type/counter
                    nCeiling        = nHighestRockPoint + 3
                    kRockPosition   = [2, nCeiling]
                    nRockType       = (nRockType + 1) % len(ROCKS)
                    nRockCount     += 1
                    bRockFalling    = True

                #end

                # Process the Jet Stream
                nCurrentJetStream = (nCurrentJetStream + 1) % len(kJetStreamPattern)
                nXDirection       = kJetStreamPattern[nCurrentJetStream]

                # Inidicate we're trying to Adjust X/Y
                bAdjustX = True
                bAdjustY = True

                # Wall Collisions (X)
                if (kRockPosition[0] + nXDirection                            ) < 0 : bAdjustX = False
                if (kRockPosition[0] + nXDirection + len(ROCKS[nRockType][-1])) > 7 : bAdjustX = False

                # Rock Collisions (X)
                if True == bAdjustX :
                    if False == hasCollided(kRockPosition[0] + nXDirection,
                                            kRockPosition[1],
                                            kCollideableAreas,
                                            nRockType) :
                        kRockPosition[0] += nXDirection
                    #end
                #end

                # Floor / Rock Collision (Y)
                if (kRockPosition[1] - 1) < 0 : bAdjustY = False
                if True == bAdjustY :
                    if False == hasCollided(kRockPosition[0],
                                            kRockPosition[1] - 1,
                                            kCollideableAreas,
                                            nRockType) :
                        kRockPosition[1] -= 1
                    else :
                        bAdjustY = False
                    #end
                #end

                # If we couldn't adjust Y, then the rock has settled.
                if False == bAdjustY :

                    # Calculate the Cached Data we need for the Loop Check, being row state and height delta
                    nOldHeight        = nHighestRockPoint
                    nHighestRockPoint = updateCollideablePattern(kRockPosition[0], kRockPosition[1], kCollideableAreas, nRockType, nHighestRockPoint)

                    nDelta = nHighestRockPoint - nOldHeight
                    kHeightDeltas.append(nDelta)

                    # Better cached data...
                    # This stores: Start Jet Stream Position, Rock Type, and the Relative Offsets from the Highest Rock Point of each of the Peaks.
                    #              This way I don't *have* to test loops, the instant I get a hit in the dictionary, it means I've found the earliest
                    #              possible interval.
                    if None != kDeltaPeaks :

                        # Test to see if we've seen this state before...
                        kBetterKey = tuple([nCurrentJetStream, nRockType] + kDeltaPeaks)
                        if kBetterKey not in kBetterCacheData :
                            kBetterCacheData[kBetterKey] = nRockCount - 1
                        else :
                            # If so, we've found a loop!
                            bLoopFound = True
                            nFoundLoopOffset = kBetterCacheData[kBetterKey]
                            nFoundLoopInterval = nRockCount - 1 - kBetterCacheData[kBetterKey]
                            break
                        #end
                    #end

                    # Indicate we need to drop the next rock
                    bRockFalling = False

                #end

            #end

            # Either return the Highest Rock Point found from manual processing...
            if False == bLoopFound :

                return nHighestRockPoint

            # Or from the Loop Data
            else :

                # Note: Just by getting here, the rock count was high enough to find a complete loop.
                #
                # Calculate the Value based on the Loop Data
                # Because we've found a loop interval, we just need to describe the heights using 5
                # values.
                #
                # Initial        - How many Rocks dropped before we started the first loop
                # Interval       - How many Rocks drop before we repeat
                # Interval Count - How many complete Intervals are possible with the Target Rock Count
                # Remainder      - What is the remaining Rock Count after the last complete Interval
                #
                # Because we have the Height Delta array describing how each rock drop impacting the
                # highest rock count as a delta, we can use these with the above values to derive:
                #
                # Initial Height
                # Interval Height
                # Remaining Hight
                #
                # With the total Height being:
                #
                # Initial Height + Remaining Height + (Interval Count * Interval Height)

                nInitialHeight   = sum(kHeightDeltas[0:nFoundLoopOffset])
                nLoopHeight      = sum(kHeightDeltas[nFoundLoopOffset:(nFoundLoopOffset+nFoundLoopInterval)])
                nIterations      = (nRockTarget - nFoundLoopOffset) // nFoundLoopInterval
                nRemainder       = (nRockTarget - nFoundLoopOffset) % nFoundLoopInterval
                nRemainderHeight = sum(kHeightDeltas[nFoundLoopOffset:(nFoundLoopOffset+nRemainder)])

                return nInitialHeight + (nLoopHeight * nIterations) + nRemainderHeight

            #end

        #end

        kJetStreamPattern = [-1 if k == "<" else 1 for k in inputFile.readline().strip()]
        print(f"Part 1: {getTowerHeight(2022,          kJetStreamPattern)}")
        print(f"Part 2: {getTowerHeight(1000000000000, kJetStreamPattern)}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
