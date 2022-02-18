#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount)
{
    const unsigned long     nStartPos   = ftell(*pFile);
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    char*                   pLine;
    size_t                  nLineCount  = 0;

    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);

    nFileSize       = nEndPos - nStartPos;

    *pkFileBuffer   = (char*) malloc((nFileSize+1)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char));

    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    (*pkFileBuffer)[nFileSize] = '\0';

    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, "\n");
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
        if (nLineLength > 0)
        {
            if (pLine[nLineLength-1] == '.')
            {
                pLine[nLineLength-1] = '\0';
            }
        }

        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;

        pLine = strtok(NULL, "\n");
    }

    *pnLineCount = nLineCount;
}

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef enum compoundType
{
    E_CHILDREN  = 0,
    E_CATS,
    E_SAMOYEDS,
    E_POMERANIANS,
    E_AKITAS,
    E_VIZSLAS,
    E_GOLDFISH,
    E_TREES,
    E_CARS,
    E_PERFUMES,
    E_LAST
} compoundType;

typedef struct compoundQuantityType
{
    compoundType    eCompound;
    size_t          nQuantity;
} compoundQuantityType;

typedef struct auntType
{
    size_t                  nSue;
    compoundQuantityType    kCompounds[E_LAST];
    size_t                  nCompoundCount;
} auntType;

compoundType getType(const char* kCompound)
{
    const char* kLookup[E_LAST] =
    {
        "children",
        "cats",
        "samoyeds",
        "pomeranians",
        "akitas",
        "vizslas",
        "goldfish",
        "trees",
        "cars",
        "perfumes"
    };
    size_t i;

    for (i = 0; i < E_LAST; ++i)
    {
        if (0 == strcmp(kCompound, kLookup[i]))
        {
            return (compoundType)i;
        }
    }

    return E_LAST;
}

BOOL inList(const compoundType eCompound, const compoundType* kList, const size_t nListSize)
{
    size_t i;
    for (i = 0; i < nListSize; ++i)
    {
        if (eCompound == kList[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}

size_t whichSue(const size_t*       kWrappingPaperCompounds,
                const auntType*     kSues,
                const size_t        nSueCount,
                const compoundType* kFewerThan,
                const size_t        nFewerSize,
                const compoundType* kGreaterThan,
                const size_t        nGreaterThanSize)
{
    size_t nSue;

    /* Iterate over each Sue */
    for (nSue = 0; nSue < nSueCount; ++nSue)
    {
        size_t nCompound;

        /* Initially assume this is our Sue */
        BOOL bValidSue = TRUE;

        /* Iterate over each compound you remember about Sue */
        for (nCompound = 0; (TRUE == bValidSue) && (nCompound < kSues[nSue].nCompoundCount); ++nCompound)
        {
            /* Determine how to compare the compound */
            /* Greater Than, Fewer Than or Equal To  */
            const compoundType eCompound    = kSues[nSue].kCompounds[nCompound].eCompound;
            const size_t       nQuantity    = kSues[nSue].kCompounds[nCompound].nQuantity;
            const BOOL         bGreaterThan = inList(eCompound, kGreaterThan, nGreaterThanSize);
            const BOOL         bFewerThan   = inList(eCompound, kFewerThan,   nFewerSize);

            if (bGreaterThan)
            {
                bValidSue = nQuantity > kWrappingPaperCompounds[eCompound];
            }
            else if (bFewerThan)
            {
                bValidSue = nQuantity < kWrappingPaperCompounds[eCompound];
            }
            else
            {
                bValidSue = nQuantity == kWrappingPaperCompounds[eCompound];
            }
        }

        /* If everything we remembered about Sue matches, we've found our Sue! */
        if (bValidSue)
        {
            return kSues[nSue].nSue;
        }
    }

    /* Shouldn't get here unless all the Sue's were wrong?!? */
    assert(FALSE);
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const size_t kWrappingPaper[E_LAST] =
        {
            3,  /* E_CHILDREN       */
            7,  /* E_CATS           */
            2,  /* E_SAMOYEDS       */
            3,  /* E_POMERANIANS    */
            0,  /* E_AKITAS         */
            0,  /* E_VIZSLAS        */
            5,  /* E_GOLDFISH       */
            3,  /* E_TREES          */
            2,  /* E_CARS           */
            1   /* E_PERFUMES       */
        };

        const compoundType      kGreaterThan[]  = {E_CATS, E_TREES};
        const compoundType      kFewerThan[]    = {E_POMERANIANS, E_GOLDFISH};

        char*                   kBuffer;
        char**                  kLines;
        size_t                  nSueCount;
        auntType*               kSues;

        size_t                  i;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nSueCount);
        fclose(pInput);

        kSues = (auntType*)malloc(nSueCount * sizeof(auntType));

        for (i = 0; i < nSueCount; ++i)
        {
            char*        pToken  = strtok(kLines[i], " ");
            size_t       j       = 0;
            compoundType eLastCompound;

            while (pToken)
            {
                if (0 == j)
                {
                }
                else if (1 == j)
                {
                    kSues[i].nSue           = strtoull(pToken, NULL, 10);
                    kSues[i].nCompoundCount = 0;
                }
                else if (0 == (j % 2))
                {
                    char* p = strstr(pToken, ":");
                    if (p)
                    {
                        *p = '\0';
                    }
                    eLastCompound = getType(pToken);
                }
                else
                {
                    kSues[i].kCompounds[kSues[i].nCompoundCount].eCompound = eLastCompound;
                    kSues[i].kCompounds[kSues[i].nCompoundCount].nQuantity = strtoull(pToken, NULL, 10);
                    ++kSues[i].nCompoundCount;
                }

                pToken = strtok(NULL, " ");
                ++j;
            }
        }

        /* Free the Line Buffers since we've parsed all the data */
        free(kBuffer);
        free(kLines);

        printf("Part 1: %u\n", (uint32_t)whichSue(kWrappingPaper, kSues, nSueCount, NULL,       0,                                        NULL,         0));
        printf("Part 2: %u\n", (uint32_t)whichSue(kWrappingPaper, kSues, nSueCount, kFewerThan, sizeof(kFewerThan)/sizeof(kFewerThan[0]), kGreaterThan, sizeof(kGreaterThan)/sizeof(kGreaterThan[0])));

        /* Free the Sues */
        free(kSues);
   }

    return 0;
}
