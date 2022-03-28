import time
import sys

def main() :

    # If only...
    def extractReindeer(tokens) :
        assert(len(tokens) == 15)
        return {"flightDistance" : int(tokens[3]), "flightTime" : int(tokens[6]), "restTime" : int(tokens[13])}
    #end

    # Read/Clean the Input File to extract the Reindeer Stats
    reindeerStats = []
    with open("../input.txt", "r") as inputFile:
        reindeerStats = [extractReindeer(line.replace("\n", "").split(" ")) for line in inputFile.readlines()]
    #end

    def winningDistance(reindeerStats, targetTime) :
        bestDistance = 0
        for reindeer in reindeerStats :
            currentTime     = targetTime
            currentDistance = 0
            while (currentTime > 0) :
                flightDuration = reindeer["flightTime"]
                if flightDuration > currentTime :
                    flightDuration = currentTime
                #end
                currentDistance += reindeer["flightDistance"] * flightDuration
                currentTime     -= (flightDuration + reindeer["restTime"])
            #end
            if currentDistance > bestDistance :
                bestDistance = currentDistance
            #end
        #end
        return bestDistance
    #end

    def winningPoints(reindeerStats, targetTime) :

        RACING              = 0
        RESTING             = 1
        reindeerState       = [{"state"             : RACING,
                                "remaining"         : R["flightTime"],
                                "flightTime"        : R["flightTime"],
                                "flightDistance"    : R["flightDistance"],
                                "restTime"          : R["restTime"]}
                                for R in reindeerStats]
        reindeerPoints      = [0 for X in reindeerStats]
        reindeerDistances   = [0 for X in reindeerStats]

        # For each second...
        for X in range(targetTime) :

            # Update each Reindeer
            for i,reindeer in enumerate(reindeerState) :
                
                if RACING == reindeer["state"] :
                    reindeerDistances[i] += reindeer["flightDistance"]
                #end
                
                reindeer["remaining"] -= 1
                if 0 == reindeer["remaining"] :
                    if RACING == reindeer["state"] :
                        reindeer["state"]       = RESTING
                        reindeer["remaining"]   = reindeer["restTime"]
                    else :
                        reindeer["state"]       = RACING
                        reindeer["remaining"]   = reindeer["flightTime"]
                    #end
                #end
            #end

            # Increment all the (joint) first place Reindeers
            maxDistance = max(reindeerDistances)
            for i in range(len(reindeerPoints)) :
                if maxDistance == reindeerDistances[i] :
                    reindeerPoints[i] += 1
                #end
            #end

        #end

        return max(reindeerPoints)

    #end

    print(f"Part 1: {winningDistance(reindeerStats, 2503)}")
    print(f"Part 2: {winningPoints  (reindeerStats, 2503)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
