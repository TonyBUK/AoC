import time
import sys
import copy
import math
 
def main() :
 
    with open("../input.txt", "r") as inputFile:

        def parseMonkey(kMonkey) :

            # Parse the Monkeys
            kMonkeyData    = kMonkey.split("\n")
            kStartingItems = [int(kItem.replace(",", "")) for kItem in kMonkeyData[1].strip().split(" ")[2:]]
            kOperation     = [int(kOp) if kOp.isnumeric() else kOp for kOp in kMonkeyData[2].strip().split(" ")[3:]]
            kDivisibleTest = int(kMonkeyData[3].strip().split(" ")[-1])
            kTrue          = int(kMonkeyData[4].strip().split(" ")[-1])
            kFalse         = int(kMonkeyData[5].strip().split(" ")[-1])

            return {"items" : kStartingItems, "operation" : kOperation, "test" : kDivisibleTest, "true" : kTrue, "false" : kFalse}

        #end

        def processOperation(nItem, kOperation) :

            # Parsing is thankfully simplistic, X + Y or X * Y.  All we have to do is substitute
            # old for the current value.  Literals will already be resolved.
            kResolvedOperation = [nItem if kElement == "old" else kElement for kElement in kOperation]

            # Now just work out the operation itself.
            if   "*" == kResolvedOperation[1] :
                return kResolvedOperation[0] * kResolvedOperation[2]
            elif "+" == kResolvedOperation[1] :
                return kResolvedOperation[0] + kResolvedOperation[2]
            #end

            assert(False)

        #end

        def monkeyActivity(kMonkeys, nRounds, nDivisor, nModulo) :

            # No Monkey Activity... yet
            kMonkeyActivity = [0 for _ in kMonkeys]
    
            for _ in range(nRounds) :

                # Iterate through each Monkey
                for i in range(len(kMonkeys)) :

                    # Grab the Current Monkey
                    kMonkey = kMonkeys[i]

                    # Increment the Total Items the Current Monkey has to Inspect
                    kMonkeyActivity[i] += len(kMonkey["items"])

                    # Process Each Item
                    for nItem in kMonkey["items"] :

                        # Process the Operation
                        nNewItem = (processOperation(nItem, kMonkey["operation"]) // nDivisor) % nModulo

                        # Test which Monkey to Throw to
                        if 0 == (nNewItem % kMonkey["test"]) :
                            kMonkeys[kMonkey["true"]] ["items"].append(nNewItem)
                        else :
                            kMonkeys[kMonkey["false"]]["items"].append(nNewItem)
                        #end

                    #end

                    # Remove all the Items now they're Thrown
                    kMonkey["items"].clear()

                #end

            #end

            # Calculate the two most active monkeys and multiply
            kMonkeyActivity = list(sorted(kMonkeyActivity))
            return kMonkeyActivity[-2] * kMonkeyActivity[-1]

        #end

        # Parse the Monkeys
        kMonkeysPart1 = [parseMonkey(kMonkey) for kMonkey in inputFile.read().split("\n\n")]
        kMonkeysPart2 = copy.deepcopy(kMonkeysPart1)

        # Calculate the LCM, which will use to benignly reduce the value
        # size without impacting divisibility for addition/multiply
        # operations
        LCM = 1
        for kMonkey in kMonkeysPart2 :
            LCM *= kMonkey["test"]
        #end

        print(f"Part 1: {monkeyActivity(kMonkeysPart1, 20,    3, math.inf)}")
        print(f"Part 2: {monkeyActivity(kMonkeysPart2, 10000, 1, LCM)}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end