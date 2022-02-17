#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <assert.h>

std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

struct reindeerProfile
{
    std::size_t nFlightTime;
    std::size_t nFlightDistance;
    std::size_t nRestTime;
};

enum reindeerCurrentState {E_REINDEER_RACING, E_REINDEER_RESTING};

struct reindeerState
{
    reindeerProfile         kProfile;
    reindeerCurrentState    eState;
    std::size_t             nStateRemainingTime;
    std::size_t             nDistance;
    std::size_t             nPoints;
};

std::size_t winningDistance(const std::vector<reindeerProfile>& kReindeerStats, const std::size_t nTargetTime)
{
    std::size_t nBestDistance = 0;

    for (std::vector<reindeerProfile>::const_iterator it = kReindeerStats.cbegin(); it != kReindeerStats.cend(); ++it)
    {
        const reindeerProfile&  kReindeer           = *it;
        int64_t                 nCurrentTime        = static_cast<int64_t>(nTargetTime);
        std::size_t             nCurrentDistance    = 0;
        while (nCurrentTime > 0)
        {
            std::size_t nDuration = kReindeer.nFlightTime;
            if (nDuration > nCurrentTime)
            {
                nDuration = nCurrentTime;
            }
            nCurrentDistance += (nDuration * kReindeer.nFlightDistance);
            nCurrentTime     -= static_cast<int64_t>(nDuration + kReindeer.nRestTime);
        }
        if (nCurrentDistance > nBestDistance)
        {
            nBestDistance = nCurrentDistance;
        }
    }

    return nBestDistance;
}

std::size_t winningPoints(const std::vector<reindeerState>& kInitialReindeerStates, const std::size_t nTargetTime)
{
    std::size_t                nBestPoints     = 0;
    std::vector<reindeerState> kReindeerStates = kInitialReindeerStates;

    for (std::size_t nSecond = 0; nSecond < nTargetTime; ++nSecond)
    {
        // Update the Distances and Compute the Best Distance
        std::size_t nBestDistance = 0;
        for (std::vector<reindeerState>::iterator it = kReindeerStates.begin(); it != kReindeerStates.end(); ++it)
        {
            reindeerState& kReindeer = *it;
            switch (kReindeer.eState)
            {
                case E_REINDEER_RACING:
                {
                    kReindeer.nDistance += kReindeer.kProfile.nFlightDistance;
                    --kReindeer.nStateRemainingTime;
                    if (0 == kReindeer.nStateRemainingTime)
                    {
                        kReindeer.eState              = E_REINDEER_RESTING;
                        kReindeer.nStateRemainingTime = kReindeer.kProfile.nRestTime;
                    }
                } break;

                case E_REINDEER_RESTING:
                {
                    --kReindeer.nStateRemainingTime;
                    if (0 == kReindeer.nStateRemainingTime)
                    {
                        kReindeer.eState              = E_REINDEER_RACING;
                        kReindeer.nStateRemainingTime = kReindeer.kProfile.nFlightTime;
                    }
                } break;

                default:
                {
                    assert(false);
                }
            }

            if (kReindeer.nDistance > nBestDistance)
            {
                nBestDistance = kReindeer.nDistance;
            }
        }

        // Update the Points and update the Best Points
        for (std::vector<reindeerState>::iterator it = kReindeerStates.begin(); it != kReindeerStates.end(); ++it)
        {
            reindeerState& kReindeer = *it;
            assert(kReindeer.nDistance <= nBestDistance);
            if (kReindeer.nDistance == nBestDistance)
            {
                ++kReindeer.nPoints;
                if (kReindeer.nPoints > nBestPoints)
                {
                    nBestPoints = kReindeer.nPoints;
                }
            }
        }
    }

    return nBestPoints;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<reindeerProfile> kReindeerProfiles;
        std::vector<reindeerState>   kReindeerStates;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);
            while (kLine.back() == '.')
            {
                kLine.pop_back();
            }

            // Add the Rule
            const std::vector<std::string> kProfile = split(kLine, ' ');
            assert(kProfile.size() == 15);

            // Construct the current Reindeer Profile/State
            reindeerState kReindeer;
            kReindeer.eState                    = E_REINDEER_RACING;
            kReindeer.kProfile.nFlightDistance  = std::stoull(kProfile.at(3));
            kReindeer.kProfile.nFlightTime      = std::stoull(kProfile.at(6));
            kReindeer.kProfile.nRestTime        = std::stoull(kProfile.at(13));
            kReindeer.nStateRemainingTime       = kReindeer.kProfile.nFlightTime;
            kReindeer.nDistance                 = 0;
            kReindeer.nPoints                   = 0;
            
            // Store the Profile/State
            kReindeerProfiles.push_back(kReindeer.kProfile);
            kReindeerStates.push_back(kReindeer);
        }

        std::cout << "Part 1: " << winningDistance(kReindeerProfiles, 2503) << std::endl;
        std::cout << "Part 2: " << winningPoints(kReindeerStates,     2503) << std::endl;
    }

    return 0;
}
