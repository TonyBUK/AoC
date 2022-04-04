#ifndef __C_DECOMPRESSOR_H__
#define __C_DECOMPRESSOR_H__

#include <string>
#include <vector>

class CDecompressor
{
    public:

        enum DecompressorActionType {E_NONE, E_DECOMPRESS, E_CALCULATE_LENGTH};

                            CDecompressor   (const std::string& kCompressedText, const DecompressorActionType eAction, const bool bRecurse);
        std::size_t         length          (void);
        const std::string&  get             (void);

    private:

        enum DecompressorStateType {E_COPYING_TEXT, E_DECODING_EXPANSION, E_EXPANDING_TEXT};

        void                processLength   (void);
        std::size_t         processLength   (const std::string& kUncompressed);
        void                process         (void);
        std::string         process         (const std::string& kUncompressed);

        std::string         m_kUncompressed;
        bool                m_bDecompressed;
        std::string         m_kDecompressed;
        bool                m_bLengthKnown;
        std::size_t         m_nLengthKnown;
        bool                m_bRecurse;
};

#endif // __C_DECOMPRESSOR_H__