#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <time.h>
#include "md5.h"

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    srand((unsigned)time(NULL));

    if (pInput)
    {
        const char*         kMovieOS                = "0123456789abcdefghijklmnopqrstuvwxyz";
        const size_t        nMovieOSSize            = strlen(kMovieOS);
        const unsigned long nStartPos               = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char*               kTestString;

        char                kPasswordPart1[9]       = {'\0'};
        char                kPasswordPart2[9]       = "        ";
        size_t              nPasswordPart2Validity  = 0;

        unsigned long       i                       = 0;

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

        while ((strlen(kPasswordPart1) < 8) || (0xFF != nPasswordPart2Validity))
        {
            const unsigned char kTarget[2] = {0x00,0x00};
            MD5_CTX             kMD5Context;
            unsigned char       kDigest[16];
            const int           len = sprintf(kTestString, "%s%ld", kBuffer, i);

            MD5Init(&kMD5Context);
            MD5Update(&kMD5Context, (unsigned char*)kTestString, len);
            MD5Final(kDigest, &kMD5Context);

            /* We're looking for 5 nibbles being "0".  We can do
                a simple 2 byte comparison for the first 4... */
            if (0 == memcmp(kTarget, kDigest, 2))
            {
                /* But the fifth needs masking */
                if ((kDigest[2] & 0xF0) == 0x00)
                {
                    const size_t nPosition = kDigest[2] & 0x0F;
                    if (strlen(kPasswordPart1) < 8)
                    {
                        sprintf(&kPasswordPart1[strlen(kPasswordPart1)], "%1x", kDigest[2] & 0x0F);
                    }

                    if (nPosition <= 7)
                    {
                        const size_t nValidityMask = 1 << nPosition;
                        if (0 == (nValidityMask & nPasswordPart2Validity))
                        {
                            const char* kHex          = "0123456789abcdef";
                            kPasswordPart2[nPosition] = kHex[kDigest[3] >> 4];
                            nPasswordPart2Validity   |= nValidityMask;
                        }
                    }
                }
            }

            ++i;

            /* Movie OS
             * This will rapidly print a garbled password containing a mix of:
             * 1. Known values which never change
             * 2. Random alphanumeric values for unknown entries
             *
             * The rate of change is unknown but acts after a large number of permutations
             * have been evaluated.
             *
             * Unfortunately going into higher than second level time precision tends to get
             * into the domain of OS specific operations, and has been deliberately excluded
             * for portability. */
            if (0 == (i % 50000))
            {
                size_t j;
                for (j = 0; j < (sizeof(kPasswordPart2) - 1); ++j)
                {
                    const size_t nValidityMask = 1 << j;
                    if (0 == (nValidityMask & nPasswordPart2Validity))
                    {
                        kPasswordPart2[j] = kMovieOS[rand() % nMovieOSSize];
                    }
                }
                printf("Hacking FBI Mainframe... %s\r", kPasswordPart2);
                fflush(stdout);
            }
        }

        printf("Welcome to the real world...          \n");

        printf("Part 1: %s\n", kPasswordPart1);
        printf("Part 2: %s\n", kPasswordPart2);

        free(kBuffer);
        free(kTestString);
    }

    return 0;
}
