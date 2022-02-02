#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <assert.h>

// Perform the Escape Encoding
void escape(const std::string& kInput, std::string& kOutput)
{
    const std::string CHARACTERS_TO_ESCAPE = "\\\"";
    kOutput = "\"";
    for (std::string::const_iterator it = kInput.cbegin(); it != kInput.cend(); ++it)
    {
        if (CHARACTERS_TO_ESCAPE.find(*it) != std::string::npos)
        {
            kOutput += "\\";
        }
        kOutput += *it;
    }
    kOutput += "\"";
}

// Perform the Escape Decoding
void unescape(const std::string& kInput, std::string& kOutput)
{
    kOutput.clear();
    std::size_t nStartAdjust    = 0;
    std::size_t nEndAdjust      = 0;

    // Handle if the Text is embedded within Quoutes
    if (('\"' == *kInput.cbegin()) && ('\"' == *kInput.crbegin()))
    {
        nStartAdjust    = 1;
        nEndAdjust      = 1;
    }

    bool bIsEscape  = false;
    for (std::string::const_iterator it = kInput.cbegin() + nStartAdjust; it != kInput.cend() - nEndAdjust; ++it)
    {
        if (!bIsEscape)
        {
            if ('\\' == *it)
            {
                bIsEscape = true;
            }
            else
            {
                kOutput += *it;
            }
        }
        else
        {
            if ('x' == *it)
            {
                // Prior to C++11, the method of performing a hex conversion is hot garbage,
                // so this falls back to the C way...
                const char kHex[] = {*it, *(it+1), '\0'};
                kOutput += static_cast<char>(strtoul(kHex, NULL, 16));
                it += 2;
            }
            else
            {
                kOutput += *it;
            }
            bIsEscape = false;
        }
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        unsigned long nRawLength     = 0;
        unsigned long nDecodedLength = 0;
        unsigned long nEncodedLength = 0;
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);
            nRawLength += kLine.size();

            std::string kDecoded;
            unescape(kLine, kDecoded);
            nDecodedLength += kDecoded.size();

            std::string kEncoded;
            escape(kLine, kEncoded);
            nEncodedLength += kEncoded.size();
        }

        std::cout << "Part 1: " << (nRawLength     - nDecodedLength) << std::endl;
        std::cout << "Part 2: " << (nEncodedLength - nRawLength)     << std::endl;
    }

    return 0;
}
