#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

typedef enum OpcodeType
{
    OPCODE_ADV             = 0,
    OPCODE_BXL             = 1,
    OPCODE_BST             = 2,
    OPCODE_JNZ             = 3,
    OPCODE_BXC             = 4,
    OPCODE_OUT             = 5,
    OPCODE_BDV             = 6,
    OPCODE_CDV             = 7
} OpcodeType;

typedef enum RegisterOffsetType
{
    REGISTER_A = 0,
    REGISTER_B = 1,
    REGISTER_C = 2
} RegisterOffsetType;

typedef struct RegisterType
{
    uint64_t kRegisters[REGISTER_C + 1];
} RegisterType;

#define REGISTER_START (12)
#define PROGRAM_START  (9)

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+3)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);
 
    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }
 
    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if (0 == bProcessUnix)
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\r';
            (*pkFileBuffer)[nFileSize+1] = '\n';
            (*pkFileBuffer)[nFileSize+2] = '\0';
        }
    }
    else
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\n';
            (*pkFileBuffer)[nFileSize+1] = '\0';
        }
    }
 
    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);
 
        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;
 
            assert(pLine < &(*pkFileBuffer)[nFileSize]);
 
            (*pkLines)[nLineCount++] = pLine;
 
            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';
 
            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

uint64_t getComboOperand(const unsigned char nOperand, const RegisterType* kRegisters)
{
    switch(nOperand)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        {
            return nOperand;
        }
        case 4:
        case 5:
        case 6:
        {
            return kRegisters->kRegisters[nOperand - 4];
        }

        default:
        {
            assert(0);
        }
    }
}

/* Opcode Execution as described by the puzzle */
void runProgram(const unsigned char* const kProgram, const size_t nProgramLength, RegisterType* pkRegisters, unsigned char* kOutput, size_t* pnOutputLength, size_t* pnMaxOutputLength)
{
    RegisterType kRegisters      = *pkRegisters;
    size_t       nProgramCounter = 0;
    size_t       nOutputLength   = 0;

    while(nProgramCounter < nProgramLength)
    {
        const OpcodeType    eOpcode  = (OpcodeType)kProgram[nProgramCounter];
        const unsigned char nOperand = kProgram[nProgramCounter+1];

        switch(eOpcode)
        {
            case OPCODE_ADV:
            {
                const uint64_t nNumerator   = kRegisters.kRegisters[REGISTER_A];
                const uint64_t nDenominator = 1 << getComboOperand(nOperand, &kRegisters);
                kRegisters.kRegisters[REGISTER_A] = nNumerator / nDenominator;
            } break;

            case OPCODE_BXL:
            {
                kRegisters.kRegisters[REGISTER_B] = (kRegisters.kRegisters[REGISTER_B] ^ (uint64_t)nOperand) % 8;
            } break;

            case OPCODE_BST:
            {
                kRegisters.kRegisters[REGISTER_B] = getComboOperand(nOperand, &kRegisters) % 8;
            } break;

            case OPCODE_JNZ:
            {
                if (kRegisters.kRegisters[REGISTER_A] != 0)
                {
                    nProgramCounter = nOperand;
                    continue;
                }
            } break;

            case OPCODE_BXC:
            {
                kRegisters.kRegisters[REGISTER_B] = (kRegisters.kRegisters[REGISTER_B] ^ kRegisters.kRegisters[REGISTER_C]) % 8;
            } break;

            case OPCODE_OUT:
            {
                if (nOutputLength == *pnMaxOutputLength)
                {
                    *pnMaxOutputLength *= 2;
                    kOutput = (unsigned char*)realloc(kOutput, *pnMaxOutputLength * sizeof(unsigned char));
                }

                kOutput[nOutputLength++] = (unsigned char)(getComboOperand(nOperand, &kRegisters) % 8);
            } break;

            case OPCODE_BDV:
            {
                const uint64_t nNumerator   = kRegisters.kRegisters[REGISTER_A];
                const uint64_t nDenominator = 1 << getComboOperand(nOperand, &kRegisters);
                kRegisters.kRegisters[REGISTER_B] = (nNumerator / nDenominator) % 8;
            } break;

            case OPCODE_CDV:
            {
                const uint64_t nNumerator   = kRegisters.kRegisters[REGISTER_A];
                const uint64_t nDenominator = 1 << getComboOperand(nOperand, &kRegisters);
                kRegisters.kRegisters[REGISTER_C] = (nNumerator / nDenominator) % 8;
            } break;

            default:
            {
                assert(0);
            }
        }

        nProgramCounter += 2;
    }

    *pnOutputLength = nOutputLength;
}

/*
 * Opcode Reverse Execution
 * - Divides become multiplies.
 * - XOR's remain unchanged.
 * - JNZ becomes an iteration flag.
 * - Output is when we test about returning the lowest A value if we've achieved the expected output depth, and therefore the expected output.
 */
uint64_t runProgramReversed(const unsigned char* const kProgram, const size_t nProgramLength, RegisterType* kRegisters, unsigned char* kOutput, size_t* pnOutputLength, size_t* pnMaxOutputLength, const size_t nProgramCounter, const size_t nDepth, const uint64_t nLowestA)
{
    size_t       nProgramCounterLocal = nProgramCounter;
    RegisterType kRegistersLocal      = *kRegisters;
    size_t       nIterations          = 0;
    uint64_t     nLowestALocal        = nLowestA;

    if (0 == nDepth)
    {
        kRegistersLocal.kRegisters[REGISTER_A] = 0;
        kRegistersLocal.kRegisters[REGISTER_B] = 0;
        kRegistersLocal.kRegisters[REGISTER_C] = 0;
        nProgramCounterLocal                   = nProgramLength - 2;
        nLowestALocal                          = (uint64_t)-1;
    }

    while(1)
    {
        const OpcodeType    eOpcode  = (OpcodeType)kProgram[nProgramCounterLocal];
        const unsigned char nOperand = kProgram[nProgramCounterLocal+1];

        switch(eOpcode)
        {
            case OPCODE_ADV:
            {
                const uint64_t nNumerator   = kRegistersLocal.kRegisters[REGISTER_A];
                const uint64_t nDenominator = 1 << getComboOperand(nOperand, &kRegistersLocal);
                const uint64_t nRegisterA   = nNumerator * nDenominator;
                uint64_t nOffset;

                for (nOffset = 0; nOffset < 8; ++nOffset)
                {
                    kRegistersLocal.kRegisters[REGISTER_A] = nRegisterA + nOffset;
                    runProgram(kProgram, nProgramLength, &kRegistersLocal, kOutput, pnOutputLength, pnMaxOutputLength);

                    if (0 == memcmp(kOutput, &kProgram[nProgramLength-*pnOutputLength], *pnOutputLength * sizeof(unsigned char)))
                    {
                        const uint64_t nCurrentA = runProgramReversed(kProgram, nProgramLength, &kRegistersLocal, kOutput, pnOutputLength, pnMaxOutputLength, nProgramCounterLocal - 2, nDepth + 1, nLowestALocal);
                        nLowestALocal = MIN(nLowestALocal, nCurrentA);
                    }
                }

            } break;

            case OPCODE_BXL:
            {
                kRegistersLocal.kRegisters[REGISTER_B] = (kRegistersLocal.kRegisters[REGISTER_B] ^ (uint64_t)nOperand) % 8;
            } break;

            case OPCODE_BST:
            {
                kRegistersLocal.kRegisters[REGISTER_B] = getComboOperand(nOperand, &kRegistersLocal) % 8;
            } break;

            case OPCODE_JNZ:
            {
                nIterations += 1;
            } break;

            case OPCODE_BXC:
            {
                kRegistersLocal.kRegisters[REGISTER_B] = (kRegistersLocal.kRegisters[REGISTER_B] ^ kRegistersLocal.kRegisters[REGISTER_C]) % 8;
            } break;

            case OPCODE_OUT:
            {
                if (nDepth == nProgramLength)
                {
                    return kRegistersLocal.kRegisters[REGISTER_A];
                }
                else if ((nDepth == 0) && (nIterations == 1))
                {
                    assert(nOperand >= 4 && nOperand <= 6);
                    kRegistersLocal.kRegisters[nOperand - 4] = kProgram[0];
                }
            } break;

            case OPCODE_BDV:
            {
                const uint64_t nNumerator              = kRegistersLocal.kRegisters[REGISTER_A];
                const uint64_t nDenominator            = 1 << getComboOperand(nOperand, &kRegistersLocal);
                kRegistersLocal.kRegisters[REGISTER_B] = (nNumerator * nDenominator) % 8;
            } break;

            case OPCODE_CDV:
            {
                const uint64_t nNumerator              = kRegistersLocal.kRegisters[REGISTER_A];
                const uint64_t nDenominator            = 1 << getComboOperand(nOperand, &kRegistersLocal);
                kRegistersLocal.kRegisters[REGISTER_C] = (nNumerator * nDenominator) % 8;
            } break;

            default:
            {
                assert(0);
            }
        }

        if (nProgramCounterLocal > 0)
        {
            nProgramCounterLocal -= 2;
        }
        else
        {
            if (nIterations > 1)
            {
                return nLowestALocal;
            }
            else
            {
                nProgramCounterLocal = nProgramLength - 2;
            }
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kBuffer;
        char**                      kPuzzleData;
 
        size_t                      nLines;
        size_t                      nMaxLineLength;
        size_t                      i;

        unsigned char*              kProgram;
        size_t                      nProgramLength;
        RegisterType                kRegisters;

        unsigned char*              kOuput;
        size_t                      nOutputLength;
        size_t                      nOutputMaxLength;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kBuffer, &kPuzzleData, &nLines, NULL, &nMaxLineLength, 0);
        fclose(pData);

        /* 5 Lines : 3 for the Registers, a Blank Line, and the Program */
        assert(nLines == 5);

        /*
         * Allocate the Program.
         *
         * Note: This is derived from each input being octal (i.e. 1 char), stored with commas, and having a prefix of the
         *       word: "Program: "
         *       It might seem like it should have 9 subtracted, however, the final comma is not present, hence 8.
         */
        nProgramLength   = (nMaxLineLength - (PROGRAM_START - 1)) / 2;
        kProgram         = (unsigned char*)malloc(nProgramLength * sizeof(unsigned char));

        /* Store the Register Data */
        kRegisters.kRegisters[REGISTER_A] = strtoull(&kPuzzleData[REGISTER_A][REGISTER_START], NULL, 10);
        kRegisters.kRegisters[REGISTER_B] = strtoull(&kPuzzleData[REGISTER_B][REGISTER_START], NULL, 10);
        kRegisters.kRegisters[REGISTER_C] = strtoull(&kPuzzleData[REGISTER_C][REGISTER_START], NULL, 10);

        /* Store the Program Data */
        for (i = PROGRAM_START; i < nMaxLineLength; i += 2)
        {
            kProgram[(i-PROGRAM_START)/2] = (unsigned char)strtoul(&kPuzzleData[4][i], NULL, 10);
        }

        /* Initialise the Output Data to a "Safe" value */
        nOutputMaxLength = nProgramLength;
        kOuput           = (unsigned char*)malloc(nOutputMaxLength * sizeof(unsigned char));

        runProgram(kProgram, nProgramLength, &kRegisters, kOuput, &nOutputLength, &nOutputMaxLength);
        printf("Part 1: ");
        for (i = 0; i < nOutputLength; ++i)
        {
            printf("%c", kOuput[i] + '0');
            if (i < nOutputLength - 1)
            {
                printf(",");
            }
        }
        printf("\n");

        printf("Part 2: %" PRIu64 "\n", runProgramReversed(kProgram, nProgramLength, &kRegisters, kOuput, &nOutputLength, &nOutputMaxLength, 0, 0, 0));

        free(kPuzzleData);
        free(kBuffer);
        free(kProgram);
        free(kOuput);
    }
 
    return 0;
}