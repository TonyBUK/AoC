#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

const size_t GRID_WIDTH  = 50;
const size_t GRID_HEIGHT = 6;

typedef char gridType[GRID_HEIGHT][GRID_WIDTH + 1];

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

    *pkFileBuffer   = (char*) malloc((nFileSize+1)  * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize)    * sizeof(char*));

    fread(*pkFileBuffer, nFileSize, sizeof(char), *pFile);
    (*pkFileBuffer)[nFileSize] = '\0';

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
    }

    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}

void gridRect(gridType pGrid, const size_t WIDTH, const size_t HEIGHT, const char kOn)
{
    size_t y;

    assert(WIDTH < GRID_WIDTH);
    assert(HEIGHT < GRID_HEIGHT);

    for (y = 0; y < HEIGHT; ++y)
    {
        size_t x;
        for (x = 0; x < WIDTH; ++x)
        {
            pGrid[y][x] = kOn;
        }
    }
}

void rotateRow(gridType pGrid, const size_t ROW, const size_t SHIFT)
{
    char   kRowBuffer[GRID_WIDTH];
    size_t x;

    assert(ROW < GRID_HEIGHT);
    memcpy(kRowBuffer, pGrid[ROW], sizeof(kRowBuffer));

    for (x = 0; x < GRID_WIDTH; ++x)
    {
        size_t nGridX = (x + SHIFT) % GRID_WIDTH;
        pGrid[ROW][nGridX] = kRowBuffer[x];
    }
}

void rotateCol(gridType pGrid, const size_t COL, const size_t SHIFT)
{
    char   kColBuffer[GRID_HEIGHT];
    size_t y;

    assert(COL < GRID_WIDTH);
    for (y = 0; y < GRID_HEIGHT; ++y)
    {
        kColBuffer[y] = pGrid[y][COL];
    }

    for (y = 0; y < GRID_HEIGHT; ++y)
    {
        size_t nGridY = (y + SHIFT) % GRID_HEIGHT;
        pGrid[nGridY][COL] = kColBuffer[y];
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;

        size_t                  nLineCount;
        size_t                  i;

        gridType                kGrid;
        size_t                  x;
        size_t                  y;
        size_t                  nCount = 0;

        /* Initialise the Grid */
        for (y = 0; y < GRID_HEIGHT; ++y)
        {
            for (x = 0; x < GRID_WIDTH; ++x)
            {
                kGrid[y][x] = ' ';
            }
            kGrid[y][GRID_WIDTH] = '\0';
        }

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);

        for (i = 0; i < nLineCount; ++i)
        {
            typedef enum CommandType {E_NONE, E_RECT /* No Laughing! */, E_ROTATE, E_ROTATE_ROW, E_ROTATE_COLUMN} CommandType;

            char*       p = strtok(kLines[i], " ");
            CommandType eCommand;
            size_t      j = 0;
            size_t      nParams[2];

            while (p)
            {
                if (0 == j)
                {
                    if (0 == strcmp(p, "rect"))
                    {
                        eCommand = E_RECT;
                    }
                    else if (0 == strcmp(p, "rotate"))
                    {
                        eCommand = E_ROTATE;
                    }
                    else
                    {
                        assert(0);
                    }
                }
                else
                {
                    switch(eCommand)
                    {
                        case E_RECT:
                        {
                            assert(1 == j);

                            char* p2 = strstr(p, "x");
                            assert(p2);
                            *p2 = '\0';
                            ++p2;

                            nParams[0] = strtoull(p, NULL, 10);
                            nParams[1] = strtoull(p2, NULL, 10);

                            gridRect(kGrid, nParams[0], nParams[1], '#');
                        } break;

                        case E_ROTATE:
                        {
                            assert(1 == j);
                            if (0 == strcmp(p, "row"))
                            {
                                eCommand = E_ROTATE_ROW;
                            }
                            else if (0 == strcmp(p, "column"))
                            {
                                eCommand = E_ROTATE_COLUMN;
                            }
                            else
                            {
                                assert(0);
                            }
                        } break;

                        case E_ROTATE_ROW:
                        case E_ROTATE_COLUMN:
                        {
                            assert(j < 5);
                            if (2 == j)
                            {
                                char* p2 = strstr(p, "=");
                                assert(p2);
                                ++p2;
                                nParams[0] = strtoull(p2, NULL, 10);
                            }
                            else if (4 == j)
                            {
                                nParams[1] = strtoull(p, NULL, 10);

                                if (E_ROTATE_ROW == eCommand)
                                {
                                    rotateRow(kGrid, nParams[0], nParams[1]);
                                }
                                else
                                {
                                    rotateCol(kGrid, nParams[0], nParams[1]);
                                }
                            }
                        } break;

                        default:
                        {
                            assert(0);
                        }
                    }
                }

                p = strtok(NULL, " ");
                ++j;
            }
        }

        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        for (y = 0; y < GRID_HEIGHT; ++y)
        {
            for (x = 0; x < GRID_WIDTH; ++x)
            {
                if (' ' != kGrid[y][x])
                {
                    ++nCount;
                }
            }
        }

        printf("Part 1: %u\n", (uint32_t)nCount);
        printf("Part 2:\n");

        for (y = 0; y < GRID_HEIGHT; ++y)
        {
            printf("%s\n", kGrid[y]);
        }
    }

    return 0;
}
