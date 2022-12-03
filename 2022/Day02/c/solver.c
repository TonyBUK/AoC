#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        /* This puzzle provides a single set of rules for A/B/C, and two sets of rules for X/Y/Z
         *
         * A = Rock     = 1
         * B = Paper    = 2
         * C = Scissors = 3
         * 
         * The game follows the usual Rock/Paper/Scissors logic of:
         * Rock     (1) Beats Scissors (3) (1 - 3 = -2 (or 1 if we insteat treat 3 as -1 or apply the modulo))
         * Paper    (2) Beats Rock     (1) (2 - 1 = 1)
         * Scissors (3) Beats Paper    (2) (3 - 2 = 1)
         * 
         * The key being that if the delta between the moves is 1 (using modulo arithmetic), you've won.
         * if the delta is 0 (i.e. both moves are the same), you've drawn.
         * otherwise (delta is 2 (using modulo arithmetic), yoy've lost.
         * 
         * Part 1:
         * 
         * We can solve this by using that property where the results are -1 (lost), 0 (drawn) and 1 (won).  By adding 1 we get:
         * 0 (lost), 1 (drawn) and 2 (won)
         * 
         * And since the scores are 0, 3 and 6 for lost, drawn, won respectively we can just multiply 3 by the above comparisons
         * to get the score.
         * 
         * Part 2:
         * 
         * We now know the game state, lost, drawn and won respectively, what we're missing is the move that got us there.  Except
         * given we know the elf move, and we know what that does to the delta, we can figure out the move using the delta.
         * 
         * WON   : Elfs Move + 1 (using modulo 3)
         * DRAWN : Elfs Move + 0
         * LOST  : Elfs Move - 1 (using module 3)
         * 
         */

        unsigned long nPart1Score = 0;
        unsigned long nPart2Score = 0;
 
        while (!feof(pData))
        {
            const int kElfMove = (int)(fgetc(pData) - 'A') + 1;
                  int kColumnB;
            fgetc(pData);
            kColumnB           = (int)(fgetc(pData) - 'X') + 1;
            fgetc(pData);

            assert((kElfMove >= 1) && (kElfMove <= 3));
            assert((kColumnB >= 1) && (kColumnB <= 3));

            /* Part 1 */
            
            /* For this algorithm to work, we want the comparison between Column B to ultimately work
             * out as:
             * Lost : 0
             * Draw : 1
             * Win  : 2
             * 
             * However just doing (move - elfMove % 3) will actually return:
             * Lost : -1 or 2
             * Draw : 0
             * Win  : 1 or -2
             * 
             * So the strategy of performing this and adding 1 won't work in some cases.
             * 
             * An example of Lost returning  2 is: Scissors (3) - Rock (1)
             * An example of Lost returning -1 is: Paper (2)    - Scissors (3)
             * An example of Won  returning  1 is: Scissors (3) - Paper (2)
             * An example of Won  returning -2 is: Rock (1) - Scissors (3)
             * 
             * We can of course test for this, and force 2 back to a state of -1, and -2 back into a state of 2,
             * but we can do this arithmetically by forcing all results to be positive, and forcing a rotation of the
             * results in the process.  That being:
             * 
             * ((Player Move - Elf Move + 4) % 3) - 1
             * Note: +3 forces the result to be positive, the extra +1 is to rotate the result.
             * 
             * Rock     (1) vs Rock     (1) (Draw) = ((1 - 1 + 4) % 3) - 1 = 0
             * Rock     (1) vs Paper    (2) (Lose) = ((1 - 2 + 4) % 3) - 1 = -1
             * Rock     (1) vs Scissors (3) (Win)  = ((1 - 3 + 4) % 3) - 1 = 1
             * Paper    (2) vs Rock     (1) (Win)  = ((2 - 1 + 4) % 3) - 1 = 1
             * Paper    (2) vs Paper    (2) (Draw) = ((2 - 2 + 4) % 3) - 1 = 0
             * Paper    (2) vs Scissors (3) (Lose) = ((2 - 3 + 4) % 3) - 1 = -1
             * Scissors (3) vs Rock     (1) (Lose) = ((3 - 1 + 4) % 3) - 1 = -1
             * Scissors (3) vs Paper    (2) (Win)  = ((3 - 2 + 4) % 3) - 1 = 1
             * Scissors (3) vs Scissors (3) (Draw) = ((3 - 3 + 4) % 3) - 1 = 0
             * 
             * But... since we actually want 0, 1, 2 instead of -1, 0, 1, we just lose the -1 subtraction meaning:
             * Score = ((Player Move - Elf Move + 4) % 3) * 3
             * 
             * Then we just add the Player Move.  Simples.
             */
            nPart1Score += (((kColumnB - kElfMove + 4) % 3) * 3) + kColumnB;

            /* Part 2 */

            /* Column B is now the Game State, Lose (1), Draw (2), Win (3), since it tried to match the enumeration of
             * Rock/Paper/Scissors, so to get the round score, we need to subtract 1 before we multiply it by 3.
             * Then all that remains is extracting the move...
             * 
             * For simplicity, we'll put the Game State to be Lose (-1), Draw (0), Win (1).  Modulo arithmetic here will
             * return our move in the range 0 - 2, where as we want 1 - 3, so we need to do the same rotation trick.
             * 
             * This time because we want to rotate the other direction, it's +2 instead of +4.
             * 
             * So we get:
             * 
             * Player Move = ((Elf Move + Game State + 2) % 3) + 1
             * 
             * Except we have a short cut, given Column B was originally 1 - 3, rather than -1 - 1, if we don't modify
             * Column B, we get the +2 for free!
             * 
             */
            nPart2Score += ((kColumnB-1) * 3) + (((kElfMove + kColumnB) % 3) + 1);
        }
 
        printf("Part 1: %lu\n", nPart1Score);
        printf("Part 2: %lu\n", nPart2Score);
 
        fclose(pData);
    }
 
    return 0;
}