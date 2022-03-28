import time
import math
import sys

def main() :

    # Read and Transpose the Input Data
    kMessages = []
    with open("../input.txt", "r") as inputFile:
        kMessages = [line.strip() for line in inputFile.readlines()]
    #end

    def getMinMaxMessages(kMessages) :

        if len(kMessages) == 0 : return "", ""

        # Parse each Row by Column and collate the Letter Frequency

        WIDTH           = len(kMessages[0])
        kMessageMetrics = []
        while len(kMessageMetrics) < WIDTH : kMessageMetrics.append({})

        for kLine in kMessages :

            assert(len(kLine) == WIDTH)

            for i,C in enumerate(kLine) :
                if C in kMessageMetrics[i] :
                    kMessageMetrics[i][C] += 1
                else :
                    kMessageMetrics[i][C] = 1
                #end
            #end

        #end

        # Construct a string from each columns minimum / maximum letter

        kMinMessage = ""
        kMaxMessage = ""

        for kMessageMetric in kMessageMetrics :

            kMinLetter = [math.inf, ""]
            kMaxLetter = [0, ""]

            for C,F in kMessageMetric.items() :
                if F < kMinLetter[0] :
                    kMinLetter[0] = F
                    kMinLetter[1] = C
                #end
                if F > kMaxLetter[0] :
                    kMaxLetter[0] = F
                    kMaxLetter[1] = C
                #end
            #end

            kMinMessage += kMinLetter[1]
            kMaxMessage += kMaxLetter[1]

        #end

        return kMinMessage, kMaxMessage

    #end

    kMinMessage, kMaxMessage = getMinMaxMessages(kMessages)

    print(f"Part 1: {kMaxMessage}")
    print(f"Part 2: {kMinMessage}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
