#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <assert.h>

typedef struct QueueType
{
    unsigned long nScore;
    unsigned long nPos;
} TQueueType;

#define TRUE                    (1)
#define FALSE                   (0)

#define MAKE_VECTOR(X,Y,W)      (((Y) * (long)(W)) + (X))
#define GET_VECTOR_X(V,W)       ((V) % (W))
#define GET_VECTOR_Y(V,W)       ((V) / (W))
#define ADD_TO_QUEUE(V,S,Q,QT,SIZE) \
{                                   \
    (Q)[QT].nScore = S;             \
    (Q)[QT].nPos   = V;             \
    ++QT;                           \
    if (QT >= SIZE) QT = 0;         \
}

#define POP_FROM_QUEUE(O,Q,QH,SIZE) \
{                                   \
    O = (Q)[QH];                    \
    ++QH;                           \
    if (QH >= SIZE) QH = 0;         \
}

int CompareQueue(const void* kLeft, const void* kRight)
{
    const TQueueType* kLeftEntry  = (const TQueueType*)kLeft;
    const TQueueType* kRightEntry = (const TQueueType*)kRight;

    if (kLeftEntry->nScore < kRightEntry->nScore)
    {
        return -1;
    }
    else if (kLeftEntry->nScore > kRightEntry->nScore)
    {
        return 1;
    }

    return 0;
}

void BFS(const size_t WIDTH, const size_t HEIGHT, const unsigned long kStart, size_t* kVisited, const unsigned long* kHeightMap)
{
    const size_t   LAST_COL = WIDTH  - 1;
    const size_t   LAST_ROW = HEIGHT - 1;
    const size_t   SIZE     = WIDTH * HEIGHT;

    long           VECTORS[4];
    size_t         nVisited     = 0;
    unsigned long  nQueueHead   = 0;
    unsigned long  nQueueTail   = 0;
    TQueueType*    kQueue1      = (TQueueType*)malloc(WIDTH * HEIGHT * sizeof(TQueueType));
    TQueueType*    kQueue2      = (TQueueType*)malloc(WIDTH * HEIGHT * sizeof(TQueueType));
    TQueueType*    kQueueBuffer = kQueue1;
    TQueueType*    kQueueOther  = kQueue2;

    size_t         i;

    /* Create the Neighbour Vectors */
    VECTORS[0] = MAKE_VECTOR( -1,  0, WIDTH);
    VECTORS[1] = MAKE_VECTOR(  1,  0, WIDTH);
    VECTORS[2] = MAKE_VECTOR(  0, -1, WIDTH);
    VECTORS[3] = MAKE_VECTOR(  0,  1, WIDTH);

    /* Push the First Neighbour into the Queue */
    ADD_TO_QUEUE(kStart, 1, kQueueBuffer, nQueueTail, SIZE);

    /* Whilst there are still Neighbours to be visited... */
    while (nQueueHead != nQueueTail)
    {
        TQueueType kEntry;
        unsigned long X, Y;
        unsigned bQueueUpdated = FALSE;
        POP_FROM_QUEUE(kEntry, kQueueBuffer, nQueueHead, SIZE);

        /* See if we've visited it already... */
        if (kVisited[kEntry.nPos])
        {
            continue;
        }
        else
        {
            kVisited[kEntry.nPos] = kEntry.nScore;
        }

        /* Extract X/Y from the Position */
        X = GET_VECTOR_X(kEntry.nPos, WIDTH);
        Y = GET_VECTOR_Y(kEntry.nPos, WIDTH);

        /* Test the 4 Neighbours */

        if (X > 0)
        {
            const unsigned long nNewPos = MAKE_VECTOR(X - 1, Y, WIDTH);
            if (((long)kHeightMap[kEntry.nPos] - (long)kHeightMap[nNewPos]) <= 1)
            {
                if (0 == kVisited[nNewPos])
                {
                    ADD_TO_QUEUE(nNewPos, kEntry.nScore + 1, kQueueBuffer, nQueueTail, SIZE);
                    bQueueUpdated = TRUE;
                }
            }
        }

        if (X < LAST_COL)
        {
            const unsigned long nNewPos = MAKE_VECTOR(X + 1, Y, WIDTH);
            if (((long)kHeightMap[kEntry.nPos] - (long)kHeightMap[nNewPos]) <= 1)
            {
                if (0 == kVisited[nNewPos])
                {
                    ADD_TO_QUEUE(nNewPos, kEntry.nScore + 1, kQueueBuffer, nQueueTail, SIZE);
                    bQueueUpdated = TRUE;
                }
            }
        }

        if (Y > 0)
        {
            const unsigned long nNewPos = MAKE_VECTOR(X, Y - 1, WIDTH);
            if (((long)kHeightMap[kEntry.nPos] - (long)kHeightMap[nNewPos]) <= 1)
            {
                if (0 == kVisited[nNewPos])
                {
                    ADD_TO_QUEUE(nNewPos, kEntry.nScore + 1, kQueueBuffer, nQueueTail, SIZE);
                    bQueueUpdated = TRUE;
                }
            }
        }

        if (Y < LAST_ROW)
        {
            const unsigned long nNewPos = MAKE_VECTOR(X, Y + 1, WIDTH);
            if (((long)kHeightMap[kEntry.nPos] - (long)kHeightMap[nNewPos]) <= 1)
            {
                if (0 == kVisited[nNewPos])
                {
                    ADD_TO_QUEUE(nNewPos, kEntry.nScore + 1, kQueueBuffer, nQueueTail, SIZE);
                    bQueueUpdated = TRUE;
                }
            }
        }

        /* Sort the Queue if needed... */
        if (bQueueUpdated)
        {
            if (nQueueHead < nQueueTail)
            {
                qsort(&kQueueBuffer[nQueueHead], (nQueueTail - nQueueHead), sizeof(TQueueType), CompareQueue);
            }
            /* TODO: Probably some sort of copy if needed... */
            else if (nQueueHead > nQueueTail)
            {
                const size_t nQueueSize = (SIZE - nQueueHead) + nQueueTail;
                TQueueType* kTemp = kQueueBuffer;
                size_t i;

                for (i = nQueueHead; i < SIZE; ++i)
                {
                    kQueueOther[i - nQueueHead] = kQueueBuffer[i];
                }
                for (i = 0; i < nQueueTail; ++i)
                {
                    kQueueOther[i + (SIZE - nQueueHead)] = kQueueBuffer[i];
                }

                /* Swap the Buffers */
                kQueueBuffer = kQueueOther;
                kQueueOther  = kTemp;

                /* Update the Head/Tail */
                nQueueHead = 0;
                nQueueTail = nQueueSize;
                qsort(&kQueueBuffer[nQueueHead], (nQueueTail - nQueueHead), sizeof(TQueueType), CompareQueue);
            }
        }
    }

    free(kQueue1);
    free(kQueue2);
}

int main(int argc, char** argv)
{
    FILE* pFile = fopen("../input.txt", "r");
 
    if (pFile)
    {
        const unsigned long DECODE_HEIGHT[128] =
        {
             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             0,  0,  0,  0,  0, 26,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,  0,  0,  0,  0,  0
        };

        size_t          nFileSize;
        unsigned char*  kRawHeightMap;
        unsigned long*  kHeightMap;
        unsigned long*  kStartMap;
        unsigned long   kEnd;
        size_t*         kVisited;

        unsigned long   nStartCount = 1;
        size_t          nBestStart  = (size_t)-1;

        size_t          WIDTH       = 0;
        size_t          HEIGHT      = 0;
        size_t          nDataPtr    = 0;
        size_t          nFilePtr    = 0;

        /* Calculate the File Size */
        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        /* Allocate the Height Map */
        kRawHeightMap = (unsigned char*)malloc(nFileSize * sizeof(unsigned char));
        kHeightMap    = (unsigned long*)malloc(nFileSize * sizeof(unsigned long));
        kStartMap     = (unsigned long*)malloc(nFileSize * sizeof(unsigned long));
        kVisited      = (size_t*)       malloc(nFileSize * sizeof(size_t));

        memset(kVisited, 0, nFileSize * sizeof(size_t));
        
        /* Buffer the Raw Data */
        fread(kRawHeightMap, nFileSize, 1, pFile);
        fclose (pFile);

        /* Create the Height Map */
        while (nFilePtr < nFileSize)
        {
            if (isalpha(kRawHeightMap[nFilePtr]))
            {
                assert(DECODE_HEIGHT[kRawHeightMap[nFilePtr]] != 0);
                kHeightMap[nDataPtr] = DECODE_HEIGHT[kRawHeightMap[nFilePtr]];

                /* Note: At this point we may or may not know how to decode
                 *       the positions, but we will!
                 */
                if      ('S' == kRawHeightMap[nFilePtr])
                {
                    kStartMap[0] = nDataPtr;
                }
                else if ('E' == kRawHeightMap[nFilePtr])
                {
                    kEnd = nDataPtr;
                }
                else if ('a' == kRawHeightMap[nFilePtr])
                {
                    kStartMap[nStartCount++] = nDataPtr;
                }

                ++nDataPtr;
            }
            else
            {
                if (0 == WIDTH)
                {
                    WIDTH    = nDataPtr;
                    assert(WIDTH > 0);
                }
            }

            ++nFilePtr;
        }

        HEIGHT   = nDataPtr / WIDTH;
        assert(HEIGHT > 0);

        /* Calculate the BFS for the End Point that will be used to
         * derive both solutions.
         *
         * Note: The scores will all be +1 as I use 0 as the test for
         *       an unvisited location.
         */
        BFS(WIDTH, HEIGHT, kEnd, kVisited, kHeightMap);

        /* Part 1 */
        printf("Part 1: %zu\n", kVisited[kStartMap[0]] - 1);

        /* Part 2 */
        for (size_t i = 1; i < nStartCount; ++i)
        {
            if (kVisited[kStartMap[i]])
            {
                if (kVisited[kStartMap[i]] < nBestStart)
                {
                    nBestStart = kVisited[kStartMap[i]];
                }
            }
        }
        printf("Part 2: %zu\n", nBestStart - 1);

        /* Cleanup */
        free(kRawHeightMap);
        free(kHeightMap);
        free(kStartMap);
        free(kVisited);
    }

    return 0;
}
