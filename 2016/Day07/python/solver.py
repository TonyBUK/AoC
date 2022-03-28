import time
import math
import sys

def main() :

    def decodeAddress(kRawAddress) :
        
        assert("[" in kRawAddress)
        assert("]" in kRawAddress)
        assert(kRawAddress.count("[") == kRawAddress.count("]"))

        kAddress = {}
        kAddress["hypernet"] = []
        kAddress["address"]  = []

        # Note: This could yield some blank entries, but that shouldn't break
        #       anything with regards to parsing later on.

        # Decode all Addresses/Hypernet Addresses
        nHypernetEnd = -1
        nHypernetStart = kRawAddress.find("[")
        while nHypernetStart != -1 :

            kAddress["address"].append(kRawAddress[nHypernetEnd+1:nHypernetStart])
            nHypernetEnd = kRawAddress.find("]", nHypernetStart + 1)
            kAddress["hypernet"].append(kRawAddress[nHypernetStart+1:nHypernetEnd])
            nHypernetStart = kRawAddress.find("[", nHypernetEnd + 1)

        #end

        # And the trailing Hypernet Address
        kAddress["address"].append(kRawAddress[nHypernetEnd+1:])

        return kAddress

    #end

    # Read and Decode the Addresses
    kAddresses = []
    with open("../input.txt", "r") as inputFile:
        kAddresses = [decodeAddress(line.strip()) for line in inputFile.readlines()]
    #end

    def countTLS(kAddresses) :

        # This checks 4 characters at a time for the ABBA sequence
        def containsABBA(kText) :
            for k1,k2,k3,k4 in zip(kText, kText[1:], kText[2:], kText[3:]) :
                if k1 == k2                  : continue
                if (k1 == k4) and (k2 == k3) : return True
            #end
            return False
        #end

        nTLSCount = 0

        for kAddress in kAddresses :

            # If the Hypernet Sequences contain the ABBA sequence, then
            # this can't support TLS
            bHypernetContainsABBA = False
            for kHypernetAddress in kAddress["hypernet"] :
                if containsABBA(kHypernetAddress) :
                    bHypernetContainsABBA = True
                    break
                #end
            #end
            if bHypernetContainsABBA : continue

            # Now we've checked Hypernet, check all address parts for TLS support
            for kIPAddress in kAddress["address"] :
                if containsABBA(kIPAddress) :
                    nTLSCount += 1
                    break
                #end
            #end

        #end

        return nTLSCount

    #end

    def countSSL(kAddresses) :

        # This checks 3 characters at a time for the ABA sequence and retrieves all
        # inverse sequences
        def containsABA(kText) :
            kInverseSequences = set()
            for k1,k2,k3 in zip(kText, kText[1:], kText[2:]) :
                if (k1 == k3) and (k1 != k2) : kInverseSequences.add(k2 + k1 + k2)
            #end
            return kInverseSequences
        #end

        nSSLCount = 0

        for kAddress in kAddresses :

            for kIPAddress in kAddress["address"] :

                # Retrieve all possible Inverse Sequences
                kABAInverseSequences = containsABA(kIPAddress)

                # Search the Hypernet
                bInverseFound = False
                for kABAInverseSequence in kABAInverseSequences :
                    for kHypernetAddress in kAddress["hypernet"] :
                        if kABAInverseSequence in kHypernetAddress :
                            bInverseFound = True
                            break
                        #end
                    else :
                        continue
                    break
                    #end
                #end

                # If we found an Inverse Sequence, no need to search again, so break
                if bInverseFound :
                    nSSLCount += 1
                    break
                #end

            #end

        #end

        return nSSLCount

    #end

    print(f"Part 1: {countTLS(kAddresses)}")
    print(f"Part 2: {countSSL(kAddresses)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
