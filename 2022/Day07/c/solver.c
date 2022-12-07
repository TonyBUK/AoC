#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define START_NODE_SIZE (256)
 
typedef struct SFolderNode
{
    size_t                  nTotalSize;
    struct SFolderNode*     pParent;
} TFolderNodeType;
 
void UpdateNodeAllocation(TFolderNodeType*** kNodeList, size_t* nNodeListSize, size_t* nMaxNodeListSize)
{
    if (*nNodeListSize >= *nMaxNodeListSize)
    {
        TFolderNodeType** kNewNodeList = (TFolderNodeType**)malloc((*nMaxNodeListSize * 2) * sizeof(TFolderNodeType*));
        size_t i;
 
        assert(kNewNodeList);
 
        /* Note for future sanity: *p[1] is different to (*p)[i].
         * The latter will get the offset from p then de-reference that address,
         * the former will deference p, and get an offset from that.
         * The latter behaviour is wanted.
         */
        for (i = 0; i < *nMaxNodeListSize; ++i)
        {
            assert((*kNodeList)[i]);
            kNewNodeList[i] = (*kNodeList)[i];
        }
 
        free(*kNodeList);
        *kNodeList = kNewNodeList;
 
        *nMaxNodeListSize = *nMaxNodeListSize * 2;
    }
}
 
void UpdateNodeList(TFolderNodeType* pNode, TFolderNodeType*** kNodeList, size_t* nNodeListSize, size_t* nMaxNodeListSize)
{
    UpdateNodeAllocation(kNodeList, nNodeListSize, nMaxNodeListSize);
    (*kNodeList)[*nNodeListSize] = pNode;
    ++(*nNodeListSize);
}
 
void InitialiseFolderNode(TFolderNodeType* pParent, TFolderNodeType** pNode, TFolderNodeType*** kNodeList, size_t* nNodeListSize, size_t* nMaxNodeListSize)
{
    /* Assert all the Pointers */
    assert(pNode);
    assert(kNodeList);
    assert(*kNodeList);
    assert(nNodeListSize);
    assert(nMaxNodeListSize);
 
    /* Allocate the Node */
    *pNode                  = (TFolderNodeType*)malloc(sizeof(TFolderNodeType));
    assert(*pNode);
 
    /* Initialise the Node */
    (*pNode)->nTotalSize    = 0;
    (*pNode)->pParent       = pParent;
 
    /* Store the Node on the List */
    UpdateNodeList(*pNode, kNodeList, nNodeListSize, nMaxNodeListSize);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        const size_t      nMaxSpace        = 70000000ul;
        const size_t      nFreeSpaceNeeded = 30000000ul;
 
        /* This is the Node Tree storing all of the Folder Data as a Tree */
        TFolderNodeType*  pRootNode;
        TFolderNodeType*  pNode;
 
        /* This is the Node Tree flattened to a 1D Structure for sanity / ease of parsing */
        size_t            nNodeListMaxSize = START_NODE_SIZE;
        size_t            nNodeListSize    = 0;
        TFolderNodeType** kNodeList        = malloc(nNodeListMaxSize * sizeof(TFolderNodeType*));
 
        size_t            nSumSmallSpace            = 0;
        size_t            nSmallestFolderToDeleted  = (size_t)-1; /* Deliberately underflow to get max size */
        size_t            nNeededSpace;
 
        size_t            nCurrentDelta             = 0;

        size_t            i;
 
        assert(kNodeList);
 
        InitialiseFolderNode(NULL, &pRootNode, &kNodeList, &nNodeListSize, &nNodeListMaxSize);
        pNode = pRootNode;
 
        /* Parse the Input Data */
        while (!feof(pData))
        {
            /* The input itself is consistent enough we can take a few tacky shortcuts... */
            /* Firstly, we'll always be hunting for a $ sign to start with */
            const char kData = fgetc(pData);
 
            if ('$' == kData)
            {
                if (0 != nCurrentDelta)
                {
                    TFolderNodeType* pLocalNode = pNode;
                    do
                    {
                        pLocalNode->nTotalSize += nCurrentDelta;
                        pLocalNode              = pLocalNode->pParent;
                    } while (pLocalNode);

                    /* Reset the Delta*/
                    nCurrentDelta = 0;
                }

                /* This is either going to be a cd or ls command, just find the first letter */
                fgetc(pData);
                const char kCommand = fgetc(pData);
                fgetc(pData); fgetc(pData); /* Interestingly, fseek doesn't seem to respect the file i/o mode... */
 
                if ('c' == kCommand)
                {
                    const char kParameter = fgetc(pData);
                    if ('.' == kParameter)
                    {
                        assert(pNode->pParent);
                        pNode = pNode->pParent;
                    }
                    else if ('/' == kParameter)
                    {
                        pNode = pRootNode;
                    }
                    else
                    {
                        TFolderNodeType* pNewNode;
                        InitialiseFolderNode(pNode, &pNewNode, &kNodeList, &nNodeListSize, &nNodeListMaxSize);
                        pNode = pNewNode;
                    }
 
                    /* Wind on the the next line... */
                    while ('\n' != fgetc(pData))
                    {
                        if (feof(pData)) break;
                    }
                }
            }
            else
            {
                if ('d' == kData)
                {
                    /* Ignore Directories at this point, */
 
                    /* Wind on the the next line... */
                    while ('\n' != fgetc(pData))
                    {
                        if (feof(pData)) break;
                    }
                }
                else
                {
                    size_t nSize = (size_t)kData - '0';
                    char   kDigit;
 
                    /* Extract the File Size */
                    while (isdigit(kDigit = fgetc(pData)))
                    {
                        nSize = (nSize * 10) + (size_t)kDigit - '0';
                    }
 
                    /* Update the Current Folder Size Delt */
                    nCurrentDelta += nSize;
 
                    /* Wind on the the next line... */
                    while ('\n' != fgetc(pData))
                    {
                        if (feof(pData)) break;
                    }
                }
            }
        }

        /* And just incase the final command was a dir list... */
        if (0 != nCurrentDelta)
        {
            TFolderNodeType* pLocalNode = pNode;
            do
            {
                pLocalNode->nTotalSize += nCurrentDelta;
                pLocalNode              = pLocalNode->pParent;
            } while (pLocalNode);
        }
 
        /* Solve Both Parts */
        nNeededSpace = pRootNode->nTotalSize - (nMaxSpace - nFreeSpaceNeeded);
        for (i = 0; i < nNodeListSize; ++i)
        {
            if (kNodeList[i]->nTotalSize <= 100000ul)
            {
                nSumSmallSpace += kNodeList[i]->nTotalSize;
            }
 
            if (kNodeList[i]->nTotalSize >= nNeededSpace)
            {
                if (kNodeList[i]->nTotalSize < nSmallestFolderToDeleted)
                {
                    nSmallestFolderToDeleted = kNodeList[i]->nTotalSize;
                }
            }
        }
 
        printf("Part 1: %zu\n", nSumSmallSpace);
        printf("Part 2: %zu\n", nSmallestFolderToDeleted);
 
        /* Clean Up... note all the tree nodes are all stored in this 1D Array,
         * By cleaning this up, we undo every allocation.
         */
        for (i = 0; i < nNodeListSize; ++i)
        {
            assert(kNodeList[i]);
            free(kNodeList[i]);
        }
 
        free(kNodeList);
        fclose(pData);
    }

    return 0;
}