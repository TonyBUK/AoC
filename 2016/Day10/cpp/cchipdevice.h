#ifndef __C_CHIP_DEVICE_H__
#define __C_CHIP_DEVICE_H__

#include <cstdint>

class CChipDevice
{
    public:

        virtual void    receiveChip (const int64_t nChip) = 0;
};

#endif // __C_CHIP_DEVICE_H__