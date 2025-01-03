import time
import sys

def main() :

    kConnectedComputers        = {}
    kConnectedComputersPartTwo = {}
    with open("../input.txt", "r") as inputFile:

        for kLine in inputFile.readlines() :

            kTokens = kLine.strip().split("-")
            assert(len(kTokens) == 2)

            if kTokens[0] not in kConnectedComputers :
                kConnectedComputers[kTokens[0]] = set([kTokens[1]])
                kConnectedComputersPartTwo[kTokens[0]] = set([kTokens[1]])
            else :
                kConnectedComputers[kTokens[0]].add(kTokens[1])
                kConnectedComputersPartTwo[kTokens[0]].add(kTokens[1])
            #end

            if kTokens[1] not in kConnectedComputers :
                kConnectedComputers[kTokens[1]] = set([kTokens[0]])
                kConnectedComputersPartTwo[kTokens[1]] = set([kTokens[0]])
            else :
                kConnectedComputers[kTokens[1]].add(kTokens[0])
                kConnectedComputersPartTwo[kTokens[1]].add(kTokens[0])
            #end

            kConnectedComputersPartTwo[kTokens[0]].add(kTokens[0])
            kConnectedComputersPartTwo[kTokens[1]].add(kTokens[1])

        #end

    #end

    def findThruple(kComputer : str, kConnectedComputers : dict[str, set[str]], kThruples : set[tuple[str, str, str]], kPossibleThruple : list[str, str, str] = None) :

        if None == kPossibleThruple :
            kPossibleThruple = [kComputer]
        #end

        for kConnectedComputer in kConnectedComputers[kComputer] :

            if kConnectedComputer not in kThruples :

                bValid = True
                for kItemInPossibleThruple in kPossibleThruple :
                    if kConnectedComputer not in kConnectedComputers[kItemInPossibleThruple] :
                        bValid = False
                        break
                    #end
                #end

                if bValid :

                    kPossibleThruple.append(kConnectedComputer)
                    if len(kPossibleThruple) == 3 :
                        kThruples.add(tuple(sorted(kPossibleThruple)))
                    else :
                        findThruple(kConnectedComputer, kConnectedComputers, kThruples, kPossibleThruple)
                    #end
                    kPossibleThruple.pop()

                #end

            #end

        #end

    #end

    def findLargestGroupRecurse(kConnectedComputers : dict[str, set[str]], kRemaining : set[str] = None, kCurrentGroup : set[str] = None, kCurrentGroupConnectionsSubset : set[str] = None, kLargestGroup : set[str] = None) -> set[str] :

        assert(kRemaining is not None)
        assert(kCurrentGroup is not None)
        assert(kLargestGroup is not None)
        assert(kCurrentGroupConnectionsSubset is not None)

        if kLargestGroup is None :
            kLargestGroup = set()
        #end

        # If the current group is smaller than the largest group, then we can stop
        # as this group will never get bigger, only smaller.
        if len(kCurrentGroupConnectionsSubset) <= len(kLargestGroup) :
            return kLargestGroup, kRemaining
        #end

        # Is this the largest group so far?
        if len(kCurrentGroup) > len(kLargestGroup) :
            kLargestGroup = set([x for x in kCurrentGroup])
        #end

        # Buffer the Remaining before we begin...
        kRemainingStart = set([x for x in kRemaining])
        while kRemaining :

            # Get the next remaining Computer
            kComputer = kRemaining.pop()

            # Tentatively create a new group subset (including this computer)
            kNewGroupSubset = kConnectedComputers[kComputer] & kCurrentGroupConnectionsSubset

            # If all computers are connected
            if (kNewGroupSubset & kCurrentGroup) == kCurrentGroup :

                # Recurse with the new group subset
                kLargestGroup, kRemaining = findLargestGroupRecurse(kConnectedComputers, kRemaining, kCurrentGroup | set([kComputer]), kNewGroupSubset, kLargestGroup)

            #end

        #end

        return kLargestGroup, kRemainingStart

    #end

    def findLargestGroup(kConnectedComputers : dict[str, set[str]], kRemaining : set[str] = None, kCurrentGroup : set[str] = None, kLargestGroup : set[str] = None) -> set[str] :

        if kRemaining is None :
            kRemaining = set(kConnectedComputers.keys())
        #end

        if kCurrentGroup is None :
            kCurrentGroup = set()
        #end

        if kLargestGroup is None :
            kLargestGroup = set()
        #end

        # Buffer the Remaining before we begin...
        for _ in range(len(kRemaining)) :

            # Get the next Computer of Interest
            kComputer = kRemaining.pop()

            # As we have no connections in the current group, then we can start with this computer...
            kCurrentGroupConnectionsSubset = kConnectedComputers[kComputer]

            if len(kCurrentGroupConnectionsSubset) > len(kLargestGroup) :

                # Recurse with the new group subset
                kLargestGroup, kRemaining = findLargestGroupRecurse(kConnectedComputers, kRemaining, kCurrentGroup | set([kComputer]), kCurrentGroupConnectionsSubset, kLargestGroup)

            #end

        #end

        return kLargestGroup

    #end

    # https://en.wikipedia.org/wiki/Bron–Kerbosch_algorithm
    def bron_kerbosch(R : set[str], P : set[str], X : set[str], kConnectedComputers : dict[str, set[str]], kLargestGroup : set[str]) -> set[str] :

        # if P and X are both empty
        if (len(P) == 0) and (len(X) == 0) :

            # report R as a maximal clique
            if len(R) > len(kLargestGroup):
                kLargestGroup = set([x for x in R])
            #end

            return kLargestGroup

        #end

        # for each vertex v in P
        for v in [v for v in P]:

            kLargestGroup = bron_kerbosch(R.union([v]), P.intersection(kConnectedComputers[v]), X.intersection(kConnectedComputers[v]), kConnectedComputers, kLargestGroup)
            P.remove(v)
            X.add(v)

        #end

        return kLargestGroup

    #end

    def findLargestGroupBronKerbosch(kConnectedComputers : dict[str, set[str]]) -> set[str] :
        return bron_kerbosch(set(), set(kConnectedComputers.keys()), set(), kConnectedComputers, set())
    #end

    kThruples = set()
    for kComputer in kConnectedComputers :
        if kComputer.startswith("t") :
            findThruple(kComputer, kConnectedComputers, kThruples)
        #end
    #end

    print(f"Part 1: {len(kThruples)}")
    print(f"Part 2: {",".join(sorted(findLargestGroup(kConnectedComputersPartTwo)))}")
#    print(f"Part 2: {",".join(sorted(findLargestGroupBronKerbosch(kConnectedComputers)))}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end