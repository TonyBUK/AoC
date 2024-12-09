#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define AOC_TRUE  (1u)
#define AOC_FALSE (0u)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct FileEntryPartOne
{
    uint64_t nId;
    uint64_t nSize;
} FileEntryPartOne;

typedef struct FileEntryPartTwo
{
    uint64_t nId;
    uint64_t nSize;
    size_t   nPosition;
} FileEntryPartTwo;

uint64_t calculateChecksum(const uint64_t* kCompressedDisk, const uint64_t nLength)
{
    uint64_t nChecksum = 0u;
    uint64_t i;

    for (i = 0; i < nLength; ++i)
    {
        nChecksum += kCompressedDisk[i] * i;
    }

    return nChecksum;
}

uint64_t compactPartOne(FileEntryPartOne* kFiles, const size_t nFileCount, uint64_t* kFreeSpace, const size_t nFreeSpaceCount, const size_t nTotalSpace)
{
    size_t      nLeftMostFreeSpace  = 0u;
    size_t      nRightMostFile      = nFileCount - 1;
    uint64_t*   kCompressedDisk     = (uint64_t*)malloc(sizeof(uint64_t) * nTotalSpace);
    size_t      nCompressedDiskPos  = 0u;

    uint64_t    nChecksum;
    uint64_t    i;

    /* We Know File One occupies the first space */
    for (i = 0; i < kFiles[0].nSize; ++i)
    {
        kCompressedDisk[nCompressedDiskPos++] = kFiles[0].nId;
    }

    /* Iterate until all files are moved */
    while (1)
    {
        /* Copy as much of the Right Most file as possible into the free space */
        const uint64_t nSpaceTopMove = MIN(kFiles[nRightMostFile].nSize, kFreeSpace[nLeftMostFreeSpace]);
        for (i = 0; i < nSpaceTopMove; ++i)
        {
            kCompressedDisk[nCompressedDiskPos++] = kFiles[nRightMostFile].nId;
        }

        /* Decrement the Used/Free Space */
        kFreeSpace[nLeftMostFreeSpace] -= nSpaceTopMove;
        kFiles[nRightMostFile].nSize   -= nSpaceTopMove;

        /* Shift the Left Most Free Space Pointer all the time it's pointing to (now) empty free space */
        while ((nLeftMostFreeSpace < nFreeSpaceCount) && (0 == kFreeSpace[nLeftMostFreeSpace]))
        {
            /* Increment the Left Most Free Space, and Copy to the Disk */
            ++nLeftMostFreeSpace;
            for (i = 0; i < kFiles[nLeftMostFreeSpace].nSize; ++i)
            {
                kCompressedDisk[nCompressedDiskPos++] = kFiles[nLeftMostFreeSpace].nId;
            }
            kFiles[nLeftMostFreeSpace].nSize = 0;
        }

        /* Shift the Right Most File Pointer all the time it's pointing to an entirely moved file */
        while ((nRightMostFile > 0) && (0 == kFiles[nRightMostFile].nSize))
        {
            --nRightMostFile;
        }

        /* When the Right Most File has been moved, we've finished */
        if (0 == kFiles[nRightMostFile].nSize)
        {
            break;
        }
    }

    /* Calculate the Checksum */
    nChecksum = calculateChecksum(kCompressedDisk, nCompressedDiskPos);

    free(kCompressedDisk);

    return nChecksum;
}

uint64_t calculateChecksumPartTwo(const FileEntryPartTwo* kCompressedDisk, const size_t nLength)
{
    uint64_t nChecksum = 0u;
    uint64_t nPosition;
    uint64_t i;

    for (i = 0; i < nLength; ++i)
    {
        uint64_t j;

        nPosition = kCompressedDisk[i].nPosition;

        for (j = 0; j < kCompressedDisk[i].nSize; ++j)
        {
            nChecksum += kCompressedDisk[i].nId * nPosition;
            ++nPosition;
        }
    }

    return nChecksum;
}

int compareFileNodes(const void *a, const void *b)
{
    FileEntryPartTwo* kLeft  = ((FileEntryPartTwo*)a);
    FileEntryPartTwo* kRight = ((FileEntryPartTwo*)b);

    if (kLeft->nPosition < kRight->nPosition)
    {
        return 1;
    }
    else if (kLeft->nPosition > kRight->nPosition)
    {
        return -1;
    }

    assert(0);
}

int compareFreeSpaceNodes(const void *a, const void *b)
{
    FileEntryPartTwo* kLeft  = *(FileEntryPartTwo**)a;
    FileEntryPartTwo* kRight = *(FileEntryPartTwo**)b;

    if (kLeft->nSize < kRight->nSize)
    {
        return 1;
    }
    else if (kLeft->nSize > kRight->nSize)
    {
        return -1;
    }
    else
    {
        if (kLeft->nPosition < kRight->nPosition)
        {
            return -1;
        }
        else if (kLeft->nPosition > kRight->nPosition)
        {
            return 1;
        }

        assert(0);
    }
}

uint64_t compactPartTwo(FileEntryPartTwo* kFiles, const size_t nFileCount, FileEntryPartTwo* kFreeSpace, const size_t nFreeSpaceCount)
{
    FileEntryPartTwo*  kNewFileList = (FileEntryPartTwo*)malloc(sizeof(FileEntryPartTwo) * nFileCount);
    size_t             nNewFileCount = 0u;

    FileEntryPartTwo** kFreeSpaceNodesBySize[10];
    size_t             kFreeSpaceNodesBySizeCount[10];

    uint64_t           nChecksum;

    size_t             i;

    kFreeSpaceNodesBySizeCount[0] = 0u;
    for (i = 1; i < (sizeof(kFreeSpaceNodesBySize) / sizeof(kFreeSpaceNodesBySize[0])); ++i)
    {
        kFreeSpaceNodesBySize[i]       = (FileEntryPartTwo**)malloc(sizeof(FileEntryPartTwo*) * nFreeSpaceCount);
        kFreeSpaceNodesBySizeCount[i] = 0u;
    }

    /*
     * For speed sake, we want to quickly get to the lowest free space of
     * each magnitude, meaning we split the data by size, and sort by
     * position.
     */
    for (i = 0; i < nFreeSpaceCount; ++i)
    {
        if (0 != kFreeSpace[i].nSize)
        {
            kFreeSpaceNodesBySize[kFreeSpace[i].nSize][kFreeSpaceNodesBySizeCount[kFreeSpace[i].nSize]++] = &kFreeSpace[i];
        }
    }

    for (i = 1; i < (sizeof(kFreeSpaceNodesBySize) / sizeof(kFreeSpaceNodesBySize[0])); ++i)
    {
        qsort(kFreeSpaceNodesBySize[i], kFreeSpaceNodesBySizeCount[i], sizeof(FileEntryPartTwo*), compareFreeSpaceNodes);
    }

    /* Work backwards from the highest node to the lowest node */
    for (i = nFileCount; i > 0; --i)
    {
        const uint64_t    nSpaceNeeded = kFiles[i - 1].nSize;
        unsigned          bFound       = AOC_FALSE;
        size_t            nLowestNode  = (size_t)-1;
        FileEntryPartTwo* kLowestNode  = NULL;
        uint64_t          nNodeSize;

        /* Find the lowest free space node that can fit the file */
        for (nNodeSize = nSpaceNeeded; nNodeSize < 10; ++nNodeSize)
        {
            if (kFreeSpaceNodesBySizeCount[nNodeSize] > 0)
            {
                if (kFreeSpaceNodesBySize[nNodeSize][0]->nPosition < kFiles[i - 1].nPosition)
                {
                    if (kFreeSpaceNodesBySize[nNodeSize][0]->nPosition < nLowestNode)
                    {
                        nLowestNode = kFreeSpaceNodesBySize[nNodeSize][0]->nPosition;
                        kLowestNode = kFreeSpaceNodesBySize[nNodeSize][0];
                        bFound      = AOC_TRUE;
                    }
                }
            }
        }

        /*
         * Add the File to either the Lowest Free Node or in its existing
         * location.
         * Note: For the final sort, we push the position to the most
         *       prominent position in the list to process data in disk
         *       order.
         */
        if (AOC_FALSE == bFound)
        {
            /* Existing Location */
            kNewFileList[nNewFileCount].nId = kFiles[i - 1].nId;
            kNewFileList[nNewFileCount].nSize = kFiles[i - 1].nSize;
            kNewFileList[nNewFileCount].nPosition = kFiles[i - 1].nPosition;

            ++nNewFileCount;
        }
        else
        {
            const uint64_t nBufferedSize = kLowestNode->nSize;

            /* New Location */
            kNewFileList[nNewFileCount].nPosition = kLowestNode->nPosition;
            kNewFileList[nNewFileCount].nSize     = kFiles[i - 1].nSize;
            kNewFileList[nNewFileCount].nId       = kFiles[i - 1].nId;

            ++nNewFileCount;

            /* Update the Free Space Nodes */
            kLowestNode->nSize     -= nSpaceNeeded;
            kLowestNode->nPosition += nSpaceNeeded;

            /* Move the now defunct node to the end of the list and decrement the node count */
            qsort(kFreeSpaceNodesBySize[nBufferedSize], kFreeSpaceNodesBySizeCount[nBufferedSize], sizeof(FileEntryPartTwo*), compareFreeSpaceNodes);
            --kFreeSpaceNodesBySizeCount[nBufferedSize];

            /* Move the Free Space Node to its new size bracket (if non-zero) */
            if (kLowestNode->nSize > 0)
            {
                kFreeSpaceNodesBySize[kLowestNode->nSize][kFreeSpaceNodesBySizeCount[kLowestNode->nSize]++] = kLowestNode;
                qsort(kFreeSpaceNodesBySize[kLowestNode->nSize], kFreeSpaceNodesBySizeCount[kLowestNode->nSize], sizeof(FileEntryPartTwo*), compareFreeSpaceNodes);
            }
        }
    }

    /* Calculate the Checksum */
    qsort(kNewFileList, nNewFileCount, sizeof(FileEntryPartTwo), compareFileNodes);

    nChecksum = calculateChecksumPartTwo(kNewFileList, nNewFileCount);

    free(kNewFileList);

    for (i = 1; i < (sizeof(kFreeSpaceNodesBySize) / sizeof(kFreeSpaceNodesBySize[0])); ++i)
    {
        free(kFreeSpaceNodesBySize[i]);
    }

    return nChecksum;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*    kData     = NULL;

        while (!feof(pData))
        {
            size_t nSize    = 0u;
            size_t nLength;

            /* Solve both Parts... */
            while ((int64_t)(nLength = getline(&kData, &nSize, pData)) != -1)
            {
                const size_t      nMaxFiles         = (nLength / 2) + 1;

                FileEntryPartOne* kFilesPartOne     = (FileEntryPartOne*)malloc(sizeof(FileEntryPartOne) * nMaxFiles);
                uint64_t*         kFreeSpacePartOne = (uint64_t*)malloc(sizeof(uint64_t) * nMaxFiles);

                FileEntryPartTwo* kFilesPartTwo     = (FileEntryPartTwo*)malloc(sizeof(FileEntryPartTwo) * nMaxFiles);
                FileEntryPartTwo* kFreeSpacePartTwo = (FileEntryPartTwo*)malloc(sizeof(FileEntryPartTwo) * nMaxFiles);

                size_t            nFileCount        = 0u;
                size_t            nFreeSpaceCount   = 0u;

                uint64_t          nTotalSpace       = 0u;

                size_t i;
                for (i = 0; i < nLength; ++i)
                {
                    const uint64_t nSize = kData[i] - '0';
                    if (0 == (i % 2))
                    {
                        kFilesPartOne[nFileCount].nId       = nFileCount;
                        kFilesPartTwo[nFileCount].nId       = nFileCount;

                        kFilesPartOne[nFileCount].nSize     = nSize;
                        kFilesPartTwo[nFileCount].nSize     = nSize;

                        kFilesPartTwo[nFileCount].nPosition = nTotalSpace;

                        ++nFileCount;
                    }
                    else
                    {
                        kFreeSpacePartOne[nFreeSpaceCount]           = nSize;

                        kFreeSpacePartTwo[nFreeSpaceCount].nId       = nFreeSpaceCount;
                        kFreeSpacePartTwo[nFreeSpaceCount].nSize     = nSize;
                        kFreeSpacePartTwo[nFreeSpaceCount].nPosition = nTotalSpace;

                        ++nFreeSpaceCount;
                    }

                    nTotalSpace += nSize;
                }

                printf("Part 1: %" PRIu64 "\n", compactPartOne(kFilesPartOne, nFileCount, kFreeSpacePartOne, nFreeSpaceCount, nTotalSpace));
                printf("Part 2: %" PRIu64 "\n", compactPartTwo(kFilesPartTwo, nFileCount, kFreeSpacePartTwo, nFreeSpaceCount));

                free(kFilesPartOne);
                free(kFreeSpacePartOne);
                free(kFilesPartTwo);
                free(kFreeSpacePartTwo);
            }
        }
        fclose(pData);

        free(kData);
    }
 
    return 0;
}