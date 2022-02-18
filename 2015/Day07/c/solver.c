#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include <assert.h>

typedef unsigned BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef enum instructionType
{
    E_SET, E_NOT, E_LSHIFT, E_RSHIFT, E_AND, E_OR
} instructionType;

typedef struct dataType
{
    BOOL        bIsLiteral;
    uint16_t    nLiteral;
    char*       kSignal;
} dataType;

typedef struct commandType
{
    instructionType       eInstruction;
    size_t                nInputCount;
    dataType              kInputs[2];
    char*                 kOutput;
} commandType;

BOOL split(char* kDestructibleLine, const char* kDelimitter, char** kSplitBuffer, uint32_t nSplitBufferMaxSize, uint32_t* nSplitBufferSize)
{
    uint32_t nCurrent = 0;
    char*    pToken   = strtok(kDestructibleLine, kDelimitter);

    while (pToken)
    {
        if (nCurrent >= nSplitBufferMaxSize)
        {
            *nSplitBufferSize = nCurrent;
            return FALSE;
        }
        kSplitBuffer[nCurrent] = pToken;
        ++nCurrent;
        pToken = strtok(NULL, kDelimitter);
    }

    *nSplitBufferSize = nCurrent;
    return TRUE;
}

BOOL isOneOf(const char* kString, const char* kArrayOfStrings[], const uint32_t nArraySize)
{
    uint32_t i;
    for (i = 0; i < nArraySize; ++i)
    {
        if (0 == strcmp(kString, kArrayOfStrings[i]))
        {
            return TRUE;
        }
    }
    return FALSE;
}

void initialiseDataLiteral(const uint16_t nLiteral, dataType* pData)
{
    pData->bIsLiteral = TRUE;
    pData->nLiteral   = nLiteral;
    pData->kSignal    = NULL;
}

void initialiseDataSignal(const char* kSignal, dataType* pData)
{
    pData->bIsLiteral = FALSE;
    pData->kSignal    = (char*)malloc(strlen(kSignal) + 1);
    strcpy(pData->kSignal, kSignal);
}

void freeCommand(commandType* pCommand)
{
    size_t i;
    for (i = 0; i < pCommand->nInputCount; ++i)
    {
        if (pCommand->kInputs[i].kSignal)
        {
            free(pCommand->kInputs[i].kSignal);
            pCommand->kInputs[i].kSignal = NULL;
        }
    }

    if (pCommand->kOutput)
    {
        free(pCommand->kOutput);
        pCommand->kOutput = NULL;
    }
}

void getData(const char* s, dataType* pData)
{
    size_t  i;
    int32_t nCandidate;
    BOOL    bLiteral = TRUE;

    /* Step 1: Determine if any non digit characters are present */
    for (i = 0; i < strlen(s); ++i)
    {
        if (0 == isdigit(s[i]))
        {
            bLiteral = FALSE;
            break;
        }
    }

    /* Step 2: Determine if the decoded number is in the range 0..65535 */
    if (TRUE == bLiteral)
    {
        nCandidate = (int32_t)strtol(s, NULL, 10);
        if (nCandidate != (nCandidate & 0xFFFF))
        {
            bLiteral = FALSE;
        }
    }

    /* Step 3: Store the Data based on the type */
    if (TRUE == bLiteral)
    {
        initialiseDataLiteral(nCandidate, pData);
    }
    else
    {
        initialiseDataSignal(s, pData);
    }
}

BOOL decodeCommand(const char* kLine, commandType* kCommand)
{
    const char* TWO_PARAM_OPCODES[] = {"AND", "OR", "LSHIFT", "RSHIFT"};
    char*       kLocalLine          = (char*)malloc(strlen(kLine)+1);
    char*       kTokens[5];      /* We know in the worst case, that the most number
                                    of tokens after splitting will be 5, for the
                                    AND/OR/LSHIFT/RSHIFT commands */
    uint32_t    nTokenCount;

    strcpy(kLocalLine, kLine);
    if (FALSE == split(kLocalLine, " ", kTokens, sizeof(kTokens)/sizeof(kTokens[0]), &nTokenCount))
    {
        assert(FALSE);
    }

    assert(nTokenCount >= 2);

    if (isOneOf(kTokens[1], TWO_PARAM_OPCODES, sizeof(TWO_PARAM_OPCODES)/sizeof(TWO_PARAM_OPCODES[0])))
    {
        assert(nTokenCount == 5);
        assert(0 == strcmp(kTokens[3], "->"));

        if      (0 == strcmp("AND",    kTokens[1])) kCommand->eInstruction = E_AND;
        else if (0 == strcmp("OR",     kTokens[1])) kCommand->eInstruction = E_OR;
        else if (0 == strcmp("LSHIFT", kTokens[1])) kCommand->eInstruction = E_LSHIFT;
        else if (0 == strcmp("RSHIFT", kTokens[1])) kCommand->eInstruction = E_RSHIFT;
        else assert(FALSE);

        kCommand->nInputCount  = 2;
        getData(kTokens[0], &kCommand->kInputs[0]);
        getData(kTokens[2], &kCommand->kInputs[1]);
        kCommand->kOutput      = (char*)malloc(strlen(kTokens[4])+1);
        strcpy(kCommand->kOutput, kTokens[4]);

        free(kLocalLine);

        return TRUE;
    }
    else if (0 == strcmp("NOT", kTokens[0]))
    {
        assert(nTokenCount == 4);
        assert(0 == strcmp(kTokens[2], "->"));

        kCommand->eInstruction = E_NOT;
        kCommand->nInputCount  = 1;
        getData(kTokens[1], &kCommand->kInputs[0]);
        kCommand->kOutput      = (char*)malloc(strlen(kTokens[3])+1);
        strcpy(kCommand->kOutput, kTokens[3]);

        free(kLocalLine);

        return TRUE;
    }
    else
    {
        assert(nTokenCount == 3);
        assert(0 == strcmp(kTokens[1], "->"));

        kCommand->eInstruction = E_SET;
        kCommand->nInputCount  = 1;
        getData(kTokens[0], &kCommand->kInputs[0]);
        kCommand->kOutput      = (char*)malloc(strlen(kTokens[2])+1);
        strcpy(kCommand->kOutput, kTokens[2]);

        free(kLocalLine);

        return TRUE;
    }

    free(kLocalLine);

    return TRUE;
}

uint16_t* hashMapGet(char* kKey)
{
    ENTRY  kItem;
    ENTRY* pItem;
    kItem.key = kKey;
    pItem = hsearch(kItem, FIND);
    if (pItem)
    {
        // if (kSignals.find(kInput.kSignal) != kSignals.end())
        return ((uint16_t*)pItem->data);
    }

    return NULL;
}

void hashMapSet(char* kKey, uint16_t nValue, uint16_t* pData, size_t* pDataPtrSize)
{
    ENTRY kItem;
    kItem.key            = malloc(strlen(kKey)+1);
    strcpy(kItem.key, kKey);
    kItem.data           = &pData[*pDataPtrSize];
    pData[*pDataPtrSize] = nValue;
    *pDataPtrSize       += 1;
    if (NULL == hsearch(kItem, ENTER))
    {
        assert(FALSE);
    }
}

uint16_t getSignalOrLiteral(const dataType* pInput)
{
    if (pInput->bIsLiteral)
    {
        return pInput->nLiteral;
    }
    else
    {
        const uint16_t* pData = hashMapGet(pInput->kSignal);
        if (pData)
        {
            return *pData;
        }
    }
    assert(FALSE);
}

void processCommands(const commandType* pGateCommands, uint16_t* pData, const size_t nCommandCount, const size_t nDataInitPtrSize)
{
    BOOL       bAnyProcessed;
    size_t     i, j;
    size_t     nDataPtrSize = nDataInitPtrSize;
    do
    {
        bAnyProcessed = FALSE;

        for (i = 0; i < nCommandCount; ++i)
        {
            // Skip Commands we've already handled
            ENTRY  kItem;
            ENTRY* pItem;
            
            kItem.key = pGateCommands[i].kOutput;
            if (hsearch(kItem, FIND)) continue;

            BOOL bValid = TRUE;
            for (j = 0; j < pGateCommands[i].nInputCount; ++j)
            {
                if (FALSE == pGateCommands[i].kInputs[j].bIsLiteral)
                {
                    kItem.key = pGateCommands[i].kInputs[j].kSignal;
                    pItem = hsearch(kItem, FIND);
                    if (NULL == pItem)
                    {
                        bValid = FALSE;
                        break;
                    }
                }
            }

            if (!bValid) continue;

            /* Indicate at least one command has been processed */
            bAnyProcessed = TRUE;

            switch(pGateCommands[i].eInstruction)
            {
                case E_SET:
                {
                    hashMapSet(pGateCommands[i].kOutput,
                               getSignalOrLiteral(&pGateCommands[i].kInputs[0]),
                               pData, &nDataPtrSize);
                } break;

                case E_NOT:
                {
                    hashMapSet(pGateCommands[i].kOutput,
                               (~getSignalOrLiteral(&pGateCommands[i].kInputs[0])) & 0xFFFF,
                               pData, &nDataPtrSize);
                } break;

                case E_LSHIFT:
                {
                    hashMapSet(pGateCommands[i].kOutput,
                               getSignalOrLiteral(&pGateCommands[i].kInputs[0]) <<
                               getSignalOrLiteral(&pGateCommands[i].kInputs[1]),
                               pData, &nDataPtrSize);
                } break;
                
                case E_RSHIFT:
                {
                    hashMapSet(pGateCommands[i].kOutput,
                               getSignalOrLiteral(&pGateCommands[i].kInputs[0]) >>
                               getSignalOrLiteral(&pGateCommands[i].kInputs[1]),
                               pData, &nDataPtrSize);
                } break;

                case E_AND:
                {
                    hashMapSet(pGateCommands[i].kOutput,
                               getSignalOrLiteral(&pGateCommands[i].kInputs[0]) &
                               getSignalOrLiteral(&pGateCommands[i].kInputs[1]),
                               pData, &nDataPtrSize);
                } break;

                case E_OR:
                {
                    hashMapSet(pGateCommands[i].kOutput,
                               getSignalOrLiteral(&pGateCommands[i].kInputs[0]) |
                               getSignalOrLiteral(&pGateCommands[i].kInputs[1]),
                               pData, &nDataPtrSize);
                } break;
            
                default:
                {
                    assert(FALSE);
                } break;
            }
        }
    } while (bAnyProcessed == TRUE);
}

int main(int argc, char** argv)
{
    FILE* pInput = fopen("../input.txt", "r");
    if (pInput)
    {
        const unsigned long nStartPos                 = ftell(pInput);
        unsigned long       nEndPos;
        unsigned long       nFileSize;

        char*               kBuffer;
        char**              kLines;
        char*               pLine;
        commandType*        kCommands;
        uint16_t*           kData;
        uint16_t            nPart1Result;
        size_t              nCommandCount = 0;

        size_t              i;

        /* Read the File to a string buffer and append a NULL Terminator */
        fseek(pInput, 0, SEEK_END);
        nEndPos = ftell(pInput);
        assert(nStartPos != nEndPos);
        fseek(pInput, nStartPos, SEEK_SET);

        nFileSize          = nEndPos - nStartPos;

        kBuffer            = (char*) malloc((nFileSize+1)  * sizeof(char));
        kLines             = (char**)malloc((nFileSize)    * sizeof(char*));

        fread(kBuffer, nFileSize, sizeof(char), pInput);
        fclose(pInput);

        kBuffer[nFileSize] = '\0';

        /* Find each line and store the result in the kLines Array */
        /* Note: This specifically relies upon strtok overwriting new line characters with
                 NUL terminators. */
        pLine = strtok(kBuffer, "\n");
        while (pLine)
        {
            kLines[nCommandCount] = pLine;
            ++nCommandCount;

            pLine = strtok(NULL, "\n");
        }

        /* We now have a Line Count and can properly process the Data */
        kCommands = malloc(sizeof(commandType) * nCommandCount);
        for (i = 0; i < nCommandCount; ++i)
        {
            if (FALSE == decodeCommand(kLines[i], &kCommands[i]))
            {
                assert(FALSE);
            }
        }

        /* Free the Line Buffers since we've parsed all the data into kCommands */
        free(kBuffer);
        free(kLines);

        /* Create the Hash Lookup Table for the Signals */
        if (0 == hcreate(nCommandCount))
        {
            assert(FALSE);
        }

        kData = (uint16_t*)malloc(sizeof(uint16_t) * nCommandCount);

        /* Part 1 - Process Command Set and get "a" */
        processCommands(kCommands, kData, nCommandCount, 0);
        nPart1Result = *hashMapGet("a");
        printf("Part 1: %u\n", nPart1Result);

        /* Destroy the Hash Lookup Table now we've finished Part 1 */
        hdestroy();

        /* Create the Hash Lookup Table for the Signals */
        if (0 == hcreate(nCommandCount))
        {
            assert(FALSE);
        }

        /* Part 2 - Store the prior "a" value in "b" and then Process Command Set and get "a" */
        i = 0; /* Dummy */
        hashMapSet("b", nPart1Result, kData, &i);
        processCommands(kCommands, kData, nCommandCount, 1);
        printf("Part 2: %u\n", *hashMapGet("a"));

        /* Destroy the Hash Lookup Table now we've finished Part 1 */
        hdestroy();

        /* Clean Up the Commands now we've finished... */
        for (i = 0; i < nCommandCount; ++i)
        {
            freeCommand(&kCommands[i]);
        }
        free(kCommands);
   }

    return 0;
}
