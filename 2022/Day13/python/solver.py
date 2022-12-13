import time
import sys
import functools
 
COMPARATOR_GT = 1
COMPARATOR_EQ = 0
COMPARATOR_LT = -1
 
def main() :
 
    with open("../input.txt", "r") as inputFile:
 
        def compareLists(kLeft, kRight) :
 
            nCompare = COMPARATOR_EQ
 
            for kLeftCompare, kRightCompare in zip(kLeft, kRight) :
 
                # Recurse if one or more of these is a list
                if type(kLeftCompare) != int or type(kRightCompare) != int :
 
                    nCompare = compareLists([kLeftCompare]  if type(kLeftCompare)  == int else kLeftCompare,
                                            [kRightCompare] if type(kRightCompare) == int else kRightCompare)
 
                else :
 
                    # Not a List, so compare the values...
                    # equal to     = Continue to the next item
                    # less than    = Left Side is in Order
                    # greater than = Left Side is NOT in Order
                    if kLeftCompare < kRightCompare :
                        nCompare = COMPARATOR_LT
                    elif kLeftCompare > kRightCompare :
                        nCompare = COMPARATOR_GT
                    #end
 
                #end
 
                # Exit if a Order has been established
                if COMPARATOR_EQ != nCompare :
                    return nCompare
                #end
 
            #end
 
            # Check this last, as the Left/Right Side being smaller is only
            # important if the lists are otherwise identical.
            if len(kLeft) < len(kRight) :
                return COMPARATOR_LT
            elif len(kLeft) > len(kRight) :
                return COMPARATOR_GT
            #end
 
            return nCompare
 
        #end

        # All this to replace an EVAL!!!!!
        def parseInput(kRawString) :

            kParsedArray = []

            assert(len(kRawString) > 0)
            assert("[" == kRawString[0])
            assert("]" == kRawString[-1])

            nStackCount      = 0
            kStackPointer    = [0]
            kArrayPointer    = kParsedArray
            nValue           = None

            for kChar in kRawString[1:-1] :

                if "[" == kChar :

                    assert(None == nValue)

                    # Push a New Array Element and Walk the Stack to Point to It
                    kArrayPointer = kParsedArray
                    for i in kStackPointer[:-1] :
                        kArrayPointer = kArrayPointer[i]
                    #end

                    kArrayPointer.append([])
                    kArrayPointer = kArrayPointer[-1]
                    kStackPointer.append(0)
                    nStackCount += 1

                elif "]" == kChar :

                    # Push a Value if Pending
                    if None != nValue :
                        kArrayPointer.append(nValue)
                    #end

                    # Pop an Array Element and Walk the Stack to Point to the Prior Level
                    nStackCount -= 1
                    kStackPointer.pop()

                    kArrayPointer = kParsedArray
                    for i in kStackPointer[:-1] :
                        kArrayPointer = kArrayPointer[i]
                    #end

                    nValue = None

                elif "," == kChar :

                    # Append an Element if Needed and Increment the Stack Pointer

                    assert(None != kArrayPointer)

                    # Push a Value if Pending
                    if None != nValue :
                        kArrayPointer.append(nValue)
                    #end

                    kStackPointer[-1] += 1
                    nValue = None

                else :

                    # Handle Value Parsing

                    if None == nValue : nValue = 0
                    nValue = (nValue * 10) + int(kChar)

                #end

                assert(nStackCount >= 0)
                assert((nStackCount+1) == len(kStackPointer))

            #end

            # Push a Value if Pending
            if None != nValue :
                kParsedArray.append(nValue)
            #end

            return kParsedArray

        #end

        # Part 1: Sum of the Indexes of all LT Comparisons
 
        # Split the Raw Input into Pairs
        kRawInputs = [[parseInput(kGoodMan) for kGoodMan in kInput.split("\n")] for kInput in inputFile.read().split("\n\n")]
        print(f"Part 1: {sum([i+1 if (COMPARATOR_GT != compareLists(kTest[0], kTest[1])) else 0 for i,kTest in enumerate(kRawInputs)])}")
 
        # Part 2: Locations of the Divisors on the Flattened List
 
        # Produce a Flattened View of the Data Items for Sorting
        DIVISORS          = [[[2]], [[6]]]
        kUnsortedInputs  = [kFlattenedRawInput for kRawInput in kRawInputs for kFlattenedRawInput in kRawInput]
        kUnsortedInputs += DIVISORS
        kSortedInputs    = sorted(kUnsortedInputs, key=functools.cmp_to_key(compareLists))
 
        # Product of the Location of the Divisors
        nProduct         = 1
        for kDivisor in DIVISORS :
            nProduct *= kSortedInputs.index(kDivisor) + 1
        #end
        print(f"Part 2: {nProduct}")
 
    #end
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end