import time
import sys
import math

class Point3D :

    def __init__(self, X : int, Y : int, Z : int) :
        self.X = X
        self.Y = Y
        self.Z = Z
    #end
        
    X : int
    Y : int
    Z : int

#end

class Cube :

    def __init__(self, START : list, END : list) :

        self.START  = Point3D(START[0], START[1], START[2])
        self.END    = Point3D(END[0],   END[1],   END[2])

        assert(self.END.Z >= self.START.Z)

    #end

    def __str__(self) :
        return f"({self.START.X}, {self.START.Y}, {self.START.Z}) -> ({self.END.X}, {self.END.Y}, {self.END.Z})"
    #end

    def tryMoveRelative(self, Z : int, kOthers) :

        self.START.Z  += Z
        self.END.Z    += Z

        for kOther in kOthers :
            if kOther != self :
                if self.intersects(kOther) :
                    self.START.Z -= Z
                    self.END.Z   -= Z
                    return False
                #end
            #end
        #end

        return True

    #end

    def intersects(self, kOther) :
            
        # Check if the X Axis Overlaps
        if self.START.X > kOther.END.X :
            return False
        elif self.END.X < kOther.START.X :
            return False
        #end

        # Check if the Y Axis Overlaps
        if self.START.Y > kOther.END.Y :
            return False
        elif self.END.Y < kOther.START.Y :
            return False
        #end

        # Check if the Z Axis Overlaps
        if self.START.Z > kOther.END.Z :
            return False
        elif self.END.Z < kOther.START.Z :
            return False
        #end

        return True

    #end

    def getNeighboursAbove(self, kOthers) :

        kSupportingCubes = []

        # Get the Neighbours Above
        for kOther in kOthers :
            if kOther != self :
                if ((kOther.START.Z == (self.END.Z + 1)) and
                    (kOther.START.X <= self.END.X and
                     kOther.END.X >= self.START.X) and
                    (kOther.START.Y <= self.END.Y and
                     kOther.END.Y >= self.START.Y)) :
                    kSupportingCubes.append(kOther)
                #end
            #end
        #end

        return kSupportingCubes

    #end

    def getNeighboursBelow(self, kOthers) :
            
        kSupportedCubes = []

        # Get the Neighbours Below
        for kOther in kOthers :
            if kOther != self :
                if (((kOther.END.Z + 1) == self.START.Z) and
                    (kOther.START.X <= self.END.X and
                     kOther.END.X >= self.START.X) and
                    (kOther.START.Y <= self.END.Y and
                     kOther.END.Y >= self.START.Y)) :
                    kSupportedCubes.append(kOther)
                #end
            #end
        #end
                    
        return kSupportedCubes

    #end

    def getClosestNeighbourBelow(self, kOthers) :
            
        # Create a Hypothetical Cube that extends to the Ground
        kHypotheticalCube = Cube([self.START.X, self.START.Y, 0],
                                 [self.END.X,   self.END.Y,   self.END.Z])

        # Get the Closest Neighbour
        kClosestNeighbour = None
        nClosestDistance  = math.inf

        # Get the Intersections
        for kOther in kOthers :
            if kOther != self :
                if kHypotheticalCube.intersects(kOther) :
                    nDistance = self.START.Z - kOther.END.Z
                    if nDistance < nClosestDistance :
                        nClosestDistance  = nDistance
                        kClosestNeighbour = kOther
                    #end
                #end
            #end
        #end
                    
        return kClosestNeighbour

    #end

    START : Point3D
    END   : Point3D

#end

def main() :

    kUnsortedBricks = []
    with open("../input.txt", "r") as inputFile:

        for kLine in inputFile.readlines() :

            kTokens = kLine.replace("\n", "").split("~")
            assert(len(kTokens) == 2)

            kUnsortedBricks.append(Cube([int(k) for k in kTokens[0].split(",")], [int(k) for k in kTokens[1].split(",")]))

        #end

    #end

    # Part 1 - Find the Number of Bricks that can be Disintegrated
    
    # First... move all the bricks downwards until they're either on the ground or
    # ontop of other bricks.

    # Copy the Bricks
    kBricksToProcess = kUnsortedBricks

    # Sort the Bricks by their Z Axis
    kBricksToProcess.sort(key=lambda kBrick : kBrick.START.Z, reverse=True)
    kBricksSorted = [kBrick for kBrick in reversed(kBricksToProcess)]

    # Move the Bricks Downwards, starting from the bottom...
    nProcessedBricks = 0
    while (len(kBricksToProcess) > 0) :

        # Get the Lowest Brick
        kBrick        = kBricksToProcess.pop()

        # Find the Closest Brick Below
        kClosestBrick = kBrick.getClosestNeighbourBelow(kBricksSorted[:nProcessedBricks])

        if None == kClosestBrick :
            # If no brick exists, it's a straight shot to the ground
            kBrick.tryMoveRelative(-kBrick.START.Z, kBricksSorted[:nProcessedBricks])
        else :
            # Otherwise, move to above the closest brick below
            nDistance = kBrick.START.Z - kClosestBrick.END.Z - 1
            kBrick.tryMoveRelative(-nDistance, kBricksSorted[:nProcessedBricks])
        #end
            
        nProcessedBricks += 1

    #end

    nNumBricksToDisintegrate = 0
    kConnectedBricks = {}
    for kBrick in kBricksSorted :

        # Initialise the Connected Brick
        kConnectedBricks[kBrick] = [[], []]

        kNeighboursAbove = kBrick.getNeighboursAbove(kBricksSorted)

        # Update the Connected Nodes
        kConnectedBricks[kBrick][0] = kNeighboursAbove
        kConnectedBricks[kBrick][1] = kBrick.getNeighboursBelow(kBricksSorted)

        if (len(kNeighboursAbove) == 0) :
            nNumBricksToDisintegrate += 1
            continue
        #end

        bOtherwiseSupported = True
        for kNeighbour in kNeighboursAbove :

            if len(kNeighbour.getNeighboursBelow(kBricksSorted)) <= 1 :
                bOtherwiseSupported = False
                break
            #end
        #end
            
        if bOtherwiseSupported :
            nNumBricksToDisintegrate += 1
        #end

    #end

    print(f"Part 1: {nNumBricksToDisintegrate}")

    def calculateChainReaction(kBrick, kConnectedBricks, kFilter = None) :

        # Remove the brick of interest so that bricks above won't see it as a neighbour
        nChain               = 0

        if kFilter is None :
            kFilter = []
        #end
            
        kFilter.append(kBrick)

        # Find all the Neighbours Above
        kNeighboursAbove     = [k for k in kConnectedBricks[kBrick][0] if k not in kFilter]
        kUnstableBricksAbove = []

        # If they have no neighbours below (because we removed the brick of interest), they're unstable.
        for kNeighboursAbove in kNeighboursAbove :
            kNeighboursBelow = [k for k in kConnectedBricks[kNeighboursAbove][1] if k not in kFilter]
            if len(kNeighboursBelow) == 0 :
                kUnstableBricksAbove.append(kNeighboursAbove)
            #end
        #end

        # Increment the Chain Length
        nChain += len(kUnstableBricksAbove)

        # Recursively calculate the chain length of the bricks above
        for kUnstableBrickAbove in kUnstableBricksAbove :
            nChain += calculateChainReaction(kUnstableBrickAbove, kConnectedBricks, kFilter)
        #end

        return nChain

    #end

    print(f"Part 2: {sum([calculateChainReaction(kBrick, kConnectedBricks) for kBrick in kBricksSorted])}")

#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end