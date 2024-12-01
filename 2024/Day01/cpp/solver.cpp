#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<uint32_t>        kLeftList;
        std::vector<uint32_t>        kRightList;
        std::map<uint32_t, uint32_t> kCounterMap;

        while (!kFile.eof())
        {
            // Parse through the File
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                // Get the Left/Right Lists
                kLeftList.push_back(std::stoi(kLine));
                kRightList.push_back(std::stoi(kLine.substr(kLine.find_last_of(' ') + 1)));

                // Increment the Counter List Hash Map
                if (kCounterMap.find(kRightList.back()) == kCounterMap.end())
                {
                    kCounterMap[kRightList.back()] = 1;
                }
                else
                {
                    kCounterMap[kRightList.back()]++;
                }
            }
        }

        // Sort the Lists
        std::sort(kLeftList.begin(),  kLeftList.end());
        std::sort(kRightList.begin(), kRightList.end());

        // Calculate the Distances and Similarity Score
        std::uint64_t nDistance   = 0u;
        std::uint64_t nSimilarity = 0u;

        for (size_t nIndex = 0; nIndex < kLeftList.size(); ++nIndex)
        {
            // Part 1: Distances
            nDistance += std::abs(static_cast<int64_t>(kLeftList[nIndex]) -
                                  static_cast<int64_t>(kRightList[nIndex]));

            // Part 2: Similarity Score
            const std::map<uint32_t, uint32_t>::const_iterator it = kCounterMap.find(kLeftList[nIndex]);
            if (it != kCounterMap.cend())
            {
                nSimilarity += kLeftList[nIndex] * it->second;
            }
        }

        std::cout << "Part 1: " << nDistance   << std::endl;
        std::cout << "Part 2: " << nSimilarity << std::endl;
   }

    return 0;
}
