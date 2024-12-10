#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define TURN_LEFT(D)  (((D) - 1) & 0x3)
#define TURN_RIGHT(D) (((D) + 1) & 0x3)
#define REVERSE(D)    (((D) + 2) & 0x3)

#define DWIDTH (4)
#define BWIDTH (2)

#define TO_KEY(X,Y,D,WIDTH)          (((Y) * (WIDTH) * (DWIDTH) * (BWIDTH)) + ((X) * (DWIDTH) * (BWIDTH)) + ((D) * (BWIDTH)))
#define TO_KEY_WITH_B(X,Y,D,B,WIDTH) (((Y) * (WIDTH) * (DWIDTH) * (BWIDTH)) + ((X) * (DWIDTH) * (BWIDTH)) + ((D) * (BWIDTH)) + (B))

#define Y_FROM_KEY(K,WIDTH) (((K) / ((DWIDTH) * (BWIDTH) * (WIDTH))))
#define X_FROM_KEY(K,WIDTH) (((K) / ((DWIDTH) * (BWIDTH))) % ((WIDTH)))
#define D_FROM_KEY(K)       (((K) / (BWIDTH)) % (DWIDTH))
#define B_FROM_KEY(K)       ((K) % (BWIDTH))

#define KEY_DATA_SIZE(HEIGHT, WIDTH) (((HEIGHT) * (WIDTH) * (DWIDTH) * (BWIDTH)))

typedef enum GuardDirection
{
    UP    = 0,
    RIGHT = 1,
    DOWN  = 2,
    LEFT  = 3,
    FIRST = UP,
    LAST  = LEFT
} GuardDirection;

const int DIRECTION_VECTORS[4][2] = {
    { 0, -1 }, /* UP */
    { 1,  0 }, /* RIGHT */
    { 0,  1 }, /* DOWN */
    {-1,  0 }  /* LEFT */
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

    *pkFileBuffer   = (char*) malloc((nFileSize+2)         * sizeof(char));
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
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';

        if (0 == bProcessUnix)
        {
            if (nFileSize >= 1)
            {
                if ((*pkFileBuffer)[nFileSize-1] != '\r')
                {
                    (*pkFileBuffer)[nFileSize-1]   = '\0';
                }
            }
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

size_t getRoute(const uint32_t* const kObstaclesSet, const uint32_t kStartGuardPos, const size_t nWidth, const size_t nHeight, uint32_t* kGuardVisit)
{
    uint32_t       kGuardPos        = kStartGuardPos;
    GuardDirection eGuardDirection  = D_FROM_KEY(kGuardPos);
    size_t         nGuardVisitCount = 1;
    unsigned*      kGuardVisitSet   = (unsigned*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(unsigned));

    memset(kGuardVisitSet, AOC_FALSE, KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));

    kGuardVisitSet[kGuardPos] = AOC_TRUE;

    while(1)
    {
        const int            nGuardX           = X_FROM_KEY(kGuardPos, nWidth);
        const int            nGuardY           = Y_FROM_KEY(kGuardPos, nWidth);
        const int            nNextX            = nGuardX + DIRECTION_VECTORS[eGuardDirection][0];
        const int            nNextY            = nGuardY + DIRECTION_VECTORS[eGuardDirection][1];

        if ((nNextX >= 0) && ((size_t)nNextX < nWidth) && (nNextY >= 0) && ((size_t)nNextY < nHeight))
        {
            const uint32_t kNextGuardPos = TO_KEY(nNextX, nNextY, 0, nWidth);
            if (kObstaclesSet[kNextGuardPos])
            {
                eGuardDirection = TURN_RIGHT(eGuardDirection);
            }
            else
            {
                kGuardPos = kNextGuardPos;
                if (AOC_FALSE == kGuardVisitSet[kGuardPos])
                {
                    kGuardVisitSet[kGuardPos]       = AOC_TRUE;
                    kGuardVisit[nGuardVisitCount++] = kGuardPos;
                }
            }
        }
        else
        {
            break;
        }
    }

    free(kGuardVisitSet);

    return nGuardVisitCount;
}

uint32_t calculateGuardNode(const uint32_t* const kObstacleSet, const uint32_t kGuardPos, const GuardDirection eGuardDirection, const size_t nWidth, const size_t nHeight)
{
    uint32_t kCurrentGuardPos = kGuardPos;
    unsigned bInBounds        = AOC_TRUE;
    uint32_t kGuardKey        = TO_KEY(X_FROM_KEY(kGuardPos, nWidth), Y_FROM_KEY(kGuardPos, nWidth), eGuardDirection, nWidth);

    while (1)
    {
        const int nGuardX = X_FROM_KEY(kCurrentGuardPos, nWidth);
        const int nGuardY = Y_FROM_KEY(kCurrentGuardPos, nWidth);
        const int nNextX  = nGuardX + DIRECTION_VECTORS[eGuardDirection][0];
        const int nNextY  = nGuardY + DIRECTION_VECTORS[eGuardDirection][1];

        if ((nNextX >= 0) && ((size_t)nNextX < nWidth) && (nNextY >= 0) && ((size_t)nNextY < nHeight))
        {
            const uint32_t kNextGuardPos = TO_KEY(nNextX, nNextY, 0, nWidth);
            if (kObstacleSet[kNextGuardPos])
            {
                break;
            }
        }
        else
        {
            bInBounds = AOC_FALSE;
            break;
        }

        kCurrentGuardPos = TO_KEY(nNextX, nNextY, 0, nWidth);
    }

    return TO_KEY_WITH_B(X_FROM_KEY(kCurrentGuardPos, nWidth), Y_FROM_KEY(kCurrentGuardPos, nWidth), TURN_RIGHT(eGuardDirection), bInBounds, nWidth);
}

unsigned getLoop(const uint32_t kFirstMovePos, const uint32_t kNewObstaclePos, const uint32_t* const kGuardCatchSet, const uint32_t* const kGuardMoveNodes, const uint32_t kGuardStartPos, const size_t nWidth, const size_t nHeight)
{
    const int nObstacleX = X_FROM_KEY(kNewObstaclePos, nWidth);
    const int nObstacleY = Y_FROM_KEY(kNewObstaclePos, nWidth);

    uint32_t  kGuardPos  = kGuardStartPos;
    unsigned  bFirstNode = AOC_TRUE;
    unsigned* kSeen      = (unsigned*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(unsigned));

    memset(kSeen, AOC_FALSE, KEY_DATA_SIZE(nHeight, nWidth) * sizeof(unsigned));

    while(1)
    {
        int            nNextPosX, nNextPosY;
        GuardDirection eNextDirection;
        unsigned       bInBounds;

        int            nGuardPosX, nGuardPosY;

        if (kSeen[kGuardPos])
        {
            free(kSeen);
            return AOC_TRUE;
        }

        kSeen[kGuardPos] = AOC_TRUE;

        nGuardPosX = X_FROM_KEY(kGuardPos, nWidth);
        nGuardPosY = Y_FROM_KEY(kGuardPos, nWidth);

        if (bFirstNode)
        {
            nNextPosX       = X_FROM_KEY(kFirstMovePos, nWidth);
            nNextPosY       = Y_FROM_KEY(kFirstMovePos, nWidth);
            eNextDirection  = D_FROM_KEY(kFirstMovePos);
            bInBounds       = B_FROM_KEY(kFirstMovePos);
            bFirstNode      = AOC_FALSE;
        }
        else
        {
            nNextPosX       = X_FROM_KEY(kGuardMoveNodes[kGuardPos], nWidth);
            nNextPosY       = Y_FROM_KEY(kGuardMoveNodes[kGuardPos], nWidth);
            eNextDirection  = D_FROM_KEY(kGuardMoveNodes[kGuardPos]);
            bInBounds       = B_FROM_KEY(kGuardMoveNodes[kGuardPos]);
        }

        assert(nNextPosX != 255);

        /*
         * Does the next position intersect with the new obstacle?
         * Note: One of the axis must match for an intersection, so we can use this to quickly discard
         *       obvious non-collisions with the new obstacle to help throughput.
         */
        if ((nObstacleX >= MIN(nGuardPosX, nNextPosX)) && (nObstacleX <= MAX(nGuardPosX, nNextPosX)) &&
            (nObstacleY >= MIN(nGuardPosY, nNextPosY)) && (nObstacleY <= MAX(nGuardPosY, nNextPosY)))
        {
            /*
             * If we intersect, do a manual move until we're back in the known node lists or if we've left
             * the grid.
             */

            const GuardDirection eLastDirection = TURN_LEFT(eNextDirection);

            /* Set our position to adjacent to the new obstacle */
            kGuardPos = TO_KEY(nObstacleX - DIRECTION_VECTORS[eLastDirection][0], nObstacleY - DIRECTION_VECTORS[eLastDirection][1], eLastDirection, nWidth);

            while(1)
            {
                nNextPosX = X_FROM_KEY(kGuardPos, nWidth) + DIRECTION_VECTORS[eNextDirection][0];
                nNextPosY = Y_FROM_KEY(kGuardPos, nWidth) + DIRECTION_VECTORS[eNextDirection][1];

                if ((nNextPosX >= 0) && ((size_t)nNextPosX < nWidth) && (nNextPosY >= 0) && ((size_t)nNextPosY < nHeight))
                {
                    /* Is this a valid key... */
                    const uint32_t kNextGuardPos = TO_KEY(X_FROM_KEY(kGuardPos, nWidth), Y_FROM_KEY(kGuardPos, nWidth), eNextDirection, nWidth);
                    if (kGuardCatchSet[kNextGuardPos])
                    {
                        nNextPosX = X_FROM_KEY(kGuardPos, nWidth);
                        nNextPosY = Y_FROM_KEY(kGuardPos, nWidth);
                        break;
                    }
                }
                else
                {
                    free(kSeen);
                    return AOC_FALSE;
                }

                kGuardPos = TO_KEY(nNextPosX, nNextPosY, eNextDirection, nWidth);
            }

            kGuardPos = TO_KEY(nNextPosX, nNextPosY, TURN_RIGHT(eNextDirection), nWidth);

            continue;
        }

        if (bInBounds == AOC_FALSE)
        {
            free(kSeen);
            return AOC_FALSE;
        }


        kGuardPos = TO_KEY(nNextPosX, nNextPosY, eNextDirection, nWidth);
    }

    assert(0);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                       kBuffer;
        char**                      kGrid;
        size_t                      nHeight;
        size_t                      nWidth;
        size_t                      nLine            = 0;
        int                         X, Y, nNode;

        unsigned*                   kObstaclesSet;
        uint32_t*                   kObstacles;
        size_t                      nObstacleCount   = 0;
        uint32_t*                   kGuardVisit;
        size_t                      nGuardVisitCount;
        unsigned*                   kGuardCatchSet;
        uint32_t*                   kGuardMoveNodes;

        uint32_t                    kGuardPos;
        uint32_t                    kGuardFirstMovePos;

        size_t                      nPossibleLoopCount = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kGrid, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /* Allocate the "hashmaps" and Arrays */
        kObstaclesSet    = (unsigned*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));
        kObstacles       = (uint32_t*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));
        kGuardVisit      = (uint32_t*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));
        kGuardCatchSet   = (unsigned*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));
        kGuardMoveNodes  = (uint32_t*)malloc(KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));

        /* We only initialise the nodes/sets */
        memset(kObstaclesSet, AOC_FALSE, KEY_DATA_SIZE(nHeight, nWidth) * sizeof(unsigned));
        memset(kGuardCatchSet, AOC_FALSE, KEY_DATA_SIZE(nHeight, nWidth) * sizeof(unsigned));
        memset(kGuardMoveNodes, 0xFF, KEY_DATA_SIZE(nHeight, nWidth) * sizeof(uint32_t));

        /* Parse the Original Grid */
        for (Y = 0; Y < nHeight; ++Y)
        {
            for (X = 0; X < nWidth; ++X)
            {
                const char kCell = kGrid[Y][X];

                if ('#' == kCell)
                {
                    kObstaclesSet[TO_KEY(X,Y,0, nWidth)] = AOC_TRUE;
                    kObstacles[nObstacleCount++] = TO_KEY(X,Y,0, nWidth);
                }
                else if ('^' == kCell)
                {
                    kGuardPos = TO_KEY(X,Y,UP, nWidth);
                }
            }
        }

        /* We no longer need the grid... */
        free(kGrid);
        free(kBuffer);

        /* Part One, get the Visited Nodes for the Guard */
        nGuardVisitCount = getRoute(kObstaclesSet, kGuardPos, nWidth, nHeight, kGuardVisit);

        printf("Part 1: %zu\n", nGuardVisitCount);

        /* Part Two... make loops. */

        /* First part, build a node list of all the points around each obstacle. */

        for (nNode = 0; nNode < nObstacleCount; ++nNode)
        {
            const int nObstacleX = X_FROM_KEY(kObstacles[nNode], nWidth);
            const int nObstacleY = Y_FROM_KEY(kObstacles[nNode], nWidth);

            size_t   nNeighbour;
            size_t   nNeighbourCount = 0;
            uint32_t kNeighbours[4];

            for (nNeighbour = FIRST; nNeighbour <= LAST; ++nNeighbour)
            {
                const int nNeighbourX = nObstacleX + DIRECTION_VECTORS[nNeighbour][0];
                const int nNeighbourY = nObstacleY + DIRECTION_VECTORS[nNeighbour][1];

                if ((nNeighbourX >= 0) && ((size_t)nNeighbourX < nWidth) && (nNeighbourY >= 0) && ((size_t)nNeighbourY < nHeight))
                {
                    if (AOC_FALSE == kObstaclesSet[TO_KEY(nNeighbourX, nNeighbourY, 0, nWidth)])
                    {
                        kNeighbours[nNeighbourCount++] = TO_KEY(nNeighbourX, nNeighbourY, REVERSE(nNeighbour), nWidth);
                    }
                }
            }

            for (nNeighbour = 0; nNeighbour < nNeighbourCount; ++nNeighbour)
            {
                /* To do... forward/reverse nodes */
                const GuardDirection eDirection = D_FROM_KEY(kNeighbours[nNeighbour]);
                const int nNeighbourX = X_FROM_KEY(kNeighbours[nNeighbour], nWidth);
                const int nNeighbourY = Y_FROM_KEY(kNeighbours[nNeighbour], nWidth);
                kGuardCatchSet[TO_KEY(nNeighbourX, nNeighbourY, eDirection, nWidth)] = AOC_TRUE;
                kGuardMoveNodes[TO_KEY(nNeighbourX, nNeighbourY, TURN_RIGHT(eDirection), nWidth)] = calculateGuardNode(kObstaclesSet, kNeighbours[nNeighbour], TURN_RIGHT(eDirection), nWidth, nHeight);
            }
        }

        /*
         * Get the first Colliding Node for the Start Position...
         * Note: Adding the start position to the actual collision nodes
         *       just introduces a world of pain!
         */
        kGuardFirstMovePos = calculateGuardNode(kObstaclesSet, kGuardPos, D_FROM_KEY(kGuardPos), nWidth, nHeight);

        for (nNode = 0; nNode < nGuardVisitCount; ++nNode)
        {
            const uint32_t kObstaclePos = kGuardVisit[nNode];
            if (kObstaclePos != kGuardPos)
            {
                const unsigned bLoop = getLoop(kGuardFirstMovePos, kObstaclePos, kGuardCatchSet, kGuardMoveNodes, kGuardPos, nWidth, nHeight);

                if (bLoop)
                {
                    ++nPossibleLoopCount;
                }
            }
        }

        printf("Part 2: %zu\n", nPossibleLoopCount);

        /* Free any Allocated Memory */
        free(kObstacles);
    }

    return 0;
}