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
#define SQUARE(x) ((x) * (x))

typedef struct RobotPositionType
{
    int64_t nX;
    int64_t nY;
} RobotPositionType;

typedef struct RobotType
{
    RobotPositionType kPosition;
    int64_t           vX;
    int64_t           vY;
} RobotType;

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

void calculateNewPosition(const RobotType* kRobots, RobotPositionType* kNewPositions, const size_t nRobotCount, const size_t nRoundCount, const size_t WIDTH, const size_t HEIGHT)
{
    /* Since they teleport back in this is just a simply multiply/modulo. */
    size_t nRobot;
    for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
    {
        /* Note: Force the value to be positive... interestingly Python didn't seem to need this */
        kNewPositions[nRobot].nX = (((kRobots[nRobot].kPosition.nX + (kRobots[nRobot].vX * (int64_t)nRoundCount)) % (int64_t) WIDTH) +  WIDTH) %  WIDTH;
        kNewPositions[nRobot].nY = (((kRobots[nRobot].kPosition.nY + (kRobots[nRobot].vY * (int64_t)nRoundCount)) % (int64_t)HEIGHT) + HEIGHT) % HEIGHT;
    }
}

uint64_t getQuadrantSafety(const RobotPositionType* kRobots, const size_t nRobotCount, const size_t WIDTH, const size_t HEIGHT)
{
    const int64_t kQuadrants[4][2][2] =
    {
        {{0,               WIDTH / 2}, {               0, HEIGHT / 2}}, /* Top Left     */
        {{(WIDTH + 1) / 2, WIDTH    }, {               0, HEIGHT / 2}}, /* Top Right    */
        {{              0, WIDTH / 2}, {(HEIGHT + 1) / 2, HEIGHT    }}, /* Bottom Left  */
        {{(WIDTH + 1) / 2, WIDTH    }, {(HEIGHT + 1) / 2, HEIGHT    }}  /* Bottom Right */
    };

    uint64_t kRobotsInQuadrant[4] = {0, 0, 0, 0};
    size_t   nQuadrant;
    size_t   nRobot;
    uint64_t nProduct = 1;

    /* Count all the Robots in each Quadrant */
    for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
    {
        const int64_t x = kRobots[nRobot].nX;
        const int64_t y = kRobots[nRobot].nY;
        for (nQuadrant = 0; nQuadrant < (sizeof(kQuadrants) / sizeof(kQuadrants[0])); ++nQuadrant)
        {
            if ((x >= kQuadrants[nQuadrant][0][0] && x < kQuadrants[nQuadrant][0][1]) &&
                (y >= kQuadrants[nQuadrant][1][0] && y < kQuadrants[nQuadrant][1][1]))
            {
                kRobotsInQuadrant[nQuadrant]++;
                break;
            }
        }
    }

    /* Multiply the Quadrants */
    for (nQuadrant = 0; nQuadrant < (sizeof(kQuadrants) / sizeof(kQuadrants[0])); ++nQuadrant)
    {
        nProduct *= kRobotsInQuadrant[nQuadrant];
    }

    return nProduct;
}

uint64_t getMaxCluster(const RobotPositionType* kRobot, const RobotPositionType* kRobots, const size_t nRobotCount, unsigned* kRobotsSeen, const RobotPositionType** kRobotsInGrid, const RobotPositionType** kRobotNeighbourQueue, const size_t WIDTH, const size_t HEIGHT)
{
    const RobotPositionType NEIGHBOURS[4] =
    {
        {-1,  0}, /* Left  */
        { 1,  0}, /* Right */
        { 0, -1}, /* Up    */
        { 0,  1}  /* Down  */
    };

    size_t   nQueueSize = 0;
    uint64_t nCount     = 0;

    kRobotNeighbourQueue[nQueueSize++] = kRobot;

    while (nQueueSize)
    {
        const RobotPositionType* kCurrent = kRobotNeighbourQueue[--nQueueSize];
        const size_t             nIndex   = TO_GRID(kCurrent->nX, kCurrent->nY, WIDTH);
        size_t                   nNeighbour;

        if (AOC_TRUE == kRobotsSeen[nIndex])
        {
            continue;
        }
        kRobotsSeen[nIndex] = AOC_TRUE;

        /* Increment the Adjacent Count */
        ++nCount;

        for (nNeighbour = 0; nNeighbour < (sizeof(NEIGHBOURS) / sizeof(NEIGHBOURS[0])); ++nNeighbour)
        {
            const RobotPositionType kNextRobot = {kCurrent->nX + NEIGHBOURS[nNeighbour].nX, kCurrent->nY + NEIGHBOURS[nNeighbour].nY};
            const size_t            nNextIndex = TO_GRID(kNextRobot.nX, kNextRobot.nY, WIDTH);

            if (kNextRobot.nX >= 0 && kNextRobot.nX < WIDTH &&
                kNextRobot.nY >= 0 && kNextRobot.nY < HEIGHT &&
                AOC_FALSE == kRobotsSeen[nNextIndex] &&
                NULL      != kRobotsInGrid[nNextIndex])
            {
                kRobotNeighbourQueue[nQueueSize++] = kRobotsInGrid[nNextIndex];
            }
        }
    }

    return nCount;
}

unsigned isTree(const RobotPositionType* kRobots, const size_t nRobotCount, const RobotPositionType** kRobotsInGrid, unsigned* kRobotsSeen, size_t* kRobotKeys, const RobotPositionType** kRobotNeighbourQueue, const size_t WIDTH, const size_t HEIGHT)
{
    /*
     * This calculates an average position, and computes the number of
     * robots that are within a 5 unit radius of that average position.
     *
     * The theory being that any image should have a centre of mass around
     * where the image is drawn, and that we would encounter a high number
     * of robots in that area.
     *
     * Because the puzzle ill defines the tree itself, we have to use some
     * arbitrary heuristic, working backwards from the answer itself, and
     * this seems adequate.
     *
     * 15 units was chosen as a radius because, for my input, it resulted
     * in the trigger point being comfortably above the number, with the
     * next closest being significantly below.
     *
     * As such, we use this as a filter for a much better test, neighbouring
     * clusters, which is a much more robust test for the tree as it checks
     * for a large number of robots adjacent to each other, however this is
     * far slower, so we can use the above heuristic to filter out testing
     * this unnecessarily.
     *
     * The thresholds are:
     *
     * Filter: 20% of robots are within a 15 unit radius of the calculated
     *         average position.
     *
     * Cluster: 10% of robots are adjacent to each other.
     */

    /* Calculate the Filter Value (20% of the Robot Count) */
    const double nFilterThreshold = (double)nRobotCount / 5.0;
    double kAveragePosition[2]    = {0.0, 0.0};
    size_t nCloseCount            = 0;
    size_t nRobot;

    /* Calculate the Average Position */
    for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
    {
        kAveragePosition[0] += (double)kRobots[nRobot].nX;
        kAveragePosition[1] += (double)kRobots[nRobot].nY;
    }
    kAveragePosition[0] /= (double)nRobotCount;
    kAveragePosition[1] /= (double)nRobotCount;

    /*
     * Count the number of Robots within a 15 unit radius of the average position
     *
     * Note : We can cheat distances by not calculate the square root, and comparing to the
     *        square of the distance we're interested in, which saves computational time.
     */
    for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
    {
        const double nDistance = SQUARE(kRobots[nRobot].nX - kAveragePosition[0]) + SQUARE(kRobots[nRobot].nY - kAveragePosition[1]);
        if (nDistance <= nFilterThreshold) /* (within a 15 unit radius) */
        {
            ++nCloseCount;
        }
    }

    /*
     * If 20% of the Robots are within a 15 unit radius of the average position
     * then we can check for clusters.
     */
    if (nCloseCount >= (size_t)nFilterThreshold)
    {
        /* Cluster Threshold (10% of the Robot Count) */
        const size_t nClusterThreshold = (size_t)nFilterThreshold / 2;

        /* Initialise the Robots In Grid */
        for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
        {
            const size_t nIndex   = TO_GRID(kRobots[nRobot].nX, kRobots[nRobot].nY, WIDTH);
            kRobotsInGrid[nIndex] = &kRobots[nRobot];
            kRobotKeys[nRobot]    = nIndex;
        }

        for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
        {
            const size_t nIndex   = kRobotKeys[nRobot];
            if (AOC_FALSE == kRobotsSeen[nIndex])
            {
                /* Get the Cluster Size and Robots we've seen */
                const size_t nSize = getMaxCluster(&kRobots[nRobot], kRobots, nRobotCount, kRobotsSeen, kRobotsInGrid, kRobotNeighbourQueue, WIDTH, HEIGHT);
                if (nSize >= nClusterThreshold)
                {
                    return AOC_TRUE;
                }
            }
        }

        /* Cleanup */
        for (nRobot = 0; nRobot < nRobotCount; ++nRobot)
        {
            const size_t nIndex   = kRobotKeys[nRobot];
            kRobotsInGrid[nIndex] = NULL;
            kRobotsSeen[nIndex]   = AOC_FALSE;
        }
    }

    return AOC_FALSE;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        /* Width/Height are defined by the Puzzle Text itself */
        const int64_t               WIDTH  = 101;
        const int64_t               HEIGHT = 103;

        char*                       kRobotBuffer;
        char**                      kRobotsRaw;
        size_t                      nRobotCount;
 
        RobotType*                  kRobots;
        RobotPositionType*          kUpdatedRobotPositions;
        const RobotPositionType**   kRobotsInGrid;
        unsigned*                   kRobotsSeen;
        size_t*                     kRobotKeys;
        const RobotPositionType**   kRobotNeighbourQueue;

        size_t                      nIndex;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kRobotBuffer, &kRobotsRaw, &nRobotCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Memory for the Robots */
        kRobots                = (RobotType*)malloc(nRobotCount * sizeof(RobotType));
        kUpdatedRobotPositions = (RobotPositionType*)malloc(nRobotCount * sizeof(RobotPositionType));
        kRobotsInGrid          = (const RobotPositionType**)calloc(WIDTH * HEIGHT, sizeof(RobotPositionType*));
        kRobotsSeen            = (unsigned*)calloc(nRobotCount, sizeof(unsigned));
        kRobotKeys             = (size_t*)malloc(nRobotCount * sizeof(size_t));
        kRobotNeighbourQueue   = (const RobotPositionType**)malloc(nRobotCount * sizeof(RobotPositionType*));

        /* Parse the Robots */
        for (nIndex = 0; nIndex < nRobotCount; ++nIndex)
        {
            const char* k = strstr(kRobotsRaw[nIndex], "=");
            assert(k);
            kRobots[nIndex].kPosition.nX = strtoll(k+1, NULL, 10);

            k = strstr(k, ",");
            assert(k);
            kRobots[nIndex].kPosition.nY = strtoll(k+1, NULL, 10);

            k = strstr(k, "=");
            assert(k);
            kRobots[nIndex].vX = strtoll(k+1, NULL, 10);

            k = strstr(k, ",");
            assert(k);
            kRobots[nIndex].vY = strtoll(k+1, NULL, 10);
        }

        /* Part 1: Quadrant Safety */
        calculateNewPosition(kRobots, kUpdatedRobotPositions, nRobotCount, 100, WIDTH, HEIGHT);
        printf("Part 1: %" PRIu64 "\n", getQuadrantSafety(kUpdatedRobotPositions, nRobotCount, WIDTH, HEIGHT));

        /* Part 2: Tree Shape */
        nIndex = 0;
        while(1)
        {
            calculateNewPosition(kRobots, kUpdatedRobotPositions, nRobotCount, nIndex, WIDTH, HEIGHT);
            if (isTree(kUpdatedRobotPositions, nRobotCount, kRobotsInGrid, kRobotsSeen, kRobotKeys, kRobotNeighbourQueue, WIDTH, HEIGHT))
            {
                printf("Part 2: %zu\n", nIndex);
                break;
            }
            ++nIndex;
        }

        /* Free any Allocated Memory */
        free(kRobotsRaw);
        free(kRobotBuffer);
        free(kRobots);
        free(kUpdatedRobotPositions);
        free(kRobotsInGrid);
        free(kRobotsSeen);
        free(kRobotKeys);
        free(kRobotNeighbourQueue);
    }
 
    return 0;
}