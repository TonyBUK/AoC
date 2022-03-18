#ifndef __CKIOSKDATA_H__
#define __CKIOSKDATA_H__

#include <vector>
#include <string>
#include <cstddef>

class CKioskData
{
    public:

                            CKioskData  (void);
                            CKioskData  (const std::string& kEncryptedData);
                            CKioskData  (const std::vector<std::string>& kEncryptedData, const std::size_t nSectorId, const std::string& kChecksum);

        void                Set         (const std::string& kEncryptedData);
        void                Set         (const std::vector<std::string>& kEncryptedData, const std::size_t nSectorId, const std::string& kChecksum);
        bool                Valid       (void) const;
        const std::string&  Decrypt     (void) const;
        const int64_t       SectorId    (void) const;

    private:

        const std::string   Checksum    (void) const;

        std::vector<std::string>    m_kEncryptedData;
        std::string                 m_kDecryptedData;
        int64_t                     m_nSectorId;
        std::string                 m_kChecksum;
        bool                        m_bValid;
};

#endif // __CKIOSKDATA_H__
