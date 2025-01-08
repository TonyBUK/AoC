import time
import sys

def main() :

    kWires          = {}
    kWireOperations = {}
    kUnknownWires   = set()

    with open("../input.txt", "r") as inputFile:

        kTokens = inputFile.read().split("\n\n")
        assert(len(kTokens) == 2)

        # All the Wires with Known States
        for kLine in kTokens[0].split("\n") :
            kWireTokens = kLine.split(": ")
            assert(len(kWireTokens) == 2)
            assert(kWireTokens[0] not in kWires)
            kWires[kWireTokens[0]] = int(kWireTokens[1])
        #end

        # All the Wire Connections
        for kLine in kTokens[1].split("\n") :
            kLine = kLine.replace(" ->", "")
            kWireTokens = kLine.split(" ")
            assert(len(kWireTokens) == 4)

            # Add the Missing Wires
            if kWireTokens[0] not in kWires :
                kUnknownWires.add(kWireTokens[0])
            #end
            if kWireTokens[2] not in kWires :
                kUnknownWires.add(kWireTokens[2])
            #end
            if kWireTokens[3] not in kWires :
                kUnknownWires.add(kWireTokens[3])
            #end

            # Add the Operation
            assert(kWireTokens[3] not in kWireOperations)
            kWireOperations[kWireTokens[3]] = (kWireTokens[0], kWireTokens[1], kWireTokens[2])

        #end

    #end

    def covertWiresToNumber(kWires : dict[str, int], kSet : str) -> int :
        nResult = 0
        for kWire in reversed(sorted([k for k in kWires.keys() if k.startswith(kSet)])) :
            nResult = (nResult << 1) | kWires[kWire]
        #end
        return nResult
    #end

    def computeWires(kWiresSource : dict[str, int], kWireOperations : dict[str, tuple[str, str, str]], kUnknownWiresSource : set[str]) -> int:

        kWires        = {k : v for k, v in kWiresSource.items()}
        kUnknownWires = set([k for k in kUnknownWiresSource])

        while kUnknownWires :

            for kWire in kUnknownWires :

                assert(kWire in kWireOperations)

                kLeft = kWireOperations[kWire][0]
                kRight = kWireOperations[kWire][2]

                if kWire in kWires.keys() :
                    continue
                #end

                assert(kWire not in kWires.keys())

                if kLeft in kWires and kRight in kWires :

                    if kWireOperations[kWire][1] == "AND" :
                        kWires[kWire] = kWires[kLeft] & kWires[kRight]
                    elif kWireOperations[kWire][1] == "OR" :
                        kWires[kWire] = kWires[kLeft] | kWires[kRight]
                    elif kWireOperations[kWire][1] == "XOR" :
                        kWires[kWire] = kWires[kLeft] ^ kWires[kRight]
                    else :
                        assert(False)
                    #end

                #end

            #end

            for kWire in kWires.keys() :
                if kWire in kUnknownWires :
                    kUnknownWires.remove(kWire)
                #end
            #end

        #end

        return covertWiresToNumber(kWires, "z")

    #end

    nPartOne = computeWires(kWires, kWireOperations, kUnknownWires)
    print(f"Part 1: {nPartOne}")

    # Part 2

    def createAdder(nBits : int) -> dict[str, tuple[str, str, str]] :
        
        kWireOperations = {}

        # Implement an Adder for an Arbitrary Number of Bits usng Half/Full Adders
        #
        # Effectively a larger version of this:
        # https://circuitverse.org/users/60917/projects/two-3-bit-number-adder

        for i in range(nBits):

            if i == 0:

                kWireOperations[f"z{i:02}"] = (f"x{i:02}", "XOR", f"y{i:02}")
                kWireOperations[f"c{i:02}"] = (f"x{i:02}", "AND", f"y{i:02}")

            elif i == 1 :

                kWireOperations[f"s{i:02}"] = (f"x{i:02}", "XOR", f"y{i:02}")
                kWireOperations[f"z{i:02}"] = (f"c{i-1:02}", "XOR", f"s{i:02}")
                kWireOperations[f"c{i:02}"] = (f"x{i:02}", "AND", f"y{i:02}")

            else :

                kWireOperations[f"s{i:02}"] = (f"x{i:02}", "XOR", f"y{i:02}")

                kWireOperations[f"d{i:02}"] = (f"x{i:02}", "AND", f"y{i:02}")
                if i <= 2 :
                    kWireOperations[f"e{i:02}"] = (f"s{i-1:02}", "AND", f"c{i-2:02}")
                    kWireOperations[f"c{i:02}"] = (f"c{i-1:02}", "OR", f"e{i:02}")
                else :
                    kWireOperations[f"e{i:02}"] = (f"c{i-1:02}", "AND", f"s{i-1:02}")
                    kWireOperations[f"c{i:02}"] = (f"d{i-1:02}", "OR", f"e{i:02}")
                #end

                kWireOperations[f"z{i:02}"] = (f"c{i:02}", "XOR", f"s{i:02}")

            #end

        #end

        kWireOperations[f"c{nBits:02}"] = (f"c{nBits-1:02}", "AND", f"s{nBits-1:02}")
        kWireOperations[f"z{nBits:02}"] = (f"d{nBits-1:02}", "OR", f"c{nBits:02}")

        return kWireOperations

    #end

    def findAdderWires(kReferenceAdder : dict[str, tuple[str, str, str]], kWireOperations : dict[str, tuple[str, str, str]], kAdderMappings : dict[str, str], kRemaining : set[str], kRenamer : dict[str, str]) -> bool :

        # This search pattern will only consider a wire for mapping if this is an output wire for known operands, this will help
        # increase the confidence of found matches.

        for k in kAdderMappings.keys() :

            # Determine which keys are relevant for this wire
            kPossibleReferenceAdderKeys = [j for j in kReferenceAdder.keys() if k in kReferenceAdder[j]]

            # Reduce the keys to only include ones with known operands
            kPossibleKeys               = [j for j in kPossibleReferenceAdderKeys if (kReferenceAdder[j][0] in kAdderMappings and kReferenceAdder[j][2] in kAdderMappings)]

            # Iterate through the Possible Keys
            for j in kPossibleKeys :

                # Iterate through the unmapped Keys
                for l in kRemaining :

                    # If the operation matches....
                    if kWireOperations[l][1] == kReferenceAdder[j][1] :

                        # Rename th Operands if known
                        kWireOperationLeft  = kRenamer.get(kWireOperations[l][0], kWireOperations[l][0])
                        kWireOperationRight = kRenamer.get(kWireOperations[l][2], kWireOperations[l][2])

                        # If the operands match, we've found a match
                        if ((kWireOperationLeft == kAdderMappings[kReferenceAdder[j][0]] and kWireOperationRight == kAdderMappings[kReferenceAdder[j][2]]) or
                            (kWireOperationLeft == kAdderMappings[kReferenceAdder[j][2]] and kWireOperationRight == kAdderMappings[kReferenceAdder[j][0]])) :

                            # Output Keys beginning with a z should always match, if not, the input has swapped an output wire
                            # with one of the intermediary wires
                            if j.startswith("z") :
                                if j != l :
                                    kRenamer[l] = j
                                    kRenamer[j] = l
                                #end
                            #end

                            # Add the Mapping
                            kAdderMappings[j] = l

                            # Reduce the unknown Wires by the now known wire
                            kRemaining.remove(l)

                            return True

                        #end

                    #end

                #end

            #end

        #end

        return False

    #end

    def bruteForceAddrWires(kReferenceAdder : dict[str, tuple[str, str, str]], kWireOperations : dict[str, tuple[str, str, str]], kAdderMappings : dict[str, str], kRemaining : set[str], kRenamer : dict[str, str]) :

        # Somethings mapped wrong, perform a brute force rename
        # This works on the premise we can pattern match based on the operands, finding two wires
        # with the same operands, but a different operator.

        # Only Consider mapped Keys that are not already renamed as the puzzle promises we won't have
        # compound errors.
        kBruteForceKeys = [k for k in kAdderMappings.keys() if kAdderMappings[k] not in kRenamer and k[0] not in ["x", "y"]]

        # For each key...
        for idx, k in enumerate(kBruteForceKeys) :

            # For each subsequent key...
            for l in kBruteForceKeys[idx+1:] :

                # If the Operands Match (ignore the operation)
                if (kReferenceAdder[k][0] in [kReferenceAdder[l][0], kReferenceAdder[l][2]] and 
                    kReferenceAdder[k][2] in [kReferenceAdder[l][0], kReferenceAdder[l][2]]) :

                    # Tentatively rename the operands
                    kRenamer[kAdderMappings[k]] = kAdderMappings[l]
                    kRenamer[kAdderMappings[l]] = kAdderMappings[k]

                    # If we can continue traversing the wires, assume we're now good
                    if findAdderWires(kReferenceAdder, kWireOperations, kAdderMappings, kRemaining, kRenamer) :
                        return
                    #end

                    # If we're here, the trial rename failed, so we'll revert the rename
                    kRenamer.pop(kAdderMappings[k])
                    kRenamer.pop(kAdderMappings[l])

                #end

            #end

        #end

        assert(False)

    #end

    # Create a "correct" adder which we can incrementally map the puzzle input to and find
    # any instances of crossed wires
    kReferenceAdder = createAdder(45)

    assert(len(kReferenceAdder) == len(kWireOperations))

    # First Pass, only trust operand inputs that use x/y
    kAdderMappings = {}
    kRemaining     = set([k for k in kWireOperations.keys()])
    kRenamer       = {}

    for k in kReferenceAdder.keys() :

        if kReferenceAdder[k][0].startswith("x") or kReferenceAdder[k][0].startswith("y") :

            for j in kRemaining :

                if kWireOperations[j][1] == kReferenceAdder[k][1] :

                    if ((kWireOperations[j][0] == kReferenceAdder[k][0] and kWireOperations[j][2] == kReferenceAdder[k][2]) or
                        (kWireOperations[j][0] == kReferenceAdder[k][2] and kWireOperations[j][2] == kReferenceAdder[k][0])) :

                        # These should match!
                        if k.startswith("z") :
                            if k != j :
                                kRenamer[j] = k
                                kRenamer[k] = j
                            #end
                        #end

                        kAdderMappings[k] = j
                        kRemaining.remove(j)

                        break

                    #end

                #end

            #end

        #end

    #end

    # Second Pass, pattern match the remaining wires incrementally searching from the existing mappings
    while kRemaining :

        # Find the remaining wires by pattern matching incrementally
        # This operates on the premise that we can pattern match based on the operation, and will only
        # accept renamed operations that start with z.
        while findAdderWires(kReferenceAdder, kWireOperations, kAdderMappings, kRemaining, kRenamer) :
            pass
        #end

        # Keep going until we've found all the wires
        if not kRemaining :
            break
        #end

        # If we're here it means we can't reasonably find any wires but there's still some unmapped, meaning
        # we've got a false positive.  This will brute force the existing mappings until findAdderWires can
        # find a match.
        bruteForceAddrWires(kReferenceAdder, kWireOperations, kAdderMappings, kRemaining, kRenamer)

    #end

    print(f"Part 2: {",".join(sorted(kRenamer.keys()))}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end