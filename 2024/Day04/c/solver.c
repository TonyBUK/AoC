#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define XMAS ("XMAS")
#define SAMX ("SAMX")
#define XMAS_SIZE (4)
#define MS   ("MS")
#define SM   ("SM")
#define MS_SIZE (2)

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

typedef struct SearchStrideType
{
    int nXStride;
    int nYStride;
} SearchStrideType;

void getSearchArea(const char** kWordSearch, const int X, const int Y, const SearchStrideType* kStride, const size_t nWidth, const size_t nHeight, char* kOutput, unsigned* bFound)
{
    int   nX      = X;
    int   nY      = Y;
    char* pOutput = kOutput;

    size_t nOutputLength = 0;
    for (size_t nCoordinate = 0; nCoordinate < XMAS_SIZE; ++nCoordinate)
    {
        /* We're only interested in full searches... */
        if ((nX < 0) || (nX >= (int)nWidth) || (nY < 0) || (nY >= (int)nHeight))
        {
            *bFound = AOC_FALSE;
            return;
        }

        *(pOutput++) = kWordSearch[nY][nX];

        /* Move the Pixels... */
        nX += kStride->nXStride;
        nY += kStride->nYStride;
    }

    *bFound = AOC_TRUE;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        const SearchStrideType SEARCH_STRIDES[4] = {
            { 1, 0}, /* Across */
            { 0, 1}, /* Down */
            {-1,-1}, /* Diagonal Backward */
            { 1,-1}  /* Diagonal Forward */
        };

        char*                       kBuffer;
        char**                      kWordSearch;
        size_t                      nHeight;
        size_t                      nWidth;
        size_t                      nLine       = 0;
        int                         X, Y;

        char                        kFound[XMAS_SIZE];

        uint64_t                    nPartOne    = 0;
        uint64_t                    nPartTwo    = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kWordSearch, &nHeight, NULL, &nWidth, 0);
        fclose(pData);

        for (Y = 0; Y < (int)nHeight; ++Y)
        {
            unsigned bYInBounds = (Y > 0) && (Y < (nHeight - 1));
            for (X = 0; X < (int)nWidth; ++X)
            {
                /*
                 * Part One
                 * ========
                 *
                 * Only Search if this is the Start or End of XMAS
                 */

                /* Note: By caching forward, we can constrain the inner search space. */
                const unsigned bForward  = kWordSearch[Y][X] == XMAS[0];
                if (bForward || (kWordSearch[Y][X] == SAMX[0]))
                {
                    size_t nCurrentSearch;

                    for (nCurrentSearch = 0; nCurrentSearch < (sizeof(SEARCH_STRIDES) / sizeof(SEARCH_STRIDES[0])); ++nCurrentSearch)
                    {
                        unsigned bFound;
                        getSearchArea((const char**)kWordSearch, X, Y, &SEARCH_STRIDES[nCurrentSearch], nWidth, nHeight, kFound, &bFound);

                        if (bFound)
                        {
                            if (bForward ? (strncmp(kFound, XMAS, XMAS_SIZE) == 0) : (strncmp(kFound, SAMX, XMAS_SIZE) == 0))
                            {
                                ++nPartOne;
                            }
                        }
                    }
                }

                /*
                 * Part Two
                 * ========
                 *
                 * We can constrain the overall search range to guarentee no out of bounds reads
                 * since a valid "A" will never be on the edges.
                 */
                if (bYInBounds && (X > 0) && (X < (nWidth - 1)))
                {
                    if (kWordSearch[Y][X] == 'A')
                    {
                        const char kPairOne[2] = {kWordSearch[Y-1][X-1], kWordSearch[Y+1][X+1]};
                        const char kPairTwo[2] = {kWordSearch[Y-1][X+1], kWordSearch[Y+1][X-1]};

                        if (((0 == strncmp(kPairOne, MS, MS_SIZE)) || (0 == strncmp(kPairOne, SM, MS_SIZE))) &&
                            ((0 == strncmp(kPairTwo, MS, MS_SIZE)) || (0 == strncmp(kPairTwo, SM, MS_SIZE))))
                        {
                            ++nPartTwo;
                        }
                    }
                }
            }
        }

        printf("Part 1: %" PRIu64 "\n", nPartOne);
        printf("Part 2: %" PRIu64 "\n", nPartTwo);

        /* Free any Allocated Memory */
        free(kBuffer);
        free(kWordSearch);
    }

    return 0;
}