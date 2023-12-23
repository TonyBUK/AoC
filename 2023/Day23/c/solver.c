#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE            (1u)
#define AOC_FALSE           (0u)

#define ENCODE_KEY(x, y, w) (((y) * (w)) + (x))

typedef struct SValidDirectionsType
{
    size_t  nValidDirectionsCount;
    int64_t kValidDirections[4][2];
} SValidDirectionsType;

typedef struct SNeighbourType
{
    int64_t  kNeighbourPos[2];
    uint64_t kNeighbourKey;
    uint64_t kNeighbourCost;
} SNeighbourType;

typedef struct SNeighboursType
{
    size_t          nNeighbourCount;
    SNeighbourType  kNeighbours[4];
} SNeighboursType;

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
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
    nReadCount = fread(*pkFileBuffer, sizeof(char), nFileSize, *pFile);
    assert(nReadCount > 0);
    assert(nReadCount <= nFileSize);

    if (strstr(*pkFileBuffer, "\r\n"))
    {
        bProcessUnix = 0;
    }

    if ((*pkFileBuffer)[nReadCount] != '\n')
    {
        (*pkFileBuffer)[nReadCount]   = '\n';
        (*pkFileBuffer)[nReadCount+1] = '\0';

        if (0 == bProcessUnix)
        {
            if (nReadCount >= 1)
            {
                if ((*pkFileBuffer)[nReadCount-1] != '\r')
                {
                    (*pkFileBuffer)[nReadCount-1]   = '\0';
                }
            }
        }
    }

    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, (bProcessUnix ? "\n" : "\r\n"));
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
        char* pBufferedLine;

        assert(pLine < &(*pkFileBuffer)[nReadCount]);

        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;

        pBufferedLine = pLine;

        pLine = strtok(NULL, (bProcessUnix ? "\n" : "\r\n"));

        /* Fix long standing bug where duplicate empty lines are skipped */
        if (pLine)
        {
            if((pLine - pBufferedLine) != nLineLength + 1)
            {
                if (bProcessUnix)
                {
                    char* pPreviousLine = pLine - 1;
                    while (*pPreviousLine == '\n')
                    {
                        (*pkLines)[nLineCount] = pPreviousLine;
                        ++nLineCount;

                        *pPreviousLine = '\0';
                        --pPreviousLine;
                    }
                }
                else
                {
                    char* pPreviousLine = pLine - 2;
                    while (*pPreviousLine == '\r')
                    {
                        (*pkLines)[nLineCount] = pPreviousLine;
                        ++nLineCount;

                        pPreviousLine[0] = '\0';
                        pPreviousLine[1] = '\0';
                        --pPreviousLine;
                        --pPreviousLine;
                    }
                }
            }
        }

        if (nLineLength > nMaxLineLength)
        {
            nMaxLineLength = nLineLength;
        }
    }

    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nReadCount+1];
        ++nLineCount;
    }

    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nReadCount;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

void generateNodes(const char** kMaze, const size_t nWidth, const size_t nHeight, SValidDirectionsType* kValidDirections, SNeighboursType* kNodes, SNeighbourType* kCache)
{
    /* For the Cache, the neighbour cost has no *actual* meaning, so let's repurpose it */
    const uint64_t nCacheDeleted  = 0;
    const uint64_t nCacheExists   = 1;

          size_t   nCacheSize     = 0;
          int64_t  nY;
          unsigned bNodesCulled;

    /* The cached values are used for easier iterating later on... */
    for (nY = 0; nY < (int64_t)nHeight; ++nY)
    {
        int64_t nX;
        for (nX = 0; nX < (int64_t)nWidth; ++nX)
        {
            const SValidDirectionsType* kDirection      = &kValidDirections[kMaze[nY][nX]];
            const size_t                nDirectionCount = kDirection->nValidDirectionsCount;
                  size_t                nDirection;

            /* We can abuse the Direction Count to detect if the Map Tile is useable */
            if (nDirectionCount > 0)
            {
                const uint64_t  nPositionKey    = ENCODE_KEY(nX, nY, nWidth);

                /* Add the Node to the Cache */
                kCache[nCacheSize].kNeighbourCost   = nCacheExists;
                kCache[nCacheSize].kNeighbourKey    = nPositionKey;
                kCache[nCacheSize].kNeighbourPos[0] = nY;
                kCache[nCacheSize].kNeighbourPos[1] = nX;
                ++nCacheSize;

                /* Iterate through the Directions */
                for (nDirection = 0; nDirection < nDirectionCount; ++nDirection)
                {
                    const int64_t nCandidateX = nX + kDirection->kValidDirections[nDirection][1];
                    const int64_t nCandidateY = nY + kDirection->kValidDirections[nDirection][0];

                    /* Are we in the Maze? */
                    if ((nCandidateX < 0)                ||
                        (nCandidateX >= (int64_t)nWidth) ||
                        (nCandidateY < 0)                ||
                        (nCandidateY >= (int64_t)nHeight))
                    {
                        continue;
                    }

                    /* Is this a Moveable Position */
                    if (kMaze[nCandidateY][nCandidateX] == '#')
                    {
                        continue;
                    }

                    /* Add the Node */
                    kNodes[nPositionKey].kNeighbours[kNodes[nPositionKey].nNeighbourCount].kNeighbourCost   = 1;
                    kNodes[nPositionKey].kNeighbours[kNodes[nPositionKey].nNeighbourCount].kNeighbourKey    = ENCODE_KEY(nCandidateX, nCandidateY, nWidth);
                    kNodes[nPositionKey].kNeighbours[kNodes[nPositionKey].nNeighbourCount].kNeighbourPos[0] = nCandidateY;
                    kNodes[nPositionKey].kNeighbours[kNodes[nPositionKey].nNeighbourCount].kNeighbourPos[1] = nCandidateX;
                    ++kNodes[nPositionKey].nNeighbourCount;
                }
            }
        }
    }
    
    /* Repeatedly remove all entries with only 2 exits.  Ultimately this will leave us with
     * only genuine intersections.
     */
    do
    {
        size_t nCacheEntry;
        bNodesCulled = AOC_FALSE;

        /* Iterate through the Cached Positions */
        for (nCacheEntry = 0; nCacheEntry < nCacheSize; ++nCacheEntry)
        {
            SNeighbourType*  kCacheEntry = &kCache[nCacheEntry];

            /* Ignore Deleted Nodes */
            if (kCacheEntry->kNeighbourCost == nCacheDeleted)
            {
                continue;
            }

            /* If we have exactly two neighbours*/
            if (kNodes[kCacheEntry->kNeighbourKey].nNeighbourCount == 2)
            {
                SNeighbourType*  kValidNeighbours[2];
                SNeighboursType* kNodeToDelete        = &kNodes[kCacheEntry->kNeighbourKey];
                size_t           nNeighbour;
                size_t           nValidNeighbourCount = 0;

                /* First double check the two neighbours are bi-directional */
                for (nNeighbour = 0; nNeighbour < 2; ++nNeighbour)
                {
                    size_t nReverseNeighbour;
                    SNeighboursType* kReverseNeighbour = &kNodes[kNodeToDelete->kNeighbours[nNeighbour].kNeighbourKey];
                    for (nReverseNeighbour = 0; nReverseNeighbour < kReverseNeighbour->nNeighbourCount; ++nReverseNeighbour)
                    {
                        if (kCacheEntry->kNeighbourKey == kReverseNeighbour->kNeighbours[nReverseNeighbour].kNeighbourKey)
                        {
                            kValidNeighbours[nNeighbour] = &kReverseNeighbour->kNeighbours[nReverseNeighbour];
                            ++nValidNeighbourCount;
                            break;
                        }
                    }
                }

                /* If the Node was Bi-Directional */
                if (2 == nValidNeighbourCount)
                {
                    /* Buffer the Positions / Keys */
                    const uint64_t kBufferedNeighbourCost[2] = {
                        kValidNeighbours[0]->kNeighbourCost,
                        kValidNeighbours[1]->kNeighbourCost
                    };

                    const int64_t kBufferedNeighbourPos[2][2] = {
                        {kNodeToDelete->kNeighbours[0].kNeighbourPos[0], kNodeToDelete->kNeighbours[0].kNeighbourPos[1]},
                        {kNodeToDelete->kNeighbours[1].kNeighbourPos[0], kNodeToDelete->kNeighbours[1].kNeighbourPos[1]}
                    };

                    const uint64_t kBufferedNeighbourKey[2] = {
                        kNodeToDelete->kNeighbours[0].kNeighbourKey,
                        kNodeToDelete->kNeighbours[1].kNeighbourKey
                    };

                    /* Make sure we're Valid */
                    assert(kValidNeighbours[0]->kNeighbourKey == kCacheEntry->kNeighbourKey);
                    assert(kValidNeighbours[1]->kNeighbourKey == kCacheEntry->kNeighbourKey);
                    assert(kCacheEntry->kNeighbourCost == 1);

                    /* Link the Neighbours */
                    kValidNeighbours[0]->kNeighbourCost  += kBufferedNeighbourCost[1];
                    kValidNeighbours[1]->kNeighbourCost  += kBufferedNeighbourCost[0];

                    kValidNeighbours[0]->kNeighbourPos[0] = kBufferedNeighbourPos[1][0];
                    kValidNeighbours[0]->kNeighbourPos[1] = kBufferedNeighbourPos[1][1];
                    kValidNeighbours[0]->kNeighbourKey    = kBufferedNeighbourKey[1];

                    kValidNeighbours[1]->kNeighbourPos[0] = kBufferedNeighbourPos[0][0];
                    kValidNeighbours[1]->kNeighbourPos[1] = kBufferedNeighbourPos[0][1];
                    kValidNeighbours[1]->kNeighbourKey    = kBufferedNeighbourKey[0];

                    /* Note: We don't actually need to delete the Node itself from the original
                     *       list.  As it will naturally be bypassed since intersections will
                     *       only jump to other intersections / unidirectional nodes.
                     */

                    /* Delete the Cache Entry */
                    kCacheEntry->kNeighbourCost = nCacheDeleted;

                    /* Indicate a Node has been Culled */
                    bNodesCulled = AOC_TRUE;
                }
            }
        }
    } while (bNodesCulled);
}

uint64_t findLongestRoute(const uint64_t nPositionKey, const uint64_t nEndKey, const uint64_t nMaxSteps, const uint64_t nSteps, const SNeighboursType* kNodes, unsigned* bVisited)
{
    const SNeighboursType* kNode = &kNodes[nPositionKey];
    uint64_t nLocalMaxSteps;
    size_t   nNeighbour;

    /* Determine if we've reached the End */
    if (nPositionKey == nEndKey)
    {
        if (nSteps > nMaxSteps)
        {
            return nSteps;
        }
        else
        {
            return nMaxSteps;
        }
    }

    /* Buffer the Max Steps */
    nLocalMaxSteps = nMaxSteps;

    /* Iterate through the Neighbours */
    for (nNeighbour = 0; nNeighbour < kNodes[nPositionKey].nNeighbourCount; ++nNeighbour)
    {
        /* Ignore the Neighbour if we've seen it */
        if (bVisited[kNode->kNeighbours[nNeighbour].kNeighbourKey])
        {
            continue;
        }

        /* Mark the Current Neighbour as Visited and Recurse */
        bVisited[kNode->kNeighbours[nNeighbour].kNeighbourKey] = AOC_TRUE;
        nLocalMaxSteps = findLongestRoute(kNode->kNeighbours[nNeighbour].kNeighbourKey, nEndKey, nLocalMaxSteps, nSteps + kNode->kNeighbours[nNeighbour].kNeighbourCost, kNodes, bVisited);
        bVisited[kNode->kNeighbours[nNeighbour].kNeighbourKey] = AOC_FALSE;
    }

    return nLocalMaxSteps;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nHeight;
        size_t                  nWidth;
        size_t                  nY;

        SValidDirectionsType    kValidDirectionsPartOne[128] = {0};
        SValidDirectionsType    kValidDirectionsPartTwo[128] = {0};

        int64_t                 nStartX;
        int64_t                 nStartY;
        int64_t                 nEndX;
        int64_t                 nEndY;

        SNeighboursType*        kNodesPartOne;
        SNeighboursType*        kNodesPartTwo;
        unsigned*               kVisitedPartOne;
        unsigned*               kVisitedPartTwo;
        SNeighbourType*         kCache;

        /* Assign Valid Directions */

        /* Dot */
        kValidDirectionsPartOne['.'].kValidDirections[0][0] = -1;
        kValidDirectionsPartOne['.'].kValidDirections[0][1] = 0;
        kValidDirectionsPartOne['.'].kValidDirections[1][0] = 1;
        kValidDirectionsPartOne['.'].kValidDirections[1][1] = 0;
        kValidDirectionsPartOne['.'].kValidDirections[2][0] = 0;
        kValidDirectionsPartOne['.'].kValidDirections[2][1] = -1;
        kValidDirectionsPartOne['.'].kValidDirections[3][0] = 0;
        kValidDirectionsPartOne['.'].kValidDirections[3][1] = 1;
        kValidDirectionsPartOne['.'].nValidDirectionsCount  = 4;
        kValidDirectionsPartTwo['.'] = kValidDirectionsPartOne['.'];

        /* Up */
        kValidDirectionsPartOne['^'].kValidDirections[0][0] = -1;
        kValidDirectionsPartOne['^'].kValidDirections[0][1] = 0;
        kValidDirectionsPartOne['^'].nValidDirectionsCount  = 1;
        kValidDirectionsPartTwo['^'] = kValidDirectionsPartOne['.'];

        /* Right */
        kValidDirectionsPartOne['>'].kValidDirections[0][0] = 0;
        kValidDirectionsPartOne['>'].kValidDirections[0][1] = 1;
        kValidDirectionsPartOne['>'].nValidDirectionsCount  = 1;
        kValidDirectionsPartTwo['>'] = kValidDirectionsPartOne['.'];

        /* Down */
        kValidDirectionsPartOne['v'].kValidDirections[0][0] = 1;
        kValidDirectionsPartOne['v'].kValidDirections[0][1] = 0;
        kValidDirectionsPartOne['v'].nValidDirectionsCount  = 1;
        kValidDirectionsPartTwo['v'] = kValidDirectionsPartOne['.'];

        /* Left */
        kValidDirectionsPartOne['<'].kValidDirections[0][0] = 0;
        kValidDirectionsPartOne['<'].kValidDirections[0][1] = -1;
        kValidDirectionsPartOne['<'].nValidDirectionsCount  = 1;
        kValidDirectionsPartTwo['<'] = kValidDirectionsPartOne['.'];

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /* Allocate the Node Memory */
        kNodesPartOne   = (SNeighboursType*)calloc(nWidth * nHeight, sizeof(SNeighboursType));
        kNodesPartTwo   = (SNeighboursType*)calloc(nWidth * nHeight, sizeof(SNeighboursType));
        kVisitedPartOne = (unsigned*)calloc(nWidth * nHeight, sizeof(unsigned));
        kVisitedPartTwo = (unsigned*)calloc(nWidth * nHeight, sizeof(unsigned));
        kCache          = (SNeighbourType*)malloc(nWidth * nHeight * sizeof(SNeighbourType));

        /* Assume Known Positions for Start/End */
        nStartX = 1;
        nStartY = 0;
        nEndX   = (int64_t)nWidth - 2;
        nEndY   = (int64_t)nHeight - 1;
        assert(kLines[nStartY][nStartX] == '.');
        assert(kLines[nEndY][nEndX]     == '.');

        /* Part 1 */
        generateNodes((const char**)kLines, nWidth, nHeight, kValidDirectionsPartOne, kNodesPartOne, kCache);
        printf("Part 1: %llu\n", findLongestRoute(ENCODE_KEY(nStartX, nStartY, nWidth), ENCODE_KEY(nEndX, nEndY, nWidth), 0, 0, kNodesPartOne, kVisitedPartOne));

        /* Part 2 */
        generateNodes((const char**)kLines, nWidth, nHeight, kValidDirectionsPartTwo, kNodesPartTwo, kCache);
        printf("Part 2: %llu\n", findLongestRoute(ENCODE_KEY(nStartX, nStartY, nWidth), ENCODE_KEY(nEndX, nEndY, nWidth), 0, 0, kNodesPartTwo, kVisitedPartTwo));

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kNodesPartOne);
        free(kNodesPartTwo);
        free(kVisitedPartOne);
        free(kVisitedPartTwo);
        free(kCache);
    }

    return 0;
}