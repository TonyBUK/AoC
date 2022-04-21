#include "CSuperAssembunny.h"

void CSuperAssembunny::Patch(void)
{
    for (std::size_t i = 2; i < m_kAssembunnyCode.size(); ++i)
    {
        OperationType& kOperation = m_kAssembunnyCode.at(i);
        if (CAssembunny::E_JNZ != kOperation.eOpcode) continue;
        {
            if (-2 != kOperation.kOperands.at(1).Get(m_kRegisters)) continue;

            OperationType& kFirst  = m_kAssembunnyCode.at(i-2);
            OperationType& kSecond = m_kAssembunnyCode.at(i-1);

            if (CAssembunny::E_INC != kFirst.eOpcode)  continue;
            if (CAssembunny::E_DEC != kSecond.eOpcode) continue;

            if (kOperation.kOperands.at(0) != kSecond.kOperands.at(0)) continue;

            // Patch the First Opcode
            kFirst.eOpcode = E_ADD;
            kFirst.kOperands.push_back(COperand(kFirst.kOperands.at(0)));
            kFirst.kOperands.at(0).SetType(COperand(kOperation.kOperands.at(0)));

            // Patch the Second Opcode
            kSecond.eOpcode = CAssembunny::E_CPY;
            kSecond.kOperands.at(0).SetType(static_cast<int64_t>(0));
            kSecond.kOperands.push_back(COperand(kOperation.kOperands.at(0)));

            // Stub out the Third Opcode
            kOperation.eOpcode = E_NOP;
            kOperation.kOperands.clear();
        }
    }
}

void CSuperAssembunny::Process(void)
{
    int64_t nAddress = 0;
    while (nAddress < m_kAssembunnyCode.size())
    {
        if (!Process(nAddress))
        {
            CAssembunny::Process(nAddress);
        }
    }
}

bool CSuperAssembunny::Process(int64_t& nAddress)
{
    assert(nAddress >= 0);

    OperationType& kOperation = m_kAssembunnyCode.at(nAddress);
    switch (kOperation.eOpcode)
    {
        case E_ADD:
        {
            assert(kOperation.kOperands.size() == 2);
            kOperation.kOperands.at(1).Add(kOperation.kOperands.at(0), m_kRegisters);
            ++nAddress;
        } break;

        case E_NOP:
        {
            assert(kOperation.kOperands.size() == 0);
            ++nAddress;
        } break;
        
        default:
        {
            return false;
        }
    }

    return true;
}