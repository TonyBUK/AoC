import time
import sys
 
from functools import cmp_to_key
 
def main() :
 
    kPageOrderingRules         = {}
    kReversedPageOrderingRules = {}
    kUpdates                   = []
 
    with open("../input.txt", "r") as inputFile:
 
        kTokens = inputFile.read().split("\n\n")
 
        for kRule in kTokens[0].split("\n") :
 
            kPair = [int(k) for k in kRule.split("|")]
 
            # For Part One - Page Ordering Rules means pages that *must*
            # come before.
            if kPair[0] not in kPageOrderingRules :
                kPageOrderingRules[kPair[0]] = set([kPair[1]])
            else :
                kPageOrderingRules[kPair[0]].add(kPair[1])
            #end
 
            # For Part Two - Reversed Page Ordering Rules means pages that
            # can't come before.
            if kPair[1] not in kReversedPageOrderingRules :
                kReversedPageOrderingRules[kPair[1]] = set([kPair[0]])
            else :
                kReversedPageOrderingRules[kPair[1]].add(kPair[0])
            #end
 
        #end
 
        kUpdates = [[int(k) for k in kUpdate.split(",")] for kUpdate in kTokens[1].split("\n")]
 
    #end
 
    def validPageOrdering(kPageOrderingRules : dict[int:int], kUpdate : list[int]) -> bool :
 
        kSeenPage = set()
 
        # For Each Page...
        for nPage in kUpdate :
 
            # If the page is in the ordering rules, make sure we've not seen any of
            # the pages that must come before it.
            if nPage in kPageOrderingRules :
                if kPageOrderingRules[nPage] & kSeenPage :
                    return False
                #end
            #end
 
            # Add to the list of seen pages
            kSeenPage.add(nPage)
 
        #end
 
        return True
 
    #end
 
    def compareUpdates(nItem1, nItem2) :
 
        nonlocal kPageOrderingRules
 
        if nItem1 not in kPageOrderingRules:
            return -1
        elif nItem2 not in kPageOrderingRules[nItem1] :
            return -1
        else :
            return 1
        #end
 
    #end

    nPartOne = 0
    nPartTwo = 0
 
    for kUpdate in kUpdates :
        if validPageOrdering(kPageOrderingRules, kUpdate) :
            nPartOne += kUpdate[len(kUpdate) // 2]
        else :
            kFixedUpdate = sorted(kUpdate, key=cmp_to_key(compareUpdates))
            nPartTwo += kFixedUpdate[len(kFixedUpdate) // 2]
        #end
    #end
 
    print(f"Part 1: {nPartOne}")
    print(f"Part 2: {nPartTwo}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end