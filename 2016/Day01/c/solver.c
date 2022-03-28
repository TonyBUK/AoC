#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

typedef enum DirectionType
{
    E_NORTH  = 0,
    E_EAST,
    E_SOUTH,
    E_WEST
} DirectionType;

typedef enum TurnType
{
    E_LEFT  = 0,
    E_RIGHT
} TurnType;

typedef struct MovesType
{
    TurnType    eTurn;
    size_t      nDistance;
} MovesType;

typedef struct PointType
{
    int64_t posX;
    int64_t posY;
} PointType;

typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount)
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

    *pkFileBuffer   = (char*) malloc((nFileSize+1)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));

    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    (*pkFileBuffer)[nFileSize] = '\0';

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

    *pnLineCount = nLineCount;
}

size_t manhattenDistance(const PointType kPositionType)
{
    return abs((int)kPositionType.posX) + abs((int)kPositionType.posY);
}

DirectionType updateDirection(const DirectionType eCurrent, const TurnType eTurn)
{
    const DirectionType DIRECTION_CHANGE[][2] =
    {
        /* E_NORTH */
        {
            E_WEST,     /* E_LEFT  */
            E_EAST      /* E_RIGHT */
        },
        /* E_EAST */
        {
            E_NORTH,    /* E_LEFT  */
            E_SOUTH     /* E_RIGHT */
        },
        /* E_SOUTH */
        {
            E_EAST,     /* E_LEFT  */
            E_WEST      /* E_RIGHT */
        },
        /* E_WEST */
        {
            E_SOUTH,    /* E_LEFT  */
            E_NORTH     /* E_RIGHT */
        }
    };

    return DIRECTION_CHANGE[eCurrent][eTurn];
}

PointType getVector(const DirectionType eDirection)
{
    const PointType kVectors[] =
    {
        { 0,  1},   /* E_NORTH */
        { 1,  0},   /* E_EAST  */
        { 0, -1},   /* E_SOUTH */
        {-1,  0}    /* E_WEST  */
    };
    return kVectors[eDirection];
}

PointType processMovesAllowRepeatedPositions(const MovesType* kMoves, const size_t nMoveCount)
{
    PointType     kPosition  = {0, 0};
    DirectionType eDirection = E_NORTH;
    size_t        i;

    for (i = 0; i < nMoveCount; ++i)
    {
        PointType kMovementVector;

        /* Update the Direction */
        eDirection = updateDirection(eDirection, kMoves[i].eTurn);

        /* Update the Position */
        kMovementVector = getVector(eDirection);
        kPosition.posX += kMovementVector.posX * kMoves[i].nDistance;
        kPosition.posY += kMovementVector.posY * kMoves[i].nDistance;
    }

    return kPosition;
}

bool isRepeated(const PointType* kVisited, const size_t nVisitedSize, const PointType* kCurrent)
{
    size_t i;
    for (i = 0; i < nVisitedSize; ++i)
    {
        if ((kVisited[i].posX == kCurrent->posX) &&
            (kVisited[i].posY == kCurrent->posY))
        {
            return true;
        }
    }

    return false;
}

PointType processMovesDisallowRepeatedPositions(const MovesType* kMoves, const size_t nMoveCount)
{
    PointType     kPosition         = {0, 0};
    DirectionType eDirection        = E_NORTH;
    PointType*    kVisited          = NULL;
    size_t        nVisitedSize      = 1;
    size_t        nMaxVisitedSize   = 0;
    size_t        i;

    /* Allocate the Worst Case for Visited Locations */
    for (i = 0; i < nMoveCount; ++i)
    {
        nMaxVisitedSize += kMoves[i].nDistance;
    }
    kVisited    = (PointType*)malloc(nMaxVisitedSize * sizeof(PointType));

    kVisited[0] = kPosition;

    for (i = 0; i < nMoveCount; ++i)
    {
        PointType kMovementVector;
        size_t    j;

        /* Update the Direction */
        eDirection = updateDirection(eDirection, kMoves[i].eTurn);

        /* Update the Position */
        kMovementVector = getVector(eDirection);

        for (j = 0; j < kMoves[i].nDistance; ++j)
        {
            kPosition.posX += kMovementVector.posX;
            kPosition.posY += kMovementVector.posY;

            if (isRepeated(kVisited, nVisitedSize, &kPosition))
            {
                free(kVisited);
                return kPosition;
            }
            kVisited[nVisitedSize] = kPosition;
            ++nVisitedSize;
        }
    }

    free(kVisited);
    return kPosition;
}

PointType processMoves(const MovesType* kMoves, const size_t nMoveCount, const bool bfinishOnFirstRepetition)
{
    if (!bfinishOnFirstRepetition)
    {
        return processMovesAllowRepeatedPositions(kMoves, nMoveCount);
    }
    else
    {
        return processMovesDisallowRepeatedPositions(kMoves, nMoveCount);
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  i;
        size_t                  nLineCount;

        MovesType*              kMoves;
        size_t                  nMoveCount      = 0;
        size_t                  nMoveCountMax   = 0;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount);
        fclose(pInput);

        /* Count the Number of Moves */
        for (i = 0; i < nLineCount; ++i)
        {
            const char* pBuffer = kLines[i];
            while(*pBuffer)
            {
                if (',' == *pBuffer)
                {
                    ++nMoveCountMax;
                }
                ++pBuffer;
            }
            nMoveCountMax += 1;
        }

        /* Allocate the number of Moves */
        kMoves = (MovesType*)malloc(nMoveCountMax * sizeof(MovesType));

        /* Convert the String Buffer into a Series of Moves */
        for (i = 0; i < nLineCount; ++i)
        {
            char* pToken = strtok(kLines[i], ", ");
            while (pToken)
            {
                if ('L' == pToken[0])
                {
                    kMoves[nMoveCount].eTurn = E_LEFT;
                }
                else if ('R' == pToken[0])
                {
                    kMoves[nMoveCount].eTurn = E_RIGHT;
                }
                else
                {
                    assert(0);
                }

                kMoves[nMoveCount].nDistance = strtoull(&pToken[1], NULL, 10);

                ++nMoveCount;
                pToken = strtok(NULL, ", ");
            }
        }

        /* Make sure we copied it all correctly... */
        assert(nMoveCountMax == nMoveCount);

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %u\n", (uint32_t)manhattenDistance(processMoves(kMoves, nMoveCount, false)));
        printf("Part 2: %u\n", (uint32_t)manhattenDistance(processMoves(kMoves, nMoveCount, true)));

        free(kMoves);
    }

    return 0;
}
