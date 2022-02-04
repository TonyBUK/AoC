import time
import math

def main() :

    # Read/Clean the Input File and extract places/routes
    places = []
    routes = {}
    with open("../input.txt", "r") as inputFile:

        def extractRoute(tokens) :
            assert(len(tokens) == 5)
            assert(tokens[1] == "to")
            assert(tokens[3] == "=")
            assert(tokens[4].isnumeric())

            return tuple(sorted([tokens[0], tokens[2]])), int(tokens[4])

        #end

        rawStrings = [line.strip() for line in inputFile.readlines()]
        places = list(set([place.split(" ")[0] for place in rawStrings] +
                          [place.split(" ")[2] for place in rawStrings]))
        routes = {k: v for k,v in (extractRoute(line.split(" ")) for line in rawStrings)}
    #end

    def findRoute(places, routes, findShortest, currentScore = 0, bestScore = None, bestScores = None, visitedMask = 0, completeMask = 0, currentPlace = None) :

        if None == bestScore :
            if findShortest : bestScore = math.inf
            else            : bestScore = 0
            bestScores   = {}
            completeMask = (1 << len(places)) - 1

            # The first call doesn't have a fixed start position...
            for place in places :
                visitedMask = 1 << places.index(place)
                for targetPlace in places :
                    if place != targetPlace :
                        # Test this is a Valid Route
                        route = tuple(sorted([place, targetPlace]))
                        if route in routes :
                            targetMask = 1 << places.index(targetPlace)
                            bestScore = findRoute(places, routes, findShortest, currentScore + routes[route], bestScore, bestScores, visitedMask | targetMask, completeMask, targetPlace)
                        #end
                    #end
                #end
            #end

        else :

            # If the Task has been completed... return our best score
            if visitedMask == completeMask :
                if findShortest :
                    if currentScore < bestScore :
                        bestScore = currentScore
                    #end
                else :
                    if currentScore > bestScore :
                        bestScore = currentScore
                    #end
                #end
                return bestScore
            #end

            # Determine if this is a duplicate state, if so, only proceed
            # *IF* we've got a better score
            currentState = tuple([currentPlace, visitedMask])
            if currentState not in bestScores :
                bestScores[currentState] = currentScore
            else :
                if findShortest :
                    if currentScore >= bestScores[currentState] :
                        return bestScore
                    #end
                else :
                    if currentScore <= bestScores[currentState] :
                        return bestScore
                    #end
                #end
            #end

            # All subsequent calls have a fixed start position
            for targetPlace in places :

                # Make sure this isn't a place we've already visited...
                targetMask = 1 << places.index(targetPlace)
                if (targetMask & visitedMask) == 0 :
                    # Test this is a Valid Route
                    route = tuple(sorted([currentPlace, targetPlace]))
                    if route in routes :
                        bestScore = findRoute(places, routes, findShortest, currentScore + routes[route], bestScore, bestScores, visitedMask | targetMask, completeMask, targetPlace)
                    #end
                #end
            #end
        #end

        return bestScore

    #end

    print(f"Part 1: {findRoute(places, routes, True)}")
    print(f"Part 2: {findRoute(places, routes, False)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
