#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>
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

typedef std::map<std::string, int64_t>       happinessType;
typedef std::map<std::string, happinessType> peopleRulesType;

int64_t calculateHighestHappiness(const peopleRulesType& kPeople, const std::vector<std::string>& kSeatingPlan, std::vector<std::size_t>& kCurrentSeatingPlan, const std::size_t nMask, const std::size_t nCompletedMask, const int64_t nScore = 0)
{
    int64_t nLocalScore = nScore;

    if (nMask == nCompletedMask)
    {
        int64_t nCalculatedScore = 0;
        for (std::size_t i = 0; i < kCurrentSeatingPlan.size(); ++i)
        {
            const std::size_t  nPerson = kCurrentSeatingPlan.at(i);
            const std::size_t  nLeft   = kCurrentSeatingPlan.at((i+kCurrentSeatingPlan.size() - 1) % kCurrentSeatingPlan.size());
            const std::size_t  nRight  = kCurrentSeatingPlan.at((i+                           + 1) % kCurrentSeatingPlan.size());
            const std::string& kPerson = kSeatingPlan.at(nPerson);
            const std::string& kLeft   = kSeatingPlan.at(nLeft);
            const std::string& kRight  = kSeatingPlan.at(nRight);

            if (kPeople.at(kPerson).find(kLeft) != kPeople.at(kPerson).end())
            {
                nCalculatedScore += kPeople.at(kPerson).at(kLeft);
            }

            if (kPeople.at(kPerson).find(kRight) != kPeople.at(kPerson).end())
            {
                nCalculatedScore += kPeople.at(kPerson).at(kRight);
            }
        }
        if (nCalculatedScore > nLocalScore)
        {
            nLocalScore = nCalculatedScore;
        }
    }
    else
    {
        for (std::size_t i = 1; i < kSeatingPlan.size(); ++i)
        {
            const std::size_t nCurrentMask = 1 << i;
            if (0 == (nCurrentMask & nMask))
            {
                kCurrentSeatingPlan.push_back(i);
                nLocalScore = calculateHighestHappiness(kPeople, kSeatingPlan, kCurrentSeatingPlan, nMask | nCurrentMask, nCompletedMask, nLocalScore);
                kCurrentSeatingPlan.pop_back();
            }
        }
    }

    return nLocalScore;
}

int64_t calculateHighestHappiness(const peopleRulesType& kPeople)
{
    const std::size_t        nCompletedMask = (1 << kPeople.size()) - 1;
    std::vector<std::string> kSeatingPlan;
    std::vector<std::size_t> kCurrentSeatingPlan;

    for (peopleRulesType::const_iterator it = kPeople.cbegin(); it != kPeople.cend(); ++it)
    {
        kSeatingPlan.push_back(it->first);
    }

    kCurrentSeatingPlan.push_back(0);
    return calculateHighestHappiness(kPeople, kSeatingPlan, kCurrentSeatingPlan, 1,  nCompletedMask);
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        peopleRulesType kPeople;

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
            const std::vector<std::string> kRule = split(kLine, ' ');
            assert(kRule.size() == 11);
            kPeople[kRule.at(0)][kRule.at(10)] = ((kRule.at(2) == "gain") ? 1 : -1) * std::stoll(kRule.at(3));
        }

        std::cout << "Part 1: " << calculateHighestHappiness(kPeople) << std::endl;
        happinessType kDummy;
        kPeople[""] = kDummy;
        std::cout << "Part 2: " << calculateHighestHappiness(kPeople) << std::endl;
    }

    return 0;
}
