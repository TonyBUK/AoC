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

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct PositionType
{
    int nX;
    int nY;
} PositionType;

typedef struct QueueEntryType
{
    PositionType  kPosition;
    size_t        nPathLength;
    PositionType* pkPath;
} QueueEntryType;

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

void appendToQueue(QueueEntryType* pkQueue, size_t* pnQueueLength, const PositionType* kPosition, const size_t nWidth, const size_t nHeight, const size_t nPathLength, const PositionType* pkPath)
{
    QueueEntryType* pkEntry = &pkQueue[(*pnQueueLength)++];
    pkEntry->kPosition = *kPosition;
    if (!pkEntry->pkPath)
    {
        pkEntry->pkPath = (PositionType*)malloc(nWidth * nHeight * sizeof(PositionType));
    }

    if (nPathLength > 0)
    {
        memcpy(pkEntry->pkPath, pkPath, nPathLength * sizeof(PositionType));
    }
    pkEntry->nPathLength         = nPathLength + 1;
    pkEntry->pkPath[nPathLength] = *kPosition;
}

void manageDoubleQueue(QueueEntryType** ppkPrimaryQueue, QueueEntryType** ppkSecondaryQueue, size_t* pnPrimaryQueueLength, size_t* pnSecondaryQueueLength)
{
    if (0 == *pnPrimaryQueueLength)
    {
        QueueEntryType* pkTemp  = *ppkPrimaryQueue;
        *ppkPrimaryQueue        = *ppkSecondaryQueue;
        *ppkSecondaryQueue      = pkTemp;

        *pnPrimaryQueueLength   = *pnSecondaryQueueLength;
        *pnSecondaryQueueLength = 0;
    }
}

void findShortestRoute(const unsigned* kWalls,
                       const size_t    nStartX, const size_t nStartY,
                       const size_t    nEndX,   const size_t nEndY,
                       const size_t    nWidth,  const size_t nHeight,
                       QueueEntryType* kPrimaryQueue,
                       QueueEntryType* kSecondaryQueue,
                       PositionType*   kShortestRoute,
                       size_t*         nShortestRouteLength)
{
    const PositionType kNeighbours[]         = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
    size_t             nLowestSteps          = SIZE_MAX;
    size_t*            kSeen                 = (size_t*)malloc(nWidth * nHeight * sizeof(size_t));

    size_t             nPrimaryQueueLength   = 0;
    size_t             nSecondaryQueueLength = 0;

    PositionType       kPosition             = {nStartX, nStartY};

    memset(kSeen, 0xFF, nWidth * nHeight * sizeof(size_t));
    appendToQueue(kPrimaryQueue, &nPrimaryQueueLength, &kPosition, nWidth, nHeight, 0, NULL);

    /* BFS Search to find the Position... */
    while (nPrimaryQueueLength > 0)
    {
        /* Get the Next Possible Move */
        QueueEntryType* pkEntry     = &kPrimaryQueue[--nPrimaryQueueLength];
        size_t          nPathLength = pkEntry->nPathLength;
        PositionType*   pkPath      = pkEntry->pkPath;
        size_t          nGridKey    = TO_GRID(pkEntry->kPosition.nX, pkEntry->kPosition.nY, nWidth);
        size_t          nNeighbour;

        /* If it's already the same or worse than the current known best move... */
        if (pkEntry->nPathLength >= nLowestSteps)
        {
            manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
            continue;
        }

        /* If this is the end position, test if we've encountered our best one yet */
        if (pkEntry->kPosition.nX == nEndX && pkEntry->kPosition.nY == nEndY)
        {
            if (pkEntry->nPathLength < nLowestSteps)
            {
                nLowestSteps = pkEntry->nPathLength;
                memcpy(kShortestRoute, pkEntry->pkPath, nWidth * nHeight * sizeof(PositionType));
            }
            else if (pkEntry->nPathLength == nLowestSteps)
            {
                /* Shouldn't Occur for this Puzzle Input */
                assert(0);
            }

            manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
            continue;
        }

        /* If we've already seen this position and at a lower step count... */
        if (kSeen[nGridKey] != SIZE_MAX)
        {
            if (kSeen[nGridKey] <= pkEntry->nPathLength)
            {
                manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
                continue;
            }
        }
        kSeen[nGridKey] = pkEntry->nPathLength;

        /* For each Neighbour */
        for (nNeighbour = 0; nNeighbour < (sizeof(kNeighbours)/sizeof(kNeighbours[0])); ++nNeighbour)
        {
            PositionType kNeighbourPosition = {pkEntry->kPosition.nX + kNeighbours[nNeighbour].nX, pkEntry->kPosition.nY + kNeighbours[nNeighbour].nY};

            /* In the Grid? */
            if (kNeighbourPosition.nX < 0 || kNeighbourPosition.nX >= nWidth || kNeighbourPosition.nY < 0 || kNeighbourPosition.nY >= nHeight)
            {
                continue;
            }

            /* Collided with wall? */
            if (AOC_FALSE == kWalls[TO_GRID(kNeighbourPosition.nX, kNeighbourPosition.nY, nWidth)])
            {
                appendToQueue(kSecondaryQueue, &nSecondaryQueueLength, &kNeighbourPosition, nWidth, nHeight, nPathLength, pkPath);
            }
        }

        manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, &nPrimaryQueueLength, &nSecondaryQueueLength);
    }

    *nShortestRouteLength = nLowestSteps;

    free(kSeen);
}

void updateDistances(const PositionType* kShortestRoute, const size_t nShortestRouteLength, size_t* kShortestDistances, const size_t nWidth, const size_t nHeight)
{
    size_t nPosition;
    for (nPosition = 0; nPosition < nShortestRouteLength; ++nPosition)
    {
        const size_t nDistanceFromEnd = nShortestRouteLength - nPosition - 1;
        const size_t nGridKey         = TO_GRID(kShortestRoute[nPosition].nX, kShortestRoute[nPosition].nY, nWidth);
        if (kShortestDistances[nGridKey] != SIZE_MAX)
        {
            assert(kShortestDistances[nGridKey] == nDistanceFromEnd);
        }
        kShortestDistances[nGridKey]  = nDistanceFromEnd;
    }
}

size_t manhattanDistance(const PositionType* kPosition1, const PositionType* kPosition2)
{
    return abs(kPosition1->nX - kPosition2->nX) + abs(kPosition1->nY - kPosition2->nY);
}

uint64_t calculateTimeSave(const PositionType* kShortestPath,
                           const size_t        nShortestPathLength,
                           size_t*             kShortestDistances,
                           const size_t        nMaxDistance,
                           const size_t        nTarget,
                           const unsigned*     kWalls,
                           const size_t        nEndX,
                           const size_t        nEndY,
                           const size_t        nWidth,
                           const size_t        nHeight,
                           unsigned*           kUnusableRoutes,
                           QueueEntryType*     kPrimaryQueue,
                           QueueEntryType*     kSecondaryQueue)
{
    uint64_t      nSavedTime = 0;
    size_t        nPosition;
    PositionType* kShorstPathDelta = NULL;

    /* For Each Position (except the last )*/
    for (nPosition = 0; nPosition < (nShortestPathLength - 1); ++nPosition)
    {
        /*
         * We essentially have a Diamond Search Pattern
         * Note: Constrain this to the inner part of the grid.
         */
        size_t nNeighbourY;
        for (nNeighbourY = (size_t)MAX(1, kShortestPath[nPosition].nY - (int)nMaxDistance); nNeighbourY <= (size_t)MIN(nHeight - 2, kShortestPath[nPosition].nY + nMaxDistance); ++nNeighbourY)
        {
            /* Compute X based on Y to keep the Diamond Shape */
            const size_t nXDistance = nMaxDistance - abs(kShortestPath[nPosition].nY - (int)nNeighbourY);
            size_t       nNeighbourX;
            for (nNeighbourX = (size_t)MAX(1, kShortestPath[nPosition].nX - (int)nXDistance); nNeighbourX <= (size_t)MIN(nWidth - 2, kShortestPath[nPosition].nX + nXDistance); ++nNeighbourX)
            {
                const PositionType kNeighbourPosition = {nNeighbourX, nNeighbourY};
                const size_t       nGridKey           = TO_GRID(kNeighbourPosition.nX, kNeighbourPosition.nY, nWidth);

                /* Is this a known position? */
                if (kShortestDistances[nGridKey] != SIZE_MAX)
                {
                    /* Coarsley Reject the Value before we apply Manhattan */
                    const size_t nPositionGridKey = TO_GRID(kShortestPath[nPosition].nX, kShortestPath[nPosition].nY, nWidth);
                    assert(kShortestDistances[nPositionGridKey] != SIZE_MAX);
                    int nDelta = (int)kShortestDistances[nPositionGridKey] - (int)kShortestDistances[nGridKey];
                    if (nDelta > (int)(nTarget + 1))
                    {
                        /* Apply Manhattan to the Delta and Retry */
                        nDelta -= (int)manhattanDistance(&kShortestPath[nPosition], &kNeighbourPosition);
                        nSavedTime += nDelta >= nTarget;
                    }
                }
                /* Is this a position that has no valid route? */
                else if (kUnusableRoutes[nGridKey] == AOC_FALSE)
                {
                    /* Is this a Wall? */
                    if (kWalls[nGridKey] == AOC_FALSE)
                    {
                        size_t nShortestPathDeltaLength;

                        if (kShorstPathDelta == NULL)
                        {
                            kShorstPathDelta = (PositionType*)malloc(nWidth * nHeight * sizeof(PositionType));
                        }

                        /*
                         * This is an unknown position that wasn't in the fastest route, most likely a closed route.
                         * Find the shortest distance from the Neighbour Position.
                         */
                        findShortestRoute(kWalls, nNeighbourX, nNeighbourY, nEndX, nEndY, nWidth, nHeight, kPrimaryQueue, kSecondaryQueue, kShorstPathDelta, &nShortestPathDeltaLength);

                        if (SIZE_MAX == nShortestPathDeltaLength)
                        {
                            kUnusableRoutes[nGridKey] = AOC_TRUE;
                            continue;
                        }

                        /* Update the Distances */
                        updateDistances(kShorstPathDelta, nShortestPathDeltaLength, kShortestDistances, nWidth, nHeight);

                        /* Coarsley Reject the Value before we apply Manhattan */
                        const size_t nPositionGridKey = TO_GRID(kShortestPath[nPosition].nX, kShortestPath[nPosition].nY, nWidth);
                        assert(kShortestDistances[nPositionGridKey] != SIZE_MAX);
                        int nDelta = (int)kShortestDistances[nPositionGridKey] - (int)kShortestDistances[nGridKey];
                        if (nDelta > (int)(nTarget + 1))
                        {
                            /* Apply Manhattan to the Delta and Retry */
                            nDelta -= (int)manhattanDistance(&kShortestPath[nPosition], &kNeighbourPosition);
                            nSavedTime += nDelta >= nTarget;
                        }
                    }
                }
            }
        }
    }

    if (kShorstPathDelta)
    {
        free(kShorstPathDelta);
    }

    return nSavedTime;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kRaceCourseBuffer;
        char**                      kRaceCourse;

        size_t                      nHeight;
        size_t                      nWidth;

        unsigned*                   kWalls;
        PositionType*               kShortestRoute;
        size_t                      nShortestRouteLength;
        size_t*                     kShortestDistances;
        unsigned*                   kUnusableRoutes;
        QueueEntryType*             kPrimaryQueue;
        QueueEntryType*             kSecondaryQueue;

        size_t                      nStartX;
        size_t                      nStartY;
        size_t                      nEndX;
        size_t                      nEndY;

        size_t                      i;
        size_t                      nY;
 
        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kRaceCourseBuffer, &kRaceCourse, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /* Allocate the Required Memory */
        kWalls             = (unsigned*)calloc(nHeight * nWidth, sizeof(unsigned));
        kShortestRoute     = (PositionType*)malloc(nHeight * nWidth * sizeof(PositionType));
        kShortestDistances = (size_t*)malloc(nHeight * nWidth * sizeof(size_t));
        kUnusableRoutes    = (unsigned*)calloc(nHeight * nWidth, sizeof(unsigned));
        kPrimaryQueue      = (QueueEntryType*)calloc(nHeight * nWidth, sizeof(QueueEntryType));
        kSecondaryQueue    = (QueueEntryType*)calloc(nHeight * nWidth, sizeof(QueueEntryType));

        memset(kShortestDistances, 0xFF, nHeight * nWidth * sizeof(size_t));

        /* Search for Walls/Start/End */
        for (nY = 0; nY < nHeight; ++nY)
        {
            size_t nX;
            for (nX = 0; nX < nWidth; ++nX)
            {
                const char c = kRaceCourse[nY][nX];
                if (c == '#')
                {
                    kWalls[TO_GRID(nX, nY, nWidth)] = AOC_TRUE;
                }
                else if (c == 'S')
                {
                    nStartX = nX;
                    nStartY = nY;
                }
                else if (c == 'E')
                {
                    nEndX = nX;
                    nEndY = nY;
                }
            }
        }

        /* Calculate the Shortest Route from the Start */
        findShortestRoute(kWalls, nStartX, nStartY, nEndX, nEndY, nWidth, nHeight, kPrimaryQueue, kSecondaryQueue, kShortestRoute, &nShortestRouteLength);

        /* Update the Distances */
        updateDistances(kShortestRoute, nShortestRouteLength, kShortestDistances, nWidth, nHeight);

        printf("Part 1: %" PRIu64 "\n", calculateTimeSave(kShortestRoute, nShortestRouteLength, kShortestDistances,  2, 100, kWalls, nEndX, nEndY, nWidth, nHeight, kUnusableRoutes, kPrimaryQueue, kSecondaryQueue));
        printf("Part 2: %" PRIu64 "\n", calculateTimeSave(kShortestRoute, nShortestRouteLength, kShortestDistances, 20, 100, kWalls, nEndX, nEndY, nWidth, nHeight, kUnusableRoutes, kPrimaryQueue, kSecondaryQueue));
 
        /* Free any Allocated Memory */
        for (i = 0; i < (nWidth * nHeight); ++i)
        {
            unsigned bAny = AOC_FALSE;

            if (kPrimaryQueue[i].pkPath)
            {
                free(kPrimaryQueue[i].pkPath);
                bAny = AOC_TRUE;
            }

            if (kSecondaryQueue[i].pkPath)
            {
                free(kSecondaryQueue[i].pkPath);
                bAny = AOC_TRUE;
            }

            if (AOC_FALSE == bAny)
            {
                break;
            }
        }

        free(kRaceCourse);
        free(kRaceCourseBuffer);
        free(kWalls);
        free(kShortestRoute);
        free(kShortestDistances);
        free(kUnusableRoutes);
        free(kPrimaryQueue);
        free(kSecondaryQueue);
    }
 
    return 0;
}