import time
import sys

DECRYPTION_KEY = 811589153
LEFT           = 0
VALUE          = 1
RIGHT          = 2

def main() :

    with open("../input.txt", "r") as inputFile:

        def applyMixingSequence(kMixerSequence) :

            MODULO_SIZE     = len(kMixerSequence) - 1
            HALF_WAY        = MODULO_SIZE // 2
            MINUS_HALF_WAY  = -HALF_WAY
            CHANGE_LEFT     = -1 - MODULO_SIZE
            CHANGE_RIGHT    = MODULO_SIZE + 1

            for i in range(len(kMixerSequence)) :

                nValue = kMixerSequence[i][VALUE]

                # Nothing to do for Zero
                if 0 != nValue :

                    nLeft  = kMixerSequence[i][LEFT]
                    nRight = kMixerSequence[i][RIGHT]
                    nIndex = i

                    # Limit the Range of the Value to avoid loop
                    # but once going through the loop, we start wanting to adjust for "ourselves"
                    # so the modulo is 1 less...
                    if nValue > 0 :
                        nValue %= MODULO_SIZE
                        
                        # If we're past the half way point move left instead
                        if nValue > HALF_WAY :
                            nValue += CHANGE_LEFT
                        #end
                    else :
                        nValue = -1 - (-nValue % MODULO_SIZE)

                        # If we're past the half way point move right instead
                        if nValue < MINUS_HALF_WAY :
                            nValue += CHANGE_RIGHT
                        #end
                    #end

                    if nValue > 0 :
                        # Traverse Right through the Circular Buffer (the slow bit!)
                        for _ in range(nValue) :
                            nIndex = kMixerSequence[nIndex][RIGHT]
                        #end
                    else :
                        # Traverse Left through the Circular Buffer (the slow bit!)
                        for _ in range(0,nValue,-1) :
                            nIndex = kMixerSequence[nIndex][LEFT]
                        #end
                    #end

                    # Update the Left/Right of the Neighbour Pair we broke by moving
                    kMixerSequence[nLeft][RIGHT] = nRight
                    kMixerSequence[nRight][LEFT] = nLeft

                    # Get the New Neighbour Pair
                    nNewRight = kMixerSequence[nIndex][RIGHT]
                    nNewLeft  = nIndex

                    # Link Ourselves Left/Right with the New Neighbours
                    kMixerSequence[nNewRight][LEFT] = i
                    kMixerSequence[nNewLeft][RIGHT] = i

                    # Insert Ourselves
                    kMixerSequence[i][LEFT]  = nNewLeft
                    kMixerSequence[i][RIGHT] = nNewRight

                #end

            #end

        #end

        def generateIndirectLookup(kMixerIndexes, kMixerSequence) :

            # Generate the Indexes for a Faster Lookup
            # The actual start point is arbitrary.
            nIndex     = 0
            nZeroIndex = None

            for i in range(len(kMixerIndexes)) :
                kMixerIndexes[i] = nIndex
                if kMixerSequence[nIndex][VALUE] == 0 :
                    nZeroIndex = i
                #end
                nIndex = kMixerSequence[nIndex][RIGHT]
            #end

            return nZeroIndex

        #end

        kMixerRawSequence = [int(X) for X in inputFile.readlines()]
        kMixerSequence    = [[  (k - 1 + len(kMixerRawSequence)) % len(kMixerRawSequence), 
                                 kMixerRawSequence[k],
                                 (k + 1)                       % len(kMixerRawSequence)]
                                for k in range(len(kMixerRawSequence))]
        kMixerIndexes = [X for X in range(len(kMixerRawSequence))]

        # Part 1

        # Mix Once
        applyMixingSequence(kMixerSequence)

        # Generate the Indexes for a Faster Lookup
        nZeroIndex = generateIndirectLookup(kMixerIndexes, kMixerSequence)

        # Get the Grove Co-Ordinates
        nOuputs1 = kMixerSequence[kMixerIndexes[(1000+nZeroIndex) % len(kMixerSequence)]][VALUE]
        nOuputs2 = kMixerSequence[kMixerIndexes[(2000+nZeroIndex) % len(kMixerSequence)]][VALUE]
        nOuputs3 = kMixerSequence[kMixerIndexes[(3000+nZeroIndex) % len(kMixerSequence)]][VALUE]

        print(f"Part 1: {nOuputs1+nOuputs2+nOuputs3}")

        # Part 2

        # Apply Decryption Key
        kMixerSequence    = [[  (k - 1 + len(kMixerRawSequence)) % len(kMixerRawSequence), 
                                 kMixerRawSequence[k] * DECRYPTION_KEY,
                                 (k + 1)                       % len(kMixerRawSequence)]
                                for k in range(len(kMixerRawSequence))]

        # Mix 10 Times
        for i in range(10) :
            applyMixingSequence(kMixerSequence)
        #end

        # Generate the Indexes for a Faster Lookup
        nZeroIndex = generateIndirectLookup(kMixerIndexes, kMixerSequence)

        # Get the Grove Co-Ordinates
        nOuputs1 = kMixerSequence[kMixerIndexes[(1000+nZeroIndex) % len(kMixerSequence)]][VALUE]
        nOuputs2 = kMixerSequence[kMixerIndexes[(2000+nZeroIndex) % len(kMixerSequence)]][VALUE]
        nOuputs3 = kMixerSequence[kMixerIndexes[(3000+nZeroIndex) % len(kMixerSequence)]][VALUE]

        print(f"Part 2: {nOuputs1+nOuputs2+nOuputs3}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
