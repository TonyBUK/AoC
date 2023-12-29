#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <assert.h>

class CScratchCard
{
    public:

                      CScratchCard(const std::vector<std::uint64_t>& kWinningNumbers, const std::vector<std::uint64_t>& kScratchNumbers);
        void          AddTicket   (const std::uint64_t nNumTickets);
        std::uint64_t GetWinningNumbersCount() const;
        std::uint64_t GetWinningPoints      () const;
        std::uint64_t GetNumTickets         () const;

    private:
    
        std::vector<std::uint64_t> m_kWinningNumbers;
        std::vector<std::uint64_t> m_kScratchNumbers;
        std::uint64_t              m_nNumTickets;
        mutable bool               m_bCachedWinners;
        mutable std::uint64_t      m_nWinningNumberCount;
};

CScratchCard::CScratchCard(const std::vector<std::uint64_t>& kWinningNumbers, const std::vector<std::uint64_t>& kScratchNumbers) :
    m_kWinningNumbers(kWinningNumbers),
    m_kScratchNumbers(kScratchNumbers),
    m_nNumTickets(1),
    m_bCachedWinners(false)
{
}

void CScratchCard::AddTicket(const std::uint64_t nNumTickets)
{
    m_nNumTickets += nNumTickets;
}

std::uint64_t CScratchCard::GetWinningNumbersCount() const
{
    if (!m_bCachedWinners)
    {
        // Count the Number of Winning Ticket
        std::uint64_t nWinningNumbers = 0;
        for (std::vector<std::uint64_t>::const_iterator it = m_kWinningNumbers.cbegin(); it != m_kWinningNumbers.cend(); ++it)
        {
            if (std::find(m_kScratchNumbers.cbegin(), m_kScratchNumbers.cend(), *it) != m_kScratchNumbers.cend())
            {
                ++nWinningNumbers;
            }
        }

        m_bCachedWinners = true;
        m_nWinningNumberCount = nWinningNumbers;
    }

    return m_nWinningNumberCount;
}

std::uint64_t CScratchCard::GetWinningPoints() const
{
    const std::uint64_t nWinningNumbers = GetWinningNumbersCount();
    if (0 == nWinningNumbers)
    {
        return 0;
    }
    else
    {
        std::uint64_t nPoints = 1;
        for (std::uint64_t i = 1; i < nWinningNumbers; ++i)
        {
            nPoints *= 2;
        }
        return nPoints;
    }
}

std::uint64_t CScratchCard::GetNumTickets() const
{
    return m_nNumTickets;
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

void extractNumbers(const std::string& kString, std::vector<std::uint64_t>& kNumbers)
{
    const std::vector<std::string> kTokens = split(kString, " ", false);
    for (std::vector<std::string>::const_iterator it = kTokens.cbegin(); it != kTokens.cend(); ++it)
    {
        if (it->length())
        {
            kNumbers.push_back(std::stoull(*it));
        }
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<CScratchCard> kScratchCards;

        // Store the Scratch Cards.
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);
            if (kLine.length())
            {
                const std::vector<std::string> kTokens = split(kLine, ":", false);
                assert(kTokens.size() == 2);
                const std::vector<std::string> kNumbers = split(kTokens[1], "|", false);
                assert(kNumbers.size() == 2);
                std::vector<std::uint64_t> kWinningNumbers;
                std::vector<std::uint64_t> kScratchNumbers;

                extractNumbers(kNumbers[0], kWinningNumbers);
                extractNumbers(kNumbers[1], kScratchNumbers);

                kScratchCards.push_back(CScratchCard(kWinningNumbers, kScratchNumbers));
            }
        }

        std::uint64_t nWinningPointsCount = 0;
        std::uint64_t nTotalTicketCount   = 0;
        for (std::vector<CScratchCard>::iterator it = kScratchCards.begin(); it != kScratchCards.end(); ++it)
        {
            const std::uint64_t nWinningNumbers     = it->GetWinningNumbersCount();
            const std::uint64_t nWinningPoints      = it->GetWinningPoints();
            const std::uint64_t nTicketCount        = it->GetNumTickets();
            nWinningPointsCount += nWinningPoints;
            nTotalTicketCount   += nTicketCount;

            if (nWinningNumbers)
            {
                // Add the number of held tickets to the Number of Winning Tickets in the following games
                for (std::vector<CScratchCard>::iterator it2 = it + 1; it2 != kScratchCards.end() && it2 != (it + nWinningNumbers + 1); ++it2)
                {
                    it2->AddTicket(nTicketCount);
                }
            }
        }

        std::cout << "Part 1: " << nWinningPointsCount << std::endl;
        std::cout << "Part 2: " << nTotalTicketCount   << std::endl;
   }

    return 0;
}
