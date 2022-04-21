#include "CAssembunny.h"
#include <assert.h>

CAssembunny::CAssembunny (void) :
    m_kAssembunnyCode   (),
    m_kRegisters        ()
{
}

CRegisters& CAssembunny::registers(void)
{
    return m_kRegisters;
}

void CAssembunny::Add(const OpcodeType eOpcode, int64_t nOperand1)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    kOperation.kOperands.push_back(COperand(nOperand1));
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, char kOperand1)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    kOperation.kOperands.push_back(COperand(kOperand1));
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, const std::string& kOperand1)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    if (m_kRegisters.legal(kOperand1))
    {
        kOperation.kOperands.push_back(COperand(kOperand1.front()));
    }
    else
    {
        kOperation.kOperands.push_back(COperand(std::stoll(kOperand1)));
    }
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, int64_t nOperand1, int64_t nOperand2)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    kOperation.kOperands.push_back(COperand(nOperand1));
    kOperation.kOperands.push_back(COperand(nOperand2));
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, int64_t nOperand1, char kOperand2)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    kOperation.kOperands.push_back(COperand(nOperand1));
    kOperation.kOperands.push_back(COperand(kOperand2));
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, char kOperand1, int64_t nOperand2)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    kOperation.kOperands.push_back(COperand(kOperand1));
    kOperation.kOperands.push_back(COperand(nOperand2));
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, char kOperand1, char kOperand2)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    kOperation.kOperands.push_back(COperand(kOperand1));
    kOperation.kOperands.push_back(COperand(kOperand2));
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Add(const OpcodeType eOpcode, const std::string& kOperand1, const std::string& kOperand2)
{
    OperationType kOperation;
    kOperation.eOpcode = eOpcode;
    if (m_kRegisters.legal(kOperand1))
    {
        kOperation.kOperands.push_back(COperand(kOperand1.front()));
    }
    else
    {
        kOperation.kOperands.push_back(COperand(std::stoll(kOperand1)));
    }
    if (m_kRegisters.legal(kOperand2))
    {
        kOperation.kOperands.push_back(COperand(kOperand2.front()));
    }
    else
    {
        kOperation.kOperands.push_back(COperand(std::stoll(kOperand2)));
    }
    m_kAssembunnyCode.push_back(kOperation);
}

void CAssembunny::Process(void)
{
    int64_t nAddress = 0;
    while (nAddress < m_kAssembunnyCode.size())
    {
        Process(nAddress);
    }
}

void CAssembunny::Process(int64_t& nAddress)
{
    assert(nAddress >= 0);

    OperationType& kOperation = m_kAssembunnyCode.at(nAddress);
    switch (kOperation.eOpcode)
    {
        case E_CPY:
        {
            assert(kOperation.kOperands.size() == 2);
            kOperation.kOperands.at(1).Set(kOperation.kOperands.at(0), m_kRegisters);
            ++nAddress;
        } break;

        case E_INC:
        {
            assert(kOperation.kOperands.size() == 1);
            kOperation.kOperands.at(0).Inc(m_kRegisters);
            ++nAddress;
        } break;

        case E_DEC:
        {
            assert(kOperation.kOperands.size() == 1);
            kOperation.kOperands.at(0).Dec(m_kRegisters);
            ++nAddress;
        } break;

        case E_JNZ:
        {
            assert(kOperation.kOperands.size() == 2);
            if (kOperation.kOperands.at(0).Get(m_kRegisters) != 0)
            {
                nAddress += kOperation.kOperands.at(1).Get(m_kRegisters);
            }
            else
            {
                ++nAddress;
            }
        } break;

        default:
        {
            assert(false);
        }
    }
}