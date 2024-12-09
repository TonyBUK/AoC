import time
import sys
import enum
import math

def main() :

    class EFileState(enum.Enum) :
        ID          = 0
        SPACE       = 1
    #end

    def compactPartOne(kFilesOriginal : dict[int, int], kFreeSpaceOriginal : list[int], nLeftMostFreeSpace : int) :

        # Caclulate the Checksum on the Compressed Disk (stored as a list of id's)
        def checksumPartOne(kCompressedDisk : list[int]) -> int :
            nChecksum = 0
            for i, nId in enumerate(kCompressedDisk) :
                nChecksum += i * nId
            #end
            return nChecksum
        #end

        kFiles     = kFilesOriginal
        kFreeSpace = kFreeSpaceOriginal

        # We Know File One occupies the first space
        kCompressedDisk = [kFiles[0][EFileState.ID]] * kFiles[0][EFileState.SPACE]
        kFiles[0][EFileState.SPACE] = 0
        nRightMostFile  = len(kFiles) - 1

        # Iterate until all files are moved
        while True :

            nSpaceTopMove    = min(kFiles[nRightMostFile][EFileState.SPACE], kFreeSpace[nLeftMostFreeSpace])
            kCompressedDisk += [kFiles[nRightMostFile][EFileState.ID]] * nSpaceTopMove

            kFreeSpace[nLeftMostFreeSpace]           -= nSpaceTopMove
            kFiles[nRightMostFile][EFileState.SPACE] -= nSpaceTopMove

            while nLeftMostFreeSpace < len(kFreeSpace) and kFreeSpace[nLeftMostFreeSpace] == 0 :
                nLeftMostFreeSpace += 1
                kCompressedDisk += [kFiles[nLeftMostFreeSpace][EFileState.ID]] * kFiles[nLeftMostFreeSpace][EFileState.SPACE]
                kFiles[nLeftMostFreeSpace][EFileState.SPACE] = 0
            #end

            while (nRightMostFile > 0) and (0 == kFiles[nRightMostFile][EFileState.SPACE]) :
                nRightMostFile -= 1
            #end

            if 0 == kFiles[nRightMostFile][EFileState.SPACE] :
                break
            #end

        #end

        return checksumPartOne(kCompressedDisk)

    #end

    def compactPartTwo(kNodes : list, kFreeSpaceNodes : list) -> int :

        # Caclulate the Checksum on the Compressed Disk (stored as a RLE id's)
        def checksumPartTwo(kNodes : list) -> int :

            nChecksum = 0
            nPos      = 0

            for kNode in kNodes :

                nPos = kNode[0]

                for _ in range(kNode[2]) :
                    nChecksum += nPos * kNode[1]
                    nPos      += 1
                #end

            #end

            return nChecksum

        #end

        kFreeSpaceNodesBySize = [[] for _ in range(10)]

        for kFreeSpaceNode in sorted(kFreeSpaceNodes) :
            if kFreeSpaceNode[0] != 0 :
                kFreeSpaceNodesBySize[kFreeSpaceNode[0]].append(kFreeSpaceNode)
            #end
        #end

        kNewFileList = []

        for kNode in reversed(kNodes) :

            nSpaceNeeded = kNode[1]

            bFound      = False
            nLowestNode = math.inf
            kLowestNode = None

            for nNodeSize in range(nSpaceNeeded, 10) :
                if len(kFreeSpaceNodesBySize[nNodeSize]) > 0 :
                    if -kFreeSpaceNodesBySize[nNodeSize][-1][1] < kNode[2] :
                        if -kFreeSpaceNodesBySize[nNodeSize][-1][1] < nLowestNode :
                            nLowestNode = -kFreeSpaceNodesBySize[nNodeSize][-1][1] 
                            kLowestNode = kFreeSpaceNodesBySize[nNodeSize][-1]
                            bFound      = True
                        #end
                    #end
                #end
            #end

            if not bFound :
                kNewFileList.append((kNode[2], kNode[0], kNode[1]))
            else :

                kNewFileList.append((-kLowestNode[1], kNode[0], kNode[1]))

                kFreeSpaceNodesBySize[kLowestNode[0]].pop()

                kLowestNode[0] -= nSpaceNeeded
                kLowestNode[1] -= nSpaceNeeded

                if kLowestNode[0] > 0 :
                    kFreeSpaceNodesBySize[kLowestNode[0]].append(kLowestNode)
                    kFreeSpaceNodesBySize[kLowestNode[0]] = sorted(kFreeSpaceNodesBySize[kLowestNode[0]])
                #end

            #end

        #end

        return checksumPartTwo(sorted(kNewFileList))

    #end

    with open("../input.txt", "r") as inputFile:

        for kLine in inputFile :

            kFilesPartOne      = {}
            kFreeSpacePartOne  = []
            kLine              = kLine.strip()
            nLeftMostFreeSpace = None
            
            kFilesPartTwo        = []
            kFreeSpacePartTwo = []

            nPos                 = 0

            for i,kSpace in enumerate(kLine) :

                nSpace = int(kSpace)
                if 0 == (i % 2) :

                    nId = i // 2

                    # File
                    kFilesPartOne[nId] = {
                        EFileState.ID          : nId,
                        EFileState.SPACE       : nSpace
                    }

                    kFilesPartTwo.append((nId, nSpace, nPos))
                    nPos += nSpace

                else :

                    # Free space
                    kFreeSpacePartOne.append(nSpace)
                    if nLeftMostFreeSpace is None :
                        nLeftMostFreeSpace = len(kFreeSpacePartOne) - 1
                    #end

                    # Note: This is deliberately a list not a tuple as we want
                    #       Python to use shallow copying in order to allow both
                    #       sorting, but easy processing of neighbours to combine
                    #       sizes.
                    #       Also pos is negative for sorting purposes.
                    kFreeSpacePartTwo.append([nSpace, -nPos, len(kFreeSpacePartTwo)])
                    nPos += nSpace

                #end

            #end

            assert(nLeftMostFreeSpace is not None)

            print(f"Part 1: {compactPartOne(kFilesPartOne, kFreeSpacePartOne, nLeftMostFreeSpace)}")
            print(f"Part 2: {compactPartTwo(kFilesPartTwo, kFreeSpacePartTwo)}")

        #end

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end