#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
 
#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

#define ACTION_UP         (1u << 0u)
#define ACTION_RIGHT      (1u << 1u)
#define ACTION_DOWN       (1u << 2u)
#define ACTION_LEFT       (1u << 3u)
#define ACTION_UP_DOWN    (1u << 4u)
#define ACTION_LEFT_RIGHT (1u << 5u)

typedef enum EDirections
{
    EDIRECTION_UP    = ACTION_UP,
    EDIRECTION_DOWN  = ACTION_DOWN,
    EDIRECTION_LEFT  = ACTION_LEFT,
    EDIRECTION_RIGHT = ACTION_RIGHT
} EDirections;

typedef struct SQueueEntry
{
    int         nX;
    int         nY;
    EDirections eDirection;
} SQueueEntry;

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

void addBeamForProcessing(const size_t nLightBeamY, const size_t nLightBeamX, const EDirections eLightBeamDirection, const size_t nWidth, const size_t nHeight, size_t* pQueueSize, SQueueEntry* kQueue, const uint16_t* kCache)
{
    /* Check if the Light Beam is in the Grid */
    if ((nLightBeamY >= 0) &&
        (nLightBeamY < nHeight) &&
        (nLightBeamX >= 0) &&
        (nLightBeamX < nWidth))
    {
        /* Check if the Light Beam is in the Cache
         * If so, then we've hit a loop, and can stop
         */
        if (0 == (kCache[(nLightBeamY * nWidth) + nLightBeamX] & eLightBeamDirection))
        {
            kQueue[*pQueueSize].nX         = nLightBeamX;
            kQueue[*pQueueSize].nY         = nLightBeamY;
            kQueue[*pQueueSize].eDirection = eLightBeamDirection;
            *pQueueSize = *pQueueSize + 1;
        }
    }
}

uint64_t getMirrorAction(const char kMirror, const EDirections eDirection)
{
    switch (kMirror)
    {
        case '/':
        {
            switch (eDirection)
            {
                case EDIRECTION_UP:    return ACTION_RIGHT;
                case EDIRECTION_DOWN:  return ACTION_LEFT;
                case EDIRECTION_LEFT:  return ACTION_DOWN;
                case EDIRECTION_RIGHT: return ACTION_UP;
            }
        } break;

        case '\\':
        {
            switch (eDirection)
            {
                case EDIRECTION_UP:    return ACTION_LEFT;
                case EDIRECTION_DOWN:  return ACTION_RIGHT;
                case EDIRECTION_LEFT:  return ACTION_UP;
                case EDIRECTION_RIGHT: return ACTION_DOWN;
            }
        } break;

        case '-':
        {
            switch (eDirection)
            {
                case EDIRECTION_UP:    return ACTION_LEFT_RIGHT;
                case EDIRECTION_DOWN:  return ACTION_LEFT_RIGHT;
                default:               return eDirection;
            }
        } break;

        case '|':
        {
            switch (eDirection)
            {
                case EDIRECTION_LEFT:  return ACTION_UP_DOWN;
                case EDIRECTION_RIGHT: return ACTION_UP_DOWN;
                default:               return eDirection;
            }
        } break;
        

        default: break;
    }

    assert(0);
}

uint64_t processMirrors(const char** kMirrors, const size_t nWidth, const size_t nHeight, const size_t nInitialX, const size_t nInitialY, const EDirections eInitialDirection, unsigned* kEnergy, uint16_t* kCache, SQueueEntry* kQueue)
{
    const int kDirections[9][2] =
    {
        {  0,  0}, /* None */
        { -1,  0}, /* EDIRECTION_UP */
        {  0,  1}, /* EDIRECTION_RIGHT */
        {  0,  0}, /* None */
        {  1,  0}, /* EDIRECTION_DOWN */
        {  0,  0}, /* None */
        {  0,  0}, /* None */
        {  0,  0}, /* None */
        {  0, -1}  /* EDIRECTION_LEFT */
    };

    uint64_t    nEnergy     = 0;
    size_t      nQueueSize  = 1;
    size_t      nPos;

    /* Reset the Energy/Cache */
    memset(kEnergy, 0, nWidth * nHeight * sizeof(unsigned));
    memset(kCache,  0, nWidth * nHeight * sizeof(uint16_t));

    kQueue[0].nX         = nInitialX;
    kQueue[0].nY         = nInitialY;
    kQueue[0].eDirection = eInitialDirection;

    while (nQueueSize > 0)
    {
        /* Get the Light Beam */
        const SQueueEntry*  pEntry  = &kQueue[nQueueSize - 1];
        const char          kMirror = kMirrors[pEntry->nY][pEntry->nX];
        --nQueueSize;

        nPos = (pEntry->nY * nWidth) + pEntry->nX;

        /* Indicate the Grid is now Energised */
        kEnergy[nPos] = AOC_TRUE;

        /* Add the Light Beam to the Cache */
        kCache[nPos] |= pEntry->eDirection;

        /* Calculate the Next Position */
        if (kMirror != '.')
        {
            const uint64_t nMirrorAction = getMirrorAction(kMirror, pEntry->eDirection);

            switch (nMirrorAction)
            {
                case ACTION_UP:
                case ACTION_DOWN:
                case ACTION_LEFT:
                case ACTION_RIGHT:
                {
                    addBeamForProcessing(pEntry->nY + kDirections[nMirrorAction][0],
                                         pEntry->nX + kDirections[nMirrorAction][1],
                                         (EDirections)nMirrorAction,
                                         nWidth,
                                         nHeight,
                                         &nQueueSize,
                                         kQueue,
                                         kCache);
                } break;
                case ACTION_UP_DOWN:
                {
                    addBeamForProcessing(pEntry->nY + kDirections[ACTION_UP][0],
                                         pEntry->nX + kDirections[ACTION_UP][1],
                                         (EDirections)ACTION_UP,
                                         nWidth,
                                         nHeight,
                                         &nQueueSize,
                                         kQueue,
                                         kCache);
                    addBeamForProcessing(pEntry->nY + kDirections[ACTION_DOWN][0],
                                         pEntry->nX + kDirections[ACTION_DOWN][1],
                                         (EDirections)ACTION_DOWN,
                                         nWidth,
                                         nHeight,
                                         &nQueueSize,
                                         kQueue,
                                         kCache);
                } break;
                case ACTION_LEFT_RIGHT:
                {
                    addBeamForProcessing(pEntry->nY + kDirections[ACTION_LEFT][0],
                                         pEntry->nX + kDirections[ACTION_LEFT][1],
                                         (EDirections)ACTION_LEFT,
                                         nWidth,
                                         nHeight,
                                         &nQueueSize,
                                         kQueue,
                                         kCache);
                    addBeamForProcessing(pEntry->nY + kDirections[ACTION_RIGHT][0],
                                         pEntry->nX + kDirections[ACTION_RIGHT][1],
                                         (EDirections)ACTION_RIGHT,
                                         nWidth,
                                         nHeight,
                                         &nQueueSize,
                                         kQueue,
                                         kCache);
                } break;
            }
        }
        else
        {
            /* Keep Moving in the Current Direction */
            addBeamForProcessing(pEntry->nY + kDirections[pEntry->eDirection][0],
                                 pEntry->nX + kDirections[pEntry->eDirection][1],
                                 pEntry->eDirection,
                                 nWidth,
                                 nHeight,
                                 &nQueueSize,
                                 kQueue,
                                 kCache);
        }
    }

    for (nPos = 0; nPos < (nHeight * nWidth); ++nPos)
    {
        nEnergy += kEnergy[nPos];
    }

    return nEnergy;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;
        size_t                  nMaxLineLength;

        unsigned*               kEnergy;
        uint16_t*               kCache;
        SQueueEntry*            kQueue;

        size_t                  nInitialX;
        size_t                  nInitialY;

        uint64_t                nMaxEnergy  = 0;
        uint64_t                nEnergy;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL, &nMaxLineLength, 0);
        fclose(pData);

        /* Allocate Memory for the Energy/Direction Arrays */
        kEnergy = (unsigned*)   malloc(nLineCount * nMaxLineLength * sizeof(unsigned));
        kCache  = (uint16_t*)   malloc(nLineCount * nMaxLineLength * sizeof(uint16_t));
        kQueue  = (SQueueEntry*)malloc(nLineCount * nMaxLineLength * sizeof(SQueueEntry));

        printf("Part 1: %llu\n", processMirrors((const char**)kLines, nMaxLineLength, nLineCount, 0, 0, EDIRECTION_RIGHT, kEnergy, kCache, kQueue));

        for (nInitialY = 0; nInitialY < nLineCount; ++nInitialY)
        {
            nEnergy = processMirrors((const char**)kLines, nMaxLineLength, nLineCount, 0, nInitialY, EDIRECTION_RIGHT, kEnergy, kCache, kQueue);
            if (nEnergy > nMaxEnergy) nMaxEnergy = nEnergy;
            nEnergy = processMirrors((const char**)kLines, nMaxLineLength, nLineCount, nMaxLineLength - 1, nInitialY, EDIRECTION_LEFT, kEnergy, kCache, kQueue);
            if (nEnergy > nMaxEnergy) nMaxEnergy = nEnergy;
        }

        for (nInitialX = 0; nInitialX < nMaxLineLength; ++nInitialX)
        {
            nEnergy = processMirrors((const char**)kLines, nMaxLineLength, nLineCount, nInitialX, 0, EDIRECTION_DOWN, kEnergy, kCache, kQueue);
            if (nEnergy > nMaxEnergy) nMaxEnergy = nEnergy;
            nEnergy = processMirrors((const char**)kLines, nMaxLineLength, nLineCount, nInitialX, nLineCount - 1, EDIRECTION_UP, kEnergy, kCache, kQueue);
            if (nEnergy > nMaxEnergy) nMaxEnergy = nEnergy;
        }

        printf("Part 2: %llu\n", nMaxEnergy);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kLines);
        free(kEnergy);
        free(kCache);
        free(kQueue);
    }
 
    return 0;
}