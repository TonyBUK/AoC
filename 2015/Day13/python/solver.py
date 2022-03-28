import time
import itertools
import sys

def main() :

    # If only...
    def extractHappiness(tokens) :
        assert(len(tokens) == 10)
        return [tokens[0], int(tokens[2]), tokens[9]]
    #end

    # Read/Clean the Input File to extract the seating Rules
    rules = []
    with open("../input.txt", "r") as inputFile:
        rules = [extractHappiness(line.replace("gain ", "").replace("lose ", "-").replace(".","").replace("\n", "").split(" ")) for line in inputFile.readlines()]
    #end

    # Populate the Rules per Person
    people = {}
    for rule in rules :
        if not rule[0] in people :
            people[rule[0]] = {}
        #end
        people[rule[0]][rule[2]] = rule[1]
    #end

    def calculateHighestHappiness(people) :

        def calculateScore(seatingPlan, people) :
            score = 0
            for i, person in enumerate(seatingPlan) :
                # Little cheat, -1 in python means last element in array
                left  = seatingPlan[i - 1] # [(i + len(seatingPlan) - 1) % len(seatingPlan)]
                right = seatingPlan[(i + 1) % len(seatingPlan)]
                if left in people[person] :
                    score += people[person][left]
                #end
                if right in people[person] :
                    score += people[person][right]
                #end
            #end
            return score
        #end

        # Ideally, we'd discard rotations in the iterations list, but since there's only 7/8 people, there's really not
        # that many seating combinations to worry abouts
        seatingPlans     = itertools.permutations(people.keys())
        highestHappiness = 0
        peopleToSeat     = list(people.keys())
        assert(len(peopleToSeat) > 0)
        first            = peopleToSeat[0]
        for seatingPlan in seatingPlans :
            if seatingPlan[0] != first : continue
            happiness = calculateScore(seatingPlan, people)
            if happiness > highestHappiness :
                highestHappiness = happiness
            #end
        #end

        return highestHappiness

    #end

    print(f"Part 1: {calculateHighestHappiness(people)}")
    # Insert Me...
    people[""] = {}
    print(f"Part 2: {calculateHighestHappiness(people)}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
