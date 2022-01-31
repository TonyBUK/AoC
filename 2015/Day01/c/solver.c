#include <stdio.h>

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        int nCount    = 0;
        int nPosition = 0;
        int nBasement = -1;
        while (!feof(pInput))
        {
            const char c = fgetc(pInput);
            if ('(' == c)
            {
                ++nCount;
            }
            else if (')' == c)
            {
                --nCount;
            }
            else
            {
                continue;
            }

            nPosition += 1;
            if ((-1 == nCount) && (-1 == nBasement))
            {
                nBasement = nPosition;
            }
        }
        printf("Part 1: %d\n", nCount);
        printf("Part 2: %d\n", nBasement);
        fclose(pInput);
    }

    return 0;
}
