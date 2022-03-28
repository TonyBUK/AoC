import time
import sys

def main() :

    def cleanByteCode(tokens) :

        assert(len(tokens) >= 2)
        assert(len(tokens) <= 3)

        opCode = [tokens[0]]
        for token in tokens[1:] :
            if "a" == token or "b" == token :
                opCode.append(token)
            else :
                opCode.append(int(token))
            #end
        #end

        return opCode

    #end

    # Read/Clean the Input File to extract the Byte Code Sequence
    byteCode = []
    with open("../input.txt", "r") as inputFile:
        byteCode = [cleanByteCode(line.strip().replace(",", "").replace("+", "").split()) for line in inputFile.readlines()]
    #end

    # Byte Code Machine
    def runByteCode(byteCode, registers) :

        address = 0

        while (address < len(byteCode)) :

            opCode = byteCode[address]

            if "hlf" == opCode[0] : # Half Register
                registers[opCode[1]] //= 2
                address += 1
            elif "tpl" == opCode[0] : # Triple Register
                registers[opCode[1]] *= 3
                address += 1
            elif "inc" == opCode[0] : # Increment Register
                registers[opCode[1]] += 1
                address += 1
            elif "jmp" == opCode[0] : # Jump (Always)
                address += opCode[1]
            elif "jie" == opCode[0] : # Jump (If Register is Even)
                if (registers[opCode[1]] % 2) == 0 :
                    address += opCode[2]
                else :
                    address += 1
                #end
            elif "jio" == opCode[0] : # Jump (If Register is One)
                if 1 == registers[opCode[1]] :
                    address += opCode[2]
                else :
                    address += 1
                #end
            else :
                assert(False)
            #end

        #end

        return registers

    #end

    print(f"Part 1: {runByteCode(byteCode, {'a' : 0, 'b' : 0})['b']}")
    print(f"Part 2: {runByteCode(byteCode, {'a' : 1, 'b' : 0})['b']}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
