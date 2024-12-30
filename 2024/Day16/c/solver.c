#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

typedef enum EDirectionsType
{
    EDirectionsType_North   = 0,
    EDirectionsType_East    = 1,
    EDirectionsType_South   = 2,
    EDirectionsType_West    = 3,
    EDirectionsType_Count   = 4,
} EDirectionsType;

typedef struct SPositionType
{
    int nX;
    int nY;
} SPositionType;

typedef struct SPositionAndDirectionType
{
    SPositionType       kPosition;
    EDirectionsType     eDirection;
} SPositionAndDirectionType;

typedef struct SNeighbourNodeType SNeighbourNodeType;

typedef struct SNodeType
{
    unsigned                    bIntersection;
    SPositionAndDirectionType   kPositionAndDirection;
    size_t                      nNeighbourCount;
    struct SNeighbourNodeType*  pNeighbours;
} SNodeType;

typedef struct SNeighbourNodeType
{
    size_t                      nScore;
    SNodeType*                  pNode;
} SNeighbourNodeType;

typedef struct NeighbourQueueType
{
    size_t                      nScore;
    SPositionAndDirectionType   kPositionAndDirection;
} NeighbourQueueType;

typedef struct BestPathType
{
    size_t                      nScore;
    size_t*                     kNodes;
    size_t                      nNodeCount;
    struct BestPathType*        pNext;
} BestPathType;

typedef struct PathQueueType
{
    size_t                      nScore;
    SPositionAndDirectionType   kPositionAndDirection;
    size_t                      nNodeCount;
    size_t*                     kNodes;
} PathQueueType;

const SPositionType DIRECTIONS[EDirectionsType_Count] = {
    { 0,-1}, /* North */
    { 1, 0}, /* East */
    { 0, 1}, /* South */
    {-1, 0}  /* West */
};
const size_t        DIRECTION_SCORES[EDirectionsType_Count][EDirectionsType_Count] = {
    {   1, 1001, 2001, 1001}, /* North */
    {1001,    1, 1001, 2001}, /* East  */
    {2001, 1001,    1, 1001}, /* South */
    {1001, 2001, 1001,    1}  /* West  */
};

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define TO_GRID_WITH_D(x, y, d, width) (((y) * (width) * (4)) + ((x) * (4)) + (d))
#define TO_X_FROM_GRID_WITH_D(n, width) (((n) / (4)) % (width))
#define TO_Y_FROM_GRID_WITH_D(n, width) ((n) / ((width) * (4)))
#define TO_D_FROM_GRID_WITH_D(n) ((n) % 4)

#define TO_GRID(x, y, width) ((y) * (width) + (x))
#define TO_X_FROM_GRID(n, width) ((n) % (width))
#define TO_Y_FROM_GRID(n, width) ((n) / (width))

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

void manageDoubleQueue(PathQueueType** pkPathQueueUsedPrimary, PathQueueType** pkPathQueueUsedSecondary, PathQueueType* pkPathQueuePrimary, PathQueueType* pkPathQueueSecondary, size_t* pnPrimaryPathQueueLength, size_t* pnSecondaryPathQueueLength)
{
    if (0 == *pnPrimaryPathQueueLength)
    {
        if (*pkPathQueueUsedPrimary == pkPathQueuePrimary)
        {
            *pkPathQueueUsedPrimary   = pkPathQueueSecondary;
            *pkPathQueueUsedSecondary = pkPathQueuePrimary;
        }
        else
        {
            *pkPathQueueUsedPrimary   = pkPathQueuePrimary;
            *pkPathQueueUsedSecondary = pkPathQueueSecondary;
        }
        *pnPrimaryPathQueueLength   = *pnSecondaryPathQueueLength;
        *pnSecondaryPathQueueLength = 0;
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

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kBuffer;
        char**                      kReindeerMaze;
 
        size_t                      nHeight;
        size_t                      nWidth;

        size_t                      nStart;
        size_t                      nEnd;

        int                         nX, nY;

        SNodeType*                  kNodes;
        size_t                      nNode;
        size_t                      nNodeCount = 0;
        size_t*                     kNodeIndex;

        size_t*                     kSeen;
        size_t*                     kSeenArray;
        size_t                      nSeenCount = 0;

        size_t                      nNeighbour;
        size_t*                     kNeighbourIntersectionSeen;
        size_t*                     kNeighbourIntersectionSeenArray;

        NeighbourQueueType*         kNeighbourQueue;

        BestPathType                kBestPaths;
        BestPathType*               pBestPath;

        unsigned*                   kPointsOnUniqueRouteGrid;
        size_t                      nPointsOnUniqueRouteCount = 0;

        PathQueueType*              kPathQueuePrimary;
        PathQueueType*              kPathQueueSecondary;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kReindeerMaze, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /*
         * Compute the Maze as a Series of Nodes, a node being any point that is has three adjacent sides,
         * Or the Start/End Points
         */
        kNodes                          = (SNodeType*)calloc(nHeight * nWidth * 4, sizeof(SNodeType));
        kNodeIndex                      = (size_t*)malloc(nHeight * nWidth * 4 * sizeof(size_t));
        kSeen                           = (size_t*)calloc(nHeight * nWidth * 4, sizeof(size_t));
        kSeenArray                      = (size_t*)malloc(nHeight * nWidth * 4 * sizeof(size_t));
        kNeighbourIntersectionSeen      = (size_t*)malloc(nHeight * nWidth * 4 * sizeof(size_t));
        kNeighbourIntersectionSeenArray = (size_t*)malloc(nHeight * nWidth * 4 * sizeof(size_t));
        kNeighbourQueue                 = (NeighbourQueueType*)malloc(nHeight * nWidth * 4 * sizeof(NeighbourQueueType));
        kPointsOnUniqueRouteGrid        = (unsigned*)calloc(nHeight * nWidth, sizeof(unsigned));

        /* The Maze is bound by a wall, we only need to process the innies. */
        for (nY = 1; (size_t)nY < (nHeight - 1); ++nY)
        {
            for (nX = 1; (size_t)nX < (nWidth - 1); ++nX)
            {
                SPositionAndDirectionType   kNeighbours[4];
                size_t                      nNeighbourCount = 0;
                size_t                      nNeighbour;
                unsigned                    bForceNode      = AOC_FALSE;
                unsigned                    bEndNode        = AOC_FALSE;

                /* Ignore the entry if it's a Wall */
                if ('#' == kReindeerMaze[nY][nX])
                {
                    continue;
                }

                /* Start */
                if ('S' == kReindeerMaze[nY][nX])
                {
                    nStart     = TO_GRID(nX, nY, nWidth);
                    bForceNode = AOC_TRUE;
                }

                /* End */
                if ('E' == kReindeerMaze[nY][nX])
                {
                    nEnd       = TO_GRID(nX, nY, nWidth);
                    bForceNode = AOC_TRUE;
                    bEndNode   = AOC_TRUE;
                }

                for (nNeighbour = 0; nNeighbour < EDirectionsType_Count; ++nNeighbour)
                {
                    const int nNeighbourX = DIRECTIONS[nNeighbour].nX + nX;
                    const int nNeighbourY = DIRECTIONS[nNeighbour].nY + nY;

                    if (('#' != kReindeerMaze[nNeighbourY][nNeighbourX]) || bEndNode)
                    {
                        kNeighbours[nNeighbourCount  ].kPosition.nX = nNeighbourX;
                        kNeighbours[nNeighbourCount  ].kPosition.nY = nNeighbourY;
                        kNeighbours[nNeighbourCount++].eDirection   = (EDirectionsType)nNeighbour;
                    }
                }

                /*
                 * Add the Node, we'll evaluate their meta-data when we have
                 * all the nodes declared
                 */
                if ((nNeighbourCount >= 3) || bForceNode)
                {
                    for (nNeighbour = 0; nNeighbour < nNeighbourCount; ++nNeighbour)
                    {
                        const size_t nNodeKey                               = TO_GRID_WITH_D(nX, nY, kNeighbours[nNeighbour].eDirection, nWidth);
                        kNodeIndex[nNodeCount++]                            = nNodeKey;
                        kNodes[nNodeKey].bIntersection                      = AOC_TRUE;
                        kNodes[nNodeKey].kPositionAndDirection.kPosition.nX = nX;
                        kNodes[nNodeKey].kPositionAndDirection.kPosition.nY = nY;
                        kNodes[nNodeKey].kPositionAndDirection.eDirection   = kNeighbours[nNeighbour].eDirection;
                    }
                }
            }
        }

        /* Evaluate the Maze as a series of scores to the next Nodes */
        for (nNode = 0; nNode < nNodeCount; ++nNode)
        {
            const int    nX    = kNodes[kNodeIndex[nNode]].kPositionAndDirection.kPosition.nX;
            const int    nY    = kNodes[kNodeIndex[nNode]].kPositionAndDirection.kPosition.nY;
            const size_t nGrid = TO_GRID(nX, nY, nWidth);

            if (nGrid != nEnd)
            {
                findNeighbourNodes(&kNodes[kNodeIndex[nNode]], (const char**)kReindeerMaze, kNodes, kSeen, kSeenArray, kNeighbourIntersectionSeen, kNeighbourIntersectionSeenArray, kNeighbourQueue, nWidth);
            }
        }

        /* Part 1: Find the Lowest Score from the Start Node to the End Node */
        kPathQueuePrimary   = (PathQueueType*)calloc(nNodeCount, sizeof(PathQueueType));
        kPathQueueSecondary = (PathQueueType*)calloc(nNodeCount, sizeof(PathQueueType));
        findBestPaths(&kBestPaths, nStart, nEnd, nWidth, kNodes, nNodeCount, kSeen, kSeenArray, kPathQueuePrimary, kPathQueueSecondary);
        printf("Part 1: %zu\n", kBestPaths.nScore);

        /*
         * Part 2: We know which nodes we went through for each route, and even the score to get there,
         *         now we just need the actual points between the nodes.
         */

        pBestPath = &kBestPaths;
        while (pBestPath->nScore == kBestPaths.nScore)
        {
            size_t i;
            for (i = 0; i < pBestPath->nNodeCount - 1; ++i)
            {
                SNodeType* kNode1 = &kNodes[pBestPath->kNodes[i]];
                SNodeType* kNode2 = &kNodes[pBestPath->kNodes[i+1]];
                for (nNeighbour = 0; nNeighbour < kNode1->nNeighbourCount; ++nNeighbour)
                {
                    if (kNode1->pNeighbours[nNeighbour].pNode == kNode2)
                    {
                        getAllPointsBetweenNodes(pBestPath->kNodes[i], pBestPath->kNodes[i+1], kNode1->pNeighbours[nNeighbour].nScore, (const char**)kReindeerMaze, kNodes, nNodeCount, kSeen, kSeenArray, kPointsOnUniqueRouteGrid, &nPointsOnUniqueRouteCount, kPathQueuePrimary, kPathQueueSecondary, nWidth);
                        break;
                    }
                }
            }

            if (pBestPath->pNext == NULL)
            {
                break;
            }
            pBestPath = pBestPath->pNext;
        }

        printf("Part 2: %zu\n", nPointsOnUniqueRouteCount);

        /* Free any Allocated Memory */
        for (nNode = 0; nNode < nNodeCount; ++nNode)
        {
            size_t nNeighbourNode;

            if (kNodes[nNode].nNeighbourCount)
            {
                free(kNodes[nNode].pNeighbours);
            }

            if (kPathQueuePrimary[nNode].kNodes)
            {
                free(kPathQueuePrimary[nNode].kNodes);
            }

            if (kPathQueueSecondary[nNode].kNodes)
            {
                free(kPathQueueSecondary[nNode].kNodes);
            }
        }

        pBestPath = &kBestPaths;
        do
        {
            BestPathType* pNextPath = pBestPath->pNext;
            free(pBestPath->kNodes);
            if (pBestPath != &kBestPaths)
            {
                free(pBestPath);
            }
            pBestPath = pNextPath;
        } while (pBestPath);

        free(kReindeerMaze);
        free(kBuffer);
        free(kNodes);
        free(kNodeIndex);
        free(kSeen);
        free(kSeenArray);
        free(kNeighbourIntersectionSeen);
        free(kNeighbourIntersectionSeenArray);
        free(kNeighbourQueue);
        free(kPointsOnUniqueRouteGrid);
        free(kPathQueuePrimary);
        free(kPathQueueSecondary);
    }
 
    return 0;
}