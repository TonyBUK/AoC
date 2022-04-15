import time
import sys
import math

def main() :

    def decodeFloor(kRaw) :
        kFloor = []
        if not "nothing relevant" in kRaw :
            if kRaw.find("contains ") :
                # This hot garbage converts an input text string from...
                #
                # The first floor contains a polonium generator, a thulium generator, and a cobalt-compatible microchip.
                # into:
                # [polonium generator,thulium generator,cobalt microchip]

                # The first floor contains a polonium generator, a thulium generator, and a cobalt-compatible microchip.
                # => The first floor contains a polonium generator, a thulium generator,, a cobalt-compatible microchip.
                #
                # Note: This inserts a comman that we know might be a duplicate to handle the following:
                #       The second floor contains a polonium-compatible microchip and a promethium-compatible microchip.
                #       => The second floor contains a polonium-compatible microchip, a promethium-compatible microchip.
                kSanitized = kRaw.replace(" and ", ", ")

                # The first floor contains a polonium generator, a thulium generator,, a cobalt-compatible microchip.
                # => The first floor contains polonium generator, thulium generator,, cobalt-compatible microchip.
                kSanitized = kSanitized.replace(" a ", " ")

                # The first floor contains polonium generator, thulium generator,, cobalt-compatible microchip.
                # => The first floor contains polonium generator, thulium generator,, cobalt-compatible microchip
                kSanitized = kSanitized.replace(".", "")

                # The first floor contains polonium generator, thulium generator,, cobalt-compatible microchip
                # => The first floor contains polonium generator,thulium generator,,cobalt-compatible microchip
                kSanitized = kSanitized.replace(", ", ",")

                # The first floor contains polonium generator,thulium generator,,cobalt-compatible microchip
                # => The first floor contains polonium generator,thulium generator,cobalt-compatible microchip
                kSanitized = kSanitized.replace(",,", ",")
                
                # The first floor contains polonium generator,thulium generator,cobalt-compatible microchip
                # => The first floor contains polonium generator,thulium generator,cobalt microchip
                kSanitized = kSanitized.replace("-compatible", "")
                
                # The first floor contains polonium generator,thulium generator,cobalt microchip
                # => polonium generator,thulium generator,cobalt microchip
                kSanitized = kSanitized.split("contains ")[1]

                # polonium generator,thulium generator,cobalt microchip
                # => [polonium generator,thulium generator,cobalt microchip]
                kFloor = kSanitized.split(",")
            #end
        #end
        return kFloor
    #end

    # Read and Compressed Text
    kFloorsInitial = []
    with open("../input.txt", "r") as inputFile:
        kFloorsInitial = [decodeFloor(line.strip()) for line in inputFile.readlines()]
    #end

    kFloors = []

    # Encode our Initial Game State
    for kItems in kFloorsInitial :
        kFloors.append({})
        kFloors[-1]["generator"] = set()
        kFloors[-1]["microchip"] = set()
        for kItem in kItems :
            kItemNameAndType = kItem.split(" ")
            kFloors[-1][kItemNameAndType[1]].add(kItemNameAndType[0])
        #end
    #end

    # Find the Corresponding Items Floor
    def findFloor(kFloors, kType, kItem) :
        for nFloor, kItems in enumerate(kFloors) :
            if kItem in kItems[kType] : return nFloor
        #end
        assert(False)
    #end

    # Reduce and Encode the floor state such that it is just a list of:
    # - Per Floor
    #   - For each Microchip, what floor is the Generator On
    #   - For each Generator, what floor is the Microchip On
    #
    # This then makes the following two scenarios interchangeable
    #
    # F4 .  .  .  .      F4 .  .  .  .  
    # F3 .  .  LG .      F3 HG .  .  .  
    # F2 HG .  .  .      F2 .  .  LG .  
    # F1 .  HM .  LM     F1 .  HM .  LM 
    #
    # But still treats the following two scenarios as unique
    #
    # F4 .  .  .  .      F4 .  .  .  .  
    # F3 .  .  LG .      F3 .  .  LM .  
    # F2 HG .  .  .      F2 HG .  .  .  
    # F1 .  HM .  LM     F1 .  HM .  LG 

    TYPES       = ["generator", "microchip"]
    OTHER_TYPES = {"generator" : "microchip", "microchip" : "generator"}

    def encodeFloorState(kFloors) :

        kFloorState = [{T : [] for T in TYPES} for _ in kFloors]

        for nFloor, kItems in enumerate(kFloors) :

            for kType in TYPES :

                for kItem in kItems[kType] :

                    kFloorState[nFloor][kType].append(findFloor(kFloors, OTHER_TYPES[kType], kItem))

                #end

            #end

        #end

        return tuple([tuple([tuple(sorted(kFloorState[nFloor][kState])) for kState in TYPES]) for nFloor,_ in enumerate(kFloors)])

    #end

    def validFloorState(kFloors) :
        for kItems in kFloors :
            if len(kItems["generator"]) > 0 :
                if len(kItems["microchip"] - kItems["generator"]) > 0 :
                    return False
                #end
            #end
        #end
        return True
    #end

    def winState(kFloors) :

        for kItems in kFloors[0:-1] :
            if len(kItems["generator"]) > 0 : return False
            if len(kItems["microchip"]) > 0 : return False
        #end
        assert(len(kFloors[-1]["generator"]) == len(kFloors[-1]["microchip"]))
        return True
    #end

    def moveItem(kFloors, kItem, kType, nCurrentFloor, nNewFloor) :
        kFloors[nCurrentFloor][kType].remove(kItem)
        kFloors[nNewFloor][kType].add(kItem)
    #end

    # This function is just a crude approximation that assumes there are no rules, and is more
    # provide a quick way of determining if a current solution strand is still worth pursuing
    # without actually trying to solve the puzzle
    #
    # And basically uses the following pattern:
    #
    # 1 - 2 items : 1 move per floor
    # 3 items     : 3 moves per floor
    # 4 items     : 5 moves per floor
    # 5 items     : 7 moves per floor
    #
    # etc.
    def countMinMoves(kFloors) :
        nCount = 0
        for i in range(len(kFloors)) :
            nFloorCount = len(kFloors[i]["generator"]) + len(kFloors[i]["microchip"])
            if nFloorCount == 0 : continue
            nFloorMultiplier = len(kFloors) - i - 1
            if nFloorCount <= 2 : nCount += 1 * nFloorMultiplier
            else                : nCount += (3 + ((nFloorCount - 3) * 2)) * nFloorMultiplier
        #end
        return nCount
    #end

    def solveElevator(kFloors, nFloor = 0, nDepth = 0, nBestDepth = math.inf, kFloorCache = None) :

        if None == kFloorCache :
            kFloorCache = {}
        #end

        if False == validFloorState(kFloors) :
            return nBestDepth
        #end

        if math.isfinite(nBestDepth) :
            if (nDepth + countMinMoves(kFloors) + 1) >= nBestDepth :
                return nBestDepth
            #end
        #endI

        kFloorState = tuple([nFloor, encodeFloorState(kFloors)])
        if kFloorState in kFloorCache :
            if kFloorCache[kFloorState] <= nDepth :
                return nBestDepth
            #end
        #end
        kFloorCache[kFloorState] = nDepth

        if winState(kFloors) :
            return nDepth
        #end

        # One thing we can optimize is on floors where multiple pairs exist, ignore all but one of the pairs
        # We can do this because the pairs are essentially interchangeable
        kIgnoreList = set()
        for kItem in kFloors[nFloor]["generator"] :
            if kItem in kFloors[nFloor]["microchip"] :
                kIgnoreList.add(kItem)
            #end
        #end

        # Ahem... all but *one* of the pairs
        if len(kIgnoreList) > 0 :
            kIgnoreList.pop()
        #end

        # Create the List of Microchips/Generators on each Floor as something we can trivially iterate on and filter
        kFloorLookup = []
        for kItem in kFloors[nFloor]["generator"] :
            if kItem not in kIgnoreList :
                kFloorLookup.append(["generator", kItem])
            #end
        #end
        for kItem in kFloors[nFloor]["microchip"] :
            if kItem not in kIgnoreList :
                kFloorLookup.append(["microchip", kItem])
            #end
        #end

        # Test Ordering
        #
        # This helps make DFS not *soooo* bad by effectively giving a priority
        # order to the processing
        kTests = [{"floor" :  1, "count" : 2}, # Two Items Upwards
                  {"floor" : -1, "count" : 1}, # One Item Downwards
                  {"floor" :  1, "count" : 1}, # One Item Upwards
                  {"floor" : -1, "count" : 2}] # Two Items Downwards

        # Iterate through the Test Cases
        for kTest in kTests :

            nNewFloor = nFloor + kTest["floor"]

            # Ignore any invalid elevator moves
            if nNewFloor not in range(0, 4) : continue
            if nNewFloor == nFloor          : continue

            for i in range(len(kFloorLookup)) :

                moveItem(kFloors, kFloorLookup[i][1], kFloorLookup[i][0], nFloor, nNewFloor)

                if kTest["count"] == 2 :
                    for j in range(i + 1, len(kFloorLookup)) :
                        moveItem(kFloors, kFloorLookup[j][1], kFloorLookup[j][0], nFloor, nNewFloor)
                        nBestDepth = solveElevator(kFloors, nNewFloor, nDepth+1, nBestDepth, kFloorCache)
                        moveItem(kFloors, kFloorLookup[j][1], kFloorLookup[j][0], nNewFloor, nFloor)
                    #end
                else :
                    nBestDepth = solveElevator(kFloors, nNewFloor, nDepth+1, nBestDepth, kFloorCache)
                #end

                moveItem(kFloors, kFloorLookup[i][1], kFloorLookup[i][0], nNewFloor, nFloor)

            #end

        #end

        return nBestDepth

    #end

    print(f"Part 1: {solveElevator(kFloors)}")
    kFloors[0]["microchip"].add("elerium")
    kFloors[0]["microchip"].add("dilithium")
    kFloors[0]["generator"].add("elerium")
    kFloors[0]["generator"].add("dilithium")
    print(f"Part 2: {solveElevator(kFloors)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)*1000}ms", file=sys.stderr)
#end