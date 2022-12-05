import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        def moveCrates(kCrateStack, nSource, nTarget, kCrates) :
            kCrateStack[nSource]  = kCrateStack[nSource][:len(kCrateStack[nSource]) - len(kCrates)]
            kCrateStack[nTarget] += kCrates
        #end

        # First extract the Crates
        kPuzzleData = [entry.split("\n") for entry in inputFile.read().split("\n\n")]

        # Create the Crate Stack Lookups
        kColumns = []
        for i,kColumn in enumerate(kPuzzleData[0][-1]) :
            if kColumn != " " :
                kColumns.append(i)
            #end
        #end

        # Create the Crate Stack
        kCrateStack = [[] for _ in kColumns]
        for kCrates in reversed(kPuzzleData[0][:-1]) :
            for i, nColumn in enumerate(kColumns) :
                if kCrates[nColumn] != " " :
                    kCrateStack[i].append(kCrates[nColumn])
                #end
            #end
        #end

        # And now I need to deep copy for the Part 2 puzzle
        kCrateStack9001 = [[kCrate for kCrate in kCrates] for kCrates in kCrateStack]

        # Next Extract the Moves
        kMoves = []
        for kMove in kPuzzleData[1] :
            kMoveTokens = kMove.split(" ")
            kMoves.append([int(kMoveTokens[1]), int(kMoveTokens[3]) - 1, int(kMoveTokens[5]) - 1])
        #end

        # Solve both Parts
        for kMove in kMoves :

            # Part 1 (Lifts One Crate at a time, meaning the order is reversed)
            moveCrates(kCrateStack, kMove[1], kMove[2], list(reversed(kCrateStack[kMove[1]]))[:kMove[0]])

            # Part 2 (Lifts Multiple Crates at a time, meaning the order is maintained)
            moveCrates(kCrateStack9001, kMove[1], kMove[2], kCrateStack9001[kMove[1]][len(kCrateStack9001[kMove[1]]) - kMove[0]:])

        #end

        print(f"Part 1: {''.join([kCrates[-1] for kCrates in kCrateStack])}")
        print(f"Part 2: {''.join([kCrates[-1] for kCrates in kCrateStack9001])}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
