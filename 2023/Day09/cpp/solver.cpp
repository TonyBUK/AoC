#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <assert.h>

class CHistory
{
    public:

        CHistory(const std::vector<std::int64_t>& kHistory);
        std::int64_t left() const;
        std::int64_t right() const;

    private:

        std::int64_t m_nLeft;
        std::int64_t m_nRight;
};

CHistory::CHistory(const std::vector<std::int64_t>& kHistory)
{
    std::vector<std::int64_t> kCurrentRow = kHistory;
    bool                      bEvenRow    = false;

    m_nLeft  = kCurrentRow.front();
    m_nRight = kCurrentRow.back();

    while (true)
    {
        bool                      bAllZeroes = true;
        std::vector<std::int64_t> kNextRow;

        for (std::vector<std::int64_t>::const_iterator it = kCurrentRow.begin(); it != kCurrentRow.end(); ++it)
        {
            if (it + 1 == kCurrentRow.end())
            {
                break;
            }

            kNextRow.push_back(*(it + 1) - *it);
            bAllZeroes &= kNextRow.back() == 0;
        }

        if (bAllZeroes)
        {
            break;
        }

        // Left Side: Add Evens, Subtract Odds
        if (bEvenRow)
        {
            m_nLeft += kNextRow.front();
        }
        else
        {
            m_nLeft -= kNextRow.front();
        }
        bEvenRow = !bEvenRow;

        // Right Side, Add Always
        m_nRight += kNextRow.back();

        // Next Row
        kCurrentRow = kNextRow;
    }
}

std::int64_t CHistory::left() const
{
    return m_nLeft;
}

std::int64_t CHistory::right() const
{
    return m_nRight;
}

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

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<std::int64_t> kHistoryLeft;
        std::vector<std::int64_t> kHistoryRight;

        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                const std::vector<std::string> kNumbers = split(kLine, " ", false);
                std::vector<std::int64_t> kNumbersDecoded;

                for (std::vector<std::string>::const_iterator it = kNumbers.begin(); it != kNumbers.end(); ++it)
                {
                    kNumbersDecoded.push_back(std::stoll(*it));
                }

                CHistory kData(kNumbersDecoded);
                kHistoryLeft.push_back(kData.left());
                kHistoryRight.push_back(kData.right());
            }
        }

        std::cout << "Part 1: " << std::accumulate(kHistoryRight.begin(), kHistoryRight.end(), 0) << std::endl;
        std::cout << "Part 2: " << std::accumulate(kHistoryLeft.begin(),  kHistoryLeft.end(),  0) << std::endl;
   }

    return 0;
}
