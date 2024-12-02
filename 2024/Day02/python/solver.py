import time
import sys
 
def main() :
 
    def isSafe(kLevels : list[int], nMinDelta : int = 1, nMaxDelta : int = 3, bDampen : bool = False) -> bool :
 
        kDeltas      = [kNext - kCurrent for kCurrent, kNext in zip(kLevels[:-1], kLevels[1:])]
        kSigns       = [kDelta > 0 for kDelta in kDeltas]
        kValidDeltas = [abs(kDelta) in range(nMinDelta, nMaxDelta + 1) for kDelta in kDeltas]
 
        # The gotcha with dampening is that it's not just ignoring
        # an error, it's whether removing the element will remove the
        # error.
        #
        # For example: 1, 2, 3, 1, 2
        #
        # If we just ignore the transition from a 3 to a 1, then this is
        # safe, but if we remove the 1, then we still have a transition from
        # 3 to 2, which is not safe.
        if bDampen :
 
            # This approach still creates new level lists and re-parses, but it ignores defintitely
            # valid entries.
 
            # Determine whether this is mostly ascending or descending.  Since we can only fix a single defect,
            # there should always be unanimous for a valid set of levels.
            bAscending = kSigns.count(True) >= kSigns.count(False)
            kSeen      = set()
 
            # Iterate through each level to process only the levels which indicate a failure
            for i in range(len(kDeltas)) :
 
                # Note: An Error here could be an error in this element or the next one
                if (kSigns[i] != bAscending) or not kValidDeltas[i] :
 
                    # Ignore the Level if it's already been checked
                    if not (i) in kSeen :
                        if isSafe([k for j,k in enumerate(kLevels) if j != (i)]) :
                            return True
                        #end
                    #end
 
                    # Process the next Level in-case that's the source of the fault
                    # Note: This can *never* have already been seen since this will always
                    #       be the next iteration in the loop.
                    if isSafe([k for j,k in enumerate(kLevels) if j != (i + 1)]) :
                        return True
                    #end
                    kSeen.add(i + 1)
 
                #end
 
            #end
 
        #end
 
        return (all(kSigns) or not any(kSigns)) and all(kValidDeltas)
 
    #end
 
    kLevels = []
    with open("input.txt", "r") as inputFile:
        kLevels = [[int(k) for k in kLine.strip().split(" ")] for kLine in inputFile]
    #end

    kUnsafe = [k for k in kLevels if not isSafe(k)]
    print(f"Part 1: {len(kLevels) - len(kUnsafe)}")

    kUnsafe = [k for k in [isSafe(k, bDampen=True) for k in kUnsafe] if k is False]
    print(f"Part 2: {len(kLevels) - len(kUnsafe)}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end