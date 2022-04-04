#include "cdecompressor.h"
#include "helper.h"

CDecompressor::CDecompressor(const std::string& kCompressedText, const DecompressorActionType eAction, const bool bRecurse) :
    m_kUncompressed (kCompressedText),
    m_bDecompressed (false),
    m_kDecompressed (),
    m_bLengthKnown  (false),
    m_nLengthKnown  (0),
    m_bRecurse      (bRecurse)
{
    if (E_CALCULATE_LENGTH == eAction)
    {
        processLength();
    }
    else if (E_DECOMPRESS == eAction)
    {
        process();
    }
}

std::size_t CDecompressor::length(void)
{
    if (!m_bLengthKnown)
    {
        processLength();
    }

    return m_nLengthKnown;
}

const std::string& CDecompressor::get(void)
{
    if (!m_bDecompressed)
    {
        process();
    }

    return m_kDecompressed;
}

void CDecompressor::processLength(void)
{
    m_nLengthKnown = processLength(m_kUncompressed);
    m_bLengthKnown = true;
}

std::size_t CDecompressor::processLength(const std::string& kUncompressed)
{
    // Decompression State
    DecompressorStateType   eState              = E_COPYING_TEXT;

    // Decompressed Length
    std::size_t             nDecompressedLength = 0;

    // Expansion Command {LENGTH}x{FREQUENCY}
    std::string             kExpansionCommand;
    std::size_t             nRepeatLength       = 0;
    std::size_t             nRepeatFrequency    = 0;

    // Expanded Text Fragment
    std::string             kExpansionText;
    std::size_t             nExpansionLength    = 0;

    for (std::string::const_iterator it = kUncompressed.cbegin(); it != kUncompressed.cend(); ++it)
    {
        switch (eState)
        {
            case E_COPYING_TEXT:
            {
                // Copying Text: Keep going until we reach a "("
                if ('(' == *it)
                {
                    eState = E_DECODING_EXPANSION;
                    kExpansionCommand.clear();
                }
                else
                {
                    ++nDecompressedLength;
                }
            } break;

            case E_DECODING_EXPANSION:
            {
                // Decoding Expansion: Keep going until we reach a ")"
                if (')' == *it)
                {
                    assert(kExpansionCommand.find('x') != std::string::npos);

                    const std::vector<std::string>  kExpansionCommandList = split(kExpansionCommand, "x");
                    assert(kExpansionCommandList.size() == 2);

                    nRepeatLength       = std::stoull(kExpansionCommandList.at(0));
                    nRepeatFrequency    = std::stoull(kExpansionCommandList.at(1));

                    if (!m_bRecurse)
                    {
                        // We can skip the Expansion Phase by manipulating the loop index
                        eState               = E_COPYING_TEXT;
                        nDecompressedLength += nRepeatLength * nRepeatFrequency;
                        it                  += nRepeatLength;
                    }
                    else
                    {
                        eState              = E_EXPANDING_TEXT;
                        kExpansionText.clear();
                        nExpansionLength    = 0;
                    }
                }
                else
                {
                    kExpansionCommand.push_back(*it);
                }
            } break;

            case E_EXPANDING_TEXT:
            {
                // Expanding Text: Keep going until we've reached all possible characters in the 

                kExpansionText.push_back(*it);
                ++nExpansionLength;

                if (nExpansionLength == nRepeatLength)
                {
                    eState = E_COPYING_TEXT;

                    nDecompressedLength += nRepeatFrequency * processLength(kExpansionText);
                }
            } break;
        }
    }

    return nDecompressedLength;
}

void CDecompressor::process(void)
{
    m_bDecompressed = true;
    m_kDecompressed = process(m_kUncompressed);
    m_bLengthKnown  = true;
    m_nLengthKnown  = m_kDecompressed.size();
}

std::string CDecompressor::process(const std::string& kUncompressed)
{
    // Decompression State
    DecompressorStateType   eState              = E_COPYING_TEXT;

    // Decompressed Length
    std::string             kDecompressedText;

    // Expansion Command {LENGTH}x{FREQUENCY}
    std::string             kExpansionCommand;
    std::size_t             nRepeatLength       = 0;
    std::size_t             nRepeatFrequency    = 0;

    // Expanded Text Fragment
    std::string             kExpansionText;
    std::size_t             nExpansionLength    = 0;

    for (std::string::const_iterator it = kUncompressed.cbegin(); it != kUncompressed.cend(); ++it)
    {
        switch (eState)
        {
            case E_COPYING_TEXT:
            {
                // Copying Text: Keep going until we reach a "("
                if ('(' == *it)
                {
                    eState = E_DECODING_EXPANSION;
                    kExpansionCommand.clear();
                }
                else
                {
                    kDecompressedText.push_back(*it);
                }
            } break;

            case E_DECODING_EXPANSION:
            {
                // Decoding Expansion: Keep going until we reach a ")"
                if (')' == *it)
                {
                    assert(kExpansionCommand.find('x') != std::string::npos);

                    const std::vector<std::string>  kExpansionCommandList = split(kExpansionCommand, "x");
                    assert(kExpansionCommandList.size() == 2);

                    nRepeatLength       = std::stoull(kExpansionCommandList.at(0));
                    nRepeatFrequency    = std::stoull(kExpansionCommandList.at(1));

                    eState              = E_EXPANDING_TEXT;
                    kExpansionText.clear();
                    nExpansionLength    = 0;
                }
                else
                {
                    kExpansionCommand.push_back(*it);
                }
            } break;

            case E_EXPANDING_TEXT:
            {
                // Expanding Text: Keep going until we've reached all possible characters in the 

                kExpansionText.push_back(*it);
                ++nExpansionLength;

                if (nExpansionLength == nRepeatLength)
                {
                    if (!m_bRecurse)
                    {
                        for (std::size_t i = 0; i < nRepeatFrequency; ++i)
                        {
                            kDecompressedText += kExpansionText;
                        }
                    }
                    else
                    {
                        const std::string kFragementDecryptedText = process(kExpansionText);
                        for (std::size_t i = 0; i < nRepeatFrequency; ++i)
                        {
                            kDecompressedText += kFragementDecryptedText;
                        }
                    }

                    eState = E_COPYING_TEXT;
                }
            } break;
        }
    }

    return kDecompressedText;
}
