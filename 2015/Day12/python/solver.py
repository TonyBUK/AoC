import time
import json

def main() :

    # Read/Clean the Input File
    parsedJSON = []
    with open("../input.txt", "r") as inputFile:
        parsedJSON = [json.loads(line.strip()) for line in inputFile.readlines()]
    #end

    def getJSONTotal(jsonRoot, filter="", total=0) :
        
        jsonValues = []

        # Determine what we need to actually iterate on
        if type(jsonRoot) == dict :
            # Handle the Filter if defined
            if filter != "" :
                if filter in jsonRoot.values() :
                    return total
                #end
            #end
            jsonValues = jsonRoot.values()
        elif type(jsonRoot) == list :
            jsonValues = jsonRoot
        else :
            assert(False)
        #end

        # Either add any integers, or recurse on any lists/dictionaries
        for jsonItem in jsonValues :
            if type(jsonItem) == int :
                total += jsonItem
            elif type(jsonItem) == list :
                total = getJSONTotal(jsonItem, filter, total)
            elif type(jsonItem) == dict :
                total = getJSONTotal(jsonItem, filter, total)
            #end
        #end
        
        return total
    #end

    for jsonRoot in parsedJSON :
        print(f"Part 1: {getJSONTotal(jsonRoot)}")
        print(f"Part 2: {getJSONTotal(jsonRoot, 'red')}")
    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
