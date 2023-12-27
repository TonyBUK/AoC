import time
import sys
import copy
import random

def main() :

    # For timing purposes only (to ensure the random seed is the same)
    # Comment out to unleash Karger!
#    random.seed(1)

    # Create the Wiring Nodes
    # To Simplify (or overcomplicate) the problem, we will create a graph of all the wiring nodes
    # including all the links omitted in the input to make everything bidirectional.
    kWiring = {}
    with open("../input.txt", "r") as inputFile:

        for kLine in inputFile :

            kLine = kLine.replace("\n", "")
            kTokens = kLine.split(": ")

            if kTokens[0] not in kWiring :
                kWiring[kTokens[0]] = []
            #end

            for kToken in kTokens[1].split(" ") :

                if kToken not in kWiring :
                    kWiring[kToken] = [kTokens[0]]
                elif kTokens[0] not in kWiring[kToken] :
                    kWiring[kToken].append(kTokens[0])
                #end

                if kToken not in kWiring[kTokens[0]] :
                    kWiring[kTokens[0]].append(kToken)
                #end

            #end
        #end

    #end

    # Solve using Karger's Algorithm
    # https://en.wikipedia.org/wiki/Karger%27s_algorithm
    def karger(kWiring : dict):

        # Covert the Graph into a set of Edges
        # Note: We don't particularly need the Graph after this, so destructive is fine.
        def createEdges(kGraph):

            kEdges      = []

            # Sorting is to ensure the only random element here is the random.shuffle
            # command.
            for kNode in sorted(kGraph):

                # Copy the Neighbours of the Current Graph Node
                kNeighbours = [k for k in kGraph[kNode]]

                # Add the Current Node/Neighbour Pair to the Edges
                for kNeighbour in kNeighbours:

                    kEdge = [kNode, kNeighbour]
                    kEdges.append(kEdge)

                    # Remove the Edge from the Graph so that it won't
                    # be added to the Edge List twice
                    if kNeighbour in kGraph[kNode] :
                        kGraph[kNode].remove(kNeighbour)
                    #end

                    if kNode in kGraph[kNeighbour] :
                        kGraph[kNeighbour].remove(kNode)
                    #end

                #end

            #end

            return kEdges

        #end

        # This Converts the Edge List back into a Graph with one slight
        # difference: The Neighbours are now a list of Edges instead of
        # a list of Nodes.
        #
        # This greatly simplifies the merging process.  This also uses
        # the fact that Python passes by reference to implicitly link
        # the Nodes and Edges.
        def createNodes(kEdges):

            # Nodes
            kNodes = {}
            for kEdge in kEdges:
                for k in kEdge :
                    if k not in kNodes:
                        kNodes[k] = []
                    #end
                    kNodes[k].append(kEdge)
                #end
            #end

            # Node Sizes (which is what the Puzzle is ultimately asking for, these will be merged too)
            kSizes = {kNode : 1 for kNode in kNodes}

            return kEdges, kNodes, kSizes

        #end

        # This takes neighbouring nodes and merges them into a single node.
        def mergeNodes(kEdges, kNodes, kNodeSizes):

            # The secret sauce, shuffle the Edges to change the starting point
            # of the merge process.
            random.shuffle(kEdges)

            # Merge nodes until only two remain
            while len(kNodes) > 2:

                # Pick a random edge
                kLeft, kRight = kEdges.pop()

                # Skip Self Loops which are created by the merge process
                if kLeft == kRight:
                    continue
                #end

                # Merge the nodes from Node B into Node A
                # Note: This may inadventently create a Self Loop which we'll deal with...
                for kEdge in kNodes[kRight]:
                    if kRight in kEdge :
                        kEdge[kEdge.index(kRight)] = kLeft
                    #end
                    kNodes[kLeft].append(kEdge)
                #end

                # Remove Node B from the Graph and ensure Node A now includes Node B's size.
                kNodes.pop(kRight)
                kNodeSizes[kLeft] += kNodeSizes[kRight]
                kNodeSizes.pop(kRight)

                # Remove Self-Loops
                kNodes[kLeft] = [kEdge for kEdge in kNodes[kLeft] if kEdge[0] != kEdge[1]]

            #end

            return kNodes, kNodeSizes

        #end

        # Create a local copy of the wiring
        kGraph = copy.deepcopy(kWiring)

        # Create the edges representation
        kEdges = createEdges(kGraph)

        # Run Karger's algorithm until the desired number of edges is reached
        while True:

            # Create the Nodes
            # Note: This deliberately absues the fact that Python passes by reference
            #       meaning the Nodes/Edges are impliclty linked, simplifying the amount
            #       of duplication mergeNodes has to perform.
            kTrialEdges, kNodes, kNodeSizes = createNodes(copy.deepcopy(kEdges))

            # Merge the Nodes using a Random Shuffle to change the Start Point
            #
            # Note: Over time this should reach a solution, however that time
            #       is not deterministic.
            #
            #       Also note, random isn't truly random, it will over time iterate over
            #       every possible combination of edges, so it will eventually find the
            #       solution.
            #
            #       However, it is possible that it will take a very long time to find
            #       the solution as the initial seed is random.
            kNodes, kNodeSizes = mergeNodes(kTrialEdges, kNodes, kNodeSizes)

            # We should now have exactly two nodes.
            # They should have identical sets of edges, we want three edges to have
            # reached the solution.
            kGroupKeys = list(kNodes)
            if len(kNodes[kGroupKeys[0]]) == 3:
                break
            #end

        #end

        return kNodeSizes[kGroupKeys[0]] * kNodeSizes[kGroupKeys[1]]

    #end

    print(f"Part 1: {karger(kWiring)}")

#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end