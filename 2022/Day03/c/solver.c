#include <stdio.h>
#include <assert.h>

#define MAX_BACKPACK_SIZE (256)

unsigned long CompartmentDuplicate(const unsigned char* kBackpack, const unsigned long nBackpackSize)
{
    const unsigned long nBackpackHalfSize = nBackpackSize / 2;
    int i, j;

    for (i = 0; i < nBackpackHalfSize; ++i)
    {
        for (j = nBackpackHalfSize; j < nBackpackSize; ++j)
        {
            if (kBackpack[i] == kBackpack[j])
            {
                return kBackpack[i];
            }
        }
    }

    assert(0);
}

unsigned long BackpackDuplicate(const unsigned char* kBackpack1, const unsigned long nBackpack1Size,
                                const unsigned char* kBackpack2, const unsigned long nBackpack2Size,
                                const unsigned char* kBackpack3, const unsigned long nBackpack3Size)
{
    int i,j,k;

    for (int i = 0; i < nBackpack1Size; ++i)
    {
        for (int j = 0; j < nBackpack2Size; ++j)
        {
            if (kBackpack1[i] == kBackpack2[j])
            {
                for (int k = 0; k < nBackpack3Size; ++k)
                {
                    if (kBackpack1[i] == kBackpack3[k])
                    {
                        return kBackpack1[i];
                    }
                }
            }
        }
    }

    assert(0);
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");

    if (pData)
    {
        const unsigned char kASCIITranslation[128] =
        {
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,
            42,43,44,45,46,47,48,49,50,51,52, 0, 0, 0, 0, 0,
             0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
            16,17,18,19,20,21,22,23,24,25,26, 0, 0, 0, 0, 0
        };

        unsigned long nPart1  = 0;
        unsigned long nPart2  = 0;

        unsigned long nBuffer = 0;
        while (!feof(pData))
        {
            unsigned char kBackpacks[3][MAX_BACKPACK_SIZE];
            unsigned long kBackpackSizes[3];

            unsigned char kItem;
            kBackpackSizes[nBuffer] = 0;
            while ((unsigned char)'\n' != (kItem = (unsigned char)fgetc(pData)))
            {
                if (!feof(pData))
                {
                    assert(kBackpackSizes[nBuffer] < MAX_BACKPACK_SIZE);
                    kBackpacks[nBuffer][kBackpackSizes[nBuffer]] = kASCIITranslation[kItem];
                    ++kBackpackSizes[nBuffer];
                }
                else
                {
                    break;
                }
            }

            assert((kBackpackSizes[nBuffer] % 2) == 0);

            // Solve Part 1
            nPart1 += CompartmentDuplicate(kBackpacks[nBuffer], kBackpackSizes[nBuffer]);

            nBuffer = (nBuffer + 1) % 3;

            // Solve Part 2
            if (nBuffer == 0)
            {
                nPart2 += BackpackDuplicate(kBackpacks[0], kBackpackSizes[0],
                                            kBackpacks[1], kBackpackSizes[1],
                                            kBackpacks[2], kBackpackSizes[2]);
            }
        }
 
        printf("Part 1: %lu\n", nPart1);
        printf("Part 2: %lu\n", nPart2);
 
        fclose(pData);
    }
 
    return 0;
}