#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#define WIDTH  (71)
#define HEIGHT (71)

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define TO_GRID(x, y) ((y) * WIDTH + (x))
#define MIN(a, b)     ((a) < (b) ? (a) : (b))

typedef struct PositionType
{
    int nX;
    int nY;
} PositionType;

typedef struct QueueType
{
    size_t nSteps;
    PositionType kPosition;
} QueueType;

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

void manageDoubleQueue(QueueType** pkUsedQueuePrimary, QueueType** pkUsedQueueSecondary, QueueType* pkQueuePrimary, QueueType* pkQueueSecondary, size_t* pnPrimaryQueueLength, size_t* pnSecondaryQueueLength)
{
    if (0 == *pnPrimaryQueueLength)
    {
        if (*pkUsedQueuePrimary == pkQueuePrimary)
        {
            *pkUsedQueuePrimary   = pkQueueSecondary;
            *pkUsedQueueSecondary = pkQueuePrimary;
        }
        else
        {
            *pkUsedQueuePrimary   = pkQueuePrimary;
            *pkUsedQueueSecondary = pkQueueSecondary;
        }
        *pnPrimaryQueueLength   = *pnSecondaryQueueLength;
        *pnSecondaryQueueLength = 0;
    }
}

unsigned findShortestRoute(const size_t nMaxSteps, const size_t* kBytesGrid, QueueType* kPrimaryQueue, QueueType* kSecondaryQueue, size_t* kSeen, size_t* kSeenArray, size_t* nFoundRouteSteps, unsigned bAnyPath)
{
    const int          NEIGHBOURS[4][2] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };
    const PositionType kEnd             = { WIDTH - 1, HEIGHT - 1};

    size_t     nPrimaryQueueLength      = 1;
    size_t     nSecondaryQueueLength    = 0;

    QueueType* pkUsedQueuePrimary   = kPrimaryQueue;
    QueueType* pkUsedQueueSecondary = kSecondaryQueue;

    size_t     nSeenCount               = 0;
    size_t     nLowestSteps             = (size_t)-1;

    size_t     nSeen;

    pkUsedQueuePrimary[0].nSteps       = 0;
    pkUsedQueuePrimary[0].kPosition.nX = 0;
    pkUsedQueuePrimary[0].kPosition.nY = 0;

    /* BFS Search to find the Position... */
    while (nPrimaryQueueLength)
    {
        const size_t       nSteps    = pkUsedQueuePrimary[--nPrimaryQueueLength].nSteps;
        const PositionType kPosition = pkUsedQueuePrimary[nPrimaryQueueLength].kPosition;
        const size_t       nGrid     = TO_GRID(kPosition.nX, kPosition.nY);

        size_t             nNeighbour;

        if (nSteps >= nLowestSteps)
        {
            manageDoubleQueue(&pkUsedQueuePrimary, &pkUsedQueueSecondary, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
            continue;
        }

        /* If this is the end position, test if we've encountered our best one yet */
        if ((kPosition.nX == kEnd.nX) && (kPosition.nY == kEnd.nY))
        {
            nLowestSteps = MIN(nLowestSteps, nSteps);
            if (bAnyPath)
            {
                break;
            }
            manageDoubleQueue(&pkUsedQueuePrimary, &pkUsedQueueSecondary, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
            continue;
        }

        /* If we've already seen this position and at a lower step count... */
        if (kSeen[nGrid])
        {
            if (nSteps >= kSeen[nGrid])
            {
                manageDoubleQueue(&pkUsedQueuePrimary, &pkUsedQueueSecondary, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
                continue;
            }
        }
        else
        {
            kSeenArray[nSeenCount++] = nGrid;
        }
        kSeen[nGrid] = nSteps;

        /* For each Neighbour */
        for (nNeighbour = 0; nNeighbour < (sizeof(NEIGHBOURS)/sizeof(NEIGHBOURS[0])); ++nNeighbour)
        {
            const int nX = kPosition.nX + NEIGHBOURS[nNeighbour][0];
            const int nY = kPosition.nY + NEIGHBOURS[nNeighbour][1];
            size_t    nNeighbourGrid;

            /* In the Grid? */
            if ((nX < 0) || (nX >= WIDTH) || (nY < 0) || (nY >= HEIGHT))
            {
                continue;
            }

            nNeighbourGrid = TO_GRID(nX, nY);

            /* Collided with bytes? */
            if (kBytesGrid[nNeighbourGrid] >= nMaxSteps)
            {
                pkUsedQueueSecondary[nSecondaryQueueLength].nSteps         = nSteps + 1;
                pkUsedQueueSecondary[nSecondaryQueueLength].kPosition.nX   = nX;
                pkUsedQueueSecondary[nSecondaryQueueLength++].kPosition.nY = nY;
            }
        }

        manageDoubleQueue(&pkUsedQueuePrimary, &pkUsedQueueSecondary, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
    }

    for (nSeen = 0; nSeen < nSeenCount; ++nSeen)
    {
        kSeen[kSeenArray[nSeen]] = 0;
    }

    /* Return the Steps/Found State */
    *nFoundRouteSteps = nLowestSteps;
    return (nLowestSteps != (size_t)-1) ? AOC_TRUE : AOC_FALSE;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kBuffer;
        char**                      kBytesRaw;
        size_t                      nByteCount;
        size_t                      i;

        PositionType*               kBytes;
        size_t*                     kBytesGrid;
        QueueType*                  kQueuePrimary;
        QueueType*                  kQueueSecondary;

        size_t*                     kSeen;
        size_t*                     kSeenArray;

        size_t                      nPartOneSteps;

        size_t                      nMin;
        size_t                      nMax;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kBytesRaw, &nByteCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Requiremed Memory */
        kBytes          = (PositionType*)malloc(nByteCount * sizeof(PositionType));
        kBytesGrid      = (size_t*)malloc(WIDTH * HEIGHT * sizeof(size_t));
        kQueuePrimary   = (QueueType*)malloc(WIDTH * HEIGHT * sizeof(QueueType));
        kQueueSecondary = (QueueType*)malloc(WIDTH * HEIGHT * sizeof(QueueType));
        kSeen           = (size_t*)calloc(WIDTH * HEIGHT, sizeof(size_t));
        kSeenArray      = (size_t*)malloc(WIDTH * HEIGHT * sizeof(size_t));
        memset(kBytesGrid, 0xFF, WIDTH * HEIGHT * sizeof(size_t));

        for (i = 0; i < nByteCount; ++i)
        {
            size_t nGrid;

            kBytes[i].nX = (size_t)strtoul(kBytesRaw[i], NULL, 10);
            kBytes[i].nY = (size_t)strtoul(strstr(kBytesRaw[i], ",") + 1, NULL, 10);
            nGrid        = TO_GRID(kBytes[i].nX, kBytes[i].nY);

            /*
             * Store the Counter Value representing when the Byte will fall within the grid.
             *
             * This neatly prevents having to redeclare a new grid each time when performing
             * the part two search.
             *
             * Nodes of Max Int are paths, and nodes >= 0 are walls, where the value represents the
             * time when the wall will be active.
             *
             * This means we can reduce a valid path to a simple < comparison of the grid value.
             */
            kBytesGrid[nGrid] = i;
        }

        /* Part 1: Find the Shortest Route for 1024 falled bytes */
        findShortestRoute(1024, kBytesGrid, kQueuePrimary, kQueueSecondary, kSeen, kSeenArray, &nPartOneSteps, AOC_FALSE);
        printf("Part 1: %zu\n", nPartOneSteps);

        /* Part 2: Find the first maze that can't be solved using a Binrary Chop Search */
        nMin = 1025;
        nMax = nByteCount;

        while (nMin < nMax)
        {
            /* Calculate the Mid Point */
            const size_t nMid = nMin + ((nMax - nMin) / 2);

            /* In this scenario, Min/Max are one apart and we know Max must be unsolved */
            if (nMin == nMid)
            {
                break;
            }

            if (AOC_FALSE == findShortestRoute(nMid, kBytesGrid, kQueuePrimary, kQueueSecondary, kSeen, kSeenArray, &nPartOneSteps, AOC_TRUE))
            {
                nMax = nMid;
            }
            else
            {
                nMin = nMid;
            }
        }

        printf("Part 2: %i,%i\n", kBytes[nMax-1].nX, kBytes[nMax-1].nY);

        /* Cleanup */
        free(kBytesRaw);
        free(kBuffer);
        free(kBytes);
        free(kBytesGrid);
        free(kQueuePrimary);
        free(kQueueSecondary);
        free(kSeen);
        free(kSeenArray);
    }
 
    return 0;
}