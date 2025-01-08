#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define ADDER_SIZE (45)

/*
 * The input data is three alphanumeric (a-z,0-9) characters, meaning we can interpret this as
 * base 36 for the purpose of indexing.
 */
#define NUMBER_BASE (36)
#define MAX_WIDTH   (3)

#define ASCII_TO_BASE_36(k) ((size_t)(((k) >= '0' && (k) <= '9') ? ((k) - '0') : ((k) - 'a' + 10)))
#define BASE_36_TO_ASCII(k) ((char)(((k) < 10) ? ((k) + '0') : ((k) + 'a' - 10)))

#define TO_CACHE(k)         ((ASCII_TO_BASE_36(k[0]) * NUMBER_BASE * NUMBER_BASE) + (ASCII_TO_BASE_36(k[1]) * NUMBER_BASE) + ASCII_TO_BASE_36(k[2]))
#define FROM_CACHE_1(k)     (BASE_36_TO_ASCII((k / (NUMBER_BASE * NUMBER_BASE)) % NUMBER_BASE))
#define FROM_CACHE_2(k)     (BASE_36_TO_ASCII((k / NUMBER_BASE) % NUMBER_BASE))
#define FROM_CACHE_3(k)     (BASE_36_TO_ASCII(k % NUMBER_BASE))

#define TO_CACHE_ABC(chr,b,c) ((ASCII_TO_BASE_36(chr) * NUMBER_BASE * NUMBER_BASE) + ((b) * NUMBER_BASE) + (c))

#define TO_CACHE_X(a,b)     TO_CACHE_ABC('x',a,b)
#define TO_CACHE_Y(a,b)     TO_CACHE_ABC('y',a,b)
#define TO_CACHE_Z(a,b)     TO_CACHE_ABC('z',a,b)

typedef enum OperationType
{
    OPERATION_AND = 0,
    OPERATION_OR,
    OPERATION_XOR
} OperationType;

typedef struct WireType
{
    size_t        nWire;
    size_t        nWireValue;
} WireType;

typedef struct GateType
{
    size_t        nOperands[2];
    OperationType eOperation;
} GateType;

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

int compare(const void* pLeft, const void* pRight)
{
    const size_t nLeft  = *(const size_t*)pLeft;
    const size_t nRight = *(const size_t*)pRight;

    if (nLeft < nRight)
    {
        return -1;
    }
    else if (nLeft > nRight)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

size_t computeWiresRecurse(const size_t nWire, const GateType* const kGates, const unsigned char* const kWireValues, const size_t* const kRenamedWires)
{
    const size_t nRenamedWire = (kRenamedWires[nWire] == (size_t)-1) ? nWire : kRenamedWires[nWire];

    /* If the Gate is Solved, return the value */
    if (0xFF != kWireValues[nRenamedWire])
    {
        return kWireValues[nRenamedWire];
    }
    else
    {
        /* Solve the Operands Recursively */
        const size_t nLeft  = computeWiresRecurse(kGates[nRenamedWire].nOperands[0], kGates, kWireValues, kRenamedWires);
        const size_t nRight = computeWiresRecurse(kGates[nRenamedWire].nOperands[1], kGates, kWireValues, kRenamedWires);

        /* Solve the Gate and update the Caches Values */
        switch (kGates[nRenamedWire].eOperation)
        {
            case OPERATION_AND:
            {
                return nLeft & nRight;
            }

            case OPERATION_OR:
            {
                return nLeft | nRight;
            }

            case OPERATION_XOR:
            {
                return nLeft ^ nRight;
            }

            default:
            {
                assert(0);
            }
        }
    }
}

uint64_t computeWires(const GateType* const kGates, const unsigned char* const kWireValues, const size_t* const kRenamedWires)
{
    size_t         i;
    uint64_t       nValue      = 0;

    /* Solve the Output Wires Recursively */
    for (i = ADDER_SIZE + 1; i > 0; --i)
    {
        const size_t nWire = TO_CACHE_Z((i-1) / 10, (i-1) % 10);
        nValue             = (nValue << 1) | (uint64_t)computeWiresRecurse(nWire, kGates, kWireValues, kRenamedWires);
    }

    return nValue;
}

void createAdder(const size_t nNumBits, GateType* kGates, size_t* kGatesArray, size_t* pnNumGates)
{
    const size_t nZWireLast     = TO_CACHE_Z  (     nNumBits / 10, nNumBits % 10);
    const size_t nCarryWireLast = TO_CACHE_ABC('c', nNumBits / 10, nNumBits % 10);

    size_t i;
    size_t nNumGates = 0;

    /*
     * Implement an Adder for an Arbitrary Number of Bits usng Half/Full Adders
     *
     * Effectively a larger version of this:
     * https://circuitverse.org/users/60917/projects/two-3-bit-number-adder
     */

    for (i = 0; i < nNumBits; ++i)
    {
        switch (i)
        {
            case 0:
            {
                const size_t nZWire     = TO_CACHE_Z  (     0, 0);
                const size_t nCarryWire = TO_CACHE_ABC('c', 0, 0);
                
                /* kWireOperations[f"z{i:02}"] = (f"x{i:02}", "XOR", f"y{i:02}") */
                /* z00 = x00 XOR y00 */
                kGatesArray[nNumGates++]    = nZWire;
                kGates[nZWire].nOperands[0] = TO_CACHE_X(0, 0);
                kGates[nZWire].nOperands[1] = TO_CACHE_Y(0, 0);
                kGates[nZWire].eOperation   = OPERATION_XOR;

                /* kWireOperations[f"c{i:02}"] = (f"x{i:02}", "AND", f"y{i:02}") */
                /* c{arry}00 = x00 AND y00 */
                kGatesArray[nNumGates++]        = nCarryWire;
                kGates[nCarryWire].nOperands[0] = TO_CACHE_X(0, 0);
                kGates[nCarryWire].nOperands[1] = TO_CACHE_Y(0, 0);
                kGates[nCarryWire].eOperation   = OPERATION_AND;
            } break;

            case 1:
            {
                const size_t nZWire     = TO_CACHE_Z  (     0, 1);
                const size_t nSumWire   = TO_CACHE_ABC('s', 0, 1);
                const size_t nCarryWire = TO_CACHE_ABC('c', 0, 1);

                /* kWireOperations[f"z{i:02}"] = (f"c{i-1:02}", "XOR", f"s{i:02}") */
                /* z01 = c00 XOR s01 */
                kGatesArray[nNumGates++]    = nZWire;
                kGates[nZWire].nOperands[0] = TO_CACHE_ABC('c', 0, 0);
                kGates[nZWire].nOperands[1] = nSumWire;
                kGates[nZWire].eOperation   = OPERATION_XOR;

                /* kWireOperations[f"s{i:02}"] = (f"x{i:02}", "XOR", f"y{i:02}") */
                /* s01 = x01 XOR y01 */
                kGatesArray[nNumGates++]      = nSumWire;
                kGates[nSumWire].nOperands[0] = TO_CACHE_X(0, 1);
                kGates[nSumWire].nOperands[1] = TO_CACHE_Y(0, 1);
                kGates[nSumWire].eOperation   = OPERATION_XOR;

                /* kWireOperations[f"c{i:02}"] = (f"x{i:02}", "AND", f"y{i:02}") */
                /* c{arry}01 = x01 AND y01 */
                kGatesArray[nNumGates++]        = nCarryWire;
                kGates[nCarryWire].nOperands[0] = TO_CACHE_X(0, 1);
                kGates[nCarryWire].nOperands[1] = TO_CACHE_Y(0, 1);
                kGates[nCarryWire].eOperation   = OPERATION_AND;
            } break;

            default:
            {
                const size_t nZWire         = TO_CACHE_Z  (     i / 10, i % 10);
                const size_t nSumWire       = TO_CACHE_ABC('s', i / 10, i % 10);
                const size_t nCarryTmp1Wire = TO_CACHE_ABC('d', i / 10, i % 10);
                const size_t nCarryTmp2Wire = TO_CACHE_ABC('e', i / 10, i % 10);
                const size_t nCarryWire     = TO_CACHE_ABC('c', i / 10, i % 10);

                /* kWireOperations[f"z{i:02}"] = (f"c{i:02}", "XOR", f"s{i:02}") */
                /* zXX = cXX XOR sXX */
                kGatesArray[nNumGates++]    = nZWire;
                kGates[nZWire].nOperands[0] = nCarryWire;
                kGates[nZWire].nOperands[1] = nSumWire;
                kGates[nZWire].eOperation   = OPERATION_XOR;

                /* kWireOperations[f"sum{i:02}"] = (f"x{i:02}", "XOR", f"y{i:02}") */
                /* sxx = xXX XOR yXX */
                kGatesArray[nNumGates++]      = nSumWire;
                kGates[nSumWire].nOperands[0] = TO_CACHE_X(i / 10, i % 10);
                kGates[nSumWire].nOperands[1] = TO_CACHE_Y(i / 10, i % 10);
                kGates[nSumWire].eOperation   = OPERATION_XOR;

                /* kWireOperations[f"d{i:02}"] = (f"x{i:02}", "AND", f"y{i:02}") */
                /* dXX = xXX AND yXX */
                kGatesArray[nNumGates++]            = nCarryTmp1Wire;
                kGates[nCarryTmp1Wire].nOperands[0] = TO_CACHE_X(i / 10, i % 10);
                kGates[nCarryTmp1Wire].nOperands[1] = TO_CACHE_Y(i / 10, i % 10);
                kGates[nCarryTmp1Wire].eOperation   = OPERATION_AND;

                kGatesArray[nNumGates++]      = nCarryTmp2Wire;
                kGatesArray[nNumGates++]      = nCarryWire;

                if (i <= 2)
                {
                    /* kWireOperations[f"e{i:02}"] = (f"s{i-1:02}", "AND", f"c{i-2:02}") */
                    /* eXX = sXX(-1) AND CXX(-2) */
                    kGates[nCarryTmp2Wire].nOperands[0] = TO_CACHE_ABC('s', (i-1) / 10, (i-1) % 10);
                    kGates[nCarryTmp2Wire].nOperands[1] = TO_CACHE_ABC('c', (i-2) / 10, (i-2) % 10);
                    kGates[nCarryTmp2Wire].eOperation   = OPERATION_AND;

                    /* kWireOperations[f"c{i:02}"] = (f"c{i-1:02}", "OR", f"e{i:02}") */
                    /* cXX = cXX(-1) OR eXX */
                    kGates[nCarryWire].nOperands[0] = TO_CACHE_ABC('c', (i-1) / 10, (i-1) % 10);
                    kGates[nCarryWire].nOperands[1] = nCarryTmp2Wire;
                    kGates[nCarryWire].eOperation   = OPERATION_OR;
                }
                else
                {
                    /* kWireOperations[f"e{i:02}"] = (f"c{i-1:02}", "AND", f"s{i-1:02}") */
                    /* eXX = cXX(-1) AND sXX(-1) */
                    kGates[nCarryTmp2Wire].nOperands[0] = TO_CACHE_ABC('s', (i-1) / 10, (i-1) % 10);
                    kGates[nCarryTmp2Wire].nOperands[1] = TO_CACHE_ABC('c', (i-1) / 10, (i-1) % 10);
                    kGates[nCarryTmp2Wire].eOperation   = OPERATION_AND;

                    /* kWireOperations[f"c{i:02}"] = (f"d{i-1:02}", "OR", f"e{i:02}") */
                    /* cXX = eXX(-1) OR eXX */
                    kGates[nCarryWire].nOperands[0] = TO_CACHE_ABC('d', (i-1) / 10, (i-1) % 10);
                    kGates[nCarryWire].nOperands[1] = nCarryTmp2Wire;
                    kGates[nCarryWire].eOperation   = OPERATION_OR;
                }
            } break;
        }
    }

    /* kWireOperations[f"z{nBits:02}"] = (f"d{nBits-1:02}", "OR", f"c{nBits:02}") */
    /* ZXX = DXX(-1) OR CXX */
    kGatesArray[nNumGates++]        = nZWireLast;
    kGates[nZWireLast].nOperands[0] = TO_CACHE_ABC('d', (nNumBits-1) / 10, (nNumBits-1) % 10);
    kGates[nZWireLast].nOperands[1] = nCarryWireLast;
    kGates[nZWireLast].eOperation   = OPERATION_OR;

    /* kWireOperations[f"c{nBits:02}"] = (f"c{nBits-1:02}", "AND", f"s{nBits-1:02}") */
    /* CXX = CXX(-1) AND SXX(-1) */
    kGatesArray[nNumGates++]            = nCarryWireLast;
    kGates[nCarryWireLast].nOperands[0] = TO_CACHE_ABC('c', (nNumBits-1) / 10, (nNumBits-1) % 10);
    kGates[nCarryWireLast].nOperands[1] = TO_CACHE_ABC('s', (nNumBits-1) / 10, (nNumBits-1) % 10);
    kGates[nCarryWireLast].eOperation   = OPERATION_AND;

    *pnNumGates = nNumGates;
}

unsigned findAdderWires(const size_t* const kReferenceAdderArray, const size_t nNumReferenceAdderGates, const GateType* const kReferenceAdder, const size_t* const kWireOperationsArray, const size_t nNumWireOperations, const GateType* const kWireOperations, size_t* kAdderMappings, size_t* kAdderReverseMappings, size_t* kRenamer, size_t* kRenamerArray, size_t* nRenamerArraySize)
{
    const size_t nZWire = TO_CACHE_Z(0, 0) / (NUMBER_BASE * NUMBER_BASE);

    size_t i;

    /*
     * This search pattern will only consider a wire for mapping if this is an output wire for known operands, this will help
     * increase the confidence of found matches.
     */

    for (i = 0; i < nNumReferenceAdderGates; ++i)
    {
        const size_t nReferenceWire    = kReferenceAdderArray[i];
        const size_t nReferenceLeft    = kReferenceAdder[nReferenceWire].nOperands[0];
        const size_t nReferenceRight   = kReferenceAdder[nReferenceWire].nOperands[1];
        size_t       nAdderMappedLeft  = kAdderMappings[nReferenceLeft];
        size_t       nAdderMappedRight = kAdderMappings[nReferenceRight];

        size_t       j;

        if ((kAdderMappings[nReferenceWire]  != (size_t)-1) ||
            (nAdderMappedLeft                == (size_t)-1) ||
            (nAdderMappedRight               == (size_t)-1))
        {
            continue;
        }

        for (j = 0; j < nNumWireOperations; ++j)
        {
            const size_t nWire          = kWireOperationsArray[j];
            const size_t nLeftOriginal  = kWireOperations[nWire].nOperands[0];
            const size_t nRightOriginal = kWireOperations[nWire].nOperands[1];
            const size_t nLeft          = kRenamer[nLeftOriginal] == (size_t)-1 ? nLeftOriginal : kRenamer[nLeftOriginal];
            const size_t nRight         = kRenamer[nRightOriginal] == (size_t)-1 ? nRightOriginal : kRenamer[nRightOriginal];

            if (((nLeft  == nAdderMappedLeft) || (nLeft  == nAdderMappedRight)) &&
                ((nRight == nAdderMappedLeft) || (nRight == nAdderMappedRight)) &&
                (kWireOperations[nWire].eOperation == kReferenceAdder[nReferenceWire].eOperation))
            {
                if ((nReferenceWire / (NUMBER_BASE * NUMBER_BASE)) == nZWire)
                {
                    if (nReferenceWire != nWire)
                    {
                        assert(kRenamer[nWire] == (size_t)-1);
                        assert(kRenamer[nReferenceWire] == (size_t)-1);

                        kRenamer[nWire]          = nReferenceWire;
                        kRenamer[nReferenceWire] = nWire;

                        kRenamerArray[(*nRenamerArraySize)++] = nReferenceWire;
                        kRenamerArray[(*nRenamerArraySize)++] = nWire;
                    }
                }

                kAdderMappings[nReferenceWire] = nWire;
                kAdderReverseMappings[nWire]   = nReferenceWire;

                return AOC_TRUE;
            }
        }

    }

    return AOC_FALSE;
}

unsigned bruteForceAddrWires(const size_t* const kReferenceAdderArray, const size_t nNumReferenceAdderGates, const GateType* const kReferenceAdder, const size_t* const kWireOperationsArray, const size_t nNumWireOperations, const GateType* const kWireOperations, size_t* kAdderMappings, size_t* kAdderReverseMappings, size_t* kRenamer, size_t* kRenamerArray, size_t* nRenamerArraySize)
{
    size_t i;

    /*
     * Somethings mapped wrong, perform a brute force rename
     * This works on the premise we can pattern match based on the operands, finding two wires
     * with the same operands, but a different operator.
     */

    /* For each key... */
    for (i = 0; i < nNumReferenceAdderGates; ++i)
    {
        /*
         * Only Consider mapped Keys that are not already renamed as the puzzle promises we won't have
         * compound errors.
         */
        if (kAdderMappings[kReferenceAdderArray[i]] != (size_t)-1)
        {
            size_t j;

            if  (kRenamer[kAdderMappings[kReferenceAdderArray[i]]] != (size_t)-1)
            {
                continue;
            }

            /* For each subsequent key... */
            for (j = i + 1; j < nNumReferenceAdderGates; ++j)
            {
                if (kAdderMappings[kReferenceAdderArray[j]] != (size_t)-1)
                {
                    if  (kRenamer[kAdderMappings[kReferenceAdderArray[j]]] != (size_t)-1)
                    {
                        continue;
                    }

                    /* If the Operands Match (ignore the operation) */
                    if (((kReferenceAdder[kReferenceAdderArray[i]].nOperands[0] == kReferenceAdder[kReferenceAdderArray[i]].nOperands[0]) || 
                         (kReferenceAdder[kReferenceAdderArray[i]].nOperands[0] == kReferenceAdder[kReferenceAdderArray[j]].nOperands[1])) &&
                        ((kReferenceAdder[kReferenceAdderArray[i]].nOperands[1] == kReferenceAdder[kReferenceAdderArray[j]].nOperands[0]) ||
                         (kReferenceAdder[kReferenceAdderArray[i]].nOperands[1] == kReferenceAdder[kReferenceAdderArray[j]].nOperands[1])))
                    {
                        /* Tentatively rename the operands */
                        kRenamer[kAdderMappings[kReferenceAdderArray[i]]] = kAdderMappings[kReferenceAdderArray[j]];
                        kRenamer[kAdderMappings[kReferenceAdderArray[j]]] = kAdderMappings[kReferenceAdderArray[i]];

                        /* If we can continue traversing the wires, assume we're now good */
                        if (AOC_TRUE == findAdderWires(kReferenceAdderArray, nNumReferenceAdderGates, kReferenceAdder, kWireOperationsArray, nNumWireOperations, kWireOperations, kAdderMappings, kAdderReverseMappings, kRenamer, kRenamerArray, nRenamerArraySize))
                        {
                            kRenamerArray[(*nRenamerArraySize)++] = kAdderMappings[kReferenceAdderArray[j]];
                            kRenamerArray[(*nRenamerArraySize)++] = kAdderMappings[kReferenceAdderArray[i]];

                            return AOC_TRUE;
                        }
                        else
                        {
                            /* If we're here, the trial rename failed, so we'll revert the rename */
                            kRenamer[kAdderMappings[kReferenceAdderArray[i]]] = (size_t)-1;
                            kRenamer[kAdderMappings[kReferenceAdderArray[j]]] = (size_t)-1;
                        }
                    }
                }
            }
        }
    }

    return AOC_FALSE;
}

size_t refreshRemainingWires(const size_t* const kAdderReverseMappings, size_t* kRemainingWires, const size_t nNumRemainingWires)
{
    size_t       nNewNumRemainingWires   = 0;
    size_t       i;

    /* Refresh the Remaining Wire Count*/
    for (i = 0; i < nNumRemainingWires; ++i)
    {
        if (kAdderReverseMappings[kRemainingWires[i]] == (size_t)-1)
        {
            kRemainingWires[nNewNumRemainingWires++] = kRemainingWires[i];
        }
    }

    return nNewNumRemainingWires;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "rb");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kInputLines;

        size_t                      nNumInputLines;
        size_t                      i;

        WireType*                   kWires;
        size_t                      nNumWires           = 0;

        GateType*                   kGates;
        size_t*                     kGatesArray;
        size_t                      nNumGates           = 0;

        GateType*                   kReferenceAdderGates;
        size_t*                     kReferenceAdderGatesArray;
        size_t                      nNumReferenceAdderGates = 0;

        size_t*                     kAdderMappings;
        size_t*                     kAdderReverseMappings;

        size_t*                     kRemainingWires;
        size_t                      nNumRemainingWires;

        unsigned                    bProcessingWires    = AOC_TRUE;

        unsigned char*              kWireValues;

        size_t*                     kRenamedWires;
        size_t*                     kRenamedWiresArray;
        size_t                      nNumRenamedWires;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nNumInputLines, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Wires */
        kWires                    = (WireType*)malloc(nNumInputLines * sizeof(WireType));
        kGates                    = (GateType*)calloc(NUMBER_BASE * NUMBER_BASE * NUMBER_BASE, sizeof(GateType));
        kGatesArray               = (size_t*)malloc(nNumInputLines * sizeof(size_t));
        kReferenceAdderGates      = (GateType*)calloc(NUMBER_BASE * NUMBER_BASE * NUMBER_BASE, sizeof(GateType));
        kReferenceAdderGatesArray = (size_t*)malloc(nNumInputLines * sizeof(size_t));
        kAdderMappings            = (size_t*)malloc(NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(size_t));
        kAdderReverseMappings     = (size_t*)malloc(NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(size_t));
        kRemainingWires           = (size_t*)malloc(nNumInputLines * sizeof(size_t));
        kWireValues               = (unsigned char*)malloc(NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(unsigned char));
        kRenamedWires             = (size_t*)malloc(NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(size_t));
        kRenamedWiresArray        = (size_t*)malloc(nNumInputLines * sizeof(size_t));

        memset(kWireValues, 0xFF, NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(unsigned char));
        memset(kRenamedWires, 0xFF, NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(size_t));
        memset(kAdderMappings, 0xFF, NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(size_t));
        memset(kAdderReverseMappings, 0xFF, NUMBER_BASE * NUMBER_BASE * NUMBER_BASE * sizeof(size_t));

        /* Read the Input Data */
        for (i = 0; i < nNumInputLines; ++i)
        {
            if (bProcessingWires)
            {
                /* Process the Wires */
                if (kInputLines[i][0] == '\0')
                {
                    bProcessingWires = AOC_FALSE;
                }
                else
                {
                    const size_t nWire = TO_CACHE(kInputLines[i]);
                    assert(kInputLines[i][3] == ':');
                    assert(kInputLines[i][4] == ' ');
                    assert((kInputLines[i][5] == '0') || (kInputLines[i][5] == '1'));
                    kWires[nNumWires].nWire        = nWire;
                    kWires[nNumWires++].nWireValue = (kInputLines[i][5] - '0');
                }
            }
            else
            {
                /* Process the Gates */
                const char*  kOperand1       = kInputLines[i];
                const char*  kOperator       = strchr(kOperand1, ' ') + 1;
                const char*  kOperand2       = strchr(kOperator, ' ') + 1;
                const char*  kOutputPreamble = strchr(kOperand2, ' ') + 1;
                const char*  kOutput         = strchr(kOutputPreamble, ' ') + 1;
                const size_t nOutputWire     = TO_CACHE(kOutput);

                assert(kOperator > kOperand1);
                assert(kOperand2 > kOperator);
                assert(kOutputPreamble > kOperand2);
                assert(kOutput > kOutputPreamble);

                kGates[nOutputWire].nOperands[0] = TO_CACHE(kOperand1);
                kGates[nOutputWire].nOperands[1] = TO_CACHE(kOperand2);

                if (strncmp(kOperator, "AND", 3) == 0)
                {
                    kGates[nOutputWire].eOperation = OPERATION_AND;
                }
                else if (strncmp(kOperator, "OR", 2) == 0)
                {
                    kGates[nOutputWire].eOperation = OPERATION_OR;
                }
                else if (strncmp(kOperator, "XOR", 3) == 0)
                {
                    kGates[nOutputWire].eOperation = OPERATION_XOR;
                }
                else
                {
                    assert(0);
                }

                kGatesArray[nNumGates++] = nOutputWire;
            }
        }

        /* Initialise Known Values */
        for (i = 0; i < nNumWires; ++i)
        {
            kWireValues[kWires[i].nWire]   = kWires[i].nWireValue;
        }

        /* Part One */
        printf("Part 1: %" PRIu64 "\n", computeWires(kGates, kWireValues, kRenamedWires));

        /* Part Two */

        /*
         * Create a "correct" adder which we can incrementally map the puzzle input to and find
         * any instances of crossed wires
         */
        createAdder(ADDER_SIZE, kReferenceAdderGates, kReferenceAdderGatesArray, &nNumReferenceAdderGates);

        /* First Pass, only trust wires that exclusively use inputs x/y */
        for (i = 0; i < nNumReferenceAdderGates; ++i)
        {
            const size_t nReferenceWire            = kReferenceAdderGatesArray[i];
            const size_t nReferenceXWire           = TO_CACHE_X(0, 0) / (NUMBER_BASE * NUMBER_BASE);
            const size_t nReferenceYWire           = TO_CACHE_Y(0, 0) / (NUMBER_BASE * NUMBER_BASE);
            const size_t nReferenceZWire           = TO_CACHE_Z(0, 0) / (NUMBER_BASE * NUMBER_BASE);
            
            const size_t nReferenceLeft            = kReferenceAdderGates[nReferenceWire].nOperands[0];
            const size_t nReferenceRight           = kReferenceAdderGates[nReferenceWire].nOperands[1];

            const size_t nReferenceWireStartsWith  = nReferenceWire / (NUMBER_BASE * NUMBER_BASE);
            const size_t nReferenceLeftStartsWith  = nReferenceLeft / (NUMBER_BASE * NUMBER_BASE);
            const size_t nReferenceRightStartsWith = nReferenceRight / (NUMBER_BASE * NUMBER_BASE);

            /* Assume that there's no weird collisions in the renamer space */
            if (((nReferenceLeftStartsWith  == nReferenceXWire) || (nReferenceLeftStartsWith  == nReferenceYWire)) &&
                ((nReferenceRightStartsWith == nReferenceXWire) || (nReferenceRightStartsWith == nReferenceYWire)))
            {
                size_t j;

                for (j = 0; j < nNumGates; ++j)
                {
                    const size_t nWire = kGatesArray[j];

                    if (kAdderMappings[nWire] != (size_t)-1)
                    {
                        continue;
                    }

                    if (kReferenceAdderGates[nReferenceWire].eOperation == kGates[nWire].eOperation)
                    {
                        const size_t nWireLeft       = kGates[nWire].nOperands[0];
                        const size_t nWireRight      = kGates[nWire].nOperands[1];

                        if (((nWireLeft  == nReferenceLeft) || (nWireLeft  == nReferenceRight)) &&
                            ((nWireRight == nReferenceLeft) || (nWireRight == nReferenceRight)))
                        {
                            kAdderMappings[nReferenceWire] = nWire;
                            kAdderReverseMappings[nWire]   = nReferenceWire;
                        }
                    }
                }
            }
        }

        /* Configure the Remaining Wires */
        nNumRemainingWires = 0;
        for (i = 0; i < nNumGates; ++i)
        {
            if (kAdderMappings[kGatesArray[i]] == (size_t)-1)
            {
                kRemainingWires[nNumRemainingWires++] = kGatesArray[i];
            }
        }

        /* Second Pass, pattern match the remaining wires incrementally searching from the existing mappings */
        while (nNumRemainingWires > 0)
        {
            /*
             * Find the remaining wires by pattern matching incrementally
             * This operates on the premise that we can pattern match based on the operation, and will only
             * accept renamed operations that start with z.
             */
            while (findAdderWires(kReferenceAdderGatesArray, nNumReferenceAdderGates, kReferenceAdderGates, kGatesArray, nNumGates, kGates, kAdderMappings, kAdderReverseMappings, kRenamedWires, kRenamedWiresArray, &nNumRenamedWires))
            {
                nNumRemainingWires = refreshRemainingWires(kAdderReverseMappings, kRemainingWires, nNumRemainingWires);
            }

            /* Keep going until we've found all the wires */
            if (0 == nNumRemainingWires)
            {
                break;
            }

            /*
             * If we're here it means we can't reasonably find any wires but there's still some unmapped, meaning
             * we've got a false positive.  This will brute force the existing mappings until findAdderWires can
             * find a match.
             */
            if (bruteForceAddrWires(kReferenceAdderGatesArray, nNumReferenceAdderGates, kReferenceAdderGates, kGatesArray, nNumGates, kGates, kAdderMappings, kAdderReverseMappings, kRenamedWires, kRenamedWiresArray, &nNumRenamedWires))
            {
                nNumRemainingWires = refreshRemainingWires(kAdderReverseMappings, kRemainingWires, nNumRemainingWires);
            }
        }

        /* Sort the Renamed Wires */
        qsort(kRenamedWiresArray, nNumRenamedWires, sizeof(size_t), compare);

        printf("Part 2: ");
        for (i = 0; i < nNumRenamedWires; ++i)
        {
            printf("%c%c%c", FROM_CACHE_1(kRenamedWiresArray[i]), FROM_CACHE_2(kRenamedWiresArray[i]), FROM_CACHE_3(kRenamedWiresArray[i]));
            if (i < nNumRenamedWires - 1)
            {
                printf(",");
            }
        }
        printf("\n");

        /* Cleanup */
        free(kInputLines);
        free(kInputBuffer);
        free(kWires);
        free(kGates);
        free(kGatesArray);
        free(kReferenceAdderGates);
        free(kReferenceAdderGatesArray);
        free(kAdderMappings);
        free(kAdderReverseMappings);
        free(kRemainingWires);
        free(kRenamedWiresArray);
        free(kWireValues);
        free(kRenamedWires);
    }
 
    return 0;
}