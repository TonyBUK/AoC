#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <mach/mach_time.h>

// Returns the current time in microseconds.
uint64_t get_microseconds()
{
    static mach_timebase_info_data_t timebase;

    // Get the timebase if this is the first time we're called.
    // We only do this once to avoid the overhead of calling mach_timebase_info.
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);
    }

    // Convert the mach time to microseconds.
    uint64_t mach_time = mach_absolute_time();
    uint64_t microseconds = mach_time * timebase.numer / timebase.denom;
    return microseconds;
}

#undef FALSE
#undef TRUE

#define FALSE (0)
#define TRUE  (1)

#define TREE_GRID(p, X, Y, W) (p[(Y * W) + X])

unsigned IsVisibleX(const unsigned char* kTreeGrid, const size_t nWidth, const size_t XStart, const size_t XEnd, const size_t Y, const unsigned char nTreeHeight)
{
    const int64_t D = (XStart > XEnd) ? -1 : 1;
    int64_t       X = (int64_t)XStart;

    while (1)
    {
        if (TREE_GRID(kTreeGrid, X, Y, nWidth) >= nTreeHeight)
        {
            return FALSE;
        }

        if (X == XEnd)
        {
            return TRUE;
        }

        X += D;
    }
 }

unsigned IsVisibleY(const unsigned char* kTreeGrid, const size_t nWidth, const size_t YStart, const size_t YEnd, const size_t X, const unsigned char nTreeHeight)
{
    const int64_t D = (YStart > YEnd) ? -1 : 1;
    int64_t       Y = (int64_t)YStart;

    while (1)
    {
        if (TREE_GRID(kTreeGrid, X, Y, nWidth) >= nTreeHeight)
        {
            return FALSE;
        }

        if (Y == YEnd)
        {
            return TRUE;
        }

        Y += D;
    }
}

size_t AxisScoreX(const unsigned char* kTreeGrid, const size_t nWidth, const size_t XStart, const size_t XEnd, const size_t Y, const unsigned char nTreeHeight)
{
    const int64_t D      = (XStart > XEnd) ? -1 : 1;
    int64_t       X      = (int64_t)XStart;
    size_t        nScore = 0;

    while (1)
    {
        ++nScore;

        if (TREE_GRID(kTreeGrid, X, Y, nWidth) >= nTreeHeight)
        {
            return nScore;
        }

        if (X == XEnd)
        {
            return nScore;
        }

        X += D;
    }
 }

size_t AxisScoreY(const unsigned char* kTreeGrid, const size_t nWidth, const size_t YStart, const size_t YEnd, const size_t X, const unsigned char nTreeHeight)
{
    const int64_t D = (YStart > YEnd) ? -1 : 1;
    int64_t       Y = (int64_t)YStart;
    size_t        nScore = 0;

    while (1)
    {
        ++nScore;

        if (TREE_GRID(kTreeGrid, X, Y, nWidth) >= nTreeHeight)
        {
            return nScore;
        }

        if (Y == YEnd)
        {
            return nScore;
        }

        Y += D;
    }
}
 
int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        /* Unless I hard code the max size, or assume a square or something of that ilk, I'm going to
         * read the file twice, once to determine dimensions, once to read the file itself.
         */
        size_t GRID_HEIGHT   = 0;
        size_t GRID_WIDTH    = 0;
        size_t nCurrentWidth = 0;
        size_t X, Y;
        char   kData;
 
        unsigned char*  kTreeGrid;
        size_t          nTreeGridRawSize;
        size_t          nDataPointer     = 0;
        size_t          nFilePointer     = 0;
        size_t          nVisibleTrees;
        size_t          nBestScenicScore = 0;

        /* Buffer and Read the Raw Data */
        fseek(pData, 0, SEEK_END);
        nTreeGridRawSize = ftell(pData);
        kTreeGrid = (unsigned char*)malloc(nTreeGridRawSize * sizeof(unsigned char));
        fseek(pData, 0, SEEK_SET);
        fread(kTreeGrid, nTreeGridRawSize, 1, pData);
        fclose(pData);

        /* Re-factor the Buffered File to Store within it the 1D Grid in a manner accessible
         * as a 2D array.
         */
        while (nFilePointer < nTreeGridRawSize)
        {
            if (isdigit(kTreeGrid[nFilePointer]))
            {
                kTreeGrid[nDataPointer] = kTreeGrid[nFilePointer] - '0';
                ++nDataPointer;
            }
            else if (0 == GRID_WIDTH)
            {
                GRID_WIDTH = nFilePointer;
            }

            ++nFilePointer;
        }
        GRID_HEIGHT = nDataPointer / GRID_WIDTH;

        /* Initialise nVisibleTrees based on Grid Dimensions.
         * Note the corners overlap so we don't want to count them twice.
         */
        nVisibleTrees = (GRID_HEIGHT * 2) + (GRID_WIDTH * 2) - 4;
 
        /* Solve Both Parts */
        for (Y = 1; Y < (GRID_HEIGHT - 1); ++Y)
        {
            for (X = 1; X < (GRID_WIDTH - 1); ++X)
            {
                const size_t nTreeHeight = TREE_GRID(kTreeGrid, X, Y, GRID_WIDTH);
                unsigned bTreeVisibility;
                size_t   nScenicScore;
 
                /* Part 1 */
                /* Iterate in all 4 Directions to determine if there's visibility in any of them. */
                                      bTreeVisibility = IsVisibleX((const unsigned char*)kTreeGrid, GRID_WIDTH, X - 1, 0,               Y, nTreeHeight); /* Left  */
                if (!bTreeVisibility) bTreeVisibility = IsVisibleX((const unsigned char*)kTreeGrid, GRID_WIDTH, X + 1, GRID_WIDTH  - 1, Y, nTreeHeight); /* Right */
                if (!bTreeVisibility) bTreeVisibility = IsVisibleY((const unsigned char*)kTreeGrid, GRID_WIDTH, Y - 1, 0,               X, nTreeHeight); /* Up    */
                if (!bTreeVisibility) bTreeVisibility = IsVisibleY((const unsigned char*)kTreeGrid, GRID_WIDTH, Y + 1, GRID_HEIGHT - 1, X, nTreeHeight); /* Down  */
 
                /* Increment the Visibility Counter if either of the Visibility Checks passed */
                if (bTreeVisibility)
                {
                    ++nVisibleTrees;
                }

                /* Part 2 */
                /* Iterate in all 4 Directions to calculate the number of smaller tree's until either the
                 * grid edge, or a larger tree is encountered.
                 * Multiply all of these together.
                 */
                nScenicScore  = AxisScoreX((const unsigned char*)kTreeGrid, GRID_WIDTH, X - 1, 0,               Y, nTreeHeight); /* Left  */
                nScenicScore *= AxisScoreX((const unsigned char*)kTreeGrid, GRID_WIDTH, X + 1, GRID_WIDTH - 1,  Y, nTreeHeight); /* Right */
                nScenicScore *= AxisScoreY((const unsigned char*)kTreeGrid, GRID_WIDTH, Y - 1, 0,               X, nTreeHeight); /* Up    */
                nScenicScore *= AxisScoreY((const unsigned char*)kTreeGrid, GRID_WIDTH, Y + 1, GRID_HEIGHT - 1, X, nTreeHeight); /* Down  */
 
                /* If this is the most scenic, update the best score */
                if (nScenicScore > nBestScenicScore)
                {
                    nBestScenicScore = nScenicScore;
                }
            }
        }
 
        printf("Part 1: %zu\n", nVisibleTrees);
        printf("Part 2: %zu\n", nBestScenicScore);

        /* Clean Up */
        free(kTreeGrid);
    }

    return 0;
}
