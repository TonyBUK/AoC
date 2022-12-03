import time
import sys

def main(SoveWithLookup) :

    WIN         = 6
    DRAW        = 3
    LOSE        = 0

    ROCK        = 1
    PAPER       = 2
    SCISSORS    = 3

    if False == SoveWithLookup :

        # Translations
        ELF_MOVE_TRANSLATIONS    = {"A" : ROCK, "B" : PAPER, "C" : SCISSORS}
        PLAYER_MOVE_TRANSLATIONS = {"X" : ROCK, "Y" : PAPER, "Z" : SCISSORS}
        SCORE_TRANSLATIONS       = {"X" : LOSE, "Y" : DRAW,  "Z" : WIN}

        # Translated Results
        WINNING_MOVES = {ROCK       : PAPER,
                        PAPER      : SCISSORS,
                        SCISSORS   : ROCK}
        LOSING_MOVES  = {ROCK       : SCISSORS,
                        PAPER      : ROCK,
                        SCISSORS   : PAPER}

        # Read the Move List into an array of Move/Response
        with open("input.txt", "r") as inputFile:

            # First Extract the Elves
            moves = [line.split() for line in inputFile.readlines()]

            # Score
            part1Score = 0
            part2Score = 0

            for move in moves :

                # Translate the Current Game State
                elfMove       = ELF_MOVE_TRANSLATIONS[move[0]]
                p1PlayerMove  = PLAYER_MOVE_TRANSLATIONS[move[1]]  # Part 1 Assumes Column B is a Move
                p2PlayerScore = SCORE_TRANSLATIONS[move[1]]        # Part 2 Assumes Column B is the intended Game State

                # Extract the Score from the Part 1 Move Criteria
                if WINNING_MOVES[elfMove] == p1PlayerMove :
                    part1Score += WIN
                elif elfMove == p1PlayerMove :
                    part1Score += DRAW
                else :
                    part1Score += LOSE
                #end

                part1Score += p1PlayerMove

                # Extract the Move from the Part 2 Score Criteria
                if LOSE == p2PlayerScore :
                    part2Score += LOSING_MOVES[elfMove]
                elif DRAW == p2PlayerScore :
                    part2Score += elfMove
                else :
                    part2Score += WINNING_MOVES[elfMove]
                #end

                part2Score += p2PlayerScore

            #end

            print(f"Part 1: {part1Score}")
            print(f"Part 2: {part2Score}")

        #end

    else :

        with open("input.txt", "r") as inputFile:

            PART1 = {
                "A X" : DRAW + ROCK,
                "A Y" : WIN  + PAPER,
                "A Z" : LOSE + SCISSORS,
                "B X" : LOSE + ROCK,
                "B Y" : DRAW + PAPER,
                "B Z" : WIN  + SCISSORS,
                "C X" : WIN  + ROCK,
                "C Y" : LOSE + PAPER,
                "C Z" : DRAW + SCISSORS
            }

            PART2 = {
                "A X" : SCISSORS + LOSE,
                "A Y" : ROCK     + DRAW,
                "A Z" : PAPER    + WIN,
                "B X" : ROCK     + LOSE,
                "B Y" : PAPER    + DRAW,
                "B Z" : SCISSORS + WIN,
                "C X" : PAPER    + LOSE,
                "C Y" : SCISSORS + DRAW,
                "C Z" : ROCK     + WIN
            }

            # First Extract the Moves
            moves = [line.strip() for line in inputFile.readlines()]

            # Score
            print(f"Part 1: {sum([PART1[move] for move in moves])}")
            print(f"Part 2: {sum([PART2[move] for move in moves])}")

        #end

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main(True)
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
