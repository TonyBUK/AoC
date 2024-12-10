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
 
void findTrail(char** kHikingTrail, const size_t nWidth, const size_t nHeight, const size_t nX, const size_t nY, const char kTrailHeight, unsigned* kSeenTrailEnds, uint64_t* nTrailHeadCount, uint64_t* nDestinctRouteCount)
{
    const int64_t NEIGHBOURS[4][2] =
    {
        { 0,-1}, /* Up */
        { 1, 0}, /* Right */
        { 0, 1}, /* Down */
        {-1, 0}  /* Left */
    };
 
    size_t nNeighbour;
 
    /* If we've reached height 9, add to the list of unique end points, and distinct routes */
    if ('9' == kTrailHeight)
    {
        const size_t nGridPos = TO_GRID(nX, nY, nWidth);
        if (AOC_FALSE == kSeenTrailEnds[nGridPos])
        {
            kSeenTrailEnds[nGridPos] = AOC_TRUE;
            *nTrailHeadCount += 1;
        }
        *nDestinctRouteCount += 1;

        return;
    }

    /* Use DFS to find all the distinct routes */
    for (nNeighbour = 0; nNeighbour < (sizeof(NEIGHBOURS) / sizeof(NEIGHBOURS[0])); ++nNeighbour)
    {
        const int64_t nDeltaX = (int64_t)nX + NEIGHBOURS[nNeighbour][0];
        const int64_t nDeltaY = (int64_t)nY + NEIGHBOURS[nNeighbour][1];
 
        if ((nDeltaX >= 0) && (nDeltaX < (int64_t)nWidth) && (nDeltaY >= 0) && (nDeltaY < (int64_t)nHeight))
        {
            if (kHikingTrail[nDeltaY][nDeltaX] == (kTrailHeight + 1))
            {
                findTrail(kHikingTrail, nWidth, nHeight, (const size_t) nDeltaX, (const size_t) nDeltaY, kTrailHeight + 1, kSeenTrailEnds, nTrailHeadCount, nDestinctRouteCount);
            }
        }
    }
}
 
int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kBuffer;
        char**                      kHikingTrail;
 
        size_t                      nHeight;
        size_t                      nWidth;
 
        unsigned*                   kSeenTrailEnds;
        size_t                      nY;
 
        uint64_t                    nTrailHeadCount     = 0;
        uint64_t                    nDestinctRouteCount = 0;
 
        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kHikingTrail, &nHeight, NULL, &nWidth, 0);
        fclose(pData);
 
        kSeenTrailEnds = (unsigned*)malloc(nHeight * nWidth * sizeof(unsigned));
 
        /* # Find all the Zero Height Trail Heads */
        for (nY = 0; nY < nHeight; ++nY)
        {
            size_t nX;
 
            for (nX = 0; nX < nWidth; ++nX)
            {
                /* '0' - '9' are also in ascending order, so no need to convert to int */
                if ('0' == kHikingTrail[nY][nX])
                {
                    /* Clear the Seen Buffer */
                    memset(kSeenTrailEnds, 0, nHeight * nWidth * sizeof(unsigned));
                    findTrail(kHikingTrail, nWidth, nHeight, nX, nY, '0', kSeenTrailEnds, &nTrailHeadCount, &nDestinctRouteCount);
                }
            }
        }
 
        printf("Part 1: %" PRIu64 "\n", nTrailHeadCount);
        printf("Part 2: %" PRIu64 "\n", nDestinctRouteCount);
 
        /* Free any Allocated Memory */
        free(kHikingTrail);
        free(kBuffer);
        free(kSeenTrailEnds);
    }
 
    return 0;
}