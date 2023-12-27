#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE            (1u)
#define AOC_FALSE           (0u)

#define CHAR_KEY_COUNT      (3u)
#define CHAR_KEY_WIDTH      (5u)

#define CHAR_1_WIDTH        (CHAR_KEY_WIDTH)
#define CHAR_2_WIDTH        (CHAR_KEY_WIDTH)
#define CHAR_3_WIDTH        (CHAR_KEY_WIDTH)

#define CHAR_1_SHIFT        (0u)
#define CHAR_2_SHIFT        (CHAR_1_SHIFT + CHAR_1_WIDTH)
#define CHAR_3_SHIFT        (CHAR_2_SHIFT + CHAR_2_WIDTH)

#define CHAR_KEY_SIZE       (1u << (CHAR_1_WIDTH + CHAR_2_WIDTH + CHAR_3_WIDTH))

#define ENCODE_KEY(a,b,c)   (((a) << CHAR_1_SHIFT) | ((b) << CHAR_2_SHIFT) | ((c) << CHAR_3_SHIFT))
#define DECODE_KEY_1(a)     (char)((((a) >> CHAR_1_SHIFT) & ((1u << CHAR_1_WIDTH) - 1)) + 'a')
#define DECODE_KEY_2(b)     (char)((((b) >> CHAR_2_SHIFT) & ((1u << CHAR_2_WIDTH) - 1)) + 'a')
#define DECODE_KEY_3(c)     (char)((((c) >> CHAR_3_SHIFT) & ((1u << CHAR_3_WIDTH) - 1)) + 'a')

typedef struct SWiringType
{
    size_t      nCount;
    size_t      nMaxSize;
    uint32_t*   kConnections;
} SWiringType;

typedef struct SEdgeType
{
    uint32_t    kEdge[2];
} SEdgeType;

typedef struct SGraphType
{
    size_t      nEdgeCount;
    size_t      nMaxEdgeCount;
    SEdgeType** kEdges;
} SGraphType;

typedef struct SGraphOfEdgesType
{
    size_t      nNodeCount;
    size_t      nMaxNodeCount;
    SEdgeType** kNodes;
} SGraphOfEdgesType;

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

uint32_t encodeKey(const char* kKey)
{
    size_t   n;
    uint32_t nKey = 0;

    for (n = 0; n < CHAR_KEY_COUNT; ++n)
    {
        if (kKey[n] < 'a' || kKey[n] > 'z')
        {
            assert(0);
        }

        nKey |= (kKey[n] - 'a') << (n * CHAR_KEY_WIDTH);
    }

    return nKey;
}

void createEdges(const SWiringType* kWiring, const uint32_t* kWires, const size_t nWireCount, SEdgeType* kEdgeBuffer, size_t* nEdgeCount, const size_t nMaxEdgeSize)
{
    size_t nLocalEdgeCount = 0;
    size_t nWire;

    for (nWire = 0; nWire < nWireCount; ++nWire)
    {
        const uint32_t      nWireKey = kWires[nWire];
        const SWiringType*  pWire    = &kWiring[nWireKey];
        size_t              nNeighbour;

        for (nNeighbour = 0; nNeighbour < pWire->nCount; ++nNeighbour)
        {
            const uint32_t nNeighbourKey = pWire->kConnections[nNeighbour];

            /* This pains me... Filter duplicate edges. */
            size_t    nEdge;
            unsigned  bFound        = AOC_FALSE;
            for (nEdge = 0; nEdge < nLocalEdgeCount; ++nEdge)
            {
                if (kEdgeBuffer[nEdge].kEdge[0] == nWireKey && kEdgeBuffer[nEdge].kEdge[1] == nNeighbourKey)
                {
                    bFound = AOC_TRUE;
                    break;
                }
                else if (kEdgeBuffer[nEdge].kEdge[0] == nNeighbourKey && kEdgeBuffer[nEdge].kEdge[1] == nWireKey)
                {
                    bFound = AOC_TRUE;
                    break;
                }
            }

            if (bFound)
            {
                continue;
            }

            kEdgeBuffer[nLocalEdgeCount].kEdge[0] = nWireKey;
            kEdgeBuffer[nLocalEdgeCount].kEdge[1] = nNeighbourKey;
            ++nLocalEdgeCount;
        }
    }

    *nEdgeCount = nLocalEdgeCount;
}

uint64_t createNodes(SEdgeType* kEdges, const size_t nEdgeCount, SGraphOfEdgesType* kGraphOfEdges, uint64_t* kGraphOfEdgesSizes, const size_t nWireCount)
{
    size_t   nEdge;
    size_t   nNodeCount = 0;

    for (nEdge = 0; nEdge < nEdgeCount; ++nEdge)
    {
        size_t nNode;

        for (nNode = 0; nNode < 2; ++nNode)
        {
            const uint32_t nNodeKey = kEdges[nEdge].kEdge[nNode];
            kGraphOfEdges[nNodeKey].nNodeCount = 0;
            kGraphOfEdgesSizes[nNodeKey]        = 1;
        }
    }

    for (nEdge = 0; nEdge < nEdgeCount; ++nEdge)
    {
        size_t nNode;

        for (nNode = 0; nNode < 2; ++nNode)
        {
            const uint32_t nNodeKey = kEdges[nEdge].kEdge[nNode];
            if (kGraphOfEdges[nNodeKey].nMaxNodeCount == 0)
            {
                kGraphOfEdges[nNodeKey].nNodeCount    = 0;
                kGraphOfEdges[nNodeKey].nMaxNodeCount = nWireCount;
                kGraphOfEdges[nNodeKey].kNodes        = (SEdgeType**)malloc(nWireCount * sizeof(SEdgeType*));
                ++nNodeCount;
            }
            kGraphOfEdges[nNodeKey].kNodes[kGraphOfEdges[nNodeKey].nNodeCount] = &kEdges[nEdge];
            ++kGraphOfEdges[nNodeKey].nNodeCount;
        }
    }    
    return nNodeCount;
}

void shuffleEdges(SEdgeType* kEdges, const size_t nEdgeCount)
{
    size_t nEdge;

    for (nEdge = 0; nEdge < nEdgeCount; ++nEdge)
    {
        const size_t nSwapEdge = rand() % nEdgeCount;
        SEdgeType    kTempEdge;

        kTempEdge         = kEdges[nEdge];
        kEdges[nEdge]     = kEdges[nSwapEdge];
        kEdges[nSwapEdge] = kTempEdge;
    }
}

uint32_t mergeNodes(SEdgeType* kEdges, const size_t nInitialEdgeCount, SGraphOfEdgesType* kGraphOfEdges, uint64_t* kGraphOfEdgesSizes, const size_t nInitialWireCount)
{
    size_t   nWireCount = nInitialWireCount;
    size_t   nEdgeCount = nInitialEdgeCount;

    uint32_t nLastLeft;

    while (nWireCount > 2)
    {
        const SEdgeType* pEdge      = &kEdges[nEdgeCount - 1];
        const uint32_t   nLeft      = pEdge->kEdge[0];
        const uint32_t   nRight     = pEdge->kEdge[1];
              size_t     nCompactedNode;
              size_t     nOtherNode;
              size_t     nEdge;
        --nEdgeCount;

        /* Skip Self Loops which are created by the merge process */
        if (nLeft == nRight)
        {
            continue;
        }

        /* Merge the nodes from Node B into Node A
         * Note: This may inadventently create a Self Loop which we'll deal with...
         */
        for (nOtherNode = 0; nOtherNode < kGraphOfEdges[nRight].nNodeCount; ++nOtherNode)
        {
            SEdgeType* pEdgeToMerge = kGraphOfEdges[nRight].kNodes[nOtherNode];
            size_t     nEdgeToMerge;

            if (pEdgeToMerge->kEdge[0] == nRight)
            {
                pEdgeToMerge->kEdge[0] = nLeft;
            }
            if (pEdgeToMerge->kEdge[1] == nRight)
            {
                pEdgeToMerge->kEdge[1] = nLeft;
            }
            kGraphOfEdges[nLeft].kNodes[kGraphOfEdges[nLeft].nNodeCount] = pEdgeToMerge;
            ++kGraphOfEdges[nLeft].nNodeCount;
        }

        assert(kGraphOfEdgesSizes[nRight] > 0);

        /* Remove Node B from the Graph and ensure Node A now includes Node B's size. */
        kGraphOfEdgesSizes[nLeft]       += kGraphOfEdgesSizes[nRight];
        kGraphOfEdgesSizes[nRight]       = 0;
        kGraphOfEdges[nRight].nNodeCount = 0;

        /* Remove Self-Loops */
        nCompactedNode = 0;
        for (nOtherNode = 0; nOtherNode < kGraphOfEdges[nLeft].nNodeCount; ++nOtherNode)
        {
            if (kGraphOfEdges[nLeft].kNodes[nOtherNode]->kEdge[0] != kGraphOfEdges[nLeft].kNodes[nOtherNode]->kEdge[1])
            {
                kGraphOfEdges[nLeft].kNodes[nCompactedNode] = kGraphOfEdges[nLeft].kNodes[nOtherNode];
                ++nCompactedNode;
            }
        }
        kGraphOfEdges[nLeft].nNodeCount -= (nOtherNode - nCompactedNode);

        --nWireCount;
        nLastLeft = nLeft;
    }

    return nLastLeft;
}

uint64_t karger(const SWiringType* kWiring, const uint32_t* kWires, const size_t nWireCount, SEdgeType* kEdgeBuffer1, SEdgeType* kEdgeBuffer2, const size_t nMaxEdgeSize, SGraphOfEdgesType* kGraphOfEdges, uint64_t* kGraphOfEdgesSizes)
{
    size_t   nEdgeCount;
    uint32_t kGroupKey;
    size_t   nCurrentWireCount;
    unsigned bFirst = AOC_TRUE;

    /* Create the Edge Representation */
    createEdges(kWiring, kWires, nWireCount, kEdgeBuffer1, &nEdgeCount, nMaxEdgeSize);
    for (size_t nWire = 0; nWire < CHAR_KEY_SIZE; ++nWire)
    {
        kGraphOfEdgesSizes[nWire] = 0;
    }

    while (1)
    {
        /* Create the Nodes from the Edges */
        memcpy(kEdgeBuffer2, kEdgeBuffer1, nEdgeCount * sizeof(SEdgeType));

        /* The secret sauce, shuffle the Edges to change the starting point
        * of the merge process.
        */
        shuffleEdges(kEdgeBuffer2, nEdgeCount);
        if (bFirst)
        {
            nCurrentWireCount = createNodes(kEdgeBuffer2, nEdgeCount, kGraphOfEdges, kGraphOfEdgesSizes, nWireCount);
            bFirst            = AOC_FALSE;
        }
        else
        {
            createNodes(kEdgeBuffer2, nEdgeCount, kGraphOfEdges, kGraphOfEdgesSizes, nWireCount);
        }
        kGroupKey         = mergeNodes(kEdgeBuffer2, nEdgeCount, kGraphOfEdges, kGraphOfEdgesSizes, nCurrentWireCount);

        if (kGraphOfEdges[kGroupKey].nNodeCount == 3)
        {
            break;
        }
    }

    return kGraphOfEdgesSizes[kGraphOfEdges[kGroupKey].kNodes[0]->kEdge[0]] * 
           kGraphOfEdgesSizes[kGraphOfEdges[kGroupKey].kNodes[0]->kEdge[1]];
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                       kBuffer;
        char**                      kLines;
        size_t                      nLineCount;
        size_t                      nLine       = 0;
        size_t                      nWireCount  = 0;

        unsigned*                   bFoundKeys;
        SWiringType*                kWiring;
        uint32_t*                   kWires;
        SGraphOfEdgesType*          kGraphOfEdges;
        uint64_t*                   kGraphOfEdgesSizes;
        size_t                      nWireMaxCount;

        SEdgeType*                  kEdges;
        SEdgeType*                  kEdgesShuffled;
        size_t                      nMaxEdgeCount;

        /* Randomise the Seed
         *
         * Commented out for timing purposes.
         * 
         * Uncomment to unleash the full power of Karger's Algorithm.
         */
        /* srand(time(NULL)); */

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate a Buffer to let us count the wires */
        bFoundKeys         = (unsigned*)calloc(CHAR_KEY_SIZE, sizeof(unsigned));
        kWiring            = (SWiringType*)calloc(CHAR_KEY_SIZE, sizeof(SWiringType));
        kGraphOfEdges      = (SGraphOfEdgesType*)calloc(CHAR_KEY_SIZE, sizeof(SGraphOfEdgesType));
        kGraphOfEdgesSizes = (uint64_t*)malloc(CHAR_KEY_SIZE * sizeof(uint64_t));
        kWires             = (uint32_t*)malloc(nLineCount * sizeof(uint32_t));
        nWireMaxCount      = nLineCount;

        /* Ordinarily here's where I would allocate buffer sizes based on line count, but this has a wrinkle, the
         * input data set isn't necessarily complete, there's no guarantee that all the wires will be on the LHS,
         * so we need to count them first before allocating/populating.
         * 
         * C'mon AoC, it's meant to be Christmas!
         */
        for (nLine = 0; nLine < nLineCount; ++nLine)
        {
            const size_t   nWidth  = 3;
            const uint32_t nKey    = encodeKey(kLines[nLine]);
                  size_t   n       = 4;

            if (AOC_FALSE == bFoundKeys[nKey])
            {
                bFoundKeys[nKey]            = AOC_TRUE;
                kWires[nWireCount]          = nKey;
                kWiring[nKey].nCount        = 0;
                kWiring[nKey].nMaxSize      = nWireMaxCount;
                kWiring[nKey].kConnections  = (uint32_t*)malloc(nWireMaxCount * sizeof(uint32_t));
                ++nWireCount;

                if (nWireCount >= nWireMaxCount)
                {
                    nWireMaxCount *= 2;
                    kWires = (uint32_t*)realloc(kWires, nWireMaxCount * sizeof(uint32_t));
                }
            }

            while (kLines[nLine][n] != '\0')
            {
                const uint32_t nNeighbourKey = encodeKey(&kLines[nLine][n + 1]);
                if (AOC_FALSE == bFoundKeys[nNeighbourKey])
                {
                    bFoundKeys[nNeighbourKey]           = AOC_TRUE;
                    kWires[nWireCount]                  = nNeighbourKey;
                    kWiring[nNeighbourKey].nCount       = 0;
                    kWiring[nNeighbourKey].nMaxSize     = nWireMaxCount;
                    kWiring[nNeighbourKey].kConnections = (uint32_t*)malloc(nWireMaxCount * sizeof(uint32_t));
                    ++nWireCount;

                    if (nWireCount >= nWireMaxCount)
                    {
                        nWireMaxCount *= 2;
                        kWires = (uint32_t*)realloc(kWires, nWireMaxCount * sizeof(uint32_t));
                    }
                }

                /* Append the Source Key to the Neighbour */
                if (kWiring[nNeighbourKey].nCount >= kWiring[nNeighbourKey].nMaxSize)
                {
                    kWiring[nNeighbourKey].nMaxSize    *= 2;
                    kWiring[nNeighbourKey].kConnections = (uint32_t*)realloc(kWiring[nNeighbourKey].kConnections, kWiring[nNeighbourKey].nMaxSize * sizeof(uint32_t));
                }
                kWiring[nNeighbourKey].kConnections[kWiring[nNeighbourKey].nCount] = nKey;
                ++kWiring[nNeighbourKey].nCount;

                /* Append the Neighbour to the Source Key */
                if (kWiring[nKey].nCount >= kWiring[nKey].nMaxSize)
                {
                    kWiring[nKey].nMaxSize    *= 2;
                    kWiring[nKey].kConnections = (uint32_t*)realloc(kWiring[nKey].kConnections, kWiring[nKey].nMaxSize * sizeof(uint32_t));
                }
                kWiring[nKey].kConnections[kWiring[nKey].nCount] = nNeighbourKey;
                ++kWiring[nKey].nCount;

                n += nWidth + 1;
            }
        }

        /* Allocate the Buffer for the Edges.  Go worst case */
        nMaxEdgeCount = nWireCount * nWireCount;
        kEdges         = (SEdgeType*)malloc(nMaxEdgeCount * sizeof(SEdgeType));
        kEdgesShuffled = (SEdgeType*)malloc(nMaxEdgeCount * sizeof(SEdgeType));

        printf("Part 1: %llu\n", karger(kWiring, kWires, nWireCount, kEdges, kEdgesShuffled, nMaxEdgeCount, kGraphOfEdges, kGraphOfEdgesSizes));

        /* Free any Allocated Memory */
        for (nLine = 0; nLine < nWireCount; ++nLine)
        {
            assert(kWiring[kWires[nLine]].nMaxSize > 0);
            free(kWiring[kWires[nLine]].kConnections);

            if (kGraphOfEdges[kWires[nLine]].nMaxNodeCount > 0)
            {
                free(kGraphOfEdges[kWires[nLine]].kNodes);
            }
        }
        free(kBuffer);
        free(kLines);
        free(bFoundKeys);
        free(kWires);
        free(kWiring);
        free(kGraphOfEdges);
        free(kEdges);
        free(kEdgesShuffled);
    }

    return 0;
}