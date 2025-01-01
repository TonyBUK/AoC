import time
import sys
import math

def main() :

    def shortestKeySequences(kKeypadLayout : dict[str, tuple[int, int]], kIllegalPosition : tuple[int, int], kSequence : str, kKey : str = "A", nSequencePosition : int = 0, kCurrentSequence : str = "", kSequences : list[str] = None, nBestLength : int = math.inf) -> tuple[list[str], int] :

        # Output Sequences
        if kSequences is None :
            kSequences = []
        #end

        nCurrentSequenceLength = len(kCurrentSequence)

        # Is this a demonstrably worse solution?
        if nCurrentSequenceLength > nBestLength :
            return kSequences, nBestLength
        #end

        if nSequencePosition == len(kSequence) :
            # We've reached the end of the sequence, add it to the list
            if nCurrentSequenceLength < nBestLength :
                assert(math.isinf(nBestLength))
                nBestLength = nCurrentSequenceLength
                kSequences = [kCurrentSequence]
            else :
                kSequences.append(kCurrentSequence)
            #end
            return kSequences, nBestLength
        #end

        # The optimal solution will *always* use the same key repeated as many times as possible.
        #
        # So really every solution boils down to:
        #
        # 1. Horizontal, Vertical
        # 2. Vertical, Horizontal
        #
        # With the more optimal solution not truly being known until we move on to the next part of the sequence.

        nHorizontalDelta    = kKeypadLayout[kSequence[nSequencePosition]][0] - kKeypadLayout[kKey][0]
        bHorizontalRequired = nHorizontalDelta != 0
        nVerticalDelta      = kKeypadLayout[kSequence[nSequencePosition]][1] - kKeypadLayout[kKey][1]
        bVerticalRequired   = nVerticalDelta != 0

        if bHorizontalRequired and bVerticalRequired :

            # Make sure we're not going through the bottom left
            bValid = kKeypadLayout[kKey][1] != kIllegalPosition[1] or (kKeypadLayout[kKey][0] + nHorizontalDelta) != kIllegalPosition[0]

            if bValid :

                # Horizontal, Vertical
                kSequences, nBestLength = shortestKeySequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=kSequence, kKey=kSequence[nSequencePosition], nSequencePosition=nSequencePosition+1,
                                                               kCurrentSequence=kCurrentSequence + ("<" if nHorizontalDelta < 0 else ">") * abs(nHorizontalDelta) + ("^" if nVerticalDelta < 0 else "v") * abs(nVerticalDelta) + "A",
                                                               kSequences=kSequences, nBestLength=nBestLength)
            #end

            # Make sure we're not going through the bottom left
            bValid = kKeypadLayout[kKey][0] != kIllegalPosition[0] or (kKeypadLayout[kKey][1] + nVerticalDelta) != kIllegalPosition[1]

            if bValid :

                # Vertical, Horizontal
                kSequences, nBestLength = shortestKeySequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=kSequence, kKey=kSequence[nSequencePosition], nSequencePosition=nSequencePosition+1,
                                                               kCurrentSequence=kCurrentSequence + ("^" if nVerticalDelta < 0 else "v") * abs(nVerticalDelta) + ("<" if nHorizontalDelta < 0 else ">") * abs(nHorizontalDelta) + "A",
                                                               kSequences=kSequences, nBestLength=nBestLength)
            #end

        elif bHorizontalRequired :

            # Horizontal
            kSequences, nBestLength = shortestKeySequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=kSequence, kKey=kSequence[nSequencePosition], nSequencePosition=nSequencePosition+1,
                                                           kCurrentSequence=kCurrentSequence + ("<" if nHorizontalDelta < 0 else ">") * abs(nHorizontalDelta) + "A",
                                                           kSequences=kSequences, nBestLength=nBestLength)
        elif bVerticalRequired :

            # Vertical
            kSequences, nBestLength = shortestKeySequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=kSequence, kKey=kSequence[nSequencePosition], nSequencePosition=nSequencePosition+1,
                                                           kCurrentSequence=kCurrentSequence + ("^" if nVerticalDelta < 0 else "v") * abs(nVerticalDelta) + "A",
                                                           kSequences=kSequences, nBestLength=nBestLength)
        else :

            # Button Press Only
            kSequences, nBestLength = shortestKeySequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=kSequence, kKey=kSequence[nSequencePosition], nSequencePosition=nSequencePosition+1,
                                                           kCurrentSequence=kCurrentSequence + "A",
                                                           kSequences=kSequences, nBestLength=nBestLength)
        #end

        return kSequences, nBestLength

    #end

    def robotSequences(kKeypadLayout : dict[str, tuple[int, int]], kIllegalPosition : tuple[int, int], kSequence : str, kCache : dict[tuple[int, str, str], int], nNumRobots : int, nCurrentRobot : int = 0, kCurrentKey : str = "A") -> int :

        # Initialse the Cache
        if kCache == None :
            kCache = {}
        #end

        # Iterate through the sequence
        nCurrentLengthLevel = 0
        for kNextKey in kSequence :

            # Check if we've seen this sequence before
            kPair = tuple([nCurrentRobot, kCurrentKey, kNextKey])

            # If we haven't...
            if kPair not in kCache :

                # Calculate the shortest key sequence(s)
                kSequences, nLength = shortestKeySequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=[kNextKey], kKey=kCurrentKey)

                # If we're at the last robot, just add the length
                if nCurrentRobot == (nNumRobots - 1) :

                    kCache[kPair]        = nLength
                    nCurrentLengthLevel += nLength

                else :

                    # Otherwise we need to calculate the next robot's sequence, so pick the shortest one
                    nMinDeltaLength = math.inf
                    for kSubsequence in kSequences :
                        nDeltaLength = robotSequences(kKeypadLayout=kKeypadLayout, kIllegalPosition=kIllegalPosition, kSequence=kSubsequence, nNumRobots=nNumRobots, nCurrentRobot=nCurrentRobot+1, kCache=kCache)
                        nMinDeltaLength = min(nMinDeltaLength, nDeltaLength)
                    #end

                    nCurrentLengthLevel += nMinDeltaLength
                    kCache[kPair]        = nMinDeltaLength

                #end

            else :

                # Increment by the previously calculate Delta
                nCurrentLengthLevel += kCache[kPair]

            #end

            # Store the Next Key
            kCurrentKey = kNextKey

        #end

        # Return the Length

        return nCurrentLengthLevel

    #end

    kSequences = []
    with open("../input.txt", "r") as inputFile:
        kSequences = inputFile.read().split("\n")
    #end

    KEYPAD_LAYOUT_NUMBER_PAD = {
        "7" : (0, 0), "8" : (1, 0), "9" : (2, 0),
        "4" : (0, 1), "5" : (1, 1), "6" : (2, 1),
        "1" : (0, 2), "2" : (1, 2), "3" : (2, 2),
                      "0" : (1, 3), "A" : (2, 3)
    }
    ILLEGAL_POSITION_NUMBER_PAD = (0, 3)

    KEYPAD_LAYOUT_DIRECTION_PAD = {
                      "^" : (1, 0), "A" : (2, 0),
        "<" : (0, 1), "v" : (1, 1), ">" : (2, 1)
    }
    ILLEGAL_POSITION_DIRECTION_PAD = (0, 0)

    nPartOne = 0
    nPartTwo = 0


    kCachePartOne = {}
    kCachePartTwo = {}
    for i,kSequence in enumerate(kSequences) :

        # Calculate the Keypad Sequence
        if 0 == i :
            kCurrentCodeSequences, _ = shortestKeySequences(kKeypadLayout=KEYPAD_LAYOUT_NUMBER_PAD, kIllegalPosition=ILLEGAL_POSITION_NUMBER_PAD, kSequence=kSequence)
        else :
            kCurrentCodeSequences, _ = shortestKeySequences(kKeypadLayout=KEYPAD_LAYOUT_NUMBER_PAD, kIllegalPosition=ILLEGAL_POSITION_NUMBER_PAD, kSequence=kSequence, kKey=kSequences[i-1][-1])
        #end

        nBestLengthPartOne = math.inf
        nBestLengthPartTwo = math.inf

        # Iterate through the Robots for the Specified Number of Robots and Compute the Best Length
        for kCurrentCodeSequence in kCurrentCodeSequences :

            nLength = robotSequences(kKeypadLayout=KEYPAD_LAYOUT_DIRECTION_PAD, kIllegalPosition=ILLEGAL_POSITION_DIRECTION_PAD, kSequence=kCurrentCodeSequence, nNumRobots=2,  kCache=kCachePartOne)
            nBestLengthPartOne = min(nLength, nBestLengthPartOne)

            nLength = robotSequences(kKeypadLayout=KEYPAD_LAYOUT_DIRECTION_PAD, kIllegalPosition=ILLEGAL_POSITION_DIRECTION_PAD, kSequence=kCurrentCodeSequence, nNumRobots=25, kCache=kCachePartTwo)
            nBestLengthPartTwo = min(nLength, nBestLengthPartTwo)

        #end

        # Calculate the Integer Component of the Result
        nIntValue = 0
        for k in kSequence :
            if k in "0123456789" :
                nIntValue = nIntValue * 10 + int(k)
            #end
        #end

        # Update the Part One/Two Results
        nPartOne += nIntValue * nBestLengthPartOne
        nPartTwo += nIntValue * nBestLengthPartTwo

    #end

    print(f"Part 1: {nPartOne}")
    print(f"Part 2: {nPartTwo}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end