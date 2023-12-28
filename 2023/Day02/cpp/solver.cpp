#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <assert.h>

class CCubeGame
{
    public:

                        CCubeGame   (const std::uint64_t nGameId, const std::vector<std::uint64_t> m_kRed, const std::vector<std::uint64_t> m_kGreen, const std::vector<std::uint64_t> m_kBlue);
        std::uint64_t   validGameId (const std::uint64_t nMaxRed, const std::uint64_t nMaxGreen, const std::uint64_t nMaxBlue) const;
        std::uint64_t   power       () const;
        std::uint64_t   id          () const;

    private:

        std::uint64_t m_nGameId;
        std::uint64_t m_nMaxRed;
        std::uint64_t m_nMaxGreen;
        std::uint64_t m_nMaxBlue;
};

CCubeGame::CCubeGame(const std::uint64_t nGameId, const std::vector<std::uint64_t> kRed, const std::vector<std::uint64_t> kGreen, const std::vector<std::uint64_t> kBlue) :
    m_nGameId(nGameId)
{
    m_nMaxRed = 0;
    for (std::vector<std::uint64_t>::const_iterator it = kRed.cbegin(); it != kRed.cend(); ++it)
    {
        m_nMaxRed = std::max(m_nMaxRed, *it);
    }

    m_nMaxGreen = 0;
    for (std::vector<std::uint64_t>::const_iterator it = kGreen.cbegin(); it != kGreen.cend(); ++it)
    {
        m_nMaxGreen = std::max(m_nMaxGreen, *it);
    }

    m_nMaxBlue = 0;
    for (std::vector<std::uint64_t>::const_iterator it = kBlue.cbegin(); it != kBlue.cend(); ++it)
    {
        m_nMaxBlue = std::max(m_nMaxBlue, *it);
    }
}

std::uint64_t CCubeGame::validGameId(const std::uint64_t nMaxRed, const std::uint64_t nMaxGreen, const std::uint64_t nMaxBlue) const
{
    return ((m_nMaxRed <= nMaxRed) && (m_nMaxGreen <= nMaxGreen) && (m_nMaxBlue <= nMaxBlue)) ? m_nGameId : 0;
}

std::uint64_t CCubeGame::power() const
{
    return m_nMaxRed * m_nMaxGreen * m_nMaxBlue;
}

std::uint64_t CCubeGame::id() const
{
    return m_nGameId;
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
        std::vector<CCubeGame> kCubeGames;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);
            if (kLine.length())
            {
                const std::vector<std::string> kTokens = split(kLine, ":", false);
                assert(kTokens.size() == 2);

                const std::vector<std::string> kGameIdTokens = split(kTokens[0], " ", false);
                assert(kGameIdTokens.size() == 2);
                const std::uint64_t nGameId = std::stoull(kGameIdTokens[1]);

                std::vector<std::uint64_t> kRed;
                std::vector<std::uint64_t> kGreen;
                std::vector<std::uint64_t> kBlue;

                const std::vector<std::string> kGameTokens = split(kTokens[1], ";", false);
                for (std::vector<std::string>::const_iterator it = kGameTokens.cbegin(); it != kGameTokens.cend(); ++it)
                {
                    const std::vector<std::string> kStoneTokens = split(*it, ",", false);

                    for (std::vector<std::string>::const_iterator stoneIt = kStoneTokens.cbegin(); stoneIt != kStoneTokens.cend(); ++stoneIt)
                    {
                        const std::vector<std::string> kStoneToken = split(*stoneIt, " ", false);
                        assert(kStoneToken.size() == 3);

                        const std::uint64_t nCount = std::stoull(kStoneToken[1]);

                        if (kStoneToken[2] == "red")
                        {
                            kRed.push_back(nCount);
                        }
                        else if (kStoneToken[2] == "green")
                        {
                            kGreen.push_back(nCount);
                        }
                        else if (kStoneToken[2] == "blue")
                        {
                            kBlue.push_back(nCount);
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                }

                kCubeGames.push_back(CCubeGame(nGameId, kRed, kGreen, kBlue));
            }
        }

        // Part 1
        std::uint64_t nGameIdSumPartOne = 0;
        for (std::vector<CCubeGame>::const_iterator it = kCubeGames.cbegin(); it != kCubeGames.cend(); ++it)
        {
            nGameIdSumPartOne += it->validGameId(12, 13, 14);
        }

        // Part 2
        std::uint64_t nGamPowerSumPartTwo = 0;
        for (std::vector<CCubeGame>::const_iterator it = kCubeGames.cbegin(); it != kCubeGames.cend(); ++it)
        {
            nGamPowerSumPartTwo += it->power();
        }

        std::cout << "Part 1: " << nGameIdSumPartOne   << std::endl;
        std::cout << "Part 2: " << nGamPowerSumPartTwo << std::endl;
   }

    return 0;
}
