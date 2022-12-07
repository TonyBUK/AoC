import time
import sys
 
def main() :
 
    with open("../input.txt", "r") as inputFile:
 
        # Folder Stack / Contents
        #
        # Note: The Folder Contents could also be done as a tree, however the Puzzle indicates flat traversal would be more useful.
        kFolderStack    = ["/"]
        kFolderContents = {}
 
        # Read the Input Data
        kInputData = [line.strip().split() for line in inputFile.readlines()]
 
        # For Each Command
        for kCommand in kInputData :
 
            # Commands should always have either 2 or 3 elements.
            #
            # Directory Command     : $ cd ?
            # File Listing          : $ ls
            # File Listing Results  : ? ?
            assert(len(kCommand) >= 2)
 
            # Is this a Command?
            if kCommand[0] == "$" :
 
                # If it'#s a Change Directory Command?
                if kCommand[1] == "cd" :
 
                    # Sanity Check it has 3 Elements
                    assert(len(kCommand) == 3)
 
                    # Root
                    if kCommand[2] == "/" :
                        kFolderStack = ["/"]
                    # Up a Level
                    elif kCommand[2] == ".." :
                        assert(len(kFolderStack) > 0)
                        kFolderStack.pop()
                    # Down a Level
                    else :
                        kFolderStack.append(kCommand[2] + "/")
                    #end
 
                # Nothing to do... until the next cycle...
                elif kCommand[1] == "ls" :
 
                    # Grab the Full Path
                    kFullPath = "".join(kFolderStack)
 
                    # Initialise it, this should also handle Duplicates!
                    kFolderContents[kFullPath] = []
 
                # Shouldn't occur
                else :
 
                    assert(False)
 
                #end
 
            # This is a File Listing
            else :
 
                # Grab the Full Path
                kFullPath = "".join(kFolderStack)
 
                # Add a Directory
                if "dir" == kCommand[0] :
                    kFolderContents[kFullPath].append({"dir" : True, "name" : kCommand[1], "size" : 0})
                # Add a File
                else :
                    kFolderContents[kFullPath].append({"dir" : False, "name" : kCommand[1], "size" : int(kCommand[0])})
                #end
 
            #end
 
        #end
 
    #end
 
    def getFolderSize(kFolderName, kFolderContents) :
        nCurrentSize = 0
        for kEntry in kFolderContents[kFolderName] :
            if False == kEntry["dir"] :
                nCurrentSize += kEntry["size"]
            else :
                nCurrentSize += getFolderSize(kFolderName + kEntry["name"] + "/", kFolderContents)
            #end
        #end
        return nCurrentSize
    #end
 
    # Store the Sizes is Size Order for Later...
    kSizes = list(sorted([getFolderSize(kFolderName, kFolderContents) for kFolderName in kFolderContents], reverse=True))
 
    # Output the Sum of any Small Sizes
    print(f"Part 1: {sum([kSize if kSize <= 100000 else 0 for kSize in kSizes])}")
 
    # Calculate the Needed Space
    MAX_SPACE         = 70000000
    FREE_SPACE_NEEDED = 30000000
    NEEDED_SPACE      = kSizes[0] - (MAX_SPACE - FREE_SPACE_NEEDED) # Note: Biggest Folder must be the Root
 
    # Get rid of any Folders that are too small to produce the required space
    while (kSizes[-1] < NEEDED_SPACE) : kSizes.pop()
 
    # Output the Smallest of these
    print(f"Part 2: {kSizes[-1]}")
 
#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end