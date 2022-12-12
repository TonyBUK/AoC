import time
import sys
import math
 
def main() :
 
    with open("../input.txt", "r") as inputFile:

        def findRoute(start, goal, maze, gScore = None, cameFrom = None) :

            if (None == gScore) or (None == cameFrom) :

                minX = 0
                maxX = len(maze[-1]) - 1
                minY = 0
                maxY = len(maze) - 1

                # Doing this Recursively on a large data set would be the kiss of death for
                # performance, so go with something A* ish
                # https://en.wikipedia.org/wiki/A*_search_algorithm
                cameFrom    = {}
                gScore      = {tuple([Y,X]) : math.inf for Y in range(minY, maxY + 1) for X in range(minX, maxX + 1)}
                fScore      = {tuple([Y,X]) : math.inf for Y in range(minY, maxY + 1) for X in range(minX, maxX + 1)}

                openSet       = set([start])
                gScore[start] = 0
                fScore[start] = 0

                lowestFScore = [[fScore[start], start]]
                mergeFScore  = []
                sortRequired = False

                def updateFScore(fScores, fScore, neighbour) :
                    for i in range(len(fScores)) :
                        if neighbour == fScores[i][1] :
                            if fScore < fScores[i][0] :
                                fScores[i][0] = fScore
                                return True
                            #end
                        else :
                            i = 0
                        #end
                    #end
                    return False
                #end

                while len(openSet) > 0 :

                    if sortRequired :
                        lowestFScore += mergeFScore
                        lowestFScore.sort(reverse=True)
                        mergeFScore.clear()
                        sortRequired = False
                    #end

                    current      = lowestFScore[-1][1]
                    lowestFScore.pop()
                    if len(lowestFScore) == 0 : sortRequired = True

                    openSet.remove(current)
                    if goal == current : break

                    Y = current[0]
                    X = current[1]

                    for coord in [[1,0],[0,-1],[0,1],[-1,0]] :

                        candidateY = Y + coord[0]
                        candidateX = X + coord[1]

                        neighbour = tuple([candidateY,candidateX])

                        if (candidateY >= minY and candidateY <= maxY and
                            candidateX >= minX and candidateX <= maxX) :

                            if (maze[Y][X] - maze[candidateY][candidateX]) <= 1 :

                                neighbour        = tuple([candidateY,candidateX])
                                
                                tentative_gScore = gScore[current] + 1
                                betterRoute = tentative_gScore < gScore[neighbour]

                                if betterRoute :

                                    cameFrom[neighbour] = current
                                    gScore[neighbour]   = tentative_gScore
                                    fScore[neighbour]   = tentative_gScore + abs(goal[0] - candidateY) + abs(goal[1] - candidateX)

                                    if False == sortRequired :
                                        sortRequired = fScore[neighbour] < lowestFScore[-1][0]
                                    #end

                                    if neighbour not in openSet :
                                        mergeFScore.append([fScore[neighbour], neighbour])
                                    else :
                                        updateFScore(mergeFScore, fScore[neighbour], neighbour)
                                    #end

                                    openSet.add(neighbour)
                                #end
                            #end
                        #end
                    #end
                #end
            #end

            # If the Goal was never possible, return a blank route anyway
            if goal not in gScore :
                return [], gScore, cameFrom
            #end

            # If the Route was reached...            
            if gScore[goal] < math.inf :

                # Return the Back Trace as a Series of Steps
                route   = []
                current = goal
                while current != start :

                    route.append(current)
                    current = cameFrom[current]

                #end

                return route, gScore, cameFrom

            #end

            # Otherwise...
            return [], gScore, cameFrom

        #end

        LOOKUP          = "abcdefghijklmnopqrstuvwxyz"
        kInput          = [[kChar for kChar in kLine.strip()] for kLine in inputFile.readlines()]
        kHeightMap      = []
        kStartPositions = set()
        
        Y = 0
        for kRow in kInput :

            X    = 0
            kHeightMap.append([])

            for kChar in kRow :

                kPos = tuple([Y, X])

                if   'S' == kChar :
                    kStartPosition = kPos
                    kHeightMap[Y].append(LOOKUP.index('a'))
                elif 'E' == kChar :
                    kEndPosition = kPos
                    kHeightMap[Y].append(LOOKUP.index('z'))
                else  :
                    kHeightMap[Y].append(LOOKUP.index(kChar))

                    # Cache all possible start locations for Part 2                    
                    if 'a' == kChar:
                        kStartPositions.add(kPos)
                    #end
                #end

                X += 1

            #end

            Y += 1

        #end

        # Do an Impossible Reverse Route to get a Cacheable G Score / Back Trace
        _,gScore,cameFrom = findRoute(kEndPosition, tuple([-1, -1]), kHeightMap)

        # Calculate the Reverse Route using Cached Data
        kRoute,_,_  = findRoute(kEndPosition, kStartPosition, kHeightMap, gScore, cameFrom)
        print(f"Part 1: {len(kRoute)}")

        # Calculate all Reverse Routes using Cached Data and keep track of the shortest
        nLowestSteps = math.inf
        while len(kStartPositions) > 0 :

            kStartPosition = kStartPositions.pop()

            # Find the Route
            kRoute,_,_ = findRoute(kEndPosition, kStartPosition, kHeightMap, gScore, cameFrom)
            nSteps = len(kRoute)

            # Determine if a Route was *actually* found
            if nSteps > 0 :
                # And if it's quicker...
                if nSteps < nLowestSteps :
                    nLowestSteps = nSteps
                #end
            #end
        #end

        print(f"Part 2: {nLowestSteps}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end