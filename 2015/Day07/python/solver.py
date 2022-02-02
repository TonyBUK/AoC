import time

def main() :

    def decodeCommand(kTokens) :

        def getLiteral(kToken) :
            if kToken.isnumeric() :
                numeric = int(kToken)
                if numeric in range(65536) :
                    return numeric
                #end
            #end
            return kToken
        #end

        TWO_PARAM_OPCODES = ["AND", "OR", "LSHIFT", "RSHIFT"]
        if kTokens[1] in TWO_PARAM_OPCODES :
            assert(len(kTokens) == 5)
            assert(kTokens[3] == "->")
            return [kTokens[1], [getLiteral(kTokens[0]), getLiteral(kTokens[2])], kTokens[4]]
        elif kTokens[0] == "NOT" :
            assert(len(kTokens) == 4)
            assert(kTokens[2] == "->")
            return ["NOT", [getLiteral(kTokens[1])], kTokens[3]]
        else :
            assert(len(kTokens) == 3)
            assert(kTokens[1] == "->")
            return ["SET", [getLiteral(kTokens[0])], kTokens[2]]
        #end
    #end

    # Read the Input File and refactor all commands into the form:
    # Operator,Inputs(s),Output
    gateCommands = []
    with open("../input.txt", "r") as inputFile:
        gateCommands = [decodeCommand(line.strip().split()) for line in inputFile.readlines()]
    #end

    def processCommands(gateCommands, signals) :

        def getSignalOrLiteral(input, signals) :
            if input in signals : return signals[input]
            return input
        #end

        while len(signals) != len(gateCommands) :

            for gateCommand in gateCommands :

                # Skip Commands we've already handled
                if gateCommand[2] in signals : continue

                # Skip any Commands for which any of the inputs are not yet available
                valid = True
                for input in gateCommand[1] :
                    if type(input) != int :
                        if input not in signals :
                            valid = False
                            break
                        #end
                    #end
                #end
                if valid is False : continue

                # Process the Command
                if gateCommand[0] == "SET" :
                    signals[gateCommand[2]] = getSignalOrLiteral(gateCommand[1][0], signals)
                elif gateCommand[0] == "NOT" :
                    signals[gateCommand[2]] = (~getSignalOrLiteral(gateCommand[1][0], signals)) & 0xFFFF
                elif gateCommand[0] == "LSHIFT" :
                    signals[gateCommand[2]] = (getSignalOrLiteral(gateCommand[1][0], signals) <<
                                               getSignalOrLiteral(gateCommand[1][1], signals))
                elif gateCommand[0] == "RSHIFT" :
                    signals[gateCommand[2]] = (getSignalOrLiteral(gateCommand[1][0], signals) >>
                                               getSignalOrLiteral(gateCommand[1][1], signals))
                elif gateCommand[0] == "AND" :
                    signals[gateCommand[2]] = (getSignalOrLiteral(gateCommand[1][0], signals) &
                                               getSignalOrLiteral(gateCommand[1][1], signals))
                elif gateCommand[0] == "OR" :
                    signals[gateCommand[2]] = (getSignalOrLiteral(gateCommand[1][0], signals) |
                                               getSignalOrLiteral(gateCommand[1][1], signals))
                else :
                    assert(False)
                #end
            #end
        #end
    #end

    # Debug Only Code : Verify the Outputs are actually unique
    # If they're not, then processCommands primary loop function will never terminate!
    outputs = [gateCommand[2] for gateCommand in gateCommands]
    assert(len(outputs) == len(set(outputs)))

    signals = {}
    processCommands(gateCommands, signals)
    print(f"Part 1: {signals['a']}")

    signals = {"b" : signals["a"]}
    processCommands(gateCommands, signals)
    print(f"Part 2: {signals['a']}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
