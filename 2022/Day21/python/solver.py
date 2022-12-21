import time
import sys
import copy

def main() :

    with open("../input.txt", "r") as inputFile:

        def decodeMonkey(kLine) :

            kTokens = kLine.split(": ")
            assert(len(kTokens) == 2)

            kOperation = kTokens[1].split(" ")
            if len(kOperation) == 1 :
                return kTokens[0], int(kOperation[0])
            elif len(kOperation) == 3 :
                return kTokens[0], kOperation
            #end

            assert(False)
        #end

        def monkeyMath(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, kIgnoredMonkey="") :

            while len(kUnsolvedMonkeys) > 0 :

                kNewUnsolvedMonkeys = set()
                nOriginalLength     = len(kUnsolvedMonkeys)
                while len(kUnsolvedMonkeys) > 0 :
                    kMonkey           = kUnsolvedMonkeys.pop()
                    kMonkeyExpression = kMonkeys[kMonkey]
                    if ((kMonkeyExpression[0] not in kSolvedMonkeys) or
                        (kMonkeyExpression[2] not in kSolvedMonkeys) or
                        (kMonkeyExpression[0] == kIgnoredMonkey)     or
                        (kMonkeyExpression[1] == kIgnoredMonkey)) :
                        kNewUnsolvedMonkeys.add(kMonkey)
                        continue
                    #end
                    
                    if kMonkeyExpression[1] == "+" :
                        kMonkeys[kMonkey] = kMonkeys[kMonkeyExpression[0]] + kMonkeys[kMonkeyExpression[2]]
                    elif kMonkeyExpression[1] == "*" :
                        kMonkeys[kMonkey] = kMonkeys[kMonkeyExpression[0]] * kMonkeys[kMonkeyExpression[2]]
                    elif kMonkeyExpression[1] == "/" :
                        assert(0 == (kMonkeys[kMonkeyExpression[0]] % kMonkeys[kMonkeyExpression[2]]))
                        kMonkeys[kMonkey] = kMonkeys[kMonkeyExpression[0]] // kMonkeys[kMonkeyExpression[2]]
                    elif kMonkeyExpression[1] == "-" :
                        kMonkeys[kMonkey] = kMonkeys[kMonkeyExpression[0]] - kMonkeys[kMonkeyExpression[2]]
                    elif kMonkeyExpression[1] == "=" :
                        kMonkeys[kMonkey] = kMonkeys[kMonkeyExpression[0]] == kMonkeys[kMonkeyExpression[2]]
                    else :
                        assert(False)
                    #end
                    kSolvedMonkeys.add(kMonkey)
                #end

                # Ok, this is tacky, but Python allows multiple return types, so sue me.
                # This is basically because the input data set gets lost the moment I re-assign it
                # to something else.
                if nOriginalLength == len(kNewUnsolvedMonkeys) :
                    return kNewUnsolvedMonkeys
                #end
                kUnsolvedMonkeys = kNewUnsolvedMonkeys

            #end

            return kMonkeys["root"]

        #end

        def monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nStartValue, kRootMonkey, kTargetMonkey) :

            if kRootMonkey == kTargetMonkey :
                return nStartValue
            #end

            kMonkeyExpression = kMonkeys[kRootMonkey]
            assert(len(kMonkeyExpression) == 3)

            if kMonkeyExpression[0] in kSolvedMonkeys :
                nMonkeyValue = kMonkeys[kMonkeyExpression[0]]
                if kMonkeyExpression[1] == "+" :
                    # Normal   : x = a + b
                    # Reversed : b = x - a
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nStartValue - nMonkeyValue, kMonkeyExpression[2], kTargetMonkey)
                elif kMonkeyExpression[1] == "*" :
                    # Normal   : x = a * b
                    # Reversed : b = x / a
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nStartValue // nMonkeyValue, kMonkeyExpression[2], kTargetMonkey)
                elif kMonkeyExpression[1] == "-" :
                    # Normal   : x = a - b
                    # Reversed : b = a - x
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nMonkeyValue - nStartValue, kMonkeyExpression[2], kTargetMonkey)
                elif kMonkeyExpression[1] == "/" :
                    # Normal   : x = a / b
                    # Reversed : b = a / x
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nMonkeyValue // nStartValue, kMonkeyExpression[2], kTargetMonkey)
                #end
                assert(False)
            elif kMonkeyExpression[2] in kSolvedMonkeys :
                nMonkeyValue = kMonkeys[kMonkeyExpression[2]]
                if kMonkeyExpression[1] == "+" :
                    # Normal   : x = a + b
                    # Reversed : a = x - b
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nStartValue - nMonkeyValue, kMonkeyExpression[0], kTargetMonkey)
                elif kMonkeyExpression[1] == "*" :
                    # Normal   : x = a * b
                    # Reversed : a = x / b
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nStartValue // nMonkeyValue, kMonkeyExpression[0], kTargetMonkey)
                elif kMonkeyExpression[1] == "-" :
                    # Normal   : x = a - b
                    # Reversed : a = x + b
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nMonkeyValue + nStartValue, kMonkeyExpression[0], kTargetMonkey)
                elif kMonkeyExpression[1] == "/" :
                    # Normal   : x = a * b
                    # Reversed : a = x * a
                    return monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, nMonkeyValue * nStartValue, kMonkeyExpression[0], kTargetMonkey)
                #end
                assert(False)
            else :
                assert(False)
            #end
        #end

        kMonkeys         = {k: v for k,v in [decodeMonkey(kLine.strip()) for kLine in inputFile.readlines()]}
        kSolvedMonkeys   = set([k for k,v in kMonkeys.items() if type(v) == int])
        kUnsolvedMonkeys = set([k for k,v in kMonkeys.items() if type(v) == list])

        assert((len(kSolvedMonkeys) + len(kUnsolvedMonkeys)) == len(kMonkeys))

        # Solve as much as we can whilst ignoring our input
        kUnsolvedMonkeys = monkeyMath(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, "humn")

        # Part 1: Nice and Normal
        print(f"Part 1: {int(monkeyMath(copy.deepcopy(kMonkeys), copy.deepcopy(kSolvedMonkeys), copy.deepcopy(kUnsolvedMonkeys)))}")

        # Part 2: Algorithmic

        # What we now have is all values but one are known, so we actually just need to walk back the whole expression
        # and reverse each operation.
        #
        # For example, if the operation is:
        #
        # 20 + xyz
        #
        # And we know the answer needs to be 200, we can derive the expression as:
        # 200 = 20 + xyz
        #
        # We know can re-arrange this to:
        #
        # xyx = 200 - 20 = 180
        #
        # And then if xyz's expression is:
        #
        # 20 * humn
        #
        # We can derive the operation:
        #
        # 180 = 20 * humn
        # humn = 180 / 20 = 9
        kSolvedMonkeys.remove("humn")
        if kMonkeys["root"][0] in kSolvedMonkeys :
            print(f"Part 2: {monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, kMonkeys[kMonkeys['root'][0]], kMonkeys['root'][2], 'humn')}")
        elif kMonkeys["root"][2] in kSolvedMonkeys :
            print(f"Part 2: {monkeyMathReverse(kMonkeys, kSolvedMonkeys, kUnsolvedMonkeys, kMonkeys[kMonkeys['root'][2]], kMonkeys['root'][0], 'humn')}")
        else :
            assert(False)
        #end

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
