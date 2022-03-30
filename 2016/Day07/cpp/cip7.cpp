#include "cip7.h"
#include "helper.h"

CIP7::CIP7(const std::string& kIPAddress) :
    m_kIPAddress(),
    m_kHypernetAddress(),
    m_bSupportsTLS(false),
    m_bSupportsSSL(false)

{
    extractIPAddress(kIPAddress);
    checkTLS();
    checkSSL();
}

bool CIP7::supportsTLS(void) const
{
    return m_bSupportsTLS;
}

bool CIP7::supportsSSL(void) const
{
    return m_bSupportsSSL;
}

void CIP7::extractIPAddress(const std::string& kIPAddress)
{
    const std::vector<std::string> kIPAddressVector = split(kIPAddress, "[]", true);
    bool                           bIsHypernet      = false;

    for (std::vector<std::string>::const_iterator it = kIPAddressVector.cbegin(); it != kIPAddressVector.cend(); ++it)
    {
        if ("[" == *it)
        {
            bIsHypernet = true;
        }
        else if ("]" == *it)
        {
            bIsHypernet = false;
        }
        else
        {
            if (bIsHypernet)
            {
                m_kHypernetAddress.push_back(*it);
            }
            else
            {
                m_kIPAddress.push_back(*it);
            }
        }
    }
}

void CIP7::checkTLS(void)
{
    // If the Hypernet Sequences contain the ABBA sequence, then
    // this can't support TLS
    for (std::vector<std::string>::const_iterator it = m_kHypernetAddress.cbegin(); it != m_kHypernetAddress.cend(); ++it)
    {
        if (containsABBA(*it))
        {
            return;
        }
    }

    // Now we've checked Hypernet, check all address parts for TLS support
    for (std::vector<std::string>::const_iterator it = m_kIPAddress.cbegin(); it != m_kIPAddress.cend(); ++it)
    {
        if (containsABBA(*it))
        {
            m_bSupportsTLS = true;
            return;
        }
    }
}

void CIP7::checkSSL(void)
{
    for (std::vector<std::string>::const_iterator it = m_kIPAddress.cbegin(); it != m_kIPAddress.cend(); ++it)
    {
        // Retrieve all possible Inverse Sequences
        const std::vector<std::string> kInverseABASequences = inverseABA(*it);

        // Search the Hypernet
        for (std::vector<std::string>::const_iterator invIt = kInverseABASequences.cbegin(); invIt != kInverseABASequences.cend(); ++invIt)
        {
            for (std::vector<std::string>::const_iterator hypIt = m_kHypernetAddress.cbegin(); hypIt != m_kHypernetAddress.cend(); ++hypIt)
            {
                if (hypIt->find(*invIt) != std::string::npos)
                {
                    m_bSupportsSSL = true;
                    return;
                }
            }
        }
    }
}

bool CIP7::containsABBA(const std::string& kString) const
{
    for (std::size_t i = 0; (i + 3) < kString.size(); ++i)
    {
        const wchar_t AA[] = {kString.at(i),   kString.at(i+3)};
        const wchar_t BB[] = {kString.at(i+1), kString.at(i+2)};

        if (AA[0] == BB[0]) continue;
        if (AA[0] != AA[1]) continue;
        if (BB[0] != BB[1]) continue;

        return true;
    }

    return false;
}

std::vector<std::string> CIP7::inverseABA(const std::string& kString) const
{
    std::vector<std::string> kInverseSequence;

    for (std::size_t i = 0; (i + 2) < kString.size(); ++i)
    {
        const wchar_t AA[] = {kString.at(i),   kString.at(i+2)};
        const wchar_t B    = kString.at(i+1);

        if (AA[0] == B)     continue;
        if (AA[0] != AA[1]) continue;

        std::string kInverse;
        kInverse.push_back(B);
        kInverse.push_back(AA[0]);
        kInverse.push_back(B);

        kInverseSequence.push_back(kInverse);
    }

    return kInverseSequence;
}
