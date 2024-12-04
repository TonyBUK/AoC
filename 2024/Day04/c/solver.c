#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>

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

void getSearchArea(const char** kWordSearch, const size_t X, const size_t Y, const int kSearchArea[4][2], const size_t nSearchSize, const size_t nWidth, const size_t nHeight, char* kOutput, size_t* nOutputSize)
{
    size_t nOutputLength = 0;
    for (size_t nCoordinate = 0; nCoordinate < nSearchSize; ++nCoordinate)
    {
        const int nX = (int)X + kSearchArea[nCoordinate][0];
        const int nY = (int)Y + kSearchArea[nCoordinate][1];

        if ((nX < 0) || (nX >= (int)nWidth) || (nY < 0) || (nY >= (int)nHeight))
        {
            continue;
        }

        kOutput[nOutputLength++] = kWordSearch[nY][nX];
    }

    *nOutputSize = nOutputLength;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");

    if (pData)
    {
        const int SEARCH_AREAS[4][4][2] = {
            {{ 0, 0}, { 1, 0}, { 2, 0}, { 3, 0}}, /* Across */
            {{ 0, 0}, { 0, 1}, { 0, 2}, { 0, 3}}, /* Down */
            {{ 0, 0}, {-1,-1}, {-2,-2}, {-3,-3}}, /* Diagonal Backward */
            {{ 0, 0}, { 1,-1}, { 2,-2}, { 3,-3}}  /* Diagonal Forward */
        };
        const char* kValidPermutations = "MS SM"; /* The space is the trick here... */

        char*                       kBuffer;
        char**                      kWordSearch;
        size_t                      nHeight;
        size_t                      nWidth;
        size_t                      nLine       = 0;
        size_t                      X, Y;

        char                        kFound[5]   = {'\0','\0','\0','\0','\0'};

        uint64_t                    nPartOne    = 0;
        uint64_t                    nPartTwo    = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kWordSearch, &nHeight, NULL, NULL, 0);
        fclose(pData);

        /* Update the Width from any Arbitrary Line */
        assert(nHeight > 0);
        nWidth = strlen(kWordSearch[0]);

        for (Y = 0; Y < nHeight; ++Y)
        {
            for (X = 0; X < nWidth; ++X)
            {
                /*
                 * Part One
                 * ========
                 *
                 * Only Search if this is the Start or End of XMAS
                 */

                if ((kWordSearch[Y][X] == 'X') || (kWordSearch[Y][X] == 'S'))
                {
                    size_t nCurrentSearch;

                    for (nCurrentSearch = 0; nCurrentSearch < (sizeof(SEARCH_AREAS) / sizeof(SEARCH_AREAS[0])); ++nCurrentSearch)
                    {
                        size_t nSearchSize;
                        getSearchArea((const char**)kWordSearch, X, Y, SEARCH_AREAS[nCurrentSearch], 4, nWidth, nHeight, kFound, &nSearchSize);

                        if (nSearchSize == 4)
                        {
                            if ((strncmp(kFound, "XMAS", 4) == 0) || (strncmp(kFound, "SAMX", 4) == 0))
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
                if (Y > 0 && Y < nHeight - 1 && X > 0 && X < nWidth - 1)
                {
                    if (kWordSearch[Y][X] == 'A')
                    {
                        const char kPairOne[3] = {kWordSearch[Y-1][X-1], kWordSearch[Y+1][X+1], '\0'};
                        const char kPairTwo[3] = {kWordSearch[Y-1][X+1], kWordSearch[Y+1][X-1], '\0'};

                        if (!(!strstr(kValidPermutations, kPairOne) || !strstr(kValidPermutations, kPairTwo)))
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