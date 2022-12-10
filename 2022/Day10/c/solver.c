#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>

typedef enum OpcodeType
{
    OPCODE_NOOP    = 0,
    OPCODE_ADDX,
    OPCODE_LAST
} TOpcodeType;

#define WIDTH  (40)
#define HEIGHT (6)

TOpcodeType getOpcode(FILE** pData)
{
    const char kData = fgetc(*pData);

    /* Handle Spaces/EOL's */
    while (isalpha(fgetc(*pData)));

    if ('n' == kData)
    {
        return OPCODE_NOOP;
    }
    else if ('a' == kData)
    {
        return OPCODE_ADDX;
    }
    assert(0);
}

long getValue(FILE** pData)
{
    long nValue = 0;
    long nSign  = 1;

    /* Handle the Sign...
     * If this doesn't get processed, the read character will be
     * processed by the Value Loop
     */
    char kData  = fgetc(*pData);
    if ('-' == kData)
    {
        nSign = -1;
        kData = fgetc(*pData);
    }

    /* Handle the Value */
    do
    {
        assert(isdigit(kData));
        nValue = (nValue * 10) + (kData - '0');
    } while (isdigit(kData = fgetc(*pData)));

    return nValue * nSign;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        const long CYCLE_COUNT[OPCODE_LAST] =
        {
            1, /* NOOP */
            2  /* ADDX */
        };

        int Y;

        /* Initial States */
        long          x                 = 1;
        long          nCycleCounter     = 0;
        long          nSum              = 0;
        char kDisplay[HEIGHT][WIDTH + 1]; /* Add the NUL terminator to simplify the print operation */

        memset(kDisplay, ' ', sizeof(kDisplay));

        while (!feof(pData))
        {
            /* Get the Next Opcode */
            const TOpcodeType eOpcode          = getOpcode(&pData);
            long              nRemainingCycles = CYCLE_COUNT[eOpcode];
            long              nOperand;

            /* Verify the Opcode / Buffer any Operands */
            switch (eOpcode)
            {
                case OPCODE_NOOP: break;
                case OPCODE_ADDX:
                {
                    nOperand = getValue(&pData);
                } break;

                default: assert(0);
            }

            /* Process the End of Cycle */
            while(nRemainingCycles)
            {
                long nRow;
                long nCol;

                ++nCycleCounter;
                --nRemainingCycles;

                /* Part 1
                * Sum of Cycles and Frequencies at specified iterations
                */
                if (0 == ((nCycleCounter - 20) % 40))
                {
                    nSum += nCycleCounter * x;
                }

                /* Part 2
                * Updating based on Pixel Position
                */
                nRow = (nCycleCounter-1) / WIDTH;
                nCol = nCycleCounter - (nRow * WIDTH);
                if ((nCol >= x) && (nCol <= (x + 2)))
                {
                    kDisplay[nRow][nCol-1] = '#';
                }
            }

            /* Process the Buffered Opcode */
            if (OPCODE_ADDX == eOpcode)
            {
                x += nOperand;
            }
        }
        fclose(pData);

        printf("Part 1: %li\n", nSum);
        printf("Part 2:\n");
        for (Y = 0; Y < HEIGHT; ++Y)
        {
            kDisplay[Y][WIDTH] = '\0'; /* Add the NUL terminator to simplify the print operation */
            printf("%s\n", kDisplay[Y]);
        }
    }

    return 0;
}
