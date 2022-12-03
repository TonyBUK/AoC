import time
import sys

def main() :

    # Read/Clean the Input File to extract the Elves
    meals = []
    with open("../input.txt", "r") as inputFile:

        # First Extract the Elves
        #
        # Note: List comprehension is essentially a loop / series of loops nexted onto one line.
        #       The commented out loop is equivalent.
        #
        # elves = []        
        # for line in inputFile.read().split("\n\n") :
        #
        #     elves.append([])
        #
        #     for calorie in line.split("\n") :
        #
        #         elves[-1].append(int(calorie))
        #
        #     #end
        #
        # #end
        elves = [[int(calorie) for calorie in line.split("\n")] for line in inputFile.read().split("\n\n")]

        # Then collate those into meals and sort
        #
        # Again the following list comprehension expansion is equivalent to the list comprehension :
        #
        # for calories in elves :
        #     meals.append(sum(calories))
        # #end
        # meals = sorted(meals, reverse=True)
        meals = sorted([sum(calories) for calories in elves], reverse=True)

    #end

    print(f"Part 1: {meals[0]}")
    print(f"Part 2: {sum(meals[:3])}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
