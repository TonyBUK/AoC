#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

typedef enum RegisterSourceType
{
    E_REGISTER_A     = 0,
    E_REGISTER_B,
    E_NOT_A_REGISTER
} RegisterSourceType;

typedef struct RegisterType
{
    int64_t nRegisters[2];
} RegisterType;

typedef enum OpcodeType
{
    E_OPCODE_HLF,
    E_OPCODE_TPL,
    E_OPCODE_INC,
    E_OPCODE_JMP,
    E_OPCODE_JIE,
    E_OPCODE_JIO
} OpcodeType;

typedef struct OperandType
{
    RegisterSourceType  eRegister;
    int64_t             nLiteral;
} OperandType;

typedef struct ByteCodeType
{
    OpcodeType      eOpcode;
    OperandType     kOperands[2];
    size_t          nOperandsCount;
} ByteCodeType;

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

    *pnLineCount = nLineCount;
}

OpcodeType decodeOpcode(const char* kOpcode)
{
    if (strcmp("hlf", kOpcode) == 0)
    {
        return E_OPCODE_HLF;
    }
    else if (strcmp("tpl", kOpcode) == 0)
    {
        return E_OPCODE_TPL;
    }
    else if (strcmp("inc", kOpcode) == 0)
    {
        return E_OPCODE_INC;
    }
    else if (strcmp("jmp", kOpcode) == 0)
    {
        return E_OPCODE_JMP;
    }
    else if (strcmp("jie", kOpcode) == 0)
    {
        return E_OPCODE_JIE;
    }
    else if (strcmp("jio", kOpcode) == 0)
    {
        return E_OPCODE_JIO;
    }

    assert(0);
}

RegisterSourceType decodeRegister(const char* kOpcode)
{
    if (strcmp(kOpcode, "a") == 0)
    {
        return E_REGISTER_A;
    }
    else if (strcmp(kOpcode, "b") == 0)
    {
        return E_REGISTER_B;
    }
    return E_NOT_A_REGISTER;
}

OperandType decodeOperand(const char* kOpcode)
{
    OperandType kOperand;

    kOperand.eRegister = decodeRegister(kOpcode);
    if (E_NOT_A_REGISTER == kOperand.eRegister)
    {
        kOperand.nLiteral = strtoll(kOpcode, NULL, 10);
    }

    return kOperand;
}

int64_t getValue(const OperandType* pkOperand, const RegisterType* pkRegisters)
{
    if (pkOperand->eRegister != E_NOT_A_REGISTER)
    {
        return pkRegisters->nRegisters[pkOperand->eRegister];
    }
    return pkOperand->nLiteral;
}

int64_t* getTarget(const OperandType* pkOperand, RegisterType* pkRegisters)
{
    if (pkOperand->eRegister != E_NOT_A_REGISTER)
    {
        return &pkRegisters->nRegisters[pkOperand->eRegister];
    }
    assert(0);
}

void runVirtualMachine(const ByteCodeType* kByteCode, const size_t nByteCodeSize, RegisterType* pkRegisters)
{
    size_t nAddress = 0;

    while (nAddress < nByteCodeSize)
    {
        switch(kByteCode[nAddress].eOpcode)
        {
            case E_OPCODE_HLF:
            {
                int64_t* pTarget = getTarget(&kByteCode[nAddress].kOperands[0], pkRegisters);
                *pTarget /= 2;
                ++nAddress;
            } break;

            case E_OPCODE_TPL:
            {
                int64_t* pTarget = getTarget(&kByteCode[nAddress].kOperands[0], pkRegisters);
                *pTarget *= 3;
                ++nAddress;
            } break;

            case E_OPCODE_INC:
            {
                int64_t* pTarget = getTarget(&kByteCode[nAddress].kOperands[0], pkRegisters);
                *pTarget += 1;
                ++nAddress;
            } break;

            case E_OPCODE_JMP:
            {
                nAddress += getValue(&kByteCode[nAddress].kOperands[0], pkRegisters);
            } break;

            case E_OPCODE_JIE:
            {
                if (0 == (getValue(&kByteCode[nAddress].kOperands[0], pkRegisters) % 2))
                {
                    nAddress += getValue(&kByteCode[nAddress].kOperands[1], pkRegisters);
                }
                else
                {
                    ++nAddress;
                }
            } break;

            case E_OPCODE_JIO:
            {
                if (1 == getValue(&kByteCode[nAddress].kOperands[0], pkRegisters))
                {
                    nAddress += getValue(&kByteCode[nAddress].kOperands[1], pkRegisters);
                }
                else
                {
                    ++nAddress;
                }
            } break;

            default:
            {
                assert(0);
            }
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        char*                   kBuffer;
        char**                  kLines;
        size_t                  i;
        size_t                  nLineCount;
        ByteCodeType*           kByteCode;
        RegisterType            kRegisters;

        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount);
        fclose(pInput);

        kByteCode = (ByteCodeType*)malloc(nLineCount * sizeof(ByteCodeType));

        for (i = 0; i < nLineCount; ++i)
        {
            char*  pToken = strtok(kLines[i], " ");
            size_t j      = 0;

            while (NULL != pToken)
            {
                /* Remove any Commas */
                char* pComma = strstr(pToken, ",");
                if (pComma)
                {
                    *pComma = '\0';
                }

                assert(j < 3);

                /* First entry is an Opcode, otherwise an Operand */
                if (0 == j)
                {
                    kByteCode[i].eOpcode        = decodeOpcode(pToken);
                    kByteCode[i].nOperandsCount = 0;
                }
                else
                {
                    kByteCode[i].kOperands[kByteCode[i].nOperandsCount] = decodeOperand(pToken);
                    ++kByteCode[i].nOperandsCount;
                }

                ++j;
                pToken = strtok(NULL, " ");
            }
        }


        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);

        kRegisters.nRegisters[0] = 0;
        kRegisters.nRegisters[1] = 0;
        runVirtualMachine(kByteCode, nLineCount, &kRegisters);
        printf("Part 1: %lli\n", kRegisters.nRegisters[1]);

        kRegisters.nRegisters[0] = 1;
        kRegisters.nRegisters[1] = 0;
        runVirtualMachine(kByteCode, nLineCount, &kRegisters);
        printf("Part 2: %lli\n", kRegisters.nRegisters[1]);

        free(kByteCode);
    }

    return 0;
}
