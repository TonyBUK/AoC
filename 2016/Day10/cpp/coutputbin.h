#ifndef __C_OUTPUT_BIN_H__
#define __C_OUTPUT_BIN_H__

#include <cstdint>
#include "cchipdevice.h"

class COutputBin : public CChipDevice
{
    public:

                    COutputBin  (void);
        void        receiveChip (const int64_t nChip);
        int64_t     value       (void) const;

    private:

    bool            m_bHasChip;
    int64_t         m_nChip;
};

#endif // __C_OUTPUT_BIN_H__