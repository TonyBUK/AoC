#include "ccaesercipher.h"

CCaeserCipher::CCaeserCipher(const std::string& kSource, const std::size_t nRotate)
{
    const std::string kAlphabet = "abcdefghijklmnopqrstuvwxyz";
    m_kDecrypted = "";

    for (std::string::const_iterator it = kSource.cbegin(); it != kSource.cend(); ++it)
    {
        const std::size_t nDecoded = (kAlphabet.find(*it) + nRotate) % kAlphabet.size();
        m_kDecrypted.push_back(kAlphabet.at(nDecoded));
    }
}

const std::string& CCaeserCipher::Decrypt(void) const
{
    return m_kDecrypted;
}
