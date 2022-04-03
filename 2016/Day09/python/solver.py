import time
import sys

def main() :

    # Read and Compressed Text
    kCompressed = []
    with open("../input.txt", "r") as inputFile:
        kCompressed = [line.strip() for line in inputFile.readlines()]
    #end

    def decompressText(kCompressedText, bRecurse, bActuallyDecompress) :

        # Decryption States
        COPYING_TEXT        = 0
        DECODING_EXPANSION  = 1
        EXPANDING_TEXT      = 2

        # Decryption State
        eDecryptionState    = COPYING_TEXT

        # Decrypted Text/Length
        kDecryptedText      = ""
        nDecryptedLength    = 0

        # Expansion Command {LENGTH}x{FREQUENCY}
        kExpansionCommand   = ""
        nRepeatLength       = 0
        nRepeatFrequency    = 0

        # Expanded Text Fragment
        kExpansionText      = 0
        nExpansionLength    = 0

        for c in kCompressedText :

            if COPYING_TEXT == eDecryptionState :

                # Copying Text: Keep going until we reach a "("

                if "(" == c :
                    eDecryptionState  = DECODING_EXPANSION
                    kExpansionCommand = ""
                else :
                    if bActuallyDecompress : kDecryptedText += c
                    nDecryptedLength += 1
                #end

            elif DECODING_EXPANSION == eDecryptionState :

                # Decoding Expansion: Keep going until we reach a ")"

                if ")" == c :
                    eDecryptionState = EXPANDING_TEXT
                    assert(-1 != kExpansionCommand.find("x"))
                    kExpansionCommandList = kExpansionCommand.split("x")
                    assert(len(kExpansionCommandList) == 2)
                    
                    nRepeatLength    = int(kExpansionCommandList[0])
                    nRepeatFrequency = int(kExpansionCommandList[1])

                    kExpansionText   = ""
                    nExpansionLength = 0
                else :
                    kExpansionCommand += c
                #end

            elif EXPANDING_TEXT == eDecryptionState :

                # Expanding Text: Keep going until we've reached all possible characters in the 

                if nExpansionLength < nRepeatLength :
                    nExpansionLength += 1
                    if bActuallyDecompress or bRecurse : kExpansionText += c
                #end

                if nExpansionLength == nRepeatLength :
                    if False == bRecurse :
                        nDecryptedLength += (nRepeatFrequency * nRepeatLength)
                        if bActuallyDecompress :
                            for _ in range(nRepeatFrequency) :
                                kDecryptedText += kExpansionText
                            #end
                        #end
                    else :
                        nFragementDecryptedText, nFragmentDecryptedLength = decompressText(kExpansionText, bRecurse, bActuallyDecompress)
                        nDecryptedLength += nRepeatFrequency * nFragmentDecryptedLength
                        if bActuallyDecompress :
                            for _ in range(nRepeatFrequency) :
                                kDecryptedText += nFragementDecryptedText
                            #end
                        #end
                    #end
                    eDecryptionState = COPYING_TEXT
                #end

            #end

        #end

        return kDecryptedText, nDecryptedLength

    #end

    for kLine in kCompressed :

        kText, nLength = decompressText(kLine, False, False)
        print(f"Part 1: {nLength}")

        kText, nLength = decompressText(kLine, True, False)
        print(f"Part 2: {nLength}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)*1000}ms", file=sys.stderr)
#end