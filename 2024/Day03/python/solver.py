import time
import sys
 
def main() :
 
    def getNextToken(kInput : str, kAllowsStartTokens : list[str], nStart : int = 0) -> str :
 
        # Determines if this is one of the allowed start tokens
        def isStartToken(kInput : str, kAllowsStartTokens : list[str], nOffset) -> bool :
            for kAllowedToken in kAllowsStartTokens :
                if kInput.startswith(kAllowedToken, nOffset) :
                    return True, len(kAllowedToken)
                #end
            #end
            return False, 0
        #end
 
        # Find the Start of the next Token
        nTokenLength = 0
 
        while True :
 
            bFound, nTokenLength = isStartToken(kInput, kAllowsStartTokens, nStart)
 
            if bFound :
                break
            #end
 
            nStart += 1
            if nStart >= len(kInput) :
                return "", ""
            #end
        #end
 
        # Find the End of the Token
        # Note: There's one annoying edge case where the token is
        #       never actually terminated, but a new token starts
        #       afterwards, so repeat the process using that as the
        #       new start point.
 
        nEnd = nStart + nTokenLength
 
        # Note: Handle an Edge Case where a line may end with "mul("
        if nEnd >= len(kInput) :
            return "", ""
        #end
 
        # Search for the End Terminator
        while kInput[nEnd] != ")" :
 
            nEnd += 1
            if nEnd >= len(kInput) :
                return "", ""
            else :
                bFound, _ = isStartToken(kInput, kAllowsStartTokens, nEnd)
                if bFound :
                    return getNextToken(kInput, kAllowsStartTokens, nEnd)
                #end
            #end
        #end
 
        return kInput[nStart:nEnd+1], kInput[nEnd:]
 
    #end
 
    # Read the Instructions
    kInstructions = []
 
    with open("../input.txt", "r") as inputFile:
        kInstructions = [k for k in inputFile.readlines()]
    #end
 
    # Allowed Start Tokens for Both Parts
    # Note: End Tokens are always ")"
    ALLOWED_TOKENS_PART_ONE = ["mul("]
    ALLOWED_TOKENS_PART_TWO = ALLOWED_TOKENS_PART_ONE + ["don't(", "do("]
 
    # Valid Characters for Both Parts
    VALID_CHARS_PART_ONE = set("mul(0123456789,)")
    VALID_CHARS_PART_TWO = VALID_CHARS_PART_ONE | set("don't")
 
    nPartOne = 0
    nPartTwo = 0
    bDo     = True
 
    # Solve both Parts...
    for kInstruction in kInstructions :
 
        # Cache the Tokens
        kTokens = kInstruction
 
        while True :
 
            # Get the Next Token
            kToken, kTokens = getNextToken(kTokens, ALLOWED_TOKENS_PART_TWO)
 
            if kTokens == "" :
                break
            #end
 
            # By Default they're valid...
            bValidPartOne = True
            bValidPartTwo = True
 
            # Ensure there's no invalid characters (Part One)
            if set(kToken) - VALID_CHARS_PART_ONE :
                bValidPartOne = False
            #end
 
            # Ensure there's no invalid characters (Part Two)
            if set(kToken) - VALID_CHARS_PART_TWO :
                bValidPartTwo = False
            #end
 
            # We know it starts with mul( and ends with )
            # so it's just the inner data for Part One.
            if not "," in kToken :
                bValidPartOne = False
            #end
 
            # If Part One is valid, do the multiplication
            # Note: This also handles Part Two as well...
            if bValidPartOne :
 
                # Get the Multiplication Values
                # Note: The challenge has a rule that input digits will be in the range 1 .. 3, and
                #       since it's a multiplication, we can effectively "cheat" by forcing any digit
                #       outside of this range to be 0.
                kValues = [0 if (len(x) not in range (1, 3 + 1)) else int(x) for x in kToken[4:-1].split(",")]
 
                nPartOne += kValues[0] * kValues[1]
 
                if bDo :
                    nPartTwo += kValues[0] * kValues[1]
                #end
            # Otherwise it's a do or a don't...
            elif bValidPartTwo :
                if kToken == "do()" :
                    bDo = True
                elif kToken == "don't()" :
                    bDo = False
                #end
            #end
        #end
    #end
 
    print(f"Part 1: {nPartOne}")
    print(f"Part 2: {nPartTwo}")
 
#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end