import time
import sys
import math

def main() :

    # Parse the Game Rules
    kGames = []
    with open("../input.txt", "r") as inputFile:
        kGameStrings = inputFile.read().split("\n\n")
        for kGameString in kGameStrings :
            kGameStringTokens = kGameString.replace("\n", " ").replace("+", " ").replace("=", " ").replace(",", "").split(" ")
            kGames.append(((int(kGameStringTokens[3]), int(kGameStringTokens[5])),
                           (int(kGameStringTokens[9]), int(kGameStringTokens[11])),
                           (int(kGameStringTokens[14]), int(kGameStringTokens[16]))))
            assert(len(kGameStringTokens) == 17)
        #end
    #end

    def countRounds(kGame, kTarget) :

        # Pen and Paper Time
        #
        # We start with the following simultaneous equations:
        #
        # X1 * a + X2 * b = P1
        # Y1 * a + Y2 * b = P2
        #
        # We can re-arrange these to:
        #
        # b = P1 - X1 * a
        #     -----------
        #         X2
        #
        # We can then b in the second equation, and after some algebra shuffling we get:
        #
        # a = Y2 * P1 - X2 * P2
        #     -----------------
        #     Y2 * X1 - X2 * Y1
        #
        # We could also do the reverse for b, but we don't need to as if we've solved a, b is trivial.

        X1 = kGame[0][0]
        Y1 = kGame[0][1]
        P1 = kTarget[0]
        X2 = kGame[1][0]
        Y2 = kGame[1][1]
        P2 = kTarget[1]

        # Solve a: Whole Numbers Only
        a_num = Y2 * P1 - X2 * P2
        a_dem = Y2 * X1 - X2 * Y1
        if a_num % a_dem != 0 :
            return 0
        #end
        a = a_num // a_dem

        # Solve b: Whole Numbers Only
        b_num = P1 - X1 * a
        b_dem = X2
        if b_num % b_dem != 0 :
            return 0
        #end
        b = b_num // b_dem

        # Return the Result
        return 3 * a + b

    #end

    nTokenCost = 0
    for kGame in kGames :
        nTokenCost += countRounds(kGame, kGame[2])
    #end
    print(f"Part 1: {nTokenCost}")

    nTokenCost = 0
    for kGame in kGames :
        nTokenCost += countRounds(kGame, (kGame[2][0] + 10000000000000, kGame[2][1] + 10000000000000))
    #end
    print(f"Part 2: {nTokenCost}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end