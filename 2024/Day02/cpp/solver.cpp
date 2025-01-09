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

class Levels
{
    public:

        Levels(const std::string& kLevels);
        bool IsSafe(void) const;
        bool IsSafeWithDampening(void) const;

    private:

        bool IsSafe(const std::vector<uint32_t>& kLevels) const;

        std::vector<uint32_t> m_kLevels;
};

Levels::Levels(const std::string& kLevels)
{
    const std::vector<std::string> kLevelsSplit = split(kLevels, " ", false);

    for (std::vector<std::string>::const_iterator it = kLevelsSplit.cbegin(); it != kLevelsSplit.cend(); ++it)
    {
        m_kLevels.push_back(std::stoi(*it));
    }
}

bool Levels::IsSafe(void) const
{
    return IsSafe(m_kLevels);
}

bool Levels::IsSafeWithDampening(void) const
{
    // Pre-allocate the new levels vector
    std::vector<uint32_t> newLevels(m_kLevels.size() - 1);

    for (std::size_t i = 0; i < m_kLevels.size(); ++i)
    {
        // Inline replace the new levels vector to remove the index indicated by i
        std::copy(m_kLevels.begin(), m_kLevels.begin() + i, newLevels.begin());
        std::copy(m_kLevels.begin() + i + 1, m_kLevels.end(), newLevels.begin() + i);

        // Re-test the new levels vector
        if (IsSafe(newLevels))
        {
            return true;
        }
    }

    return false;
}

bool Levels::IsSafe(const std::vector<uint32_t>& kLevels) const
{
    bool bLastSign = false;
    bool bFirst    = true;
 
    for (std::vector<uint32_t>::const_iterator it = kLevels.cbegin() + 1; it != kLevels.cend(); ++it)
    {
        const int32_t nDelta = static_cast<int32_t>(*it) - static_cast<int32_t>(*(it - 1));

        // Delta must be between -3 .. -1 or 1 .. 3.
        if ((nDelta == 0) || (abs(nDelta) > 3))
        {
            return false;
        }

        // Sign must be the same for all deltas.
        const bool bSign = (nDelta > 0);
        if (!bFirst && (bSign != bLastSign))
        {
            return false;
        }

        bFirst    = false;
        bLastSign = bSign;
    }

    return true;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    std::uint64_t nSafeLevelsPartOne = 0;
    std::uint64_t nSafeLevelsPartTwo = 0;

    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            // Parse through the File
            std::string kLine;
            std::getline(kFile, kLine);

            Levels kLevel(kLine);

            // If the level is safe, increment the counter for both parts as there's no need to dampen
            if (kLevel.IsSafe())
            {
                ++nSafeLevelsPartOne;
                ++nSafeLevelsPartTwo;
            }
            // Dampen any failures from Part One
            else if (kLevel.IsSafeWithDampening())
            {
                ++nSafeLevelsPartTwo;
            }
        }
        kFile.close();

        std::cout << "Part 1: " << nSafeLevelsPartOne << std::endl;
        std::cout << "Part 2: " << nSafeLevelsPartTwo << std::endl;
   }

    return 0;
}
