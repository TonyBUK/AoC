import time
import hashlib
import random

def main() :

    # Read the Encrypted Data
    kDoorIds = []
    with open("../input.txt", "r") as inputFile:
        kDoorIds = [line.strip() for line in inputFile.readlines()]
    #end


    def getPasswords(kDoorId) :

        MOVIE_OS            = "0123456789abcdefghijklmnopqrstuvwxyz"
        PREAMBLE            = "00000"
        VALID_INDEXES       = "01234567"
        kPasswordPart1      = ""
        kPasswordPart2      = list("        ")
        kPasswordPart2Valid = [False] * len(kPasswordPart2)
        nIndex              = 0
        hackTime            = time.perf_counter()

        while (len(kPasswordPart1) < 8) or (all(kPasswordPart2Valid) == False) :

            kMD5 = hashlib.md5((kDoorId + str(nIndex)).encode()).hexdigest()
            if kMD5.startswith(PREAMBLE) :
                if len(kPasswordPart1) < 8 :
                    kPasswordPart1 += kMD5[5]
                #end
                if all(kPasswordPart2Valid) == False :
                    if kMD5[5] in VALID_INDEXES :
                        nPosition = int(kMD5[5])
                        if False == kPasswordPart2Valid[nPosition] :
                            kPasswordPart2[nPosition]      = kMD5[6]
                            kPasswordPart2Valid[nPosition] = True
                        #end
                    #end
                #end
            #end

            # Movie OS
            # This will rapidly print a garbled password containing a mix of:
            # 1. Known values which never change
            # 2. Random alphanumeric values for unknown entries
            #
            # The rate of change will be around 10Hz, fast enough that you can see what's going on,
            # but not too fast that we just utterly kill throughput.
            #
            # I could have made this a simpler modulo check on nIndex, but that would make the frame
            # rate CPU bound, which would just look bad.
            if ((time.perf_counter() - hackTime) > 0.1) or all(kPasswordPart2Valid) :
                kPasswordPart2 = [C if bValid else random.choice(MOVIE_OS) for C,bValid in zip(kPasswordPart2, kPasswordPart2Valid)]
                print("Hacking FBI Mainframe... " + "".join(kPasswordPart2), end="\r")
                hackTime = time.perf_counter()
                if all(kPasswordPart2Valid) :
                    print("Wake up Neo...  The Matrix has you")
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
