#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
 
#define AOC_TRUE  (1)
#define AOC_FALSE (0)
 
#define TO_GRID(x, y, width) ((y) * (width) + (x))
#define TO_GRID_PERIMETER(x, y, width) (((y)+1) * ((width)+2) + ((x)+1))

typedef struct FarmLocationType
{
    int nX;
    int nY;
} FarmLocationType;

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

void calculateCropRegion(const char** kFarm, const char kCrop, const size_t nCropX, const size_t nCropY, const size_t nWidth, const size_t nHeight, unsigned* kSeen, FarmLocationType* kPositionQueueBuffer, uint64_t* pArea, uint64_t* pPerimeter, FarmLocationType* pkPerimeterLocations)
{
    const int    NEIGHBOURS[4][2]    = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    const size_t NEIGHBOURS_SIZE     = sizeof(NEIGHBOURS) / sizeof(NEIGHBOURS[0]);

    uint64_t nArea                   = 0;
    uint64_t nPerimeter              = 0;
    size_t   nPositionQueueSize      = 1;

    assert(kSeen);
    assert(kPositionQueueBuffer);
    assert(pArea);
    assert(pPerimeter);

    kPositionQueueBuffer[0].nX       = nCropX;
    kPositionQueueBuffer[0].nY       = nCropY;

    while (nPositionQueueSize > 0)
    {
        const FarmLocationType kPosition    = kPositionQueueBuffer[--nPositionQueueSize];
        const size_t           nPositionKey = TO_GRID(kPosition.nX, kPosition.nY, nWidth);

        size_t                 nNeighbour;

        /* Ignore if Seen */
        if (kSeen[nPositionKey])
        {
            continue;
        }
        kSeen[nPositionKey] = AOC_TRUE;

        /* Increment the Area */
        ++nArea;

        for (nNeighbour = 0; nNeighbour < NEIGHBOURS_SIZE; ++nNeighbour)
        {
            const int nX = kPosition.nX + NEIGHBOURS[nNeighbour][0];
            const int nY = kPosition.nY + NEIGHBOURS[nNeighbour][1];

            if (nY >= 0 && nY < nHeight)
            {
                if (nX >= 0 && nX < nWidth)
                {
                    const size_t nNeighbourKey = TO_GRID(nX, nY, nWidth);

                    if (kFarm[nY][nX] == kCrop)
                    {
                        kPositionQueueBuffer[nPositionQueueSize].nX = nX;
                        kPositionQueueBuffer[nPositionQueueSize].nY = nY;
                        ++nPositionQueueSize;
                    }
                    else
                    {
                        if (pkPerimeterLocations)
                        {
                            pkPerimeterLocations[nPerimeter].nX = nX;
                            pkPerimeterLocations[nPerimeter].nY = nY;
                        }
                        ++nPerimeter;
                    }
                }
                else
                {
                    if (pkPerimeterLocations)
                    {
                        pkPerimeterLocations[nPerimeter].nX = nX;
                        pkPerimeterLocations[nPerimeter].nY = nY;
                    }
                    ++nPerimeter;
                }
            }
            else
            {
                if (pkPerimeterLocations)
                {
                    pkPerimeterLocations[nPerimeter].nX = nX;
                    pkPerimeterLocations[nPerimeter].nY = nY;
                }
                ++nPerimeter;
            }
        }
    }

    /* Output the Area/Perimeter */
    *pArea      = nArea;
    *pPerimeter = nPerimeter;
}

void followSide(const FarmLocationType* kPerimeter, const size_t nPerimeterSize, const int X, const int Y, const int nXDelta, const int nYDelta, const size_t nWidth, const size_t nHeight, const unsigned* kPerimeterMap, unsigned* kSeen, size_t* kSeenList, size_t* pSeenCount)
{
    const size_t nPositionKey = TO_GRID_PERIMETER(X, Y, nWidth);
    int nNextX, nNextY;

    /* Append this Position to the Seen List */
    kSeenList[(*pSeenCount)++] = nPositionKey;
    kSeen[nPositionKey]        = AOC_TRUE;

    nNextX = X + nXDelta;
    nNextY = Y + nYDelta;

    if ((nNextX >= -1) && (nNextX <= (int)nWidth) && (nNextY >= -1) && (nNextY <= (int)nHeight))
    {
        const size_t nPositionKey = TO_GRID_PERIMETER(nNextX, nNextY, nWidth);

        if ((AOC_TRUE == kPerimeterMap[nPositionKey]) && (AOC_FALSE == kSeen[nPositionKey]))
        {
            followSide(kPerimeter, nPerimeterSize, nNextX, nNextY, nXDelta, nYDelta, nWidth, nHeight, kPerimeterMap, kSeen, kSeenList, pSeenCount);
        }
    }

    nNextX = X - nXDelta;
    nNextY = Y - nYDelta;

    if ((nNextX >= -1) && (nNextX <= (int)nWidth) && (nNextY >= -1) && (nNextY <= (int)nHeight))
    {
        const size_t nPositionKey = TO_GRID_PERIMETER(nNextX, nNextY, nWidth);

        if ((AOC_TRUE == kPerimeterMap[nPositionKey]) && (AOC_FALSE == kSeen[nPositionKey]))
        {
            followSide(kPerimeter, nPerimeterSize, nNextX, nNextY, nXDelta, nYDelta, nWidth, nHeight, kPerimeterMap, kSeen, kSeenList, pSeenCount);
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kFarmBuffer;
        char**                      kFarm;

        char*                       kExplodedFarmBuffer;
        char**                      kExplodedFarm;

        FarmLocationType*           kPositionQueueBuffer;

        size_t                      nHeight;
        size_t                      nWidth;
        size_t                      nExplodedHeight;
        size_t                      nExplodedWidth;

        unsigned*                   kSeenCropOnFarm;
        unsigned*                   kSeenCropOnExplodedFarm;
        unsigned*                   kSeenCropOnExplodedFarmPerimeter;
        unsigned*                   kSeenCropOnExplodedFarmSide;
        size_t*                     kSeenCropOnExplodedFarmList;
        FarmLocationType*           kCropPerimeterOnExplodedFarm;
        unsigned*                   kCropPerimeterOnExplodedFarmMap;
        FarmLocationType*           kCropPerimeterOnExplodedFarmUnique;

        size_t                      nY;

        uint64_t                    nPartOneResult = 0;
        uint64_t                    nPartTwoResult = 0;
 
        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kFarmBuffer, &kFarm, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /* Exploded Farm Dimensions for Part Two */
        nExplodedHeight = nHeight * 3;
        nExplodedWidth  = nWidth  * 3;

        /* Allocate the Required Memory */
        kPositionQueueBuffer               = (FarmLocationType*)malloc(nExplodedHeight * nExplodedWidth * sizeof(FarmLocationType));
        kSeenCropOnFarm                    = (unsigned*)calloc(nHeight * nWidth, sizeof(unsigned));
        kSeenCropOnExplodedFarm            = (unsigned*)calloc(nExplodedHeight * nExplodedWidth, sizeof(unsigned));
        kSeenCropOnExplodedFarmPerimeter   = (unsigned*)calloc((nExplodedHeight+2) * (nExplodedWidth+2), sizeof(unsigned));
        kSeenCropOnExplodedFarmSide        = (unsigned*)calloc((nExplodedHeight+2) * (nExplodedWidth+2), sizeof(unsigned));
        kSeenCropOnExplodedFarmList        = (size_t*)malloc((nExplodedHeight+2) * (nExplodedWidth+2) * sizeof(size_t));
        kCropPerimeterOnExplodedFarm       = (FarmLocationType*)malloc(nExplodedHeight * nExplodedWidth * sizeof(FarmLocationType));
        kCropPerimeterOnExplodedFarmMap    = (unsigned*)calloc((nExplodedHeight+2) * (nExplodedWidth+2), sizeof(unsigned));
        kCropPerimeterOnExplodedFarmUnique = (FarmLocationType*)malloc(nExplodedHeight * nExplodedWidth * sizeof(FarmLocationType));
        kExplodedFarmBuffer                = (char*)malloc(nExplodedHeight * nExplodedWidth * sizeof(char));
        kExplodedFarm                      = (char**)malloc(nExplodedHeight * sizeof(char*));

        /* Explode the Farm whilst also performing Part One */
        for (nY = 0; nY < nHeight; ++nY)
        {
            size_t nX;

            /* Part Two - Explode the Farm */
            kExplodedFarm[(nY * 3) + 0] = &kExplodedFarmBuffer[TO_GRID(0, (nY * 3) + 0, nWidth * 3)];
            kExplodedFarm[(nY * 3) + 1] = &kExplodedFarmBuffer[TO_GRID(0, (nY * 3) + 0, nWidth * 3)];
            kExplodedFarm[(nY * 3) + 2] = &kExplodedFarmBuffer[TO_GRID(0, (nY * 3) + 0, nWidth * 3)];

            for (nX = 0; nX < nWidth; ++nX)
            {
                const size_t nCropKey = TO_GRID(nX, nY, nWidth);
                const size_t nExplodedX = nX * 3;
                const size_t nExplodedY = nY * 3;

                /* Part One - Calculate the Area and Perimeter */
                if (AOC_FALSE == kSeenCropOnFarm[nCropKey])
                {
                    uint64_t nArea;
                    uint64_t nPerimeter;
                    calculateCropRegion((const char**)kFarm, kFarm[nY][nX], nX, nY, nWidth, nHeight, kSeenCropOnFarm, kPositionQueueBuffer, &nArea, &nPerimeter, NULL);
                    nPartOneResult += (nArea * nPerimeter);
                }

                /* Part Two - Explode the Farm */
                kExplodedFarm[(nY * 3) + 0][nExplodedX + 0] = kFarm[nY][nX];
                kExplodedFarm[(nY * 3) + 0][nExplodedX + 1] = kFarm[nY][nX];
                kExplodedFarm[(nY * 3) + 0][nExplodedX + 2] = kFarm[nY][nX];

                kExplodedFarm[(nY * 3) + 1][nExplodedX + 0] = kFarm[nY][nX];
                kExplodedFarm[(nY * 3) + 1][nExplodedX + 1] = kFarm[nY][nX];
                kExplodedFarm[(nY * 3) + 1][nExplodedX + 2] = kFarm[nY][nX];

                kExplodedFarm[(nY * 3) + 2][nExplodedX + 0] = kFarm[nY][nX];
                kExplodedFarm[(nY * 3) + 2][nExplodedX + 1] = kFarm[nY][nX];
                kExplodedFarm[(nY * 3) + 2][nExplodedX + 2] = kFarm[nY][nX];
            }
        }

        for (nY = 0; nY < nExplodedHeight; ++nY)
        {
            size_t nX;
            for (nX = 0; nX < nExplodedWidth; ++nX)
            {
                const size_t nCropKey = TO_GRID(nX, nY, nExplodedWidth);

                /* Part One - Calculate the Perimeter */
                if (AOC_FALSE == kSeenCropOnExplodedFarm[nCropKey])
                {
                    size_t   nSides                  = 0;
                    uint64_t nArea;
                    uint64_t nPerimeter;
                    uint64_t nPerimeterUnique        = 0;
                    size_t   i;

                    /* Calculate the Perimieter Positions for the Exploded Farm */
                    calculateCropRegion((const char**)kExplodedFarm, kExplodedFarm[nY][nX], nX, nY, nExplodedWidth, nExplodedHeight, kSeenCropOnExplodedFarm, kPositionQueueBuffer, &nArea, &nPerimeter, kCropPerimeterOnExplodedFarm);

                    /* Initialise the Seen Key Map */
                    for (i = 0; i < nPerimeter; ++i)
                    {
                        const size_t nPerimeterKey = TO_GRID_PERIMETER(kCropPerimeterOnExplodedFarm[i].nX, kCropPerimeterOnExplodedFarm[i].nY, nExplodedWidth);
                        if (AOC_FALSE == kCropPerimeterOnExplodedFarmMap[nPerimeterKey])
                        {
                            kCropPerimeterOnExplodedFarmMap[nPerimeterKey] = AOC_TRUE;
                            kCropPerimeterOnExplodedFarmUnique[nPerimeterUnique  ].nX = kCropPerimeterOnExplodedFarm[i].nX;
                            kCropPerimeterOnExplodedFarmUnique[nPerimeterUnique++].nY = kCropPerimeterOnExplodedFarm[i].nY;
                        }
                    }

                    /* Count the Sides */
                    for (i = 0; i < nPerimeterUnique; ++i)
                    {
                        const size_t nPerimeterKey = TO_GRID_PERIMETER(kCropPerimeterOnExplodedFarmUnique[i].nX, kCropPerimeterOnExplodedFarmUnique[i].nY, nExplodedWidth);
                        size_t nHorizontalSideCount = 0;
                        size_t nVerticalSideCount   = 0;
                        size_t j                    = 0;

                        if (AOC_TRUE == kSeenCropOnExplodedFarmPerimeter[nPerimeterKey])
                        {
                            continue;
                        }

                        followSide(kCropPerimeterOnExplodedFarm,
                                   nPerimeter,
                                   kCropPerimeterOnExplodedFarmUnique[i].nX, kCropPerimeterOnExplodedFarmUnique[i].nY,
                                   1,  0,
                                   nExplodedWidth, nExplodedHeight,
                                   kCropPerimeterOnExplodedFarmMap,
                                   kSeenCropOnExplodedFarmSide,
                                   kSeenCropOnExplodedFarmList,
                                   &nHorizontalSideCount);

                        for (j = 0; j < nHorizontalSideCount; ++j)
                        {
                            const size_t nKey = kSeenCropOnExplodedFarmList[j];
                            assert(kSeenCropOnExplodedFarmSide[nKey]);
                            kSeenCropOnExplodedFarmSide[nKey]      = AOC_FALSE;
                            kSeenCropOnExplodedFarmPerimeter[nKey] = AOC_TRUE;
                        }

                        followSide(kCropPerimeterOnExplodedFarm,
                                   nPerimeter,
                                   kCropPerimeterOnExplodedFarmUnique[i].nX, kCropPerimeterOnExplodedFarmUnique[i].nY,
                                   0,  1,
                                   nExplodedWidth, nExplodedHeight,
                                   kCropPerimeterOnExplodedFarmMap,
                                   kSeenCropOnExplodedFarmSide,
                                   kSeenCropOnExplodedFarmList,
                                   &nVerticalSideCount);

                        for (j = 0; j < nVerticalSideCount; ++j)
                        {
                            const size_t nKey = kSeenCropOnExplodedFarmList[j];
                            assert(kSeenCropOnExplodedFarmSide[nKey]);
                            kSeenCropOnExplodedFarmSide[nKey]      = AOC_FALSE;
                            kSeenCropOnExplodedFarmPerimeter[nKey] = AOC_TRUE;
                        }

                        if (nHorizontalSideCount > 1)
                        {
                            ++nSides;
                        }
                        if (nVerticalSideCount > 1)
                        {
                            ++nSides;
                        }
                    }

                    /* Clean Up the Perimieter Buffer*/
                    for (i = 0 ; i < nPerimeterUnique; ++i)
                    {
                        const size_t nPerimeterKey = TO_GRID_PERIMETER(kCropPerimeterOnExplodedFarmUnique[i].nX, kCropPerimeterOnExplodedFarmUnique[i].nY, nExplodedWidth);
                        assert(kCropPerimeterOnExplodedFarmMap[nPerimeterKey]);
                        kCropPerimeterOnExplodedFarmMap[nPerimeterKey]  = AOC_FALSE;
                        kSeenCropOnExplodedFarmPerimeter[nPerimeterKey] = AOC_FALSE;
                    }

                    nPartTwoResult += nSides * (nArea / (3 * 3));
                }
            }
        }

        printf("Part 1: %" PRIu64 "\n", nPartOneResult);
        printf("Part 2: %" PRIu64 "\n", nPartTwoResult);
 
        /* Free any Allocated Memory */
        free(kFarm);
        free(kFarmBuffer);
        free(kPositionQueueBuffer);
        free(kSeenCropOnFarm);
        free(kSeenCropOnExplodedFarm);
        free(kSeenCropOnExplodedFarmPerimeter);
        free(kSeenCropOnExplodedFarmSide);
        free(kSeenCropOnExplodedFarmList);
        free(kExplodedFarm);
        free(kExplodedFarmBuffer);
        free(kCropPerimeterOnExplodedFarm);
        free(kCropPerimeterOnExplodedFarmMap);
        free(kCropPerimeterOnExplodedFarmUnique);
    }
 
    return 0;
}