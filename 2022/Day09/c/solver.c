#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define FALSE (0)
#define TRUE  (1)

#define KNOT_GRID(p, X, Y, OX, OY, W) (p[((Y + OY) * W) + X + OX])
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength)
{
    const unsigned long     nStartPos   = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    char*                   pLine;
    size_t                  nLineCount  = 0;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';
    }
    else
    {
        (*pkFileBuffer)[nFileSize]   = '\0';
    }
 
    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, "\n");
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;

        pLine = strtok(NULL, "\n");
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

typedef struct Vector
{
    long X;
    long Y;
} TVector;

typedef struct MovementVector
{
    long    N;
    TVector V;
} TMovementVector;
 
int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        long                    MIN_X = 0;
        long                    MAX_X = 0;
        long                    MIN_Y = 0;
        long                    MAX_Y = 0;
        long                    OFFSET_X;
        long                    OFFSET_Y;
        long                    SIZE_X;
        long                    SIZE_Y;

        long                    X     = 0;
        long                    Y     = 0;

        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;

        size_t                  i;

        TMovementVector*        pMoves;
        unsigned*               pUniqueBufferPart1;
        unsigned*               pUniqueBufferPart2;
        size_t                  nUniqueCountPart1 = 0;
        size_t                  nUniqueCountPart2 = 0;

        TVector                 kKnots[10];

        /* Read the entire file into a line buffer. */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pData);

        /* First allocate the movement vectors.  All we care about
         * is the direction a movement will cause.
         */
        pMoves = (TMovementVector*)malloc(nLineCount * sizeof(TMovementVector));
        for (i = 0; i < nLineCount; ++i)
        {
            pMoves[i].N = strtol(&kLines[i][2], NULL, 10);

            switch (kLines[i][0])
            {
                case 'L':
                {
                    pMoves[i].V.X = -1;
                    pMoves[i].V.Y =  0;
                    X -= pMoves[i].N;
                    MIN_X = MIN(X, MIN_X);
                } break;

                case 'R':
                {
                    pMoves[i].V.X =  1;
                    pMoves[i].V.Y =  0;
                    X += pMoves[i].N;
                    MAX_X = MAX(X, MAX_X);
                } break;

                case 'U':
                {
                    pMoves[i].V.X =  0;
                    pMoves[i].V.Y =  1;
                    Y += pMoves[i].N;
                    MAX_Y = MAX(Y, MAX_Y);
                } break;

                case 'D':
                {
                    pMoves[i].V.X =  0;
                    pMoves[i].V.Y = -1;
                    Y -= pMoves[i].N;
                    MIN_Y = MIN(Y, MIN_Y);
                } break;

                default:
                {
                    assert(0);
                }
            }
        }

        /* The whole purpose of buffering the file is the unique buffer.
         * Specifically what we don't want to do is constantly search the
         * buffer repeatedly to see if a position is contained, we want to
         * *index* by the position instead, but they can go negative, so
         * first thing is to determine the bounds of the movement.  If we
         * establish a bounding rectangle in which the heads movements
         * could reside, the same will be true of the tail, as it can't
         * overshoot the head.
         */
        OFFSET_X = abs((int)MIN_X);
        OFFSET_Y = abs((int)MIN_Y);
        SIZE_X   = MAX_X - MIN_X;
        SIZE_Y   = MAX_Y - MIN_Y;

        pUniqueBufferPart1 = (unsigned*)malloc(SIZE_X * SIZE_Y * sizeof(unsigned));
        pUniqueBufferPart2 = (unsigned*)malloc(SIZE_X * SIZE_Y * sizeof(unsigned));

        memset(pUniqueBufferPart1, 0, SIZE_X * SIZE_Y * sizeof(unsigned));
        memset(pUniqueBufferPart2, 0, SIZE_X * SIZE_Y * sizeof(unsigned));

        /* Solve both parts... */
        memset(&kKnots, 0, sizeof(kKnots));
        for (i = 0; i < nLineCount; ++i)
        {
            size_t    N;
            unsigned* p;

            for (N = 0; N < pMoves[i].N; ++N)
            {
                size_t j;

                kKnots[0].X += pMoves[i].V.X;
                kKnots[0].Y += pMoves[i].V.Y;

                for (j = 1; j < (sizeof(kKnots) / sizeof(kKnots[0])); ++j)
                {
                    if ((abs((int)(kKnots[j].X - kKnots[j-1].X)) > 1) ||
                        (abs((int)(kKnots[j].Y - kKnots[j-1].Y)) > 1))
                    {
                        const long    DX = kKnots[j-1].X - kKnots[j].X;
                        const long    DY = kKnots[j-1].Y - kKnots[j].Y;
                        const TVector kTailVector =
                        {
                            MIN(1, MAX( -1, DX)),
                            MIN(1, MAX( -1, DY))
                        };

                        kKnots[j].X += kTailVector.X;
                        kKnots[j].Y += kTailVector.Y;
                    }
                }

                /* Part 1 acts on the 2nd Knot */
                p = &KNOT_GRID(pUniqueBufferPart1, kKnots[1].X, kKnots[1].Y, OFFSET_X, OFFSET_X, SIZE_X);
                if (FALSE == *p)
                {
                    *p = TRUE;
                    ++nUniqueCountPart1;
                }

                /* Part 2 acts on the Last Knot */
                p = &KNOT_GRID(pUniqueBufferPart2, kKnots[(sizeof(kKnots) / sizeof(kKnots[0])) - 1].X, kKnots[(sizeof(kKnots) / sizeof(kKnots[0])) - 1].Y, OFFSET_X, OFFSET_X, SIZE_X);
                if (FALSE == *p)
                {
                    *p = TRUE;
                    ++nUniqueCountPart2;
                }
            }
        }

        printf("Part 1: %zu\n", nUniqueCountPart1);
        printf("Part 2: %zu\n", nUniqueCountPart2);

        /* Clean Up */
        free(pMoves);
        free(pUniqueBufferPart1);
        free(pUniqueBufferPart2);
    }

    return 0;
}
