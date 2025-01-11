#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

std::vector<std::string> split(const std::string& kString, const std::string& kSeperators, const bool bIncludeSeparators)
{
    std::vector<std::string> kOutput;
    std::string::size_type   nPrevPos = 0, nPos = 0;

    while((nPos = kString.find_first_of(kSeperators, nPos)) != std::string::npos)
    {
        std::string kSubString( kString.substr(nPrevPos, nPos-nPrevPos) );
        kOutput.push_back(kSubString);

        if (bIncludeSeparators)
        {
            kOutput.push_back(std::string(1, kString.at(nPos)));
        }

        nPrevPos = ++nPos;
    }
    kOutput.push_back(kString.substr(nPrevPos, nPos-nPrevPos)); // Last word
    return kOutput;
}

class Tokenizer
{
    public:

        Tokenizer(const std::string& kString, const std::string& kStartDelimiter, const std::string& kEndDelimiter);
        bool MaybeHasNext(void) const;

    protected:

        std::string Pre(void) const;
        std::string Next(void);

    private:

        bool FindNext(void) const;

        std::string                     m_kString;
        std::string                     m_kStartDelimiter;
        std::string                     m_kEndDelimiter;
        mutable std::string::size_type  m_nStartPos;
        mutable std::string::size_type  m_nEndPos;
};

Tokenizer::Tokenizer(const std::string& kString, const std::string& kStartDelimiter, const std::string& kEndDelimiter) :
    m_kString(kString),
    m_kStartDelimiter(kStartDelimiter),
    m_kEndDelimiter(kEndDelimiter),
    m_nStartPos(0),
    m_nEndPos(0)
{
}

bool Tokenizer::MaybeHasNext(void) const
{
    return FindNext();
}

std::string Tokenizer::Pre(void) const
{
    if (FindNext())
    {
        return m_kString.substr(0, m_nStartPos);
    }
    else
    {
        return m_kString.substr(m_nStartPos, m_kString.size());
    }
}

std::string Tokenizer::Next(void)
{
    if (FindNext())
    {
        const std::string::size_type nStart = m_nStartPos + m_kStartDelimiter.size();
        const std::string::size_type nEnd   = m_nEndPos   - m_kEndDelimiter.size();
        m_nStartPos = m_nEndPos;
        return m_kString.substr(nStart, nEnd - nStart);
    }

    return "";
}

bool Tokenizer::FindNext(void) const
{
    const std::string::size_type nStartPos = m_kString.find(m_kStartDelimiter, m_nStartPos);
    if (nStartPos == std::string::npos)
    {
        return false;
    }

    const std::string::size_type nEndPos   = m_kString.find(m_kEndDelimiter, nStartPos);
    if (nEndPos == std::string::npos)
    {
        return false;
    }

    m_nStartPos = nStartPos;
    m_nEndPos   = nEndPos + m_kEndDelimiter.size();

    return true;
}

class MultTokenizer : public Tokenizer
{
    public:

        MultTokenizer(const std::string& kString, const bool bDo);
        bool Next(std::string& kToken, bool& bDo);

    private:

        bool Valid(const std::string& kToken) const;

        bool m_bDo;
};

MultTokenizer::MultTokenizer(const std::string& kString, const bool bDo)
    : Tokenizer(kString, "mul(", ")"),
      m_bDo(bDo)
{
}

bool MultTokenizer::Next(std::string& kToken, bool& bDo)
{
    // Handle the Do/Don't Modifiers
    const std::string      kPre     = Pre();
    std::string::size_type nDoPos   = kPre.rfind("do()");
    std::string::size_type nDontPos = kPre.rfind("don't()");

    // Determine if either Do or Don't is present
    if (((nDoPos != std::string::npos) && (nDontPos == std::string::npos)) || ((nDoPos != std::string::npos) && (nDoPos > nDontPos)))
    {
        bDo = true;
    }
    else if (((nDontPos != std::string::npos) && (nDoPos == std::string::npos)) || ((nDontPos != std::string::npos) && (nDontPos > nDoPos)))
    {
        bDo = false;
    }

    // Handle the Next Token
    kToken = Tokenizer::Next();
    if (!Valid(kToken))
    {
        MultTokenizer kTokenizer(kToken + ")", bDo);
        if (kTokenizer.MaybeHasNext())
        {
            return kTokenizer.Next(kToken, bDo);
        }
        else
        {
            kToken = "";
            return false;
        }
    }

    return true;
}

bool MultTokenizer::Valid(const std::string& kToken) const
{
    // Obvious Test - What shouldn't be there
    if (std::string::npos != kToken.find_first_not_of("0123456789,"))
    {
        return false;
    }

    // Digits, Comma, Digits
    const std::string::size_type nStartDigit   = kToken.find_first_of("0123456789");
    const std::string::size_type nFirstComma   = kToken.find_first_of(",");
    const std::string::size_type nLastComma    = kToken.find_last_of(",");
    const std::string::size_type nEndDigit     = kToken.find_last_of("0123456789");

    // Digits or Commas Missing
    if ((nStartDigit == std::string::npos) || (nFirstComma == std::string::npos) || (nLastComma == std::string::npos) || (nEndDigit == std::string::npos))
    {
        return false;
    }

    // More than one Comma
    if (nFirstComma != nLastComma)
    {
        return false;
    }

    // Comma before Digits
    if (nFirstComma <= nStartDigit)
    {
        return false;
    }

    // Last Digit before Comma
    if (nEndDigit <= nFirstComma)
    {
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    std::uint64_t nPartOne = 0;
    std::uint64_t nPartTwo = 0;
    bool          bDo      = true;

    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            // Parse through the File
            std::string kLine;
            std::getline(kFile, kLine);

            // Parse through the Line Tokens
            MultTokenizer kTokenizer(kLine, bDo);
            while (kTokenizer.MaybeHasNext())
            {
                // At this point, something roughly matching the format has been found, but unvalidated, now
                // actually read the token.
                std::string kToken;
                if (kTokenizer.Next(kToken, bDo))
                {
                    // A Valid Token has been found, now process it
                    const std::vector<std::string> kTokens = split(kToken, ",", false);
                    const uint64_t                 nToken0 = std::stoull(kTokens[0]);
                    const uint64_t                 nToken1 = std::stoull(kTokens[1]);

                    // Part One Accepts all tokens
                    nPartOne += nToken0 * nToken1;

                    // Part Two Skips anything in a don't() block
                    if (bDo)
                    {
                        nPartTwo += nToken0 * nToken1;
                    }
                }
            }
        }
        kFile.close();

        std::cout << "Part 1: " << nPartOne << std::endl;
        std::cout << "Part 2: " << nPartTwo << std::endl;
   }

    return 0;
}
