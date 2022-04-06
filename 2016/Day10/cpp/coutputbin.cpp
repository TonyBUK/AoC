#include "coutputbin.h"
#include <assert.h>

COutputBin::COutputBin (void) :
    m_bHasChip  (false),
    m_nChip     (0)
{    
}

void COutputBin::receiveChip(const int64_t nChip)
{
    assert(!m_bHasChip);
    m_bHasChip = true;
    m_nChip    = nChip;
}


int64_t COutputBin::value(void) const
{
    assert(m_bHasChip);
    return m_nChip;
}
