import time
import sys
 
def main() :

    kSequence = []
    kNodes    = {}

    with open("../input.txt", "r") as inputFile:

        kSequence = [0 if k == 'L' else 1 for k in inputFile.readline().strip()]

        for kLine in inputFile:

            kLine = kLine.strip().replace(" ", "").replace("(", "").replace(")", "").replace(",", "=")
            if len(kLine.strip()) == 0: continue

            kTokens = kLine.split("=")
            assert(len(kTokens) == 3)
            assert(kTokens[0] not in kNodes)

            kNodes[kTokens[0]] = (kTokens[1], kTokens[2])
            
        #end

    #end

    # Part 1 - Simple Traversal
    kNode     = "AAA"
    nPosition = 0
    nSteps    = 0

    while kNode != "ZZZ":

        nSteps += 1
        nDirection = kSequence[nPosition]
        nPosition = (nPosition + 1) % len(kSequence)

        kNode = kNodes[kNode][nDirection]

    #end

    print(f"Part 1: {nSteps}")

    # Part 2 - Ghost Routes
    kGhostNodes         = [k for k in kNodes if k.endswith("A")]
    kGhostNodesEndNodes = {}

    # The strategy here is to not try and perform simultaneous traversal at all
    # instead, for each ghost, find when the sequence to each end node repeats.
    for kGhostNode in kGhostNodes :

        kNode                           = kGhostNode

        nPosition                       = 0
        kCumulativeStps                 = []

        kGhostNodesEndNodes[kGhostNode] = []
        kEndNodes                       = kGhostNodesEndNodes[kGhostNode]

        while True :

            nDirection = kSequence[nPosition]
            nPosition = (nPosition + 1) % len(kSequence)

            kNode      = kNodes[kNode][nDirection]

            kCumulativeStps.append(nDirection)

            if kNode.endswith("Z") :

                if len(kCumulativeStps) >= len(kSequence) :

                    # I'm shocked this works... what it means is that
                    # there's a repeating pattern from the first node to
                    # the target node.
                    #
                    # Other puzzles normally have some form of pre-amble
                    # into the network before it starts repeating on a
                    # subset.
                    if (len(kCumulativeStps) % 2) == 0 :

                        nMid = len(kCumulativeStps) // 2
                        if kCumulativeStps[0:nMid] == kCumulativeStps[nMid:] :
                            break
                        #end

                    #end

                #end

                kEndNodes.append(len(kCumulativeStps))

            #end

        #end

    #end

    # Be lazy... the input sets for everyone only ever has each start node visiting
    # one end node.
    assert(all(len(k) == 1 for k in kGhostNodesEndNodes.values()))

    # Now all we have to do is find the intersection, or the LCM of all the values
    nValue          = 0
    nIncrement      = min(kGhostNodesEndNodes.values())[0]
    kRemainingItems = set(k[0] for k in kGhostNodesEndNodes.values() if k[0] != nIncrement)

    # Keep Looping until we've incorporated all of the values
    while len(kRemainingItems) > 0 :

        # Increment by the current amount
        nValue += nIncrement

        # Iterate through the remaining items and find any that are divisible
        kItemsToPop = set()
        for k in kRemainingItems :
            if (nValue % k) == 0 :
                kItemsToPop.add(k)
                nIncrement = nValue
            #end
        #end

        # Remove the items that are divisible
        for k in kItemsToPop :
            kRemainingItems.remove(k)
        #end

    #end

    print(f"Part 2: {nIncrement}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end