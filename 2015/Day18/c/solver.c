#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

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

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define MAKE_POSITION(X,Y,WIDTH,HEIGHT) (((Y) * WIDTH) + (X))
#define EXTRACT_Y(POS, WIDTH, HEIGHT)   (POS / WIDTH)
#define EXTRACT_X(POS, WIDTH, HEIGHT)   (POS % WIDTH)
typedef size_t positionType;

size_t initialiseConwayCandidates(const positionType* kLiveConways, const size_t nConwaySize, BOOL* kLiveConwaysGrid, size_t* kConwayNeighbours, const size_t nWidth, const size_t nHeight)
{
    size_t i;
    size_t nConwayCandidatesSize = 0;
    size_t nLiveCount            = 0;
    memset(kLiveConwaysGrid,  FALSE, nWidth * nHeight * sizeof(BOOL));
    memset(kConwayNeighbours,     0, nWidth * nHeight * sizeof(size_t));

    for (i = 0; i < nConwaySize; ++i)
    {
        size_t tY;
        size_t tX;

        /* If this is a duplicate... */
        if (TRUE == kLiveConwaysGrid[kLiveConways[i]]) continue;
        
        /* Indicate the Current Conway is Live... */
        kLiveConwaysGrid[kLiveConways[i]] = TRUE;
        nLiveCount += 1;

        /* Extract X/Y */
        const size_t Y = EXTRACT_Y(kLiveConways[i], nWidth, nHeight);
        const size_t X = EXTRACT_X(kLiveConways[i], nWidth, nHeight);

        /* Set the Y Bounds */
        const size_t tYStart = (Y >             1) ? Y - 1 : 0;
        const size_t tYEnd   = (Y < (nHeight - 2)) ? Y + 1 : nHeight - 1;

        /* Set the X Bounds */
        const size_t tXStart = (X >             1) ? X - 1 : 0;
        const size_t tXEnd   = (X < (nWidth  - 2)) ? X + 1 : nWidth - 1;

        for (tY = tYStart; tY <= tYEnd; ++tY)
        {
            for (tX = tXStart; tX <= tXEnd; ++tX)
            {
                if ((tY != Y) || (tX != X))
                {
                    const positionType nPos = MAKE_POSITION(tX, tY, nWidth, nHeight);
                    kConwayNeighbours[nPos] += 1;
                }
            }
        }
    }

    return nLiveCount;
}

size_t processConways(const positionType* kInitialLiveConways, const size_t nInitialConwaySize, const size_t nIterations, const size_t nWidth, const size_t nHeight, const BOOL bForceCorners)
{
    const positionType  kCorners[4]         =
    {
        MAKE_POSITION(0,                    0, nWidth, nHeight),
        MAKE_POSITION(0,          nHeight - 1, nWidth, nHeight),
        MAKE_POSITION(nWidth - 1,           0, nWidth, nHeight),
        MAKE_POSITION(nWidth - 1, nHeight - 1, nWidth, nHeight)
    };
    positionType*       kLiveConways[2]     =
    {
        (positionType*)malloc(nWidth * nHeight * sizeof(positionType)),
        (positionType*)malloc(nWidth * nHeight * sizeof(positionType))
    };
    BOOL*               kLiveConwaysGrid    = (BOOL*)malloc(nWidth * nHeight * sizeof(BOOL));
    size_t*             kConwayNeighbours   = (size_t*)malloc(nWidth * nHeight * sizeof(size_t));
    size_t              kLiveConwaySize[sizeof(kLiveConways)/sizeof(kLiveConways[0])];
    size_t              i;
    size_t              nBuffer             = 0;
    size_t              nLiveCount;

    /* Initialise Conway */
    kLiveConwaySize[nBuffer] = nInitialConwaySize;
    for (i = 0; i < nInitialConwaySize; ++i)
    {
        kLiveConways[nBuffer][i] = kInitialLiveConways[i];
    }

    for (i = 0; i < nIterations; ++i)
    {
        const size_t nOtherBuffer = 1 - nBuffer;
        size_t       nPos;

        if (TRUE == bForceCorners)
        {
            size_t n = 0;
            for (n = 0; n < (sizeof(kCorners)/sizeof(kCorners[0])); ++n)
            {
                kLiveConways[nBuffer][kLiveConwaySize[nBuffer]] = kCorners[n];
                ++kLiveConwaySize[nBuffer];
            }
        }
        initialiseConwayCandidates(kLiveConways[nBuffer], kLiveConwaySize[nBuffer], kLiveConwaysGrid, kConwayNeighbours, nWidth, nHeight);

        kLiveConwaySize[nOtherBuffer] = 0;
        for (nPos = 0; nPos < (nWidth * nHeight); ++nPos)
        {
            if (TRUE == kLiveConwaysGrid[nPos])
            {
                if ((2 == kConwayNeighbours[nPos]) || (3 == kConwayNeighbours[nPos]))
                {
                    kLiveConways[nOtherBuffer][kLiveConwaySize[nOtherBuffer]] = nPos;
                    ++kLiveConwaySize[nOtherBuffer];
                }
            }
            else
            {
                if (3 == kConwayNeighbours[nPos])
                {
                    kLiveConways[nOtherBuffer][kLiveConwaySize[nOtherBuffer]] = nPos;
                    ++kLiveConwaySize[nOtherBuffer];
                }
            }
        }

        /* Update theBuffer in Use */
        nBuffer = nOtherBuffer;
    }

    if (TRUE == bForceCorners)
    {
        size_t n = 0;
        for (n = 0; n < (sizeof(kCorners)/sizeof(kCorners[0])); ++n)
        {
            kLiveConways[nBuffer][kLiveConwaySize[nBuffer]] = kCorners[n];
            ++kLiveConwaySize[nBuffer];
        }
    }
    nLiveCount = initialiseConwayCandidates(kLiveConways[nBuffer], kLiveConwaySize[nBuffer], kLiveConwaysGrid, kConwayNeighbours, nWidth, nHeight);

    for (i = 0; i < (sizeof(kLiveConways)/sizeof(kLiveConways[0])); ++i)
    {
        free(kLiveConways[i]);
    }
    free(kLiveConwaysGrid);
    free(kConwayNeighbours);

    return nLiveCount;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        size_t                  nHeight;
        size_t                  nWidth;

        positionType*           kLiveConways;
        size_t                  nLiveConwaySize = 0;

        size_t                  y;
        size_t                  x;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nHeight);
        fclose(pInput);

        /* Calculate the Width */
        nWidth = strlen(kLines[0]);

        /* Allocate the Conways */
        kLiveConways = (positionType*)malloc(nWidth * nHeight * sizeof(positionType));

        /* Store the Live Conways */
        for (y = 0; y < nHeight; ++y)
        {
            for (x = 0; x < strlen(kLines[y]); ++x)
            {
                if ('#' == kLines[y][x])
                {
                    kLiveConways[nLiveConwaySize] = MAKE_POSITION(x, y, nWidth, nHeight);
                    ++nLiveConwaySize;
                }
            }
        }

        /* Free the Line Buffers since we've parsed all the data */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %u\n", (uint32_t)processConways(kLiveConways, nLiveConwaySize, 100, nWidth, nHeight, FALSE));
        printf("Part 2: %u\n", (uint32_t)processConways(kLiveConways, nLiveConwaySize, 100, nWidth, nHeight, TRUE));

        /* Free the Conways */
        free(kLiveConways);
   }

    return 0;
}
