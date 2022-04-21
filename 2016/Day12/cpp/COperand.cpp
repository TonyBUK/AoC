#include "COperand.h"
#include <assert.h>

COperand::COperand(const char kRegister) :
    m_eType     (E_REGISTER),
    m_kRegister (kRegister),
    m_nValue    (0)
{
}

COperand::COperand(const int64_t nValue) :
    m_eType     (E_LITERAL),
    m_kRegister (' '),
    m_nValue    (nValue)
{
}

COperand::COperand(const COperand& kOperand) :
    m_eType     (kOperand.m_eType),
    m_kRegister (kOperand.m_kRegister),
    m_nValue    (kOperand.m_nValue)
{
}

void COperand::SetType(const char kRegister)
{
    m_eType     = E_REGISTER;
    m_kRegister = kRegister;
}

void COperand::SetType(const int64_t nValue)
{
    m_eType     = E_LITERAL;
    m_nValue    = nValue;
}

void COperand::SetType(const COperand& kOperand)
{
    m_eType     = kOperand.m_eType;
    m_nValue    = kOperand.m_nValue;
    m_kRegister = kOperand.m_kRegister;
}

int64_t COperand::Set(const COperand& kOperand, CRegisters& kRegisters)
{
    assert(E_REGISTER == m_eType);
    return kRegisters.set(m_kRegister, kOperand.Get(kRegisters));
}

int64_t COperand::Add(const COperand& kOperand, CRegisters& kRegisters)
{
    assert(E_REGISTER == m_eType);
    return kRegisters.add(m_kRegister, kOperand.Get(kRegisters));
}

int64_t COperand::Get(const CRegisters& kRegisters) const
{
    if (E_LITERAL == m_eType)
    {
        return m_nValue;
    }
    else
    {
        return kRegisters.get(m_kRegister);
    }
}

int64_t COperand::Inc(CRegisters& kRegisters)
{
    assert(E_REGISTER == m_eType);
    return kRegisters.inc(m_kRegister);
}

int64_t COperand::Dec(CRegisters& kRegisters)
{
    assert(E_REGISTER == m_eType);
    return kRegisters.dec(m_kRegister);
}

bool COperand::operator== (const COperand& kCompare) const
{
    if (m_eType != kCompare.m_eType)
    {
        return false;
    }
    if (E_LITERAL == m_eType)
    {
        return m_nValue == kCompare.m_nValue;
    }
    else
    {
        return m_kRegister == kCompare.m_kRegister;
    }
}

bool COperand::operator!= (const COperand& kCompare) const
{
    return !(*this == kCompare);
}