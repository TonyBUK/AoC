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

#if 0
void findNeighbourNodes(SNodeType* kNode, const char** kMaze, SNodeType* kNodes, size_t* kSeen, size_t* kSeenArray, size_t* kNeighbourIntersectionSeen, size_t* kNeighbourIntersectionSeenArray, NeighbourQueueType* kQueue, const size_t nWidth)
{
    size_t nQueueLength        = 1;
    size_t nSeenCount          = 0;
    size_t nNeighbourSeenCount = 0;
    size_t i;

    /* Initialise the Queue */
    kQueue[0].nScore                = 0;
    kQueue[0].kPositionAndDirection = kNode->kPositionAndDirection;

    while (nQueueLength)
    {
        const size_t                    nScore                = kQueue[--nQueueLength].nScore;
        const SPositionAndDirectionType kPositionAndDirection = kQueue[nQueueLength].kPositionAndDirection;
        const size_t                    nKey                  = TO_GRID_WITH_D(kPositionAndDirection.kPosition.nX, kPositionAndDirection.kPosition.nY, kPositionAndDirection.eDirection, nWidth);

        size_t                          nNeighbourDirection;

        for (nNeighbourDirection = EDirectionsType_North; nNeighbourDirection < EDirectionsType_Count; ++nNeighbourDirection)
        {
            int    nX;
            int    nY;
            size_t nNeighbourScore;
            size_t nNeighbourKey;

            /* Disallow Double Backing */
            if (abs((int)nNeighbourDirection - (int)kPositionAndDirection.eDirection) == 2)
            {
                continue;
            }

            nX = kPositionAndDirection.kPosition.nX + DIRECTIONS[nNeighbourDirection].nX;
            nY = kPositionAndDirection.kPosition.nY + DIRECTIONS[nNeighbourDirection].nY;

            /* Ignore if this is a wall */
            if ('#' == kMaze[nY][nX])
            {
                continue;
            }

            /* Calculate the New Score */
            nNeighbourScore = nScore + DIRECTION_SCORES[kPositionAndDirection.eDirection][nNeighbourDirection];

            /* If this isn't a key target (an intersection)... */
            nNeighbourKey = TO_GRID_WITH_D(nX, nY, nNeighbourDirection, nWidth);

            if (AOC_FALSE == kNodes[nNeighbourKey].bIntersection)
            {
                /* This position isn't any better than the last time we got here... */
                if (kSeen[nNeighbourKey])
                {
                    if (nNeighbourScore >= kSeen[nNeighbourKey])
                    {
                        continue;
                    }
                }

                /* Score the Distance to this Node, and append to the Queue */
                kQueue[nQueueLength].nScore                               = nNeighbourScore;
                kQueue[nQueueLength].kPositionAndDirection.kPosition.nX   = nX;
                kQueue[nQueueLength].kPositionAndDirection.kPosition.nY   = nY;
                kQueue[nQueueLength++].kPositionAndDirection.eDirection   = (EDirectionsType)nNeighbourDirection;
            }
            else /* Store the Key Target (intersection) */
            {
                if (0 == kNeighbourIntersectionSeen[nNeighbourKey])
                {
                    /* Add this Key Target to the seen Intersections */
                    kNeighbourIntersectionSeen[nNeighbourKey]              = nNeighbourScore;
                    kNeighbourIntersectionSeenArray[nNeighbourSeenCount++] = nNeighbourKey;
                }
                else
                {
                    kNeighbourIntersectionSeen[nNeighbourKey] = MIN(kNeighbourIntersectionSeen[nNeighbourKey], nNeighbourScore);   
                }
            }
        }

        if (kSeen[nKey])
        {
            kSeen[nKey] = MIN(kSeen[nKey], nScore);
        }
        else
        {
            kSeen[nKey]              = nScore;
            kSeenArray[nSeenCount++] = nKey;
        }
    }

    /* Store the Neighbour Nodes (if any exist) */
    if (nNeighbourSeenCount > 0)
    {
        kNode->nNeighbourCount = nNeighbourSeenCount;
        kNode->pNeighbours     = (SNeighbourNodeType*)malloc(nNeighbourSeenCount * sizeof(SNeighbourNodeType));
        for (i = 0; i < nNeighbourSeenCount; ++i)
        {
            kNode->pNeighbours[i].nScore = kNeighbourIntersectionSeen[kNeighbourIntersectionSeenArray[i]];
            kNode->pNeighbours[i].pNode  = &kNodes[kNeighbourIntersectionSeenArray[i]];

            kNeighbourIntersectionSeen[kNeighbourIntersectionSeenArray[i]] = 0;
        }
    }

    /* Cleanup */
    for (i = 0; i < nSeenCount; ++i)
    {
        kSeen[kSeenArray[i]] = 0;
    }
}

void findBestPaths(BestPathType* pkBestPaths, const size_t nStart, const size_t nEnd, const size_t nWidth, SNodeType* kNodes, const size_t nNodeCount, size_t* kSeen, size_t* kSeenArray, PathQueueType* kPathQueuePrimary, PathQueueType* kPathQueueSecondary)
{
    BestPathType*  pBestPath;
    PathQueueType* pkPathQueuePrimary;
    PathQueueType* pkPathQueueSecondary;
    size_t         nPrimaryPathQueueLength   = 1;
    size_t         nSecondaryPathQueueLength = 0;
    size_t         nSeenCount                = 0;
    size_t         nNode;

    /* Initialise the Best Paths Link List */
    pkBestPaths->nScore     = (size_t)-1;
    pkBestPaths->kNodes     = (size_t*)malloc(nNodeCount * sizeof(size_t));
    pkBestPaths->nNodeCount = 0;
    pkBestPaths->pNext      = NULL;
    pBestPath               = pkBestPaths;

    kPathQueuePrimary[0].kNodes                                    = (size_t*)malloc(nNodeCount * sizeof(size_t));
    kPathQueuePrimary[0].nScore                                    = 0;
    kPathQueuePrimary[0].kPositionAndDirection.kPosition.nX        = TO_X_FROM_GRID(nStart, nWidth);
    kPathQueuePrimary[0].kPositionAndDirection.kPosition.nY        = TO_Y_FROM_GRID(nStart, nWidth);
    kPathQueuePrimary[0].kPositionAndDirection.eDirection          = EDirectionsType_East;
    kPathQueuePrimary[0].kNodes[kPathQueuePrimary[0].nNodeCount++] = TO_GRID_WITH_D(kPathQueuePrimary[0].kPositionAndDirection.kPosition.nX, kPathQueuePrimary[0].kPositionAndDirection.kPosition.nY, kPathQueuePrimary[0].kPositionAndDirection.eDirection, nWidth);

    pkPathQueuePrimary                                             = kPathQueuePrimary;
    pkPathQueueSecondary                                           = kPathQueueSecondary;

    while(nPrimaryPathQueueLength)
    {
        const size_t                    nCurrentScore                = pkPathQueuePrimary[--nPrimaryPathQueueLength].nScore;
        const SPositionAndDirectionType kCurrentPositionAndDirection = pkPathQueuePrimary[nPrimaryPathQueueLength].kPositionAndDirection;
        const size_t                    nCurrentNodeCount            = pkPathQueuePrimary[nPrimaryPathQueueLength].nNodeCount;
        const size_t*                   kCurrentNodes                = pkPathQueuePrimary[nPrimaryPathQueueLength].kNodes;
        const size_t                    nCurrentGrid                 = TO_GRID(kCurrentPositionAndDirection.kPosition.nX, kCurrentPositionAndDirection.kPosition.nY, nWidth);
        const size_t                    nCurrentGridWithDirection    = TO_GRID_WITH_D(kCurrentPositionAndDirection.kPosition.nX, kCurrentPositionAndDirection.kPosition.nY, kCurrentPositionAndDirection.eDirection, nWidth);

        if (nCurrentGrid == nEnd)
        {
            if (nCurrentScore <= pkBestPaths->nScore)
            {
                if (nCurrentScore < pkBestPaths->nScore)
                {
                    pkBestPaths->nScore     = nCurrentScore;
                    pkBestPaths->nNodeCount = nCurrentNodeCount;
                    memcpy(pkBestPaths->kNodes, kCurrentNodes, nCurrentNodeCount * sizeof(size_t));
                    pBestPath               = pkBestPaths;
                }
                else
                {
                    pBestPath->nScore     = nCurrentScore;
                    pBestPath->nNodeCount = nCurrentNodeCount;
                    memcpy(pBestPath->kNodes, kCurrentNodes, nCurrentNodeCount * sizeof(size_t));
                }

                if (pBestPath->pNext == NULL)
                {
                    pBestPath->pNext      = (BestPathType*)malloc(sizeof(BestPathType));
                    pBestPath             = pBestPath->pNext;
                    pBestPath->nScore     = (size_t)-1;
                    pBestPath->kNodes     = (size_t*)malloc(nNodeCount * sizeof(size_t));
                    pBestPath->nNodeCount = 0;
                    pBestPath->pNext      = NULL;
                }
                else
                {
                    pBestPath             = pBestPath->pNext;
                }
            }

            manageDoubleQueue(&pkPathQueuePrimary, &pkPathQueueSecondary, kPathQueuePrimary, kPathQueueSecondary, &nPrimaryPathQueueLength, &nSecondaryPathQueueLength);
            continue;
        }

        if (kSeen[nCurrentGridWithDirection])
        {
            /*
                * Note: For Part Two, it's necessary to allow "equal" scores to be re-evaluated
                *       since the path taken to reach here may differ, and we need all unique paths.
                */
            if (nCurrentScore > kSeen[nCurrentGridWithDirection])
            {
                manageDoubleQueue(&pkPathQueuePrimary, &pkPathQueueSecondary, kPathQueuePrimary, kPathQueueSecondary, &nPrimaryPathQueueLength, &nSecondaryPathQueueLength);
                continue;
            }
        }

        for (nNode = 0; nNode < kNodes[nCurrentGridWithDirection].nNeighbourCount; ++nNode)
        {
            const size_t                    nNeighbourScore    = nCurrentScore + kNodes[nCurrentGridWithDirection].pNeighbours[nNode].nScore;
            const SPositionAndDirectionType kNeighbourPosition = kNodes[nCurrentGridWithDirection].pNeighbours[nNode].pNode->kPositionAndDirection;
            const size_t                    nNeighbourGrid     = TO_GRID_WITH_D(kNeighbourPosition.kPosition.nX, kNeighbourPosition.kPosition.nY, kNeighbourPosition.eDirection, nWidth);

            if (kSeen[nNeighbourGrid])
            {
                if (nNeighbourScore > kSeen[nNeighbourGrid])
                {
                    continue;
                }
            }

            pkPathQueueSecondary[nSecondaryPathQueueLength].nScore                                    = nNeighbourScore; 
            pkPathQueueSecondary[nSecondaryPathQueueLength].kPositionAndDirection                     = kNeighbourPosition;
            pkPathQueueSecondary[nSecondaryPathQueueLength].nNodeCount                                = nCurrentNodeCount + 1;
            if (NULL == pkPathQueueSecondary[nSecondaryPathQueueLength].kNodes)
            {
                pkPathQueueSecondary[nSecondaryPathQueueLength].kNodes = (size_t*)malloc(nNodeCount * sizeof(size_t));
            }
            memcpy(pkPathQueueSecondary[nSecondaryPathQueueLength].kNodes, kCurrentNodes, nCurrentNodeCount * sizeof(size_t));
            pkPathQueueSecondary[nSecondaryPathQueueLength++].kNodes[nCurrentNodeCount]               = nNeighbourGrid;
        }

        /* Store the Seen Score */
        if (kSeen[nCurrentGridWithDirection])
        {
            kSeen[nCurrentGridWithDirection] = MIN(kSeen[nCurrentGridWithDirection], nCurrentScore);
        }
        else
        {
            kSeen[nCurrentGridWithDirection] = nCurrentScore;
            kSeenArray[nSeenCount++]         = nCurrentGridWithDirection;
        }

        /*
            * Manage the Queue
            * Note: Queue swapping is a very coarse method of sorting in so far as we'll have
            *       some form of oldest to newest, but with no real heuristics.
            */
        manageDoubleQueue(&pkPathQueuePrimary, &pkPathQueueSecondary, kPathQueuePrimary, kPathQueueSecondary, &nPrimaryPathQueueLength, &nSecondaryPathQueueLength);
    }

    /* Clean-Up the Seen Nodes */
    for (nNode = 0; nNode < nSeenCount; ++nNode)
    {
        kSeen[kSeenArray[nNode]] = 0;
    }
}

void getAllPointsBetweenNodes(const size_t nNode1, const size_t nNode2, const size_t nTargetScore, const char** kMaze, SNodeType* kNodes, const size_t nNodeCount, size_t* kSeen, size_t* kSeenArray, unsigned* kBestPointsSeen, size_t* pnBestPointsSeenSize, PathQueueType* kPrimaryQueue, PathQueueType* kSecondaryQueue, const size_t nWidth)
{
    size_t       nBestPointsSeenSize = *pnBestPointsSeenSize;
    size_t       nSeenCount          = 0;
    size_t       nPrimaryQueueSize   = 1;
    size_t       nSeconaryQueueSize  = 0;
    size_t       nNode;

    if (NULL == kPrimaryQueue[0].kNodes)
    {
        kPrimaryQueue[0].kNodes = (size_t*)malloc(nNodeCount * sizeof(size_t));
    }
    kPrimaryQueue[0].nScore                = 0;
    kPrimaryQueue[0].kPositionAndDirection = kNodes[nNode1].kPositionAndDirection;
    kPrimaryQueue[0].nNodeCount            = 1;
    kPrimaryQueue[0].kNodes[0]             = nNode1;

    while(nPrimaryQueueSize)
    {
        const size_t                    nCurrentScore                = kPrimaryQueue[--nPrimaryQueueSize].nScore;
        const SPositionAndDirectionType kCurrentPositionAndDirection = kPrimaryQueue[nPrimaryQueueSize].kPositionAndDirection;
        const size_t                    nCurrentNodeCount            = kPrimaryQueue[nPrimaryQueueSize].nNodeCount;
        const size_t*                   kCurrentNodes                = kPrimaryQueue[nPrimaryQueueSize].kNodes;
        const size_t                    nCurrentNodeGrid             = TO_GRID_WITH_D(kCurrentPositionAndDirection.kPosition.nX, kCurrentPositionAndDirection.kPosition.nY, kCurrentPositionAndDirection.eDirection, nWidth);
        size_t                          nNeighbourDirection;

        /* If this is the end node, add the route. */
        if (nCurrentNodeGrid == nNode2)
        {
            /*
             * Note: Weirdly we can actually get here with a lower score, as the "shortest" path
             *       to the node may actually be through another node.  But if that's the case,
             *       it'll be found when comparing those nodes.
             */
            if (nCurrentScore == nTargetScore)
            {
                size_t nCurrentNode;
                for (nCurrentNode = 0; nCurrentNode < nCurrentNodeCount; ++nCurrentNode)
                {
                    const int    nX    = TO_X_FROM_GRID_WITH_D(kCurrentNodes[nCurrentNode], nWidth);
                    const int    nY    = TO_Y_FROM_GRID_WITH_D(kCurrentNodes[nCurrentNode], nWidth);
                    const size_t nGrid = TO_GRID(nX, nY, nWidth);
                    if (AOC_FALSE == kBestPointsSeen[nGrid])
                    {
                        kBestPointsSeen[nGrid] = AOC_TRUE;
                        ++nBestPointsSeenSize;
                    }
                }
            }

            manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueSize, &nSeconaryQueueSize);
            continue;
        }

        /* Have we been here already, if so, skip. */
        if (kSeen[nCurrentNodeGrid])
        {
            if (nCurrentScore > kSeen[nCurrentNodeGrid])
            {
                manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueSize, &nSeconaryQueueSize);
                continue;
            }
        }
        else
        {
            kSeenArray[nSeenCount++] = nCurrentNodeGrid;
        }
        kSeen[nCurrentNodeGrid] = nCurrentScore;

        /* For each possible neighbour. */
        for (nNeighbourDirection = EDirectionsType_North; nNeighbourDirection < EDirectionsType_Count; ++nNeighbourDirection)
        {
            /* Disallow Double Backing */
            if (abs((int)nNeighbourDirection - (int)kCurrentPositionAndDirection.eDirection) != 2)
            {
                const int    nNeighbourX     = kCurrentPositionAndDirection.kPosition.nX + DIRECTIONS[nNeighbourDirection].nX;
                const int    nNeighbourY     = kCurrentPositionAndDirection.kPosition.nY + DIRECTIONS[nNeighbourDirection].nY;
                const size_t nNeighbourScore = nCurrentScore + DIRECTION_SCORES[kCurrentPositionAndDirection.eDirection][nNeighbourDirection];

                /* Ignore if this is a wall */
                if ('#' != kMaze[nNeighbourY][nNeighbourX])
                {
                    /* Ignore if we've exceeded the Target Score */
                    if (nNeighbourScore <= nTargetScore)
                    {
                        /* Add the new position to the queue. */
                        kSecondaryQueue[nSeconaryQueueSize].nScore                             = nNeighbourScore;
                        kSecondaryQueue[nSeconaryQueueSize].kPositionAndDirection.kPosition.nX = nNeighbourX;
                        kSecondaryQueue[nSeconaryQueueSize].kPositionAndDirection.kPosition.nY = nNeighbourY;
                        kSecondaryQueue[nSeconaryQueueSize].kPositionAndDirection.eDirection   = (EDirectionsType)nNeighbourDirection;
                        kSecondaryQueue[nSeconaryQueueSize].nNodeCount                         = nCurrentNodeCount + 1;
                        if (NULL == kSecondaryQueue[nSeconaryQueueSize].kNodes)
                        {
                            kSecondaryQueue[nSeconaryQueueSize].kNodes = (size_t*)malloc(nNodeCount * sizeof(size_t));
                        }
                        memcpy(kSecondaryQueue[nSeconaryQueueSize].kNodes, kCurrentNodes, nCurrentNodeCount * sizeof(size_t));
                        kSecondaryQueue[nSeconaryQueueSize++].kNodes[nCurrentNodeCount]        = TO_GRID_WITH_D(nNeighbourX, nNeighbourY, nNeighbourDirection, nWidth);
                    }
                }
            }
        }

        manageDoubleQueue(&kPrimaryQueue, &kSecondaryQueue, kPrimaryQueue, kSecondaryQueue, &nPrimaryQueueSize, &nSeconaryQueueSize);
    }

    /* Clean-Up the Seen Nodes */
    for (nNode = 0; nNode < nSeenCount; ++nNode)
    {
        kSeen[kSeenArray[nNode]] = 0;
    }

    /* Update the Best Points Size */
    *pnBestPointsSeenSize = nBestPointsSeenSize;
}
#endif

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