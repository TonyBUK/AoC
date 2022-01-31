#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct positionType
{
    int X;
    int Y;
} positionType;

void addToSet(positionType* kBuffer, unsigned long* nBufferCount, const positionType* kEntry)
{
    int i = 0;
    for (i = 0; i < *nBufferCount; ++i)
    {
        if ((kBuffer[i].X == kEntry->X) &&
            (kBuffer[i].Y == kEntry->Y))
        {
            return;
        }
    }
    kBuffer[*nBufferCount] = *kEntry;
    *nBufferCount += 1;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;

        unsigned long       nPositionCount            = 1;
        positionType*       kPositions;
        positionType        kPosition                 = {0,0};

        unsigned long       nCombinedPositionCount    = 1;
        positionType*       kCombinedPositions;
        positionType        kCombinedPosition[2]      = {{0,0},{0,0}};
        int                 nCurrent                  = 0;

        int i;

        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*)        malloc(nFileSize * sizeof(char));
        kPositions         = (positionType*)malloc(nFileSize * sizeof(positionType));
        kCombinedPositions = (positionType*)malloc(nFileSize * sizeof(positionType));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kPositions[0]         = kPosition;
        kCombinedPositions[0] = kCombinedPosition[0];

        for (i = 0; i < nFileSize; ++i)
        {
            if ('<' == kBuffer[i])
            {
                kCombinedPosition[nCurrent].X -= 1;
                kPosition.X                   -= 1;
            }
            else if ('^' == kBuffer[i])
            {
                kCombinedPosition[nCurrent].Y += 1;
                kPosition.Y                   += 1;
            }
            else if ('>' == kBuffer[i])
            {
                kCombinedPosition[nCurrent].X += 1;
                kPosition.X                   += 1;
            }
            else if ('v' == kBuffer[i])
            {
                kCombinedPosition[nCurrent].Y -= 1;
                kPosition.Y                   -= 1;
            }

            addToSet(kPositions,         &nPositionCount,         &kPosition);
            addToSet(kCombinedPositions, &nCombinedPositionCount, &kCombinedPosition[nCurrent]);
            nCurrent = 1 - nCurrent;
        }

        printf("Part 1: %ld\n", nPositionCount);
        printf("Part 2: %ld\n", nCombinedPositionCount);
 
        free(kBuffer);
        free(kPositions);
        free(kCombinedPositions);
    }

    return 0;
}
