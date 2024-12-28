#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define TO_GRID(nX, nY, nWidth) ((nY) * (nWidth) + (nX))

typedef struct PositionType
{
    uint64_t nX;
    uint64_t nY;
} PositionType;

typedef struct BoxPositionType
{
    PositionType kPosition;
    size_t       nKey;
} BoxPositionType;

typedef struct BoxPairPositionType
{
    BoxPositionType             kPositionAndKey;
    struct BoxPairPositionType* pPairedBox;
} BoxPairPositionType;

typedef struct DeltaType
{
    size_t               nKeyFrom;
    size_t               nKeyTo;
    BoxPairPositionType* pBox;
    PositionType         kBoxPosition;
} DeltaType;

typedef enum DirectionType
{
    DIRECTION_LEFT = 0,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_RIGHT
} DirectionType;

const int64_t G_DIRECTIONS[4][2] = {
    { -1,  0 }, /* DIRECTION_LEFT  */
    {  0, -1 }, /* DIRECTION_UP    */
    {  0,  1 }, /* DIRECTION_DOWN  */
    {  1,  0 }  /* DIRECTION_RIGHT */
};

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+3)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);
 
    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }
 
    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if (0 == bProcessUnix)
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\r';
            (*pkFileBuffer)[nFileSize+1] = '\n';
            (*pkFileBuffer)[nFileSize+2] = '\0';
        }
    }
    else
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\n';
            (*pkFileBuffer)[nFileSize+1] = '\0';
        }
    }
 
    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);
 
        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;
 
            assert(pLine < &(*pkFileBuffer)[nFileSize]);
 
            (*pkLines)[nLineCount++] = pLine;
 
            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';
 
            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

/* Part 1 Box Movement as DFS */
unsigned moveBoxPartOne(const DirectionType eDirection, const PositionType* pkBox, const size_t nBoxIndex, BoxPositionType** pkBoxes, const unsigned* pkWalls, const size_t nWidth)
{
    /* Calculate the Next Box Position*/
    const PositionType kNextBoxPosition =
    {
        (uint64_t)((int64_t)pkBox->nX + G_DIRECTIONS[eDirection][0]),
        (uint64_t)((int64_t)pkBox->nY + G_DIRECTIONS[eDirection][1])
    };
    const size_t       nNextBoxIndex    = TO_GRID(kNextBoxPosition.nX, kNextBoxPosition.nY, nWidth);

    /* If it's collided with a Wall, ignore the move */
    if (pkWalls[nNextBoxIndex])
    {
        return AOC_FALSE;
    }

    /* If its collided with a box, see if the next box can move, otherwise ignore the move */
    if (pkBoxes[nNextBoxIndex])
    {
        if (AOC_FALSE == moveBoxPartOne(eDirection, &kNextBoxPosition, nNextBoxIndex, pkBoxes, pkWalls, nWidth))
        {
            return AOC_FALSE;
        }
    }

    /* Move the Box */
    pkBoxes[nNextBoxIndex]            = pkBoxes[nBoxIndex];
    pkBoxes[nBoxIndex]                = NULL;
    pkBoxes[nNextBoxIndex]->nKey      = nNextBoxIndex;
    pkBoxes[nNextBoxIndex]->kPosition = kNextBoxPosition;

    /* Indicate to any other box moves that the move is legal */
    return AOC_TRUE;
}

void moveRobotPartOne(const DirectionType eDirection, PositionType* pkRobot, BoxPositionType** pkBoxes, const unsigned* pkWalls, const size_t nWidth)
{
    /* Calculate the Robot Position */
    const PositionType kNextRobotPosition =
    {
        (uint64_t)((int64_t)pkRobot->nX + G_DIRECTIONS[eDirection][0]),
        (uint64_t)((int64_t)pkRobot->nY + G_DIRECTIONS[eDirection][1])
    };
    const size_t       nNextRobotIndex    = TO_GRID(kNextRobotPosition.nX, kNextRobotPosition.nY, nWidth);

    /* If it's collided with a Wall, ignore the move */
    if (pkWalls[nNextRobotIndex])
    {
        return;
    }

    /* If its collided with a box, see if the box can move, otherwise ignore the move */
    if (pkBoxes[nNextRobotIndex])
    {
        if (AOC_FALSE == moveBoxPartOne(eDirection, &kNextRobotPosition, nNextRobotIndex, pkBoxes, pkWalls, nWidth))
        {
            return;
        }
    }

    /* Move the Robot */
    pkRobot->nX = kNextRobotPosition.nX;
    pkRobot->nY = kNextRobotPosition.nY;
}

/* Part 2 Box Movement as DFS */
unsigned moveBoxesPartTwo(const DirectionType eDirection, const PositionType* pkBox1, const size_t nBox1Index, const PositionType* pkBox2, const size_t nBox2Index, BoxPairPositionType** pkBoxes, const unsigned* pkWalls, DeltaType* kDeltas, size_t* pnDeltaCount, const size_t nWidth)
{
    /*
     * Calculate the next position(s)
     * Note: This relies upon X being ordered left to right
     */
    const PositionType kNextBoxPositions[2] =
    {
        { (uint64_t)((int64_t)pkBox1->nX + G_DIRECTIONS[eDirection][0]), (uint64_t)((int64_t)pkBox1->nY + G_DIRECTIONS[eDirection][1]) },
        { (uint64_t)((int64_t)pkBox2->nX + G_DIRECTIONS[eDirection][0]), (uint64_t)((int64_t)pkBox2->nY + G_DIRECTIONS[eDirection][1]) }
    };

    const size_t      kNextBoxIndexes[2]    =
    {
        TO_GRID(kNextBoxPositions[0].nX, kNextBoxPositions[0].nY, nWidth),
        TO_GRID(kNextBoxPositions[1].nX, kNextBoxPositions[1].nY, nWidth)
    };

    /* Walls are Impossible Moves */
    if (pkWalls[kNextBoxIndexes[0]] || pkWalls[kNextBoxIndexes[1]])
    {
        return AOC_FALSE;
    }

    /* If we're moving Left, find any adjacent boxes to the left side and move it first */
    if (DIRECTION_LEFT == eDirection)
    {
        if (pkBoxes[kNextBoxIndexes[0]])
        {
            /* Note: As we're moving left, the found box *must* be the right side of a box */
            const BoxPairPositionType* kRightBox = pkBoxes[kNextBoxIndexes[0]];
            const BoxPairPositionType* kLeftBox  = kRightBox->pPairedBox;
            assert(kLeftBox->kPositionAndKey.kPosition.nX < kRightBox->kPositionAndKey.kPosition.nX);

            if (AOC_FALSE == moveBoxesPartTwo(eDirection, &kLeftBox->kPositionAndKey.kPosition, kLeftBox->kPositionAndKey.nKey, &kRightBox->kPositionAndKey.kPosition, kRightBox->kPositionAndKey.nKey, pkBoxes, pkWalls, kDeltas, pnDeltaCount, nWidth))
            {
                return AOC_FALSE;
            }
        }
    }
    /* If we're moving Left, find any adjacent boxes to the right side and move it first */
    else if (DIRECTION_RIGHT == eDirection)
    {
        if (pkBoxes[kNextBoxIndexes[1]])
        {
            /* Note: As we're moving right, the found box *must* be the left side of a box */
            const BoxPairPositionType* kLeftBox  = pkBoxes[kNextBoxIndexes[1]];
            const BoxPairPositionType* kRightBox = kLeftBox->pPairedBox;
            assert(kLeftBox->kPositionAndKey.kPosition.nX < kRightBox->kPositionAndKey.kPosition.nX);

            if (AOC_FALSE == moveBoxesPartTwo(eDirection, &kLeftBox->kPositionAndKey.kPosition, kLeftBox->kPositionAndKey.nKey, &kRightBox->kPositionAndKey.kPosition, kRightBox->kPositionAndKey.nKey, pkBoxes, pkWalls, kDeltas, pnDeltaCount, nWidth))
            {
                return AOC_FALSE;
            }
        }
    }
    /* Moving Up/Down */
    else
    {
        /*
         * As the boxes are 2 units wide, we will either collide with:
         *
         * 1. A Wall
         * 2. One Box
         * 3. Two Boxes
         */
        size_t i;
        for (i = 0; i < sizeof(kNextBoxIndexes) / sizeof(kNextBoxIndexes[0]); ++i)
        {
            if (pkBoxes[kNextBoxIndexes[i]])
            {
                /* Note: We won't know if we've collided with a left or right edge... */
                const BoxPairPositionType* kBox1     = pkBoxes[kNextBoxIndexes[i]];
                const BoxPairPositionType* kBox2     = kBox1->pPairedBox;
                const BoxPairPositionType* kLeftBox  = (kBox1->kPositionAndKey.kPosition.nX < kBox2->kPositionAndKey.kPosition.nX) ? kBox1 : kBox2;
                const BoxPairPositionType* kRightBox = kLeftBox->pPairedBox;
                assert(kLeftBox->kPositionAndKey.kPosition.nX < kRightBox->kPositionAndKey.kPosition.nX);

                if (AOC_FALSE == moveBoxesPartTwo(eDirection, &kLeftBox->kPositionAndKey.kPosition, kLeftBox->kPositionAndKey.nKey, &kRightBox->kPositionAndKey.kPosition, kRightBox->kPositionAndKey.nKey, pkBoxes, pkWalls, kDeltas, pnDeltaCount, nWidth))
                {
                    return AOC_FALSE;
                }
            }
        }
    }

    /*
     * Buffer the Delta Moves
     *
     * Note: This is done to ensure that all boxes can move before actually moving them.
     *       Also duplicates are harmless.
     */
    kDeltas[*pnDeltaCount].nKeyFrom         = nBox1Index;
    kDeltas[*pnDeltaCount].nKeyTo           = kNextBoxIndexes[0];
    kDeltas[*pnDeltaCount].pBox             = pkBoxes[nBox1Index];
    kDeltas[(*pnDeltaCount)++].kBoxPosition = kNextBoxPositions[0];

    kDeltas[*pnDeltaCount].nKeyFrom         = nBox2Index;
    kDeltas[*pnDeltaCount].nKeyTo           = kNextBoxIndexes[1];
    kDeltas[*pnDeltaCount].pBox             = pkBoxes[nBox2Index];
    kDeltas[(*pnDeltaCount)++].kBoxPosition = kNextBoxPositions[1];

    return AOC_TRUE;
}

void moveRobotPartTwo(const DirectionType eDirection, PositionType* pkRobot, BoxPairPositionType** pkBoxes, DeltaType* kDeltas, const unsigned* pkWalls, const size_t nWidth)
{
    /* Calculate the Robot Position */
    const PositionType kNextRobotPosition =
    {
        (uint64_t)((int64_t)pkRobot->nX + G_DIRECTIONS[eDirection][0]),
        (uint64_t)((int64_t)pkRobot->nY + G_DIRECTIONS[eDirection][1])
    };
    const size_t       nNextRobotIndex    = TO_GRID(kNextRobotPosition.nX, kNextRobotPosition.nY, nWidth);
    size_t             nDeltaCount        = 0;
    size_t             nDelta;

    /* If it's collided with a Wall, ignore the move */
    if (pkWalls[nNextRobotIndex])
    {
        return;
    }

    /* If its collided with a box, see if the box can move, otherwise ignore the move */
    if (pkBoxes[nNextRobotIndex])
    {
        const BoxPairPositionType* kBox1       = pkBoxes[nNextRobotIndex];
        const BoxPairPositionType* kBox2       = kBox1->pPairedBox;
        const BoxPairPositionType* kLeftBox    = (kBox1->kPositionAndKey.kPosition.nX < kBox2->kPositionAndKey.kPosition.nX) ? kBox1 : kBox2;
        const BoxPairPositionType* kRightBox   = kLeftBox->pPairedBox;

        if (AOC_FALSE == moveBoxesPartTwo(eDirection, &kLeftBox->kPositionAndKey.kPosition, kLeftBox->kPositionAndKey.nKey, &kRightBox->kPositionAndKey.kPosition, kRightBox->kPositionAndKey.nKey, pkBoxes, pkWalls, kDeltas, &nDeltaCount, nWidth))
        {
            return;
        }
    }

    /* Move the Robot */
    pkRobot->nX = kNextRobotPosition.nX;
    pkRobot->nY = kNextRobotPosition.nY;

    /* Move the Boxes */

    /*
     * First clear all the old positions.
     * Note: The reason this is done as a discrete step is this may end up accidentally
     *       removing a moved robot from all future move considerations.
     */
    for (nDelta = 0; nDelta < nDeltaCount; ++nDelta)
    {
        pkBoxes[kDeltas[nDelta].nKeyFrom] = NULL;
    }

    /* Apply the New Positions */
    for (nDelta = 0; nDelta < nDeltaCount; ++nDelta)
    {
        const size_t nKeyTo       = kDeltas[nDelta].nKeyTo;
        BoxPairPositionType* pBox = kDeltas[nDelta].pBox;

        pkBoxes[nKeyTo]                 = pBox;
        pBox->kPositionAndKey.nKey      = nKeyTo;
        pBox->kPositionAndKey.kPosition = kDeltas[nDelta].kBoxPosition;
    }
}

DirectionType getDirection(const char kDirection)
{
    switch (kDirection)
    {
        case '<': return DIRECTION_LEFT;
        case '^': return DIRECTION_UP;
        case 'v': return DIRECTION_DOWN;
        case '>': return DIRECTION_RIGHT;
    }

    assert(0);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        /* Width/Height are defined by the Puzzle Text itself */
        char*                       kInputBuffer;
        char**                      kInputLines;
        size_t                      nLineCount;

        size_t                      nWidth          = 0;
        size_t                      nHeight         = 0;

        size_t                      nInputStart;

        size_t                      nLine;
        size_t                      nY;
        size_t                      nBoxCount = 0;
        size_t                      nBox;

        BoxPositionType**           kBoxesPartOneInGrid;
        BoxPositionType*            kBoxesPartOne;
        unsigned*                   kWallsPartOne;
        BoxPairPositionType*        kBoxesPartTwo;
        BoxPairPositionType**       kBoxesPartTwoInGrid;
        unsigned*                   kWallsPartTwo;
        DeltaType*                  kDeltaPartTwo;

        PositionType                kRobotPositionPartOne;
        PositionType                kRobotPositionPartTwo;

        uint64_t                    nPartOne    = 0;
        uint64_t                    nPartTwo    = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nLineCount, NULL, NULL, 0);
        fclose(pData);

        /* Parse the Input Data */
        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            const size_t nLength = strlen(kInputLines[nLine]);
            if (0 == nWidth)
            {
                nWidth = nLength;
            }
            else if (0 == nLength)
            {
                nInputStart = nLine + 1;
                break;
            }

            ++nHeight;
        }

        /* Allocate the Data Buffers */
        kBoxesPartOne       = (BoxPositionType*)malloc(nWidth * nHeight * sizeof(BoxPositionType));
        kBoxesPartOneInGrid = (BoxPositionType**)calloc(nWidth * nHeight, sizeof(BoxPositionType*));
        kWallsPartOne       = (unsigned*)calloc(nWidth * nHeight, sizeof(unsigned));
        kBoxesPartTwo       = (BoxPairPositionType*)malloc(nWidth * 2 * nHeight * sizeof(BoxPairPositionType));
        kBoxesPartTwoInGrid = (BoxPairPositionType**)calloc(nWidth * 2 * nHeight, sizeof(BoxPairPositionType*));
        kWallsPartTwo       = (unsigned*)calloc(nWidth * 2 * nHeight, sizeof(unsigned));
        kDeltaPartTwo       = (DeltaType*)malloc(nWidth * 2 * nHeight * sizeof(DeltaType));

        /* Assign the Wall/Boxes Data/Player */
        for (nY = 0; nY < nHeight; ++nY)
        {
            size_t nX;
            for (nX = 0; nX < nWidth; ++nX)
            {
                switch (kInputLines[nY][nX])
                {
                    /* Walls - Part One as-is, part two, twice as wide*/
                    case '#':
                    {
                        kWallsPartOne[TO_GRID(nX,         nY, nWidth  )] = AOC_TRUE;
                        kWallsPartTwo[TO_GRID((nX*2) + 0, nY, nWidth*2)] = AOC_TRUE;
                        kWallsPartTwo[TO_GRID((nX*2) + 1, nY, nWidth*2)] = AOC_TRUE;
                    } break;

                    /* Player Position - Double for Part Two */
                    case '@':
                    {
                        kRobotPositionPartOne.nX = nX;
                        kRobotPositionPartOne.nY = nY;
                        kRobotPositionPartTwo.nX = nX * 2;
                        kRobotPositionPartTwo.nY = nY;
                    } break;

                    /* Boxes - Double width for Part Two, but also paired */
                    case 'O':
                    {
                        const size_t nIndexPartOne   = TO_GRID(nX,         nY, nWidth  );
                        const size_t nIndexPartTwo_1 = TO_GRID((nX*2) + 0, nY, nWidth*2);
                        const size_t nIndexPartTwo_2 = TO_GRID((nX*2) + 1, nY, nWidth*2);

                        /* Part One Boxes stored as an array but also in grid space */
                        kBoxesPartOne[nBoxCount].kPosition.nX = nX;
                        kBoxesPartOne[nBoxCount].kPosition.nY = nY;
                        kBoxesPartOne[nBoxCount].nKey         = nIndexPartOne;
                        kBoxesPartOneInGrid[nIndexPartOne]    = &kBoxesPartOne[nBoxCount];

                        /* Part Two Boxes stored as an array but also in grid space */
                        kBoxesPartTwo[(nBoxCount * 2) + 0].kPositionAndKey.kPosition.nX = (nX * 2) + 0;
                        kBoxesPartTwo[(nBoxCount * 2) + 0].kPositionAndKey.kPosition.nY = nY;
                        kBoxesPartTwo[(nBoxCount * 2) + 0].kPositionAndKey.nKey         = nIndexPartTwo_1;
                        kBoxesPartTwo[(nBoxCount * 2) + 0].pPairedBox                   = &kBoxesPartTwo[(nBoxCount * 2) + 1];
                        kBoxesPartTwo[(nBoxCount * 2) + 1].kPositionAndKey.kPosition.nX = (nX * 2) + 1;
                        kBoxesPartTwo[(nBoxCount * 2) + 1].kPositionAndKey.kPosition.nY = nY;
                        kBoxesPartTwo[(nBoxCount * 2) + 1].kPositionAndKey.nKey         = nIndexPartTwo_2;
                        kBoxesPartTwo[(nBoxCount * 2) + 1].pPairedBox                   = &kBoxesPartTwo[(nBoxCount * 2) + 0];

                        kBoxesPartTwoInGrid[nIndexPartTwo_1] = &kBoxesPartTwo[(nBoxCount * 2) + 0];
                        kBoxesPartTwoInGrid[nIndexPartTwo_2] = &kBoxesPartTwo[(nBoxCount * 2) + 1];

                        ++nBoxCount;
                    } break;

                    default:
                    {
                    } break;
                }
            }
        }

        /* Move the Boxes */
        for (nLine = nInputStart; nLine < nLineCount; ++nLine)
        {
            const char*  kLine   = kInputLines[nLine];
            const size_t nLength = strlen(kLine);
            size_t       nMove;
            for (nMove = 0; nMove < nLength; ++nMove)
            {
                const DirectionType eDirection = getDirection(kLine[nMove]);

                /* Move the Robot (Part One) */
                moveRobotPartOne(eDirection, &kRobotPositionPartOne, kBoxesPartOneInGrid, kWallsPartOne, nWidth);

                /* Move the Robot (Part Two) */
                moveRobotPartTwo(eDirection, &kRobotPositionPartTwo, kBoxesPartTwoInGrid, kDeltaPartTwo, kWallsPartTwo, nWidth * 2);
            }
        }

        /* Calculate the Solution based on the Final Box positions */
        for (nBox = 0; nBox < nBoxCount; ++nBox)
        {
            nPartOne += (kBoxesPartOne[nBox].kPosition.nY                     * 100) + kBoxesPartOne[nBox].kPosition.nX;

            /* Note: For Part Two, only count the evens as this is the left side of a *single* box. */
            nPartTwo += (kBoxesPartTwo[nBox * 2].kPositionAndKey.kPosition.nY * 100) + kBoxesPartTwo[nBox * 2].kPositionAndKey.kPosition.nX;
        }

        printf("Part 1: %" PRIu64 "\n", nPartOne);
        printf("Part 2: %" PRIu64 "\n", nPartTwo);

        /* Free any Allocated Memory */
        free(kInputLines);
        free(kInputBuffer);
        free(kBoxesPartOne);
        free(kBoxesPartOneInGrid);
        free(kWallsPartOne);
        free(kBoxesPartTwo);
        free(kBoxesPartTwoInGrid);
        free(kWallsPartTwo);
        free(kDeltaPartTwo);
    }
 
    return 0;
}