#ifndef __CCAESERCIPHER_H__
#define __CCAESERCIPHER_H__

#include <string>

class CCaeserCipher
{
    public:

                            CCaeserCipher   (const std::string& kSource, const std::size_t nRotate);
        const std::string&  Decrypt         (void) const;

    private:

        std::string m_kDecrypted;    
    
};

#endif // __CCAESERCIPHER_H__
