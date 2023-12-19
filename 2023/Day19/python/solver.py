import time
import sys
import dataclasses
import enum
 
class ECommandType(enum.IntEnum) :
    JUMP    = 0
    COMPARE = 1
    ACCEPT  = 2
    REJECT  = 3
#end
 
@dataclasses.dataclass
class OpcodeData :
    kOperator : ECommandType
    kOperand  : list
#end
 
def main() :
 
    kRules = {}
    kParts = []
    kFirstRule = "in"
    kJumpSources = {}
 
    # Create two Ruies to prevent things getting too convoluted
    kRules["A"] = [OpcodeData(ECommandType.ACCEPT, [])]
    kRules["R"] = [OpcodeData(ECommandType.REJECT, [])]
 
    with open("../input.txt", "r") as inputFile:
 
        kTokens = inputFile.read().split("\n\n")
        assert(len(kTokens) == 2)

        # Parse the rules, ultimately, we'll arrive with a series of rules that either:
        # 1. Perform a comparison and jump if True
        # 2. Jump to another rule unconditionally
        #
        # Note: Else is implictly handled by nesting rules.
        for kRule in kTokens[0].split("\n") :
 
            kRuleTokens = kRule.split("{")
            assert(len(kRuleTokens) == 2)
 
            kRuleName = kRuleTokens[0].strip()
            assert(kRuleName not in kRules)
            kRules[kRuleName] = []
 
            kComparisonTokens = kRuleTokens[1].replace("}", "").split(",")
            assert(len(kComparisonTokens) >= 2)
 
            for kComparisonRule in kComparisonTokens :

                # Parse the Current Rule
                if ":" in kComparisonRule :

                    # Parse the Comparison
                    kComparisonRuleTokens = kComparisonRule.split(":")
                    assert(len(kComparisonRuleTokens) == 2)
 
                    kComparisonRule = kComparisonRuleTokens[0].split("<" if "<" in kComparisonRuleTokens[0] else ">")
 
                    kRules[kRuleName].append(OpcodeData(ECommandType.COMPARE,
                                             [kComparisonRule[0],
                                              "<" if "<" in kComparisonRuleTokens[0] else ">",
                                              int(kComparisonRule[1]),
                                              kComparisonRuleTokens[1]]))

                    # This part is for part 2, to produce a list of all possible ways of jumping to a given
                    # rule.
                    if kComparisonRuleTokens[1] not in kJumpSources :
                        kJumpSources[kComparisonRuleTokens[1]] = []
                    #end
                    kJumpSources[kComparisonRuleTokens[1]].append([kRuleName, len(kRules[kRuleName]) - 1])

                else :

                    # Parse the unconditional Jump
                    kRules[kRuleName].append(OpcodeData(ECommandType.JUMP, [kComparisonRule]))

                    # This part is for part 2, to produce a list of all possible ways of jumping to a given
                    # rule.
                    if kComparisonRule not in kJumpSources :
                        kJumpSources[kComparisonRule] = []
                    #end
                    kJumpSources[kComparisonRule].append([kRuleName, len(kRules[kRuleName]) - 1])

                #end

            #end

        #end

        # Parse the parts
        for kPart in kTokens[1].split("\n") :
 
            kPartTokens = kPart.replace("{", "").replace("}", "").split(",")
            kParts.append({})

            for kPartToken in kPartTokens :
                kAssignmentToken = kPartToken.split("=")
                assert(kAssignmentToken[0] not in kParts[-1])
                kParts[-1][kAssignmentToken[0]] = int(kAssignmentToken[1])
            #end
 
        #end
 
    #end

    # Iteratively process the rules until we either reach an Accept or Reject
    def processPart(kRules, kFirstRule, kPart) :
 
        kCurrentRule = kFirstRule
 
        while True :
 
            for kRule in kRules[kCurrentRule] :
 
                if kRule.kOperator == ECommandType.JUMP :
                    kCurrentRule = kRule.kOperand[0]
                    break
                elif kRule.kOperator == ECommandType.ACCEPT :
                    return True
                elif kRule.kOperator == ECommandType.REJECT :
                    return False
                elif kRule.kOperator == ECommandType.COMPARE :
                    if kRule.kOperand[1] == "<" :
                        if kPart[kRule.kOperand[0]] < kRule.kOperand[2] :
                            kCurrentRule = kRule.kOperand[3]
                            break
                        #end
                    else :
                        if kPart[kRule.kOperand[0]] > kRule.kOperand[2] :
                            kCurrentRule = kRule.kOperand[3]
                            break
                        #end
                    #end
                #end
 
            #end
 
        #end
 
    #end

    print(len(kJumpSources))
    print(max([len(v) for v in kJumpSources.values()]))

    # Part 1 - Check the Part Totals for any Accepted Rule
    nAcceptedPartTotal = 0
    for kPart in kParts :
        if processPart(kRules, kFirstRule, kPart) :
            nAcceptedPartTotal += sum(kPart.values())
        #end
    #end
    print(f"Part 1: {nAcceptedPartTotal}")
 
    # Part 2 wants us to find combinations of rules that will accept the part
    # from 1 to 4000
    #
    # But 4000 * 4000 * 4000 * 4000 is too many permutations to brute force.
    #
    # Instead we need to work backwards. From each rule that *can* accept the
    # part, how do we get there. So create a series of backtrace paths from
    # accept to each starting point, and from there, we can define acceptible
    # ranges.
 
    def getValidRanges(kRuleKey, kRuleKeySource, nRuleCommand, kRules, kJumpSources, kTargetNode, kValidRanges = None) :
 
        if kValidRanges is None :
            kLocalValidRanges = {"k" : kRuleKey, "x": [1, 4000], "m" : [1, 4000], "a" : [1, 4000], "s" : [1, 4000]}
        else :
            kLocalValidRanges = {k : [x for x in v] for k, v in kValidRanges.items()}
            kLocalValidRanges["k"] = kRuleKey
        #end

        kReturnValidRanges  = [kLocalValidRanges]
        nInitialRuleCommand = nRuleCommand
 
        while True :

            # Process all the rules in reverse order from the entry point.
            while (nRuleCommand >= 0) :

                # Grab the Rule
                kRule = kRules[kRuleKey][nRuleCommand]

                # Process the Comparison (we don't need to do anything if it's an unconditional jump)
                if kRule.kOperator == ECommandType.COMPARE :
 
                    # If this is the node we last jumped from, then we want the result to be true.
                    if (kRule.kOperand[-1] == kRuleKeySource) and (nRuleCommand == nInitialRuleCommand) :
 
                        if kRule.kOperand[1] == ">" :
                            kLocalValidRanges[kRule.kOperand[0]][0] = max(kLocalValidRanges[kRule.kOperand[0]][0], kRule.kOperand[2] + 1)
                        else :
                            kLocalValidRanges[kRule.kOperand[0]][1] = min(kLocalValidRanges[kRule.kOperand[0]][1], kRule.kOperand[2] - 1)
                        #end

                    # Otherwise we want the result to be false.
                    else :
 
                        if kRule.kOperand[1] == "<" :
                            kLocalValidRanges[kRule.kOperand[0]][0] = max(kLocalValidRanges[kRule.kOperand[0]][0], kRule.kOperand[2])
                        else :
                            kLocalValidRanges[kRule.kOperand[0]][1] = min(kLocalValidRanges[kRule.kOperand[0]][1], kRule.kOperand[2])
                        #end
 
                    #end
 
                    # This path is now impossible...
                    if kLocalValidRanges[kRule.kOperand[0]][0] > kLocalValidRanges[kRule.kOperand[0]][1] :
                        return []
                    #end
 
                #end
 
                nRuleCommand -= 1
 
            #end

            # We've reached the rule we actually care about
            if kRuleKey == kTargetNode :
                return kReturnValidRanges
            #end

            # Iterate through all the possible ways we could have gotten to this node
            for kJumpSource in kJumpSources[kRuleKey] :
                kReturnValidRanges += getValidRanges(kJumpSource[0], kRuleKey, kJumpSource[1], kRules, kJumpSources, kTargetNode, kLocalValidRanges)
            #end
 
            # I'm too lazy to return the nodes properly!
            kReturnValidRanges = [k for k in kReturnValidRanges if k["k"] == kTargetNode]

            # Return the Ranges we've found so far, passing up the chain only the IN nodes.
            return kReturnValidRanges
 
        #end
 
    #end

    # This uses the given range to calculate total Permutations
    def calculatePermutations(kValidRange) :
        xRange = kValidRange["x"][1] - kValidRange["x"][0] + 1
        mRange = kValidRange["m"][1] - kValidRange["m"][0] + 1
        aRange = kValidRange["a"][1] - kValidRange["a"][0] + 1
        sRange = kValidRange["s"][1] - kValidRange["s"][0] + 1
        return xRange * mRange * aRange * sRange
    #end

    # This calculates the overlap between two ranges
    def getOverlap(kPrimaryRange, kSecondaryRange) :
        return {"x" : [max(kPrimaryRange["x"][0], kSecondaryRange["x"][0]),
                       min(kPrimaryRange["x"][1], kSecondaryRange["x"][1])],
                "m" : [max(kPrimaryRange["m"][0], kSecondaryRange["m"][0]),
                       min(kPrimaryRange["m"][1], kSecondaryRange["m"][1])],
                "a" : [max(kPrimaryRange["a"][0], kSecondaryRange["a"][0]),
                       min(kPrimaryRange["a"][1], kSecondaryRange["a"][1])],
                "s" : [max(kPrimaryRange["s"][0], kSecondaryRange["s"][0]),
                       min(kPrimaryRange["s"][1], kSecondaryRange["s"][1])]}
    #end
 
    # Get all of the Valid Ranges
    kValidRanges = getValidRanges("A", "", 0, kRules, kJumpSources, kFirstRule)
    nSuperSet    = 0

    # For each Valid Range
    for i, kPrimaryRange in enumerate(kValidRanges) :

        # Increment the Number of Permutations
        nSuperSet += calculatePermutations(kPrimaryRange)

        # Subtract the Overlap
        for kSecondaryRange in kValidRanges[i+1:] :
            kOverlapRange = getOverlap(kPrimaryRange, kSecondaryRange)
            if ((kOverlapRange["x"][0] <= kOverlapRange["x"][1]) and
                (kOverlapRange["m"][0] <= kOverlapRange["m"][1]) and
                (kOverlapRange["a"][0] <= kOverlapRange["a"][1]) and
                (kOverlapRange["s"][0] <= kOverlapRange["s"][1])) :
                nSuperSet -= calculatePermutations(kOverlapRange)
            #end
        #end
    #end
 
    print(f"Part 2: {nSuperSet}")
 
#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end