#include "CRegisters.h"
#include <assert.h>

CRegisters::CRegisters()
{
    m_kRegisters['a'] = 0;
    m_kRegisters['b'] = 0;
    m_kRegisters['c'] = 0;
    m_kRegisters['d'] = 0;
}

int64_t CRegisters::get(const char kRegister) const
{
    assert(legal(kRegister));
    return m_kRegisters.at(kRegister);
}

int64_t CRegisters::set(const char kRegister, const int64_t nValue)
{
    assert(legal(kRegister));
    m_kRegisters.at(kRegister) = nValue;
    return m_kRegisters.at(kRegister);
}

int64_t CRegisters::inc(const char kRegister)
{
    assert(legal(kRegister));
    ++m_kRegisters.at(kRegister);
    return m_kRegisters.at(kRegister);
}

int64_t CRegisters::dec(const char kRegister)
{
    assert(legal(kRegister));
    --m_kRegisters.at(kRegister);
    return m_kRegisters.at(kRegister);
}

int64_t CRegisters::add(const char kRegister, const int64_t nValue)
{
    assert(legal(kRegister));
    m_kRegisters.at(kRegister) += nValue;
    return m_kRegisters.at(kRegister);
}

void CRegisters::reset(void)
{
    for (std::map<char, int64_t>::iterator it = m_kRegisters.begin(); it != m_kRegisters.end(); ++it)
    {
        it->second = 0;
    }
}

bool CRegisters::legal(const char kRegister) const
{
    return m_kRegisters.find(kRegister) != m_kRegisters.cend();
}

bool CRegisters::legal(const std::string& kRegister) const
{
    if (kRegister.size() != 1)
    {
        return false;
    }
    return legal(kRegister.front());
}
