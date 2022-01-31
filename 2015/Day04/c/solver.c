#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "md5.h"

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char*               kTestString;

        unsigned long       i      = 1;
        unsigned long       nPart1 = 0;
        unsigned long       nPart2 = 0;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*)        malloc((nFileSize+1)  * sizeof(char));
        kTestString        = (char*)        malloc((nFileSize+10) * sizeof(char));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        while ((0 == nPart1) || (0 == nPart2))
        {
            const unsigned char kTarget[3] = {0x00,0x00,0x00};
            MD5_CTX             kMD5Context;
            unsigned char       kDigest[16];
            const int           len = sprintf(kTestString, "%s%ld", kBuffer, i);

            MD5Init(&kMD5Context);
            MD5Update(&kMD5Context, (unsigned char*)kTestString, len);
            MD5Final(kDigest, &kMD5Context);

            if (0 == nPart1)
            {
                /* We're looking for 5 nibbles being "0".  We can do
                   a simple 2 byte comparison for the first 4... */
                if (0 == memcmp(kTarget, kDigest, 2))
                {
                    /* But the fifth needs masking */
                    if ((kDigest[2] & 0xF0) == 0x00)
                    {
                        nPart1 = i;
                    }
                }
            }

            if (0 == nPart2)
            {
                /* 6 nibbles is a much simpler 3 byte comparison */
                if (0 == memcmp(kTarget, kDigest, 3))
                {
                    nPart2 = i;
                }
            }

            ++i;
        }

        printf("Part 1: %ld\n", nPart1);
        printf("Part 2: %ld\n", nPart2);

        free(kBuffer);
        free(kTestString);
    }

    return 0;
}
