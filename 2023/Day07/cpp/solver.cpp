#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>

bool compareValues(const std::pair<char, std::uint64_t>&a, const std::pair<int, std::uint64_t>&b)
{
   return a.second < b.second;
}

class CHand
{
    public:

        CHand(const std::string& kHand, const std::uint64_t nBid, const bool bAllowJoker = false);
        std::uint64_t getBid() const;
        bool operator<(const CHand& kOther) const;

    private:

        enum EHand
        {
            eHighCard = 0,
            eOnePair,
            eTwoPairs,
            eThreeOfAKind,
            eFullHouse,
            eFourOfAKind,
            eFiveOfAKind
        };

        std::map<char, std::uint64_t> calculateCardCounts(const std::string& kHand) const;
        EHand                         getHandType(const std::string& kHand) const;
        std::string                   calculateBestHand(const std::string& kHand) const;
        std::uint64_t                 calculateHandStrength(const std::string& kHand, const std::string& kCardStrengths) const;

        std::uint64_t m_nBid;
        std::uint64_t m_nHandStrength;
        EHand         m_eHandType;
};

CHand::CHand(const std::string& kHand, const std::uint64_t nBid, const bool bAllowJoker) :
    m_nBid(nBid)
{
    assert(kHand.size() == 5);

    const std::string kCardStrengths = bAllowJoker ? "J23456789TQKA" : "23456789TJQKA";
    const std::string kBestHand      = bAllowJoker ? calculateBestHand(kHand) : kHand;

    m_eHandType     = getHandType(kBestHand);
    m_nHandStrength = calculateHandStrength(kHand, kCardStrengths);
}

std::uint64_t CHand::getBid() const
{
    return m_nBid;
}

bool CHand::operator<(const CHand& kOther) const
{
    if (m_eHandType < kOther.m_eHandType)
    {
        return true;
    }
    else if (m_eHandType > kOther.m_eHandType)
    {
        return false;
    }
    else
    {
        return m_nHandStrength < kOther.m_nHandStrength;
    }
}

std::map<char, std::uint64_t> CHand::calculateCardCounts(const std::string& kHand) const
{
    std::map<char, std::uint64_t> kCardCounts;
    for (std::string::const_iterator it = kHand.begin(); it != kHand.end(); ++it)
    {
        if (kCardCounts.find(*it) == kCardCounts.end())
        {
            kCardCounts[*it] = 1;
        }
        else
        {
            ++kCardCounts[*it];
        }
    }

    return kCardCounts;
}

CHand::EHand CHand::getHandType(const std::string& kHand) const
{
    const std::map<char, std::uint64_t> kCardCounts = calculateCardCounts(kHand);

    switch(kCardCounts.size())
    {
        case 1:
        {
            return eFiveOfAKind;
        }

        case 2:
        {
            if (4 == std::max_element(kCardCounts.begin(), kCardCounts.end(), compareValues)->second)
            {
                return eFourOfAKind;
            }
            else
            {
                return eFullHouse;
            }
        }

        case 3:
        {
            if (3 == std::max_element(kCardCounts.begin(), kCardCounts.end(), compareValues)->second)
            {
                return eThreeOfAKind;
            }
            else
            {
                return eTwoPairs;
            }
        }

        case 4:
        {
            return eOnePair;
        }

        case 5:
        {
            return eHighCard;
        }

        default:
        {
            assert(false);
            return eHighCard;
        }
    }
}

std::string CHand::calculateBestHand(const std::string& kHand) const
{
    // Early Out
    if (kHand.find('J') == std::string::npos)
    {
        return kHand;
    }

    // Get the Count for Each Unique Card
    const std::map<char, std::uint64_t> kCardCounts = calculateCardCounts(kHand);
    std::uint64_t                       nHighestFrequency     = 0;
    char                                cHighestFrequencyCard;
    std::string                         kBestHand;

    for (std::map<char, std::uint64_t>::const_iterator it = kCardCounts.cbegin(); it != kCardCounts.cend(); ++it)
    {
        if ('J' == it->first)
        {
            continue;
        }

        if (it->second > nHighestFrequency)
        {
            nHighestFrequency     = it->second;
            cHighestFrequencyCard = it->first;
        }
    }

    // Adjust the Hand based on the Joker
    // Note: Because of the way the comparisons work, we don't have to worry about picking the best
    //       of the highest frequency cards, as the hand strength will be the same regardless.
    kBestHand = kHand;
    if (nHighestFrequency > 0)
    {
        for (std::string::iterator it = kBestHand.begin(); it != kBestHand.end(); ++it)
        {
            if ('J' == *it)
            {
                *it = cHighestFrequencyCard;
            }
        }
    }

    return kBestHand;
}

std::uint64_t CHand::calculateHandStrength(const std::string& kHand, const std::string& kCardStrengths) const
{
    std::uint64_t nHandStrength = 0;
    for (std::string::const_iterator it = kHand.begin(); it != kHand.end(); ++it)
    {
        nHandStrength = (nHandStrength * kCardStrengths.size()) + kCardStrengths.find(*it);
    }

    return nHandStrength;
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
        std::vector<CHand>          kCardsPartOne;
        std::vector<CHand>          kCardsPartTwo;

        // Store the Race Times/Distances
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                const std::vector<std::string> kSplit = split(kLine, " ", false);
                assert(kSplit.size() == 2);

                kCardsPartOne.push_back(CHand(kSplit.at(0), std::stoull(kSplit.at(1)), false));
                kCardsPartTwo.push_back(CHand(kSplit.at(0), std::stoull(kSplit.at(1)), true));
            }
        }

        assert(kCardsPartOne.size() == kCardsPartTwo.size());

        // Sort the Cards by Strength
        std::sort(kCardsPartOne.begin(), kCardsPartOne.end());
        std::sort(kCardsPartTwo.begin(), kCardsPartTwo.end());

        std::uint64_t nPartOneRankedBids = 0;
        std::uint64_t nPartTwoRankedBids = 0;
        for (std::size_t i = 0; i < kCardsPartOne.size(); ++i)
        {
            nPartOneRankedBids += (i + 1) * kCardsPartOne.at(i).getBid();
            nPartTwoRankedBids += (i + 1) * kCardsPartTwo.at(i).getBid();
        }

        std::cout << "Part 1: " << nPartOneRankedBids << std::endl;
        std::cout << "Part 2: " << nPartTwoRankedBids << std::endl;
   }

    return 0;
}
