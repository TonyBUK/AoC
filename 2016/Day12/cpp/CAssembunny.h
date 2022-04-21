#ifndef __C_ASSEMBUNNY_H__
#define __C_ASSEMBUNNY_H__

#include <cstdint>
#include <vector>
#include <string>
#include "COperand.h"
#include "CRegisters.h"

class CAssembunny
{
    public:

        enum OpcodeType
        {
            E_CPY = 0,
            E_INC,
            E_DEC,
            E_JNZ,
            E_LAST
        };

                    CAssembunny (void);
        CRegisters& registers   (void);
        void        Add         (const OpcodeType eOpcode, const int64_t      nOperand1);
        void        Add         (const OpcodeType eOpcode, const char         kOperand1);
        void        Add         (const OpcodeType eOpcode, const std::string& kOperand1);
        void        Add         (const OpcodeType eOpcode, const int64_t      nOperand1, const int64_t      nOperand2);
        void        Add         (const OpcodeType eOpcode, const int64_t      nOperand1, const char         kOperand2);
        void        Add         (const OpcodeType eOpcode, const char         kOperand1, const int64_t      nOperand2);
        void        Add         (const OpcodeType eOpcode, const char         kOperand1, const char         kOperand2);
        void        Add         (const OpcodeType eOpcode, const std::string& kOperand1, const std::string& kOperand2);
        void        Process     (void);

    protected:

        void        Process     (int64_t& nAddress);
        struct OperationType
        {
            uint64_t                eOpcode;
            std::vector<COperand>   kOperands;
        };

        std::vector<OperationType>  m_kAssembunnyCode;
        CRegisters                  m_kRegisters;
};

#endif // __C_ASSEMBUNNY_H__