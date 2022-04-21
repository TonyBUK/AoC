import time
import sys

def main() :

    # Read and Compressed Text
    kAssembunny = []
    with open("../input.txt", "r") as inputFile:
        kAssembunny = [[int(C) if C.replace("-","").isnumeric() else C for C in line.strip().split(" ")] for line in inputFile.readlines()]
    #end

    def patchAssembunny(kAssembunny) :

        for i, kOpcode in enumerate(kAssembunny) :

            if "jnz"      != kOpcode[0]          : continue
            if -2         != kOpcode[2]          : continue
            if i          <  2                   : continue
            if "dec"      != kAssembunny[i-1][0] : continue
            if kOpcode[1] != kAssembunny[i-1][1] : continue
            if "inc"      != kAssembunny[i-2][0] : continue

            kAssembunny[i-2] = ["add", kOpcode[1], kAssembunny[i-2][1]]
            kAssembunny[i-1] = ["cpy", 0, kOpcode[1]]
            kAssembunny[i]   = ["nop"]

        #end
    #end

    def processAssembunny(kAssembunny, kRegisters = None) :

        if None == kRegisters :
            kRegisters = {
                "a" : 0,
                "b" : 0,
                "c" : 0,
                "d" : 0
            }
        #end

        nAddress        = 0
        while nAddress < len(kAssembunny) :

            kOpcode = kAssembunny[nAddress]

            if "jnz" == kOpcode[0] :

                kOperand = kRegisters[kOpcode[1]] if kOpcode[1] in kRegisters else kOpcode[1]
                if 0 != kOperand :
                    nAddress += kOpcode[2]
                else :
                    nAddress += 1
                #end

            elif "cpy" == kOpcode[0] :

                kRegisters[kOpcode[2]] = kRegisters[kOpcode[1]] if kOpcode[1] in kRegisters else kOpcode[1]
                nAddress += 1

            elif "inc" == kOpcode[0] :

                kRegisters[kOpcode[1]] += 1
                nAddress += 1

            elif "dec" == kOpcode[0] :

                kRegisters[kOpcode[1]] -= 1
                nAddress += 1

            elif "add" == kOpcode[0] :

                kRegisters[kOpcode[2]] += kRegisters[kOpcode[1]] if kOpcode[1] in kRegisters else kOpcode[1]
                nAddress += 1

            elif "nop" == kOpcode[0] :

                nAddress += 1

            else :
                assert(False)
            #end

        #end

        return kRegisters

    #end

    kRegistersPart2 = {
        "a" : 0,
        "b" : 0,
        "c" : 1,
        "d" : 0
    }

    # Pre-patch the code, we're looking for patterns of:
    #
    # inc {X}
    # dec {Y}
    # jnz {Y} -2
    #
    # Such that we can replace this with:
    # add Y X
    # cpy 0 Y
    # nop

    patchAssembunny(kAssembunny)

    print(f"Part 1: {processAssembunny(kAssembunny)['a']}")
    print(f"Part 2: {processAssembunny(kAssembunny, kRegistersPart2)['a']}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)*1000}ms", file=sys.stderr)
#end
