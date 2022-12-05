#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength)
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
 
    *pkFileBuffer   = (char*) malloc((nFileSize+2)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));
 
    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    if ((*pkFileBuffer)[nFileSize] != '\n')
    {
        (*pkFileBuffer)[nFileSize]   = '\n';
        (*pkFileBuffer)[nFileSize+1] = '\0';
    }
    else
    {
        (*pkFileBuffer)[nFileSize]   = '\0';
    }
 
    /* Find each line and store the result in the kLines Array */
    /* Note: This specifically relies upon strtok overwriting new line characters with
                NUL terminators. */
    pLine = strtok(*pkFileBuffer, "\n");
    while (pLine)
    {
        const size_t nLineLength = strlen(pLine);
 
        (*pkLines)[nLineCount] = pLine;
        ++nLineCount;

        pLine = strtok(NULL, "\n");

        /* This doesn't seem to handle multiiple new line hits...
         * Bodge to just handle a double \n\n for now.
         */
        if (NULL != pLine)
        {
            if (pLine[-1] == '\n')
            {
                (*pkLines)[nLineCount] = &pLine[-1];
                pLine[-1] = '\0';
                ++nLineCount;
            }
        }
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

long GetNumber(char** p)
{
    long nResult;

    while (!isdigit(**p))
    {
        ++(*p);
    }

    nResult = strtol(*p, NULL, 10);

    while (isdigit(**p))
    {
        ++(*p);
    }

    return nResult;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  nLineCount;

        /* Read the entire file into a line buffer. */
        readLines(&pData, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pData);

        {
            /* Determine where the dividor is between the Crates and the Moves */
            size_t  MAX_SIZE;
            size_t  nMoveStart;
            int     i, j;
            size_t  nColumnCount;
            char**  kCrateStack9000;
            char**  kCrateStack9001;
            size_t* kCrateStackDepth; /* This will always be common, as crate order changes, not the quantity. */

            /* We need to work out the worst case size of a crate stack.  We can approximate this by
             * and multipling it by columns.  The correct way would be counting crates, but since this
             * is purely to ensure this allocates enough memory, the approximation is fine.
             */            
            for (i = 0; i < nLineCount; ++i)
            {
                if (kLines[i][0] == '\0')
                {
                    assert(i > 0);

                    nMoveStart   = i + 1;
                    nColumnCount = (strlen(kLines[i-1]) + 1) / 4;

                    /* Create the Create Stacks */
                    kCrateStack9000      = (char**) malloc(nColumnCount * sizeof(char*));
                    kCrateStack9001      = (char**) malloc(nColumnCount * sizeof(char*));
                    kCrateStackDepth     = (size_t*)malloc(nColumnCount * sizeof(size_t));

                    MAX_SIZE             = (i-1) * nColumnCount;

                    for (j = 0; j < nColumnCount; ++j)
                    {
                        kCrateStack9000[j]      = (char*)malloc(MAX_SIZE * sizeof(char));
                        kCrateStack9001[j]      = (char*)malloc(MAX_SIZE * sizeof(char));
                        kCrateStackDepth[j]     = 0;
                    }

                    break;
                }
            }

            /* Setup the initial state of the Crate Stacks.  One will follow the
             * Crate Mover 9000 rules, the other, Crate Mover 9001.
             */
            for (i = (nMoveStart-3); i >= 0; --i)
            {
                for (j = 0; j < nColumnCount; ++j)
                {
                    const char kCandidate = kLines[i][(j * 4) + 1];
                    if (kCandidate != ' ')
                    {
                        kCrateStack9000[j][kCrateStackDepth[j]] = kCandidate;
                        kCrateStack9001[j][kCrateStackDepth[j]] = kCandidate;
                        ++kCrateStackDepth[j];
                    }
                }
            }

            /* Solve the Puzzle */
            for (i = nMoveStart; i < nLineCount; ++i)
            {
                /* Extract the Count, Source and Target */
                char* p       = kLines[i];
                long  nCount  = GetNumber(&p);
                long  nSource = GetNumber(&p) - 1;
                long  nTarget = GetNumber(&p) - 1;

                /* The Crate Mover 9000 will *reverse* the Crate Order
                 * The Crate Mover 9001 will *maintain* the Crate Order
                 */
                for (j = 0; j < nCount; ++j)
                {
                    kCrateStack9000[nTarget][kCrateStackDepth[nTarget] + j] =
                        kCrateStack9000[nSource][kCrateStackDepth[nSource] - 1 - j];

                    /* Note: This should actually be:
                     * kCrateStack9001[nSource][kCrateStackDepth[nSource] - 1 - (nCount - j - 1)]
                     * but the two -1's cancel out.
                     */
                    kCrateStack9001[nTarget][kCrateStackDepth[nTarget] + j] =
                        kCrateStack9001[nSource][kCrateStackDepth[nSource] - (nCount - j)];
                }
                kCrateStackDepth[nTarget] += nCount;
                kCrateStackDepth[nSource] -= nCount;

                assert(kCrateStackDepth[nTarget] < MAX_SIZE);
            }

            /* Output Part 1 */
            printf("Part 1: ");
            for (j = 0; j < nColumnCount; ++j)
            {
                printf("%c", kCrateStack9000[j][kCrateStackDepth[j] - 1]);
            }
            printf("\n");

            /* Output Part 2 */
            printf("Part 2: ");
            for (j = 0; j < nColumnCount; ++j)
            {
                printf("%c", kCrateStack9001[j][kCrateStackDepth[j] - 1]);
            }
            printf("\n");

            /* Cleanup on isle 6...*/
            for (j = 0; j < nColumnCount; ++j)
            {
                free(kCrateStack9000[j]);
                free(kCrateStack9001[j]);
            }

            free(kCrateStack9000);
            free(kCrateStack9001);
            free(kCrateStackDepth);
        }

        free(kBuffer);
        free(kLines);
    }
 
    return 0;
}