import time

def main() :

    # Read/Clean the Input File and convert to an array of integers for processing
    seeSays = []
    with open("../input.txt", "r") as inputFile:
        seeSays = [[int(x) for x in line.strip()] for line in inputFile.readlines()]
    #end

    def storeSeeSay(repetitionDigit, repetitionCount, seeSay) :
        if repetitionCount > 9 :
            tmp = str(repetitionCount)
            for c in tmp :
                seeSay.append(int(c))
            #end
        else :
            seeSay.append(repetitionCount)
        #end
        seeSay.append(repetitionDigit)
    #end

    for seeSay in seeSays :

        currentSeeSay = [i for i in seeSay]

        # Python Loops are always < target, not <= target, hence +1
        for x in range(1,50+1):

            # Initialise Repetitions / Next See Say String
            repetitionDigit = currentSeeSay[0]
            repetitionCount = 1
            newSeeSay       = []

            # Skip the first digit as we already know the See/Say Number
            for i in currentSeeSay[1:] :

                if repetitionDigit == i :

                    # Increment the Reptition Count
                    repetitionCount += 1
                else :

                    # Store the Current Repetition
#                    storeSeeSay(repetitionDigit, repetitionCount, newSeeSay)
                    newSeeSay.append(repetitionCount)
                    newSeeSay.append(repetitionDigit)

                    # Setup the next Repetition
                    repetitionDigit = i
                    repetitionCount = 1
                #end
            #end

            # The final repetition needs storing
#            storeSeeSay(repetitionDigit, repetitionCount, newSeeSay)
            newSeeSay.append(repetitionCount)
            newSeeSay.append(repetitionDigit)

            # Copy the repetition for the next cycle
            currentSeeSay = newSeeSay

            # Part 1 - 40 Iterations
            if 40 == x :
                print(f"Part 1: {len(currentSeeSay)}")
            #end

        #end

        # Part 2 - 50 Iterations
        print(f"Part 2: {len(currentSeeSay)}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
