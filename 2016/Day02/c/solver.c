#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

typedef enum DirectionType
{
    E_UP    = 0,
    E_RIGHT,
    E_DOWN,
    E_LEFT
} DirectionType;

typedef struct DirectionGroupType
{
    DirectionType*  kDirections;
    size_t          nDirectionsSize;
} DirectionGroupType;

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

    *pnLineCount  = nLineCount;
    *pnFileLength = nFileSize;
}

PointType getVector(const DirectionType eDirection)
{
    const PointType kVectors[] =
    {
        { 0, -1},   /* E_UP    */
        { 1,  0},   /* E_RIGHT */
        { 0,  1},   /* E_DOWN  */
        {-1,  0}    /* E_LEFT  */
    };
    return kVectors[eDirection];
}

const char* getPasscode(const char** kKeypad, const size_t nHeight, const size_t nWidth, const DirectionGroupType* kMoveSet, const size_t nMoveSetSize, const char cStart, char* kPasscode)
{
    PointType   kPosition   = {0,0};

    /* Find the Start Position, and extract the Height/Width of the Keypad */
    bool        bFound    = false;
    size_t      Y;
    size_t      nMoveSet;

    for (Y = 0; (false == bFound) && (Y < nHeight); ++Y)
    {
        size_t X;
        for (X = 0; (false == bFound) && (X < nWidth); ++X)
        {
            if (cStart == kKeypad[Y][X])
            {
                kPosition.posX = X;
                kPosition.posY = Y;
                bFound         = true;
            }
        }
    }

    /* Make sure we actually found the start position... */
    assert(true == bFound);

    /* Initialise the Passcode */
    strcpy(kPasscode, "");

    /* For each set of moves ... */
    for (nMoveSet = 0; nMoveSet < nMoveSetSize; ++nMoveSet)
    {
        const size_t nMoveCount = kMoveSet[nMoveSet].nDirectionsSize;
        size_t       nMove;
        
        /* Process all moves in the current set */
        for (nMove = 0; nMove < nMoveCount; ++nMove)
        {
            /* Store the potential new position */
            const PointType kMoveVector        = getVector(kMoveSet[nMoveSet].kDirections[nMove]);
            const PointType kCandidatePosition = {
                                                    kPosition.posX + kMoveVector.posX,
                                                    kPosition.posY + kMoveVector.posY
            };

            /* Ensure the new position is on the Keypad Grid */
            if ((kCandidatePosition.posX >= 0) && (kCandidatePosition.posX < nWidth) &&
                (kCandidatePosition.posY >= 0) && (kCandidatePosition.posY < nHeight))
            {
                /* Ensure the new position is on the Keypad Itself */
                if ('.' != kKeypad[kCandidatePosition.posY][kCandidatePosition.posX])
                {
                    kPosition = kCandidatePosition;
                }
            }
        }

        /* Add the current keypad value to the Passcode */
        strncat(kPasscode, &kKeypad[kPosition.posY][kPosition.posX], 1);
    }

    return kPasscode;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const char* kStandardKeypad[] =
        {
            "123",
            "456",
            "789"
        };

        const char* kBathroomKeypad[] =
        {
            "..1..",
            ".234.",
            "56789",
            ".ABC.",
            "..D.."
        };

        char*                   kBuffer;
        char**                  kLines;
        size_t                  i;
        size_t                  nLineCount;
        size_t                  nFileSize;

        DirectionGroupType*     kMoves;
        char*                   kPasscode;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, &nFileSize);
        fclose(pInput);

        /* Allocate/Decode the Moves/Passcode */
        kMoves    = (DirectionGroupType*)malloc(nLineCount * sizeof(DirectionGroupType));
        kPasscode = (char*)malloc((nLineCount+1) * sizeof(char));
        for (i = 0; i < nLineCount; ++i)
        {
            size_t nMove;
\
            /* Allocate the Number of moves based on line length */
            kMoves[i].nDirectionsSize = strlen(kLines[i]);
            kMoves[i].kDirections     = (DirectionType*)malloc(kMoves[i].nDirectionsSize * sizeof(DirectionType));

            for (nMove = 0; nMove < kMoves[i].nDirectionsSize; ++nMove)
            {
                switch(kLines[i][nMove])
                {
                    case 'U':
                    {
                        kMoves[i].kDirections[nMove] = E_UP;
                    } break;

                    case 'R':
                    {
                        kMoves[i].kDirections[nMove] = E_RIGHT;
                    } break;

                    case 'D':
                    {
                        kMoves[i].kDirections[nMove] = E_DOWN;
                    } break;

                    case 'L':
                    {
                        kMoves[i].kDirections[nMove] = E_LEFT;
                    } break;

                    default:
                    {
                        assert(false);
                    }
                }
            }
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %s\n", getPasscode(kStandardKeypad, sizeof(kStandardKeypad)/sizeof(kStandardKeypad[0]), strlen(kStandardKeypad[0]), kMoves, nLineCount, '5', kPasscode));
        printf("Part 2: %s\n", getPasscode(kBathroomKeypad, sizeof(kBathroomKeypad)/sizeof(kBathroomKeypad[0]), strlen(kBathroomKeypad[0]), kMoves, nLineCount, '5', kPasscode));

        /* Free the Moves Memory */
        for (i = 0; i < nLineCount; ++i)
        {
            /* Allocate the Number of moves based on line length */
            free(kMoves[i].kDirections);
        }
        free(kMoves);
        free(kPasscode);
    }

    return 0;
}
