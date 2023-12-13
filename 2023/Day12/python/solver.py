import time
import sys
 
def main() :

    kSpringData = []
    with open("input.txt", "r") as inputFile:
        kSpringData = [[k.split()[0], [int(num) for num in k.split()[1].split(',')]] for k in inputFile.readlines() if k]
    #end

    # Rather than trying to Dynamically create the strings themselves, we can
    # abstract the strings to a set of states...
    #
    # nCurrentDamageGearsCount : For the Current Damaged Gear of interest, how many consecutive damaged gears have we seen?
    # nRecordStatusIndex       : What character of the Damage Record are we looking at? (Puzzle Input)
    # kDamagedGearsIndex       : What is the Current Damaged Gear of Interest? (Puzzle Input)
    #
    # So for the input ???.###
    #
    # If we were trying to represent:
    #
    # #.?.### 1,1,3
    #  ^
    # Then we would have:
    #
    # nCurrentDamageGearsCount = 0
    # nRecordStatusIndex       = 1
    # kDamagedGearsIndex       = 1
    def getPermutationCount(kRecordStatus, kDamagedGears, nCurrentDamageGearsCount = 0, nRecordStatusIndex = 0, kDamagedGearsIndex = 0, kCache = None) :

        # Initialise the Cache
        if None == kCache :
            kCache = {}
        #end

        # Create the Key for the Current State and Check if it's in the Cache
        kCacheKey = (nCurrentDamageGearsCount, nRecordStatusIndex, kDamagedGearsIndex)
        if kCacheKey in kCache :
            return kCache[kCacheKey]
        #end

        # Have we reached a possible solution?
        if len(kRecordStatus) == nRecordStatusIndex :

            # Have we processed all the gears?
            if len(kDamagedGears) == kDamagedGearsIndex :
                return 1
            #end

            return 0

        #end

        # Result for this state since we couldn't use the cache
        nResult = 0

        # Process the Hash / Hash Substitution
        if kRecordStatus[nRecordStatusIndex] in "#?" :

            # For a Hash case we incrememnt the number of damaged gears we've seen and then process the next character.
            if kDamagedGearsIndex < len(kDamagedGears) :

                # Just for break...
                while True :

                    # Check to see if this is the last item, and if so, check to see if we've processed the correct number of gears.
                    # Otherwise we need to do a rather convoluted check elsewhere...
                    if len(kRecordStatus) == (nRecordStatusIndex + 1) :
                        if nCurrentDamageGearsCount + 1 == kDamagedGears[kDamagedGearsIndex] :
                            nResult += getPermutationCount(kRecordStatus, kDamagedGears, 0, nRecordStatusIndex + 1, kDamagedGearsIndex + 1, kCache)
                            break
                        #end
                    #end

                    # Increment the Gear Count
                    if nCurrentDamageGearsCount < kDamagedGears[kDamagedGearsIndex] :
                        nResult += getPermutationCount(kRecordStatus, kDamagedGears, nCurrentDamageGearsCount + 1, nRecordStatusIndex + 1, kDamagedGearsIndex, kCache)
                    #end

                    break

                #end

            #end

        #end

        # Process the Dot / Dot Substitution
        if kRecordStatus[nRecordStatusIndex] in ".?" :

            # For a Dot or Dot substitution there's really only two things we care about.
            #
            # Are we not processing a gear, in which case move on to the next character.
            # Are we processing a gear, in which we can only move onto the next gear if we've processed the correct number of gears.
            # Otherwise do nothing...

            # Not processing a Damaged Gear, so no state change here...
            if nCurrentDamageGearsCount == 0 :

                nResult += getPermutationCount(kRecordStatus, kDamagedGears, nCurrentDamageGearsCount, nRecordStatusIndex + 1, kDamagedGearsIndex, kCache)

            # Determine if we can move onto the next Damaged Gear
            elif (kDamagedGearsIndex < len(kDamagedGears)) and (nCurrentDamageGearsCount == kDamagedGears[kDamagedGearsIndex]) :

                nResult += getPermutationCount(kRecordStatus, kDamagedGears, 0, nRecordStatusIndex + 1, kDamagedGearsIndex + 1, kCache)

            #end

        #end

        # Cache the Result so we don't repeat the work
        kCache[kCacheKey] = nResult

        return nResult
    
    #end

    # Calculate the Permutations for Part 1
    nPermutations = 0
    for kSpring in kSpringData :
        nPermutations += getPermutationCount(kSpring[0], kSpring[1])
    #end

    print(f"Part 1: {nPermutations}")

    # Unfold the Input for Part 2, then Calculated the Permutations

    FOLD_SIZE = 5

    nPermutations = 0
    for kSpring in kSpringData :

        kSpringDataPartTwo = [[], []]
        kSpringDataPartTwo[0] = "?".join(kSpring[0] for _ in range(FOLD_SIZE))
        kSpringDataPartTwo[1] = [k for _ in range(FOLD_SIZE) for k in kSpring[1]]

        nPermutations += getPermutationCount(kSpringDataPartTwo[0], kSpringDataPartTwo[1])

    #end

    print(f"Part 2: {nPermutations}")

#end

if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end