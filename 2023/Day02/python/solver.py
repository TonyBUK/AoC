import time
import sys

def main() :

    def extractCubeSet(kLine) :

        kTokens = kLine.split(":")

        # Easy Mode
        if False :

            nGameID = int(kTokens[0].split(" ")[1])

            kCubes = []
            for kGameList in kTokens[1].split(";") :
                kCubes.append({})
                for kCube in kGameList.split(",") :
                    kCubeList = kCube.strip().split(" ")
                    kCubes[-1][kCubeList[1]] = int(kCubeList[0])
                #end
            #end

            return nGameID, kCubes

        #end

        # Medium Mode
        if False :

            nGameID = int(kTokens[0].split(" ")[1])

            kCubeList = [k.strip().split(",") for k in kTokens[1].strip().replace("\n", "").replace(", ", ",").split(";")]
            kCubes = [{kCube.split(" ")[1]: int(kCube.split(" ")[0]) for kCube in kCubeSet} for kCubeSet in kCubeList]

            return nGameID, kCubes

        #end

        # Hard Mode
        return int(kTokens[0].split(" ")[1]), [{kCube.split(" ")[1]: int(kCube.split(" ")[0]) for kCube in k.strip().split(",")} for k in kTokens[1].strip().replace("\n", "").replace(", ", ",").split(";")]

    #end

    def possibleGame(MAX_RED, MAX_GREEN, MAX_BLUE, kGame) :

        RED_HIGH   = max(kCube.get("red", 0) for kCube in kGame)
        GREEN_HIGH = max(kCube.get("green", 0) for kCube in kGame)
        BLUE_HIGH  = max(kCube.get("blue", 0) for kCube in kGame)

        if (RED_HIGH > MAX_RED) or (GREEN_HIGH > MAX_GREEN) or (BLUE_HIGH > MAX_BLUE) :
            return False
        #end

        return True

    #end

    def cubePower(kGame) :
        return max(kCube.get("red", 0) for kCube in kGame) * max(kCube.get("green", 0) for kCube in kGame) * max(kCube.get("blue", 0) for kCube in kGame)
    #end

    kCubes = []
    with open("../input.txt", "r") as inputFile:
        kCubes = {k : v for k, v in (extractCubeSet(kLine) for kLine in inputFile.readlines())}
    #end

    print(f"Part 1: {sum([nGameId for nGameId, kCubeSets in kCubes.items() if possibleGame(12, 13, 14, kCubeSets)])}")
    print(f"Part 1: {sum([cubePower(kCubeSets) for kCubeSets in kCubes.values()])}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
