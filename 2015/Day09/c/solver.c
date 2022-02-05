#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include <assert.h>

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef struct routeType
{
    char*           pPlaces[2];
    unsigned long   nDistance;
} routeType;

typedef struct routesType
{
    routeType*  pRoutes;
    size_t      nRoutesSize;
    void*       pRoutesStartNode;
} routesType;

typedef struct placesType
{
    char**          pPlaces;
    size_t          nPlacesSize;
} placesType;

typedef struct bestScoreType
{
    char*           pPlace;
    unsigned long   nVisitedMask;
    unsigned long   nBestDistance;
} bestScoreType;

typedef struct bestScoresType
{
    bestScoreType*  pBestScores;
    size_t          nBestScoresSize;
    size_t          nBestScoresMaxSize;
    void*           pBestScoresStartNode;
} bestScoresType;

int routeCompare(const void *L, const void *R)
{
    const routeType *pLeft        = (routeType*)L;
    const routeType *pRight       = (routeType*)R;
    const int       nStartCompare = strcmp(pLeft->pPlaces[0], pRight->pPlaces[0]);
    const int       nEndCompare   = strcmp(pLeft->pPlaces[1], pRight->pPlaces[1]);

    if (nStartCompare != 0)
    {
        return nStartCompare;
    }
    return nEndCompare;
}

int bestScoreCompare(const void *L, const void *R)
{
    const bestScoreType *pLeft        = (bestScoreType*)L;
    const bestScoreType *pRight       = (bestScoreType*)R;
    const int           nPlaceCompare = strcmp(pLeft->pPlace, pRight->pPlace);

    if (nPlaceCompare != 0)
    {
        return nPlaceCompare;
    }

    return (long)pLeft->nVisitedMask - (long)pRight->nVisitedMask;
}

void bestScoresAdd(bestScoresType* pBestScores, bestScoreType* pEntry)
{
    assert(pBestScores->nBestScoresSize <= pBestScores->nBestScoresMaxSize);

    pBestScores->pBestScores[pBestScores->nBestScoresSize].pPlace        = pEntry->pPlace;
    pBestScores->pBestScores[pBestScores->nBestScoresSize].nVisitedMask  = pEntry->nVisitedMask;
    pBestScores->pBestScores[pBestScores->nBestScoresSize].nBestDistance = pEntry->nBestDistance;

    tsearch(&pBestScores->pBestScores[pBestScores->nBestScoresSize], &pBestScores->pBestScoresStartNode, &bestScoreCompare);
    ++pBestScores->nBestScoresSize;
}

void addToSet(char** pBuffer, size_t* nBufferSize, const char* kEntry)
{
    size_t i = 0;
    for (i = 0; i < *nBufferSize; ++i)
    {
        if (0 == strcmp(pBuffer[i], kEntry))
        {
            return;
        }
    }

    pBuffer[*nBufferSize] = malloc(sizeof(char) * (strlen(kEntry) + 1));
    strcpy(pBuffer[*nBufferSize], kEntry);
    *nBufferSize = *nBufferSize + 1;
}

#define findRouteWithDefaults(kPlaces, kRoutes, bFindShortest)                                                  \
              findRoute(kPlaces, kRoutes, bFindShortest,                                                        \
                        /* const unsigned long nCurrentScore = */ 0, /*const unsigned long nBestScore = */ 0,   \
                        /* bestScoresType* kBestScores = */ NULL,                                               \
                        /* const unsigned long nVisitedMask = */ 0,                                             \
                        /* const unsigned long nCompleteMask = */ 0,                                            \
                        /* char* kCurrentPlace = */ "")
unsigned long findRoute(const placesType* kPlaces, const routesType* kRoutes, const BOOL bFindShortest,
                        const unsigned long nCurrentScore, const unsigned long nBestScore,
                        bestScoresType* pBestScores,
                        const unsigned long nVisitedMask, const unsigned long nCompleteMask, char* kCurrentPlace)
{
    unsigned long nLocalBestScore = nBestScore;

    if (NULL == pBestScores)
    {
        nLocalBestScore = bFindShortest ? (unsigned long)-1 : 0;
        bestScoresType      kBestScoresBuffer   = {malloc(sizeof(bestScoreType) * 10000), 0, 10000, NULL};
        const unsigned long nCompleteMaskTarget = (1 << kPlaces->nPlacesSize) - 1;
        size_t              i;

        for (i = 0; i < kPlaces->nPlacesSize; ++i)
        {
            const unsigned long nPlaceMask = 1 << i;
            size_t j;
            for (j = 0; j < kPlaces->nPlacesSize; ++j)
            {
                if (i != j)
                {
                    /* Make sure the Route Exists */
                    routeType   kRoute = {{kPlaces->pPlaces[i], kPlaces->pPlaces[j]}, 0};
                    routeType** pFoundRoute = tfind(&kRoute, &kRoutes->pRoutesStartNode, &routeCompare);

                    if (NULL != pFoundRoute)
                    {
                        const unsigned long nTargetMask = 1 << j;
                        nLocalBestScore = findRoute(kPlaces, kRoutes, bFindShortest,
                                                    nCurrentScore + (*pFoundRoute)->nDistance, nLocalBestScore,
                                                    &kBestScoresBuffer,
                                                    nVisitedMask | nPlaceMask | nTargetMask, nCompleteMaskTarget,
                                                    kPlaces->pPlaces[j]);
                    }
                }
            }
        }

        free(kBestScoresBuffer.pBestScores);
    }
    else
    {
        size_t i;

        /* If the Task has been completed... return our best score */
        if (nVisitedMask == nCompleteMask)
        {
            if (bFindShortest)
            {
                if (nCurrentScore < nLocalBestScore)
                {
                    nLocalBestScore = nCurrentScore;
                }
            }
            else
            {
                if (nCurrentScore > nLocalBestScore)
                {
                    nLocalBestScore = nCurrentScore;
                }
            }
            return nLocalBestScore;
        }

        if (TRUE)
        {
            /* Determine if this is a duplicate state, if so, only proceed */
            /* *IF* we've got a better score */
            bestScoreType   kCurrentState = {kCurrentPlace, nVisitedMask, nCurrentScore};
            bestScoreType** pCurrentScore = tfind(&kCurrentState, &pBestScores->pBestScoresStartNode, &bestScoreCompare);

            if (NULL == pCurrentScore)
            {
                bestScoresAdd(pBestScores, &kCurrentState);
            }
            else
            {
                if (bFindShortest)
                {
                    if (nCurrentScore >= (*pCurrentScore)->nBestDistance)
                    {
                        return nLocalBestScore;
                    }
                    else
                    {
                        (*pCurrentScore)->nBestDistance = nCurrentScore;
                    }
                }
                else
                {
                    if (nCurrentScore <= (*pCurrentScore)->nBestDistance)
                    {
                        return nLocalBestScore;
                    }
                    else
                    {
                        (*pCurrentScore)->nBestDistance = nCurrentScore;
                    }
                }
            }
        }

        /* All subsequent calls have a fixed start position */
        for (i = 0; i < kPlaces->nPlacesSize; ++i)
        {
            /* Make sure this isn't a place we've already visited... */
            const unsigned long nTargetMask = 1 << i;
            if ((nTargetMask & nVisitedMask) == 0)
            {
                /* Test this is a Valid Route */
                routeType   kRoute = {{kCurrentPlace, kPlaces->pPlaces[i]}, 0};
                routeType** pFoundRoute = tfind(&kRoute, &kRoutes->pRoutesStartNode, &routeCompare);

                if (NULL != pFoundRoute)
                {
                    nLocalBestScore = findRoute(kPlaces, kRoutes, bFindShortest,
                                                nCurrentScore + (*pFoundRoute)->nDistance, nLocalBestScore,
                                                pBestScores,
                                                nVisitedMask | nTargetMask, nCompleteMask,
                                                kPlaces->pPlaces[i]);
                }
            }
        }
    }

    return nLocalBestScore;
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                   = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char**              kLines;
        char*               pLine;

        placesType          kPlaces                     = {NULL, 0};
        routesType          kRoutes                     = {NULL, 0, NULL};

        size_t              nLineCount                  = 0;
        size_t              i;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*) malloc((nFileSize+1)  * sizeof(char));
        kLines             = (char**)malloc((nFileSize)    * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");
        while (pLine)
        {
            kLines[nLineCount] = pLine;
            ++nLineCount;

            pLine = strtok(NULL, "\n");
        }

        /* At most, there will be 2 * Line Count for the number of unique places... */
        /* And there will definately be 2 * Line Count for the number of routes */
        kPlaces.pPlaces = malloc(sizeof(char**)    * nLineCount * 2);
        kRoutes.pRoutes = malloc(sizeof(routeType) * nLineCount * 2);

        /* We now have a Line Count and can properly process the Data */
        for (i = 0; i < nLineCount; ++i)
        {
            int j = 0;

            char* pToken = strtok(kLines[i], " ");
            while (pToken)
            {
                if ((0 == j) || (2 == j))
                {
                    addToSet(kPlaces.pPlaces, &kPlaces.nPlacesSize, pToken);
                    kRoutes.pRoutes[kRoutes.nRoutesSize].pPlaces[j/2] = pToken;
                }

                if (4 == j)
                {
                    kRoutes.pRoutes[kRoutes.nRoutesSize].nDistance = strtoul(pToken, NULL, 10);
                    ++kRoutes.nRoutesSize;

                    /* Insert to the Hash Map */
                    tsearch(&kRoutes.pRoutes[kRoutes.nRoutesSize-1], &kRoutes.pRoutesStartNode, &routeCompare);

                    kRoutes.pRoutes[kRoutes.nRoutesSize].pPlaces[0] = kRoutes.pRoutes[kRoutes.nRoutesSize-1].pPlaces[1];
                    kRoutes.pRoutes[kRoutes.nRoutesSize].pPlaces[1] = kRoutes.pRoutes[kRoutes.nRoutesSize-1].pPlaces[0];
                    kRoutes.pRoutes[kRoutes.nRoutesSize].nDistance  = kRoutes.pRoutes[kRoutes.nRoutesSize-1].nDistance;
                    ++kRoutes.nRoutesSize;

                    /* Insert to the Hash Map */
                    tsearch(&kRoutes.pRoutes[kRoutes.nRoutesSize-1], &kRoutes.pRoutesStartNode, &routeCompare);
                }

                pToken = strtok(NULL, " ");
                ++j;
            }
        }

        /* Deliberately don't free the Buffer as this will be the simplest */
        /* method of having something for the route storage to re-use... */
        free(kLines);

        printf("Part 1: %li\n", findRouteWithDefaults(&kPlaces, &kRoutes, TRUE));
        printf("Part 2: %li\n", findRouteWithDefaults(&kPlaces, &kRoutes, FALSE));

        free(kPlaces.pPlaces);
        free(kRoutes.pRoutes);

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);
   }

    return 0;
}
