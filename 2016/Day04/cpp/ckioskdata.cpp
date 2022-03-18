#include "ckioskdata.h"
#include "ccaesercipher.h"
#include "common.h"
#include <map>
#include <algorithm>

CKioskData::CKioskData(void)
{
}

CKioskData::CKioskData (const std::string& kEncryptedData)
{
    Set(kEncryptedData);
}

CKioskData::CKioskData(const std::vector<std::string>& kEncryptedData, const std::size_t nSectorId, const std::string& kChecksum)
{
    Set(kEncryptedData, nSectorId, kChecksum);
}

void CKioskData::Set(const std::string& kEncryptedData)
{
    const std::string               kNormalised = replace(replace(kEncryptedData, "]", ""), "[", "-");
    const std::vector<std::string>  kTokens     = split(kNormalised, "-");
    assert(kTokens.size() >= 2);

    const std::vector<std::string>  kEncryptedTokens(kTokens.cbegin(), kTokens.cend() - 2);
    const int64_t                   nSectorId   = std::stoll(kTokens.at(kTokens.size() - 2));
    const std::string               kChecksum   = kTokens.back();

    Set(kEncryptedTokens, nSectorId, kChecksum);
}

void CKioskData::Set(const std::vector<std::string>& kEncryptedData, const std::size_t nSectorId, const std::string& kChecksum)
{
    m_kEncryptedData = kEncryptedData;
    m_nSectorId      = nSectorId;
    m_kChecksum      = kChecksum;
    m_bValid         = kChecksum == Checksum();
    m_kDecryptedData = "";

    if (m_bValid)
    {
        for (std::vector<std::string>::const_iterator it = m_kEncryptedData.cbegin(); it != m_kEncryptedData.cend(); ++it)
        {
            CCaeserCipher kDecrypted(*it, nSectorId);
            if ("" == m_kDecryptedData)
            {
                m_kDecryptedData = kDecrypted.Decrypt();
            }
            else
            {
                m_kDecryptedData += "-" + kDecrypted.Decrypt();
            }
        }
    }
}

bool CKioskData::Valid(void) const
{
    return m_bValid;
}

const std::string& CKioskData::Decrypt(void) const
{
    return m_kDecryptedData;
}

const int64_t CKioskData::SectorId(void) const
{
    return m_nSectorId;
}

const std::string CKioskData::Checksum(void) const
{
    std::map<char, std::size_t> kStringMetrics;

    for (std::vector<std::string>::const_iterator itString = m_kEncryptedData.cbegin(); itString != m_kEncryptedData.cend(); ++itString)
    {
        for (std::string::const_iterator it = itString->cbegin(); it != itString->cend(); ++it)
        {
            if (kStringMetrics.find(*it) == kStringMetrics.end())
            {
                kStringMetrics[*it] = 1;
            }
            else
            {
                ++kStringMetrics[*it];
            }
        }
    }

    std::map<std::size_t, std::string> kStringMetricsByFrequency;

    for (std::map<char, std::size_t>::const_iterator it = kStringMetrics.cbegin(); it != kStringMetrics.cend(); ++it)
    {
        if (kStringMetricsByFrequency.find(it->second) == kStringMetricsByFrequency.cend())
        {
            kStringMetricsByFrequency[it->second] = std::string(1, it->first);
        }
        else
        {
            kStringMetricsByFrequency[it->second].push_back(it->first);
            std::sort(kStringMetricsByFrequency[it->second].begin(), kStringMetricsByFrequency[it->second].end());
        }
    }

    std::vector<std::size_t> kSizes;
    for (std::map<std::size_t, std::string>::const_iterator it = kStringMetricsByFrequency.cbegin(); it != kStringMetricsByFrequency.cend(); ++it)
    {
        kSizes.push_back(it->first);
    }
    std::sort(kSizes.begin(), kSizes.end());

    std::string kChecksum;
    for (std::vector<std::size_t>::const_reverse_iterator itSize = kSizes.crbegin(); itSize != kSizes.crend(); ++itSize)
    {
        for (std::string::const_iterator it = kStringMetricsByFrequency.at(*itSize).cbegin(); it != kStringMetricsByFrequency.at(*itSize).cend(); ++it)
        {
            kChecksum += std::string(1, *it);
            if (kChecksum.size() == 5)
            {
                break;
            }
        }
        if (kChecksum.size() == 5)
        {
            break;
        }
    }

    return kChecksum;
}
