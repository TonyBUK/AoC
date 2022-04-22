#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
 
typedef unsigned bool;
#ifndef true
#define true 1
#define false 0
#endif
 
typedef enum OpcodeType
{
    E_CPY = 0,
    E_INC,
    E_DEC,
    E_JNZ,
    E_ADD,
    E_NOP
} OpcodeType;
 
typedef struct OperandType
{
    bool                    bRegister;
    char                    kRegister;
    int64_t                 nLiteral;
} OperandType;
 
typedef struct OperationType
{
    OpcodeType              eOpcode;
    OperandType             kOperands[2];
    size_t                  nOperandCount;
} OperationType;
 
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
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
}
 
void Process(const OperationType* kAssembunny, const int64_t nSize, int64_t* kRegisters)
{
    int64_t nAddress = 0;
 
    while (nAddress < nSize)
    {
        const OperationType* kOpcode = &kAssembunny[nAddress];
 
        switch(kOpcode->eOpcode)
        {
            case E_CPY:
            {
                assert(2 == kOpcode->nOperandCount);
                assert(true == kOpcode->kOperands[1].bRegister);
                int64_t nValue;
                if (true == kOpcode->kOperands[0].bRegister)
                {
                    nValue = kRegisters[kOpcode->kOperands[0].kRegister];
                }
                else
                {
                    nValue = kOpcode->kOperands[0].nLiteral;
                }
                kRegisters[kOpcode->kOperands[1].kRegister] = nValue;
                ++nAddress;
            } break;
 
            case E_INC:
            {
                assert(1 == kOpcode->nOperandCount);
                assert(true == kOpcode->kOperands[0].bRegister);
                ++kRegisters[kOpcode->kOperands[0].kRegister];
                ++nAddress;
            } break;
 
            case E_DEC:
            {
                assert(1 == kOpcode->nOperandCount);
                --kRegisters[kOpcode->kOperands[0].kRegister];
                ++nAddress;
            } break;
 
            case E_JNZ:
            {
                assert(2 == kOpcode->nOperandCount);
                int64_t nValue;
 
                if (true == kOpcode->kOperands[0].bRegister)
                {
                    nValue = kRegisters[kOpcode->kOperands[0].kRegister];
                }
                else
                {
                    nValue = kOpcode->kOperands[0].nLiteral;
                }
               
                if (0 != nValue)
                {
                    if (true == kOpcode->kOperands[1].bRegister)
                    {
                        nValue = kRegisters[kOpcode->kOperands[1].kRegister];
                    }
                    else
                    {
                        nValue = kOpcode->kOperands[1].nLiteral;
                    }
                    nAddress += nValue;
                }
                else
                {
                    ++nAddress;
                }
            } break;
           
            case E_ADD:
            {
                assert(2 == kOpcode->nOperandCount);
                assert(true == kOpcode->kOperands[1].bRegister);
                int64_t nValue;
                if (true == kOpcode->kOperands[0].bRegister)
                {
                    nValue = kRegisters[kOpcode->kOperands[0].kRegister];
                }
                else
                {
                    nValue = kOpcode->kOperands[0].nLiteral;
                }
                kRegisters[kOpcode->kOperands[1].kRegister] += nValue;
                ++nAddress;
            } break;
            
            case E_NOP:
            {
                assert(0 == kOpcode->nOperandCount);
                ++nAddress;
            } break;
 
            default:
            {
                assert(false);
            }
        }
    }
}
 
bool CompareOperands(const OperandType* kLeft, const OperandType* kRight)
{
    if (kLeft->bRegister != kRight->bRegister)
    {
        return false;
    }
 
    if (kLeft->bRegister)
    {
        return kLeft->kRegister == kRight->kRegister;
    }
    else
    {
        return kLeft->nLiteral == kRight->nLiteral;
    }
}
 
void Patch(OperationType* kAssembunny, const size_t nSize)
{
    size_t i;
   
    for (i = 2; i < nSize; ++i)
    {
        OperationType* kOperation = &kAssembunny[i];
       
        if (E_JNZ == kOperation->eOpcode)
        {
            OperationType* kFirst  = &kAssembunny[i-2];
            OperationType* kSecond = &kAssembunny[i-1];
 
            if (E_INC != kFirst->eOpcode)  continue;
            if (E_DEC != kSecond->eOpcode) continue;
           
            if (false == CompareOperands(&kOperation->kOperands[0], &kSecond->kOperands[0])) continue;
 
            kFirst->eOpcode                 = E_ADD;
            kFirst->kOperands[1]            = kFirst->kOperands[0];
            kFirst->kOperands[0]            = kOperation->kOperands[0];
            kFirst->nOperandCount           = 2;

            kSecond->eOpcode                = E_CPY;
            kSecond->kOperands[0].bRegister = false;
            kSecond->kOperands[0].nLiteral  = 0;
            kSecond->kOperands[1]           = kOperation->kOperands[0];
            kSecond->nOperandCount          = 2;
 
            kOperation->eOpcode             = E_NOP;
            kOperation->nOperandCount       = 0;
        }
    }
}
 
int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const char*             REGISTERS   = "abcd";
       
        char*                   kBuffer;
        char**                  kLines;
       
        int64_t                 kRegisters[256] = {0};
 
        size_t                  nLineCount;
        size_t                  i;
 
        OperationType*          kAssembunny;
 
        /* Read the Input File and split into lines... */
        readLines(&pInput, &kBuffer, &kLines, &nLineCount, NULL);
        fclose(pInput);
 
        kAssembunny = (OperationType*)malloc(nLineCount * sizeof(OperationType));
        assert(kAssembunny);
 
        for (i = 0; i < nLineCount; ++i)
        {
            char*  p = strtok(kLines[i], " ");
            size_t j = 0;
                       
            while (p)
            {
                if (0 == j)
                {
                    if (0 == strcmp(p, "cpy"))
                    {
                        kAssembunny[i].eOpcode          = E_CPY;
                        kAssembunny[i].nOperandCount    = 2;
                    }
                    else if (0 == strcmp(p, "inc"))
                    {
                        kAssembunny[i].eOpcode          = E_INC;
                        kAssembunny[i].nOperandCount    = 1;
                    }
                    else if (0 == strcmp(p, "dec"))
                    {
                        kAssembunny[i].eOpcode          = E_DEC;
                        kAssembunny[i].nOperandCount    = 1;
                    }
                    else if (0 == strcmp(p, "jnz"))
                    {
                        kAssembunny[i].eOpcode          = E_JNZ;
                        kAssembunny[i].nOperandCount    = 2;
                    }
                    else
                    {
                        assert(false);
                    }
                }
                else
                {
                    switch (kAssembunny[i].eOpcode)
                    {
                        case E_CPY:
                        {
                            if (strstr(REGISTERS, p))
                            {
                                kAssembunny[i].kOperands[j-1].bRegister = true;
                                kAssembunny[i].kOperands[j-1].kRegister = p[0];
                            }
                            else
                            {
                                kAssembunny[i].kOperands[j-1].bRegister = false;
                                kAssembunny[i].kOperands[j-1].nLiteral  = strtoll(p, NULL, 10);;
                            }
                        } break;
 
                        case E_INC:
                        {
                            assert(strstr(REGISTERS, p));
                            kAssembunny[i].kOperands[j-1].bRegister = true;
                            kAssembunny[i].kOperands[j-1].kRegister = p[0];
                        } break;
 
                        case E_DEC:
                        {
                            assert(strstr(REGISTERS, p));
                            kAssembunny[i].kOperands[j-1].bRegister = true;
                            kAssembunny[i].kOperands[j-1].kRegister = p[0];
                        } break;
 
                        case E_JNZ:
                        {
                            if (strstr(REGISTERS, p))
                            {
                                kAssembunny[i].kOperands[j-1].bRegister = true;
                                kAssembunny[i].kOperands[j-1].kRegister = p[0];
                            }
                            else
                            {
                                kAssembunny[i].kOperands[j-1].bRegister = false;
                                kAssembunny[i].kOperands[j-1].nLiteral  = strtoll(p, NULL, 10);;
                            }
                        } break;
 
                        default:
                        {
                            assert(false);
                        }
                    }
                }
           
                p = strtok(NULL, " ");
                ++j;
            }
           
            assert((j-1) == kAssembunny[i].nOperandCount);
        }
 
        /* Free the Line Buffers now they're no longer needed */
        free(kBuffer);
        free(kLines);
 
        Patch(kAssembunny, nLineCount);
       
        kRegisters[(unsigned char)'a'] = 0;
        kRegisters[(unsigned char)'b'] = 0;
        kRegisters[(unsigned char)'c'] = 0;
        kRegisters[(unsigned char)'d'] = 0;
        Process(kAssembunny, (int64_t)nLineCount, kRegisters);
        printf("Part 1: %lli\n", kRegisters[(unsigned char)'a']);
 
        kRegisters[(unsigned char)'a'] = 0;
        kRegisters[(unsigned char)'b'] = 0;
        kRegisters[(unsigned char)'c'] = 1;
        kRegisters[(unsigned char)'d'] = 0;
        Process(kAssembunny, (int64_t)nLineCount, kRegisters);
        printf("Part 2: %lli\n", kRegisters[(unsigned char)'a']);
       
        free(kAssembunny);
    }
 
    return 0;
}