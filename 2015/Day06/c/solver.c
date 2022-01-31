#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long HEIGHT                    = 1000;
        const unsigned long WIDTH                     = 1000;
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char*               pName;

        unsigned long*      kLight;
        unsigned long*      kLightDimmer;

        unsigned long       nPart1 = 0;
        unsigned long       nPart2 = 0;

        size_t              X;
        size_t              Y;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*)        malloc((nFileSize+1)  * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        /* Allocate and initialise the Lights/Dimmer Arrays */
        kLight          = (unsigned long*)malloc(WIDTH * HEIGHT * sizeof(unsigned long));
        kLightDimmer    = (unsigned long*)malloc(WIDTH * HEIGHT * sizeof(unsigned long));

        memset(kLight,       0, WIDTH * HEIGHT * sizeof(unsigned long));
        memset(kLightDimmer, 0, WIDTH * HEIGHT * sizeof(unsigned long));

        pName = strtok(kBuffer, "\n");
        while (pName)
        {
            size_t xStart;
            size_t yStart;
            size_t xEnd;
            size_t yEnd;
            if (strstr(pName, "toggle"))
            {
                char* pStartX = &pName[7];
                char* pStartY = strstr(pStartX, ",") + 1;
                char* pEndX   = strstr(&pStartY[4], " ") + 1;
                char* pEndY   = strstr(pEndX, ",") + 1;
                xStart = atoi(pStartX);
                yStart = atoi(pStartY);
                xEnd   = atoi(pEndX);
                yEnd   = atoi(pEndY);
            }
            else
            {
                char* pStartX = strstr(&pName[5], " ") + 1;
                char* pStartY = strstr(pStartX, ",") + 1;
                char* pEndX   = strstr(&pStartY[4], " ") + 1;
                char* pEndY   = strstr(pEndX, ",") + 1;
                xStart = atoi(pStartX);
                yStart = atoi(pStartY);
                xEnd   = atoi(pEndX);
                yEnd   = atoi(pEndY);
            }

            for (Y = yStart; Y <= yEnd; ++Y)
            {
                for (X = xStart; X <= xEnd; ++X)
                {
                    if (strstr(pName, "toggle"))
                    {
                        kLight[(Y * WIDTH) + X]        = 1 - kLight[(Y * WIDTH) + X];
                        kLightDimmer[(Y * WIDTH) + X] += 2;
                    }
                    else if (strstr(pName, "off"))
                    {
                        kLight[(Y * WIDTH) + X] = 0;
                        if (kLightDimmer[(Y * WIDTH) + X] > 0) --kLightDimmer[(Y * WIDTH) + X];
                    }
                    else if (strstr(pName, "on"))
                    {
                        kLight[(Y * WIDTH) + X] = 1;
                        ++kLightDimmer[(Y * WIDTH) + X];
                    }
                    else
                    {
                        assert(0);
                    }
                }
            }

            pName = strtok(NULL, "\n");
        }

        for (size_t Y = 0; Y < HEIGHT; ++Y)
        {
            for (size_t X = 0; X < WIDTH; ++X)
            {
                nPart1 += kLight[(Y * WIDTH) + X];
                nPart2 += kLightDimmer[(Y * WIDTH) + X];
            }
        }

        printf("Part 1: %ld\n", nPart1);
        printf("Part 2: %ld\n", nPart2);

        free(kBuffer);
        free(kLight);
        free(kLightDimmer);
    }

    return 0;
}
