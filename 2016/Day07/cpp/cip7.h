#ifndef __CIP7_H__
#define __CIP7_H__

#include <string>
#include <vector>

class CIP7
{
    public:

                                    CIP7                    (const std::string& kIPAddress);
        bool                        supportsTLS             (void) const;
        bool                        supportsSSL             (void) const;

    private :

        void                        extractIPAddress        (const std::string& kIPAddress);
        void                        checkTLS                (void);
        void                        checkSSL                (void);
        bool                        containsABBA            (const std::string& kString) const;
        std::vector<std::string>    inverseABA              (const std::string& kString) const;

        std::vector<std::string>    m_kIPAddress;
        std::vector<std::string>    m_kHypernetAddress;
        bool                        m_bSupportsTLS;
        bool                        m_bSupportsSSL;
};

#endif // __CIP7_H__
