import time
import sys
import math

def main() :

    with open("../input.txt", "r") as inputFile:

        def parseValve(kTokens) :
            kValves = [kToken.replace(",", "") for kToken in kTokens[9:]]
            nFlowRate = int(kTokens[4].strip("rate=").strip(";"))
            return kTokens[1], {"valves" : kValves, "flowRate" : nFlowRate}
        #end

        def shortestPath(kCurrentValve, kTargetValve, kValves) :

            kQueue          = [[1, [kCurrentValve], kCurrentValve]]
            nBestPathLength = math.inf
            kBestPath       = None

            while len(kQueue) > 0 :

                kEntry      = kQueue.pop()
                nPathLength = kEntry[0]
                kPath       = kEntry[1]
                kValve      = kEntry[2]

                # We've arrived at the Destination, cull all the no hopers
                if kTargetValve == kValve :
                    nBestPathLength = nPathLength
                    kBestPath       = [kValve for kValve in kPath]
                    kQueue          = [kEntry for kEntry in kQueue if kEntry[0] <= (nBestPathLength - 2)]
                    continue
                #end

                # Only continue if it's worthwhile to do so...
                if (nPathLength + 1) < nBestPathLength :
                    for kQueuedValve in kValves[kValve]["valves"] :
                        # Avoid looping
                        if kQueuedValve not in kPath :
                            kQueue.append([nPathLength + 1, kPath + [kQueuedValve], kQueuedValve])
                        #end
                    #end
                #end

            #end

            return kBestPath[1:]

        #end

        def bestFlowRate(kShortestMoves, kWorthwhileValves, kValves, nStartTime = 30, kVisitedSets = None) :

            kQueue        = [[0, nStartTime - len(kShortestMoves["AA"][kValve]), kValve, set()] for kValve in kWorthwhileValves]
            nBestPressure = 0

            while len(kQueue) > 0 :

                kEntry        = kQueue.pop()
                nPressure     = kEntry[0]
                nTime         = kEntry[1]
                kTargetValve  = kEntry[2]
                kOpenedValves = kEntry[3]

                if kTargetValve not in kOpenedValves :

                    kNewOpenedValves       = kOpenedValves | set([kTargetValve])
                    nNewPressure = nPressure + ((nTime - 1) * kValves[kTargetValve]["flowRate"])
                    if nNewPressure > nBestPressure :
                        nBestPressure = nNewPressure
                    #end

                    if None != kVisitedSets :
                        kNewOpenedValvesFrozen = frozenset(kNewOpenedValves)
                        if kNewOpenedValvesFrozen in kVisitedSets :
                            if nNewPressure > kVisitedSets[kNewOpenedValvesFrozen] :
                                kVisitedSets[kNewOpenedValvesFrozen] = nNewPressure
                            #end
                        else :
                            kVisitedSets[kNewOpenedValvesFrozen] = nNewPressure
                        #end
                    #end

                    # Select each of the Best Target Valves
                    for kNewTarget in kWorthwhileValves :
                        if kNewTarget not in kOpenedValves :
                            if kNewTarget != kTargetValve :
                                if len(kShortestMoves[kTargetValve][kNewTarget]) < nTime :
                                    kQueue.append([nNewPressure, nTime - len(kShortestMoves[kTargetValve][kNewTarget]) - 1, kNewTarget, kNewOpenedValves])
                                #end
                            #end
                        #end
                    #end

                #end

            #end

            return nBestPressure

        #end

        kValves = {k : v for k,v in [parseValve(kLine.strip().split(" ")) for kLine in inputFile.readlines()]}

        # Let's treat this as a problem where we have to get from worthwhile nodes to worthwhile nodes
        # A worthwhile node is a node with a non-zero flow rate
        kWorthwhileValves = [k for k,v in kValves.items() if v["flowRate"] > 0]

        # Next, find the shortest path from every Node to Every Node
        kShortestMoves = {}
        for kValve in kValves :
            kShortestMoves[kValve] = {}
            for kTargetValve in kWorthwhileValves :

                if kTargetValve == kValve : continue
                kShortestMoves[kValve][kTargetValve] = shortestPath(kValve, kTargetValve, kValves)

            #end
        #end

        # Part 1
        print(f"Part 1: {bestFlowRate(kShortestMoves, kWorthwhileValves, kValves)}")

        # Part 2
        kVisited = {}
        bestFlowRate(kShortestMoves, kWorthwhileValves, kValves, 26, kVisited)
        kVisitedCopy = {k:v for k,v in kVisited.items()} # Weirdly seems quicker that bunging the keys in a set?!?

        # Now test this as permutations
        nBestPressure = 0
        for kMe in kVisited :
            # Remove ourselves from the copy as otherwise we'll end
            # up testing the same combined permutations multipe times.
            kVisitedCopy.pop(kMe)
            if len(kVisitedCopy) > 0 :

                # Crudely check what's left... we could limit this based on intersection testing but typically
                # the overhead negates the payoff
                nMaxValue = max([kVisitedCopy[v] for v in kVisitedCopy])

                # And a sanity check to exist when no combination of what's remaining would help improve
                # the score
                if (nMaxValue * 2) < nBestPressure :
                    break
                #end
                
                # Only bother testing intersections / permutations if something remaining is worth testing
                if (kVisited[kMe] + nMaxValue) > nBestPressure :
                    for kThem in kVisitedCopy :
                        if len(kMe.intersection(kThem)) == 0 :
                            nPressure = kVisited[kMe] + kVisited[kThem]
                            if nPressure > nBestPressure :
                                nBestPressure = nPressure
                            #end
                        #end
                    #end
                #end
            #end
        #end

        print(f"Part 2: {nBestPressure}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
