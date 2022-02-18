#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <limits>
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

void calculatePermutations(const std::vector<std::size_t>& kContainers, const std::size_t nTarget, std::map<std::size_t, std::size_t>& kPermutations, const std::size_t nCurrent = 0, const std::size_t nCount = 0, const std::size_t nIndex = 0)
{
    // Update the Permutation if we've reached our target
    if (nTarget == nCurrent)
    {
        if (kPermutations.find(nCount) == kPermutations.end())
        {
            kPermutations[nCount] = 1;
        }
        else
        {
            kPermutations[nCount] += 1;
        }
        return;
    }

    // Iterate through the remaining possible permutations
    for (std::size_t i = nIndex; i < kContainers.size(); ++i)
    {
        const std::size_t nQuantity = kContainers.at(i);
        if ((nCurrent + nQuantity) <= nTarget)
        {
            calculatePermutations(kContainers, nTarget, kPermutations, nCurrent + nQuantity, nCount + 1, i + 1);
        }
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        // Containers
        std::vector<std::size_t> kContainers;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);
            kContainers.push_back(std::stoull(kLine, NULL, 10));
        }

        std::map<std::size_t, std::size_t> kPermutations;
        calculatePermutations(kContainers, 150, kPermutations);

        std::size_t nTotal = 0;
        std::size_t nMin   = std::numeric_limits<std::size_t>::max();
        for (std::map<std::size_t, std::size_t>::const_iterator it = kPermutations.cbegin(); it != kPermutations.cend(); ++it)
        {
            // Get the total of all permutations for Part 1
            nTotal += it->second;

            // Get the minimum number of containers used for Part 2
            if (it->first < nMin) nMin = it->first;
        }

        std::cout << "Part 1: " << nTotal              << std::endl;
        std::cout << "Part 2: " << kPermutations[nMin] << std::endl;
    }

    return 0;
}
