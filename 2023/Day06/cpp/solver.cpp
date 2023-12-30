#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>

class CRace
{
    public:

        CRace();
        CRace(const std::uint64_t& nTime, const std::uint64_t& nDistance);
        void update(const std::uint64_t& nTime, const std::uint64_t& nDistance);
        std::uint64_t numberWinners() const;

    private:

        std::uint64_t m_nTime;
        std::uint64_t m_nDistance;
};

CRace::CRace() :
    m_nTime(0),
    m_nDistance(0)
{
}

CRace::CRace(const std::uint64_t& nTime, const std::uint64_t& nDistance) :
    m_nTime(nTime),
    m_nDistance(nDistance)
{
}

void CRace::update(const std::uint64_t& nTime, const std::uint64_t& nDistance)
{
    m_nTime     = nTime;
    m_nDistance = nDistance;
}

std::uint64_t CRace::numberWinners() const
{
    const double nTime       = (double)m_nTime;
    const double nDistance   = (double)m_nDistance;
    const double nUpperBound = (-nTime - sqrt(pow(nTime, 2) + (-4.0) * (-1.0) * (-nDistance)))/-2.0;
    const double nLowerBound = (-nTime + sqrt(pow(nTime, 2) + (-4.0) * (-1.0) * (-nDistance)))/-2.0;

    return (std::uint64_t)(ceil(nUpperBound) - floor(nLowerBound)) - 1;
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
        bool                     bTimes     = true;
        std::vector<std::string> kTimes;
        std::vector<std::string> kDistances;
        std::vector<CRace>       kRacesPartOne;
        CRace                    kRacePartTwo;

        // Store the Race Times/Distances
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                const std::vector<std::string> kSplit = split(kLine, " ", false);

                assert(kSplit.size() > 0);
                for (std::vector<std::string>::const_iterator it = kSplit.begin() + 1; it != kSplit.end(); ++it)
                {
                    if (it->length())
                    {
                        if (bTimes)
                        {
                            kTimes.push_back(*it);
                        }
                        else
                        {
                            kDistances.push_back(*it);
                        }
                    }
                }
            }

            if (bTimes)
            {
                bTimes = false;
            }
        }

        assert(!bTimes);
        assert(kTimes.size() == kDistances.size());

        std::string kTime;
        std::string kDistance;

        for (std::size_t i = 0; i < kTimes.size(); ++i)
        {
            kTime     += kTimes.at(i);
            kDistance += kDistances.at(i);

            kRacesPartOne.push_back(CRace(std::stoull(kTimes.at(i)), std::stoull(kDistances.at(i))));
        }
        kRacePartTwo.update(std::stoull(kTime), std::stoull(kDistance));

        std::uint64_t nPartOneProduct = 1;
        for (std::vector<CRace>::const_iterator it = kRacesPartOne.begin(); it != kRacesPartOne.end(); ++it)
        {
            nPartOneProduct *= it->numberWinners();
        }

        std::cout << "Part 1: " << nPartOneProduct              << std::endl;
        std::cout << "Part 2: " << kRacePartTwo.numberWinners() << std::endl;
   }

    return 0;
}
