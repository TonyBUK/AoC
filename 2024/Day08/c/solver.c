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

typedef struct Antenna
{
    unsigned    nX;
    unsigned    nY;
} Antenna;

typedef struct Antennas
{
    size_t      nAntennaCount;
    Antenna*    kAntennas;
} Antennas;

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

void flagAntiNodesForAntenna(unsigned* kAntiNodes, const int64_t nAntiNodeX, const int64_t nAntiNodeY, const int64_t nAntiNodeDeltaX, const int64_t nAntiNodeDeltaY, const size_t nWidth, const size_t nHeight, const unsigned bAnyDistance)
{
    int64_t nX = nAntiNodeX;
    int64_t nY = nAntiNodeY;

    while (1)
    {
        nX += nAntiNodeDeltaX;
        nY += nAntiNodeDeltaY;

        if (nX >= 0 && nX < nWidth && nY >= 0 && nY < nHeight)
        {
            kAntiNodes[TO_GRID(nX, nY, nWidth)] = AOC_TRUE;
        }
        else
        {
            break;
        }

        if (AOC_FALSE == bAnyDistance)
        {
            break;
        }
    }
}

void flagAntiNodes(unsigned* kAntiNodes, const Antenna* kAntenna1, const Antenna* kAntenna2, const size_t nWidth, const size_t nHeight, const unsigned bAnyDistance)
{
    const int64_t kAntenna1X = (int64_t)kAntenna1->nX;
    const int64_t kAntenna1Y = (int64_t)kAntenna1->nY;
    const int64_t kAntenna2X = (int64_t)kAntenna2->nX;
    const int64_t kAntenna2Y = (int64_t)kAntenna2->nY;

    const int64_t nDeltaX    = kAntenna2X - kAntenna1X;
    const int64_t nDeltaY    = kAntenna2Y - kAntenna1Y;

    if (bAnyDistance)
    {
        kAntiNodes[TO_GRID(kAntenna1X, kAntenna1Y, nWidth)] = AOC_TRUE;
        kAntiNodes[TO_GRID(kAntenna2X, kAntenna2Y, nWidth)] = AOC_TRUE;
    }

    flagAntiNodesForAntenna(kAntiNodes, kAntenna1X, kAntenna1Y, -nDeltaX, -nDeltaY, nWidth, nHeight, bAnyDistance);
    flagAntiNodesForAntenna(kAntiNodes, kAntenna2X, kAntenna2Y,  nDeltaX,  nDeltaY, nWidth, nHeight, bAnyDistance);
}

uint64_t getAntiNodeCount(const unsigned* const kAntiNodes, const size_t nWidth, const size_t nHeight)
{
    uint64_t nAntiNodeCount = 0;
    size_t   X, Y;

    for (Y = 0; Y < nHeight; ++Y)
    {
        for (X = 0; X < nWidth; ++X)
        {
            if (kAntiNodes[TO_GRID(X, Y, nWidth)])
            {
                ++nAntiNodeCount;
            }
        }
    }

    return nAntiNodeCount;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        char*                       kBuffer;
        char**                      kAntennaBuffer;

        Antennas*                   kAntennas;
        char*                       kUniqueAntennas;
        size_t                      nUniqueAntennaCount = 0;

        size_t                      nAntenna;

        size_t                      nHeight;
        size_t                      nWidth;

        size_t                      nY;

        unsigned*                   kAntiNodesPartOne;
        unsigned*                   kAntiNodesPartTwo;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kAntennaBuffer, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        /*
         * Because the input is ASCII, we can be wasteful and just assign our "hashing" area to
         * non-extended ASCII (0 .. 127).
         */
        kUniqueAntennas = (char*)calloc(128, sizeof(char));
        kAntennas       = (Antennas*)calloc(128, sizeof(Antennas));

        /* Allocate the Antinode Buffer (grid copies), since only one anti-node can occupy a given position */
        kAntiNodesPartOne = (unsigned*)calloc(nHeight * nWidth, sizeof(unsigned));
        kAntiNodesPartTwo = (unsigned*)calloc(nHeight * nWidth, sizeof(unsigned));

        /* Find the Antenna */
        for (nY = 0; nY < nHeight; ++nY)
        {
            size_t nX;
            for (nX = 0; nX < nWidth; ++nX)
            {
                const size_t nAntennaType = (size_t)kAntennaBuffer[nY][nX];
                if (isalnum(nAntennaType))
                {
                    if (kAntennas[nAntennaType].nAntennaCount == 0)
                    {
                        kAntennas[nAntennaType].kAntennas      = (Antenna*)malloc(nWidth * nHeight * sizeof(Antenna));
                        kUniqueAntennas[nUniqueAntennaCount++] = nAntennaType;
                    }
                    kAntennas[nAntennaType].kAntennas[kAntennas[nAntennaType].nAntennaCount  ].nX = nX;
                    kAntennas[nAntennaType].kAntennas[kAntennas[nAntennaType].nAntennaCount++].nY = nY;
                }
            }
        }

        for (nAntenna = 0; nAntenna < nUniqueAntennaCount; ++nAntenna)
        {
            const size_t nAntennaType = kUniqueAntennas[nAntenna];
            size_t nAntenna1;
            for (nAntenna1 = 0; nAntenna1 < kAntennas[nAntennaType].nAntennaCount; ++nAntenna1)
            {
                size_t nAntenna2;
                for (nAntenna2 = nAntenna1 + 1; nAntenna2 < kAntennas[nAntennaType].nAntennaCount; ++nAntenna2)
                {
                    flagAntiNodes(kAntiNodesPartOne,
                                  &kAntennas[nAntennaType].kAntennas[nAntenna1],
                                  &kAntennas[nAntennaType].kAntennas[nAntenna2],
                                  nWidth,
                                  nHeight,
                                  AOC_FALSE);

                    flagAntiNodes(kAntiNodesPartTwo,
                                  &kAntennas[nAntennaType].kAntennas[nAntenna1],
                                  &kAntennas[nAntennaType].kAntennas[nAntenna2],
                                  nWidth,
                                  nHeight,
                                  AOC_TRUE);
                }
            }
        }

        printf("Part 1: %" PRIu64 "\n", getAntiNodeCount(kAntiNodesPartOne, nWidth, nHeight));
        printf("Part 2: %" PRIu64 "\n", getAntiNodeCount(kAntiNodesPartTwo, nWidth, nHeight));

        /* Free any Allocated Memory */
        for (nAntenna = 0; nAntenna < nUniqueAntennaCount; ++nAntenna)
        {
            free(kAntennas[kUniqueAntennas[nAntenna]].kAntennas);
        }
        free(kAntennas);
        free(kUniqueAntennas);
        free(kAntennaBuffer);
        free(kBuffer);
        free(kAntiNodesPartOne);
        free(kAntiNodesPartTwo);
    }

    return 0;
}