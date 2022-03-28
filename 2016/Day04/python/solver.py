import time
import sys

def main() :

    def extractEncryptedData(kRaw) :

        # Double check this contains the Bare Minimum
        assert("-" in kRaw)
        assert("]" in kRaw)
        assert("[" in kRaw)

        # Split the Data into a list, for simplicity, normalise the square brackets first
        kTokens = kRaw.replace("]","").replace("[","-").split("-")
        assert(len(kTokens) >= 2) # We need at least 2 elements

        # Store the Data
        kData = {}
        kData["encodedStrings"] = [kEncoded for kEncoded in kTokens[:-2]]
        kData["sectorId"]       = int(kTokens[-2])
        kData["checkSum"]       = kTokens[-1]

        return kData

    #end

    # Read the Encrypted Data
    kEncryptedData = []
    with open("../input.txt", "r") as inputFile:
        kEncryptedData = [extractEncryptedData(line.strip()) for line in inputFile.readlines()]
    #end

    def calculateChecksum(kEncodedStrings) :
        kStringMetrics = {}
        for kEncodedString in kEncodedStrings :
            for C in kEncodedString :
                if not C in kStringMetrics :
                    kStringMetrics[C] = 0
                #end
                kStringMetrics[C] += 1
            #end
        #end

        kStringMetricsByFrequency = {}
        for kString,nStringMetric in kStringMetrics.items() :
            if not nStringMetric in kStringMetricsByFrequency :
                kStringMetricsByFrequency[nStringMetric] = ""
            #end
            kStringMetricsByFrequency[nStringMetric] += kString
            kStringMetricsByFrequency[nStringMetric] = sorted(kStringMetricsByFrequency[nStringMetric])
        #end

        kChecksum = ""

        for nIndex in sorted(kStringMetricsByFrequency.keys(), reverse=True) :
            for C in kStringMetricsByFrequency[nIndex] :
                kChecksum += C
                if len(kChecksum) == 5: return kChecksum
            #end
        #end

        return kChecksum

    #end

    def decryptData(kEncodedString, nSectorId) :
        ALPHABET = "abcdefghijklmnopqrstuvwxyz"
        kDecryptedData = "".join([ALPHABET[((ALPHABET.index(C) + nSectorId) % len(ALPHABET))] for C in kEncodedString])
        # Equivalent to:
        # kDecryptedData = ""
        # for C in kEncodedString :
        #     nUnrotatedIndex = ALPHABET.index(C)
        #     nRotatedIndex   = (nUnrotatedIndex + nSectorId) % len(ALPHABET)
        #     kDecryptedData += ALPHABET[nRotatedIndex]
        # #end
        return kDecryptedData
    #end

    sectorSum       = 0
    sectorNorthPole = 0
    for currentData in kEncryptedData :

        # Validate the Checksum
        if currentData["checkSum"] == calculateChecksum(currentData["encodedStrings"]) :

            sectorSum += currentData["sectorId"]

            # Decrypt the Data
            for kEncodedString in currentData["encodedStrings"] :
                if "northpole" == decryptData(kEncodedString, currentData["sectorId"]) :
                    assert(0 == sectorNorthPole)
                    sectorNorthPole = currentData["sectorId"]
                #end
            #end
        #end
    #end

    print(f"Part 1: {sectorSum}")
    print(f"Part 2: {sectorNorthPole}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
