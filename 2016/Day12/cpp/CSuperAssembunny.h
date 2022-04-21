#ifndef __C_SUPER_ASSEMBUNNY_H__
#define __C_SUPER_ASSEMBUNNY_H__

#include "CAssembunny.h"

class CSuperAssembunny : public CAssembunny
{
    public:

        void    Patch      (void);
        void    Process    (void);

    private:

        bool    Process     (int64_t& nAddress);

        enum SuperOpcodeType
        {
            E_ADD = CAssembunny::E_LAST,
            E_NOP,
            E_LAST
        };
};

#endif // __C_SUPER_ASSEMBUNNY_H__