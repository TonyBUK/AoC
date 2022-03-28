import time
import math
import sys

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

            return [[tuple(sorted([tokens[0], tokens[2]], reverse=False)), int(tokens[4])],
                    [tuple(sorted([tokens[0], tokens[2]], reverse=True )), int(tokens[4])]]

        #end

        rawStrings = [line.strip() for line in inputFile.readlines()]
        places = list(set([place.split(" ")[0] for place in rawStrings] +
                          [place.split(" ")[2] for place in rawStrings]))
        routes = {k: v for line in rawStrings for k,v in extractRoute(line.split(" "))}

    #end

    def findRoute(PLACES, ROUTES, FIND_SHORTEST, currentScore = 0, bestScore = None, bestScores = None, visitedMask = 0, completeMask = 0, CURRENT_PLACE = None) :

        if None == bestScore :
            if FIND_SHORTEST : bestScore = math.inf
            else             : bestScore = 0
            bestScores   = {}
            completeMask = (1 << len(PLACES)) - 1

            # The first call doesn't have a fixed start position...
            for place in PLACES :
                visitedMask = 1 << PLACES.index(place)
                for targetPlace in PLACES :
                    if place != targetPlace :
                        # Test this is a Valid Route
                        route = tuple([place, targetPlace])
                        if route in ROUTES :
                            targetMask = 1 << PLACES.index(targetPlace)
                            bestScore = findRoute(PLACES, ROUTES, FIND_SHORTEST, currentScore + ROUTES[route], bestScore, bestScores, visitedMask | targetMask, completeMask, targetPlace)
                        #end
                    #end
                #end
            #end

        else :

            # If the Task has been completed... return our best score
            if visitedMask == completeMask :
                if FIND_SHORTEST :
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
            currentState = tuple([CURRENT_PLACE, visitedMask])
            if currentState not in bestScores :
                bestScores[currentState] = currentScore
            else :
                if FIND_SHORTEST :
                    if currentScore >= bestScores[currentState] :
                        return bestScore
                    else :
                        bestScores[currentState] = currentScore
                    #end
                else :
                    if currentScore <= bestScores[currentState] :
                        return bestScore
                    else :
                        bestScores[currentState] = currentScore
                    #end
                #end
            #end

            # All subsequent calls have a fixed start position
            for targetPlace in PLACES :

                # Make sure this isn't a place we've already visited...
                targetMask = 1 << PLACES.index(targetPlace)
                if (targetMask & visitedMask) == 0 :
                    # Test this is a Valid Route
                    route = tuple([CURRENT_PLACE, targetPlace])
                    if route in ROUTES :
                        bestScore = findRoute(PLACES, ROUTES, FIND_SHORTEST, currentScore + ROUTES[route], bestScore, bestScores, visitedMask | targetMask, completeMask, targetPlace)
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
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
