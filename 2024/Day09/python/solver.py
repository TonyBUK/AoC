import time
import sys
import math

def main() :

    def compactPartOne(kFilesOriginal : dict[int, int], kFreeSpaceOriginal : list[int], nLeftMostFreeSpace : int) :

        ID    = 0
        SPACE = 1

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
        kCompressedDisk  = [kFiles[0][ID]] * kFiles[0][SPACE]
        kFiles[0][SPACE] = 0
        nRightMostFile   = len(kFiles) - 1

        # Iterate until all files are moved
        while True :

            nSpaceTopMove    = min(kFiles[nRightMostFile][SPACE], kFreeSpace[nLeftMostFreeSpace])
            kCompressedDisk += [kFiles[nRightMostFile][ID]] * nSpaceTopMove

            kFreeSpace[nLeftMostFreeSpace] -= nSpaceTopMove
            kFiles[nRightMostFile][SPACE]  -= nSpaceTopMove

            while nLeftMostFreeSpace < len(kFreeSpace) and kFreeSpace[nLeftMostFreeSpace] == 0 :
                nLeftMostFreeSpace += 1
                kCompressedDisk    += [kFiles[nLeftMostFreeSpace][ID]] * kFiles[nLeftMostFreeSpace][SPACE]
                kFiles[nLeftMostFreeSpace][SPACE] = 0
            #end

            while (nRightMostFile > 0) and (0 == kFiles[nRightMostFile][SPACE]) :
                nRightMostFile -= 1
            #end

            if 0 == kFiles[nRightMostFile][SPACE] :
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

        # For speed sake, we want to quickly get to the lowest free space of
        # each magnitude, meaning we split the data by size, and sort by
        # position.
        kFreeSpaceNodesBySize = [[] for _ in range(10)]
        for kFreeSpaceNode in sorted(kFreeSpaceNodes) :
            if kFreeSpaceNode[0] != 0 :
                kFreeSpaceNodesBySize[kFreeSpaceNode[0]].append(kFreeSpaceNode)
            #end
        #end

        # New File List we'll be construction
        kNewFileList = []

        # Work backwards from the highest node to the lowest node
        for kNode in reversed(kNodes) :

            nSpaceNeeded = kNode[1]

            bFound      = False
            nLowestNode = math.inf
            kLowestNode = None

            # Find the lowest free space node that can fit the file
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

            # Add the File to either the Lowest Free Node or in its existing
            # location.
            # Note: For the final sort, we push the position to the most
            #       prominent position in the list to process data in disk
            #       order.
            if not bFound :

                # Existing Location
                kNewFileList.append((kNode[2], kNode[0], kNode[1]))

            else :

                # New Location
                kNewFileList.append((-kLowestNode[1], kNode[0], kNode[1]))

                # Remove the Free Space Node from its current size bracket÷÷÷
                kFreeSpaceNodesBySize[kLowestNode[0]].pop()

                # Update the Free Space Nodes
                kLowestNode[0] -= nSpaceNeeded
                kLowestNode[1] -= nSpaceNeeded

                # Move the Free Space Node to its new size bracket (if non-zero)
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
                    kFilesPartOne[nId] = [nId, nSpace]
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