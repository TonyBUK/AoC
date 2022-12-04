import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        kLookup     = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

        # First Extract the Items and Bisect
        kItems      = [[[kLookup.index(char) for char in line.strip()[:len(line.strip())//2]],
                        [kLookup.index(char) for char in line.strip()[len(line.strip())//2:]]] for line in inputFile.readlines()]
        kDuplicates = list(list(set(item[0]) & set(item[1])) for item in kItems)

        print(f"Part 1: {sum(sum(kDuplicate) for kDuplicate in kDuplicates)}")

        # Merge the Compartments back and make them Sets
        kItemsAsSets   = list(set(item[0]) | set(item[1]) for item in kItems)
        kBadges        = [(kSet1 & kSet2 & kSet3).pop() for kSet1,kSet2,kSet3 in zip(kItemsAsSets[0::3], kItemsAsSets[1::3], kItemsAsSets[2::3])]

        print(f"Part 2: {sum(kBadges)}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
