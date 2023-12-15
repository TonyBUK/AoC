import time
import sys

def main() :

    kHashInput = []
    with open("../input.txt", "r") as inputFile:
        kHashInput = inputFile.read().replace("\n", "").split(",")
    #end

    def calculateHash(kString) :

        nHash = 0
        for k in kString :

            nHash += ord(k)
            nHash *= 17
            nHash %= 256

        #end
            
        return nHash

    #end

    print(f"Part 1: {sum([calculateHash(k) for k in kHashInput])}")

    # Part 2:
    kBoxes       = [[] for _ in range(256)]
    kLabelValues = {}

    for k in kHashInput :

        # Assignment
        if "=" in k :

            kHashTokens = k.split("=") 
            assert(len(kHashTokens) == 2)

            # Calculate the Label / Box and Value
            kHashLabel  = kHashTokens[0]
            nHashBox    = calculateHash(kHashLabel)
            nHashValue  = int(kHashTokens[1])

            # Append to the Box if Needed
            if kHashLabel not in kBoxes[nHashBox] :
                kBoxes[nHashBox].append(kHashLabel)
            #end

            # Update the Label Value (Note: This is deliberately not stored in the Boxes themselves to simplify searching)
            kLabelValues[kHashLabel] = nHashValue

        elif "-" in k :

            assert(k.count("-") == 1)
            assert(k.endswith("-"))

            # Calculate the Label / Box
            kHashLabel  = k[:-1]
            nHashBox    = calculateHash(kHashLabel)

            # Remove the Label from the Box if Needed
            if kHashLabel in kBoxes[nHashBox] :
                kBoxes[nHashBox].pop(kBoxes[nHashBox].index(kHashLabel))
            #end
                
            # Note: The Label Value is fine to leave in the dictionary, as it is no longer referenced
            #       by any box, and will therefore not form part of the Power Calculation

        else :
            assert(False)
        #end

    #end

    # Calculate the Power of the Boxes
    print(f"Part 2: {sum((nBox + 1) * (nSlot + 1) * kLabelValues[kBoxes[nBox][nSlot]]for nBox in range(len(kBoxes)) for nSlot in range(len(kBoxes[nBox])))}")

#end

if __name__ == "__main__" :
   startTime = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end