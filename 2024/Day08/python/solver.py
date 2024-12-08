import time
import sys

def main() :

    def getAntiNodes(kNode1 : tuple[int,int], kNode2 : tuple[int,int], WIDTH : int, HEIGHT : int, bAnyDistance : bool = False) -> list[tuple[int,int]] :

        def getNodes(nAntiNodeX : int, nAntiNodeY : int, nAntiNodeDeltaX : int, nAntiNodeDeltaY : int, WIDTH : int, HEIGHT : int, bAnyDistance : bool) -> set[tuple[int,int]] :

            kAntiNodes = set()

            if bAnyDistance :
                kAntiNodes.add((nAntiNodeX, nAntiNodeY))
            #end

            while True :

                nAntiNodeX += nAntiNodeDeltaX
                nAntiNodeY += nAntiNodeDeltaY

                if nAntiNodeX in range(WIDTH) and nAntiNodeY in range(HEIGHT) :
                    kAntiNodes.add((nAntiNodeX, nAntiNodeY))
                else :
                    break
                #end

                if bAnyDistance == False :
                    break
                #end

            #end

            return kAntiNodes

        #end

        nNodeDeltaX = kNode2[0] - kNode1[0]
        nNodeDeltaY = kNode2[1] - kNode1[1]

        kAntiNodes = set()
        kAntiNodes |= getNodes(kNode1[0], kNode1[1], -nNodeDeltaX, -nNodeDeltaY, WIDTH, HEIGHT, bAnyDistance)
        kAntiNodes |= getNodes(kNode2[0], kNode2[1],  nNodeDeltaX,  nNodeDeltaY, WIDTH, HEIGHT, bAnyDistance)

        return kAntiNodes

    #end

    VALID_NODES = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    WIDTH       = 0
    HEIGHT      = 0

    kNodes = {}
    with open("../input.txt", "r") as inputFile:
        for Y,kLine in enumerate(inputFile.readlines()) :
            kLine   = kLine.strip()
            HEIGHT += 1
            WIDTH   = max(WIDTH, len(kLine))
            for X,kChar in enumerate(kLine) :
                if kChar in VALID_NODES :
                    if kChar not in kNodes :
                        kNodes[kChar] = [(X,Y)]
                    else :
                        kNodes[kChar].append((X,Y))
                    #end
                #end
            #end
        #end
    #end

    kUniqueAntiNodesPartOne = set()
    kUniqueAntiNodesPartTwo = set()

    for kNode in kNodes :
        if len(kNodes[kNode]) > 1 :
            for nNode1Pos in range(len(kNodes[kNode])) :
                for nNode2Pos in range(nNode1Pos + 1, len(kNodes[kNode])) :
                    kUniqueAntiNodesPartOne |= getAntiNodes(kNodes[kNode][nNode1Pos], kNodes[kNode][nNode2Pos], WIDTH, HEIGHT)
                    kUniqueAntiNodesPartTwo |= getAntiNodes(kNodes[kNode][nNode1Pos], kNodes[kNode][nNode2Pos], WIDTH, HEIGHT, True)
                #end
            #end
        #end
    #end

    print(f"Part 1: {len(kUniqueAntiNodesPartOne)}")
    print(f"Part 2: {len(kUniqueAntiNodesPartTwo)}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end