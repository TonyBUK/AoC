import time
import sys

def main() :

    WRAPPING_PAPER ={   "children"      : 3,
                        "cats"          : 7,
                        "samoyeds"      : 2,
                        "pomeranians"   : 3,
                        "akitas"        : 0,
                        "vizslas"       : 0,
                        "goldfish"      : 5,
                        "trees"         : 3,
                        "cars"          : 2,
                        "perfumes"      : 1
                    }

    # Or don't...
    def extractSue(tokens) :
        assert(len(tokens) >= 2)
        assert((len(tokens) % 2) == 0)
        return {int(tokens[1]) : {k: int(v) for k,v in zip(*[iter(tokens[2:])] * 2)}}
    #end

    # Read/Clean the Input File to extract the Ingredient Properties
    auntSues = {}
    with open("../input.txt", "r") as inputFile:
        auntSues = {k: v
                        for line in inputFile.readlines()
                            for k,v in extractSue(line.replace(":", "").replace(",", "").replace("\n", "").split(" ")).items()}
    #end

    def whichSue(wrappingPaperCompounds, sues, greaterThan = None, fewerThan = None) :

        if greaterThan is None : greaterThan = []
        if fewerThan   is None : fewerThan   = []

        # Iterate over each Sue
        for sue,compounds in sues.items() :

            # Initially assume this is our Sue
            validSue = True

            # Iterate over each compound you remember about Sue
            for compound,quantity in compounds.items() :

                # This should always be the case...
                if compound in wrappingPaperCompounds :

                    # Determine how to compare the compound
                    # Greater Than, Fewer Than or Equal To
                    if compound in greaterThan :
                        validSue = quantity > wrappingPaperCompounds[compound]
                    elif compound in fewerThan :
                        validSue = quantity < wrappingPaperCompounds[compound]
                    else :
                        validSue = quantity == wrappingPaperCompounds[compound]
                    #end

                    # If this isn't our Sue, move onto the next Sue
                    if validSue is False : break

                else :

                    # Can't happen unless the MFCSAM ticker tape list wasn't exhaustive
                    assert(False)

                #end

            #end

            # If everything we remembered about Sue matches, we've found our Sue!
            if validSue : return sue

        #end

        # Shouldn't get here unless all the Sue's were wrong?!?
        assert(False)

    #end

    print(f"Part 1: {whichSue(WRAPPING_PAPER, auntSues)}")
    print(f"Part 2: {whichSue(WRAPPING_PAPER, auntSues, ['cats', 'trees'], ['pomeranians', 'goldfish'])}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
