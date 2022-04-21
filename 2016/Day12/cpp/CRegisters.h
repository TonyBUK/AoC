#ifndef __C_REGISTERS_H__
#define __C_REGISTERS_H__

#include <cstdint>
#include <string>
#include <map>

class CRegisters
{
    public:

                CRegisters  (void);
        int64_t get         (const char kRegister) const;
        int64_t set         (const char kRegister, const int64_t nValue);
        int64_t inc         (const char kRegister);
        int64_t dec         (const char kRegister);
        int64_t add         (const char kRegister, const int64_t nValue);
        void    reset       (void);
        bool    legal       (const char         kRegister) const;
        bool    legal       (const std::string& kRegister) const;

    private:

        std::map<char, int64_t> m_kRegisters;
};

#endif // __C_REGISTERS_H__