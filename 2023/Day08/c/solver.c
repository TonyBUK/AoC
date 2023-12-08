#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

/* Pinky swear to keep KEY_SIZE and MAX_NODES consistent... BASE_SIZE**KEY_SIZE */
#define KEY_SIZE  (3)
#define BASE_SIZE (26)
#define MAX_NODES (BASE_SIZE * BASE_SIZE * BASE_SIZE)
#define encodeBaseDigit(kValue) (kValue - 'A')

typedef struct tNode
{
    uint64_t kNodes[2];
} tNode;

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength)
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
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';
    }
    else
    {
        (*pkFileBuffer)[nFileSize]   = '\0';
    }
 
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
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

/* Shamelessly stolen from the interwebs... */
uint64_t gcd(uint64_t a, uint64_t b)
{
    if (b == 0)
    {
        return a;
    }
    return gcd(b, a % b);
}
 
uint64_t findlcm(uint64_t* arr, size_t n)
{
    /* Initialize result */
    uint64_t ans = arr[0];
    uint64_t i;
 
    /* ans contains LCM of arr[0], ..arr[i]
     * after i'th iteration,
     */
    for (i = 1; i < n; i++)
    {
        ans = ((arr[i] * ans)) / (gcd(arr[i], ans));
    }
 
    return ans;
}

uint64_t encodeBaseValue(const char kValue[KEY_SIZE])
{
    uint64_t nValue = 0;
    size_t   i;

    for (i = 0; i < KEY_SIZE; ++i)
    {
        assert(kValue[i] >= 'A' && kValue[i] <= 'Z');
        nValue = (nValue * BASE_SIZE) + encodeBaseDigit(kValue[i]);
    }

    return nValue;
}
uint64_t findFirstTargetNode(const uint64_t nStartNode, const size_t* kSequence, const size_t nSequenceCount, const tNode* kNodes)
{
    const uint64_t  kGhostNodeType  = encodeBaseDigit('Z');
    uint64_t        nCurrentNode    = nStartNode;
    uint64_t        nSequenceSteps  = 0;
    uint64_t        nSequenceIndex  = 0;

    while (kGhostNodeType != (nCurrentNode % BASE_SIZE))
    {
        const size_t nDirection = kSequence[nSequenceIndex];

        nCurrentNode = kNodes[nCurrentNode].kNodes[nDirection];
        nSequenceIndex = (nSequenceIndex + 1) % nSequenceCount;
        ++nSequenceSteps;
    }

    return nSequenceSteps;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        const uint64_t          kStartNodeType  = encodeBaseDigit('A');
        const uint64_t          kGhostNodeType  = encodeBaseDigit('Z');

        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nLine;

        size_t*                 kSequence;
        size_t                  nSequenceCount = 0;
        size_t                  nSequenceIndex;

        tNode*                  kNodes;
        uint64_t*               kStartNodes;

        uint64_t                nCurrentNode;
        uint64_t                nTargetNode;

        uint64_t                nSequenceSteps;

        size_t                  nStartNodeCount = 0;
        size_t                  nEndNodeCount   = 0;

        size_t                  nCurrentNodeIndex;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pData);

        assert(nLineCount >= 2);

        nSequenceCount  = strlen(kLines[0]);
        kSequence       = (size_t*)malloc(strlen(kLines[0]) * sizeof(size_t));

        kNodes          = (tNode*)malloc(MAX_NODES * sizeof(tNode));
        kStartNodes     = (uint64_t*)malloc(nLineCount * sizeof(uint64_t));

        for (nLine = 0; nLine < nSequenceCount; ++nLine)
        {
            if ('L' == kLines[0][nLine])
            {
                kSequence[nLine] = 0;
            }
            else if ('R' == kLines[0][nLine])
            {
                kSequence[nLine] = 1;
            }
            else
            {
                assert(0);
            }
        }

        for (nLine = 1; nLine < nLineCount; ++nLine)
        {
            const uint64_t nNode  = encodeBaseValue(&kLines[nLine][0]);
            const uint64_t nLeft  = encodeBaseValue(strstr(kLines[nLine], "(")  + 1);
            const uint64_t nRight = encodeBaseValue(strstr(kLines[nLine], ", ") + 2);
            const uint64_t nType  = nNode % BASE_SIZE;

            /* Store the Node */
            kNodes[nNode].kNodes[0] = nLeft;
            kNodes[nNode].kNodes[1] = nRight;

            /* If this is a Start Node */
            if (kStartNodeType == nType)
            {
                kStartNodes[nStartNodeCount] = nNode;
                ++nStartNodeCount;
            }
        }

        /* Part 1: Follow the Route */
        nCurrentNode   = encodeBaseValue("AAA");
        nTargetNode    = encodeBaseValue("ZZZ");
        nSequenceSteps = 0;
        nSequenceIndex  = 0;

        while (nCurrentNode != nTargetNode)
        {
            const size_t nDirection = kSequence[nSequenceIndex];

            nCurrentNode = kNodes[nCurrentNode].kNodes[nDirection];
            nSequenceIndex = (nSequenceIndex + 1) % nSequenceCount;
            ++nSequenceSteps;
        }

        printf("Part 1: %llu\n", nSequenceSteps);

        /* Part 2: Ghost Routes */

        /* Analysis of multiple puzzle inputs shows each ghost loops, with no lead in, meaning the first time it encounters
         * any ghost node, it will eventually re-reach the exact same ghost node using the exact same number of steps.  This
         * greatly simplifies the problem, as we can just find the first ghost node for each starting point.
         *
         * If this doesn't work for your input, let me know!!!
         */
        for (nCurrentNodeIndex = 0; nCurrentNodeIndex < nStartNodeCount; ++nCurrentNodeIndex)
        {
            kStartNodes[nCurrentNodeIndex] = findFirstTargetNode(kStartNodes[nCurrentNodeIndex], kSequence, nSequenceCount, kNodes);
        }

        printf("Part 2: %llu\n", findlcm(kStartNodes, nStartNodeCount));

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kSequence);
        free(kNodes);
        free(kStartNodes);
    }
 
    return 0;
}