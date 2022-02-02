import time

def main() :

    # Read/Clean the Input File
    rawStrings = []
    with open("../input.txt", "r") as inputFile:
        rawStrings = [line.strip() for line in inputFile.readlines()]
    #end

    # Perform the Escape Encoding
    def escape(text) :
        CHARACTED_TO_ESCAPE = "\\\""
        escapedText = "\""
        for c in text :
            if c in CHARACTED_TO_ESCAPE :
                escapedText += "\\"
            #end
            escapedText += c
        #end
        return escapedText + "\""
    #end

    # Perform the Escape Decoding
    def unescape(text) :

        unescapedText = ""
        isEscape      = False
        index         = 0
        lenAdjust     = 0

        # Handle if the Text is embedded within Quoutes
        if "\"" == text[0] and "\"" == text[-1] :
            index     = 1
            lenAdjust = -1
        #end

        while index < (len(text)+lenAdjust) :
            if isEscape is False :
                if "\\" == text[index] :
                    isEscape = True
                else :
                    unescapedText += text[index]
                #end
            else :
                if "x" == text[index] :
                    unescapedText += chr(int(text[index+1:index+2], base=16))
                    index += 2
                else :
                    unescapedText += text[index]
                #end
                isEscape = False
            #end
            index += 1
        #end
        return unescapedText
    #end

    nRawLength     = 0
    nDecodedLength = 0
    nEncodedLength = 0
    for line in rawStrings :
        nRawLength     += len(line)
        nDecodedLength += len(unescape(line))
        nEncodedLength += len(escape(line))
    #end

    print(f"Part 1: {nRawLength-nDecodedLength}")
    print(f"Part 2: {nEncodedLength-nRawLength}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
