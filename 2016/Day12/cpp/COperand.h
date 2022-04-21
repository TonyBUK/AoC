#ifndef __C_OPERAND_H__
#define __C_OPERAND_H__

#include <cstdint>
#include "CRegisters.h"

class COperand
{
    public:

                COperand    (const char kRegister);
                COperand    (const int64_t nValue);
                COperand    (const COperand& kOperand);
        void    SetType     (const char kRegister);
        void    SetType     (const int64_t nValue);
        void    SetType     (const COperand& kOperand);
        int64_t Set         (const COperand& kOperand, CRegisters& kRegisters);
        int64_t Add         (const COperand& kOperand, CRegisters& kRegisters);
        int64_t Get         (const CRegisters& kRegisters) const;
        int64_t Inc         (CRegisters& kRegisters);
        int64_t Dec         (CRegisters& kRegisters);
        bool operator== (const COperand& kCompare) const;
        bool operator!= (const COperand& kCompare) const;

    private:

        enum OperandType {E_REGISTER, E_LITERAL};

        OperandType m_eType;
        char        m_kRegister;
        int64_t     m_nValue;
};

#endif // __C_OPERAND_H__