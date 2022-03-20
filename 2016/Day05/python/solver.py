import time
import hashlib

def main() :

    # Read the Encrypted Data
    kDoorIds = []
    with open("../input.txt", "r") as inputFile:
        kDoorIds = [line.strip() for line in inputFile.readlines()]
    #end


    def getPasswords(kDoorId) :

        PREAMBLE       = "00000"
        VALID_INDEXES  = "01234567"
        kPasswordPart1 = ""
        kPasswordPart2 = list("        ")
        nIndex         = 0

        while (len(kPasswordPart1) < 8) or (" " in kPasswordPart2) :

            kMD5 = hashlib.md5((kDoorId + str(nIndex)).encode()).hexdigest()
            if 0 == kMD5.find(PREAMBLE) :
                if len(kPasswordPart1) < 8 :
                    kPasswordPart1 += kMD5[5]
                #end
                if " " in kPasswordPart2 :
                    if kMD5[5] in VALID_INDEXES :
                        nPosition = int(kMD5[5])
                        if " " == kPasswordPart2[nPosition] :
                            kPasswordPart2[nPosition] = kMD5[6]
                        #end
                    #end
                #end
            #end

            nIndex += 1

        #end

        return kPasswordPart1, "".join(kPasswordPart2)

    #end

    for kDoorId in kDoorIds :
        kPasswordPart1, kPasswordPart2 = getPasswords(kDoorId)
        print(f"Part 1: {kPasswordPart1}")
        print(f"Part 2: {kPasswordPart2}")
    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
