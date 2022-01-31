#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

unsigned long min(const unsigned long* kArray, const unsigned long nSize)
{
    unsigned long minValue = 0xFFFFFFFF;
    unsigned long i;
    for (i = 0; i < nSize; ++i)
    {
        if (kArray[i] < minValue)
        {
            minValue = kArray[i];
        }
    }
    return minValue;
}

int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

unsigned long calculateWrappingPaper(const unsigned long box[3])
{
    const int           L           = 0;
    const int           W           = 1;
    const int           H           = 2;
    const unsigned long SIDES[]     = {box[L] * box[W], box[W] * box[H], box[H] * box[L]};
    unsigned long       sideArea    = 0;
    int                 i;

    for (i = 0; i < sizeof(SIDES) / sizeof(const unsigned long); ++i)
    {
        sideArea += 2 * SIDES[i];
    }

    return min(SIDES, sizeof(SIDES) / sizeof(const unsigned long)) + sideArea;
}

unsigned long calculateRibbon(const unsigned long box[3])
{
    unsigned long sortedBox[] = {box[0], box[1], box[2]};
    qsort (sortedBox, sizeof(sortedBox)/sizeof(unsigned long), sizeof(unsigned long), compare);
    return (2 * sortedBox[0]) + (2 * sortedBox[1]) + (box[0] * box[1] * box[2]);
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        unsigned long box[3]        = {0,0,0};
        int           i             = 0;
        unsigned long wrappingPaper = 0;
        unsigned long ribbon        = 0;
        while (!feof(pInput))
        {
            const char c = fgetc(pInput);
            if (isdigit(c))
            {
                box[i] = (box[i] * 10) + (c - '0');
            }
            else if ('x' == c)
            {
                assert(i < 2);
                i     += 1;
                box[i] = 0;
            }
            else
            {
                assert(i == 2);
                wrappingPaper += calculateWrappingPaper(box);
                ribbon        += calculateRibbon(box);
                i      = 0;
                box[i] = 0;
            }
        }
        printf("Part 1: %lu\n", wrappingPaper);
        printf("Part 2: %lu\n", ribbon);
        fclose(pInput);
    }

    return 0;
}
