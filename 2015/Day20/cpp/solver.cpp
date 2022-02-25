#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>
#include <assert.h>

#define MAX_SIZE_T static_cast<std::size_t>(static_cast<uint64_t>(-1))

std::size_t findLowestHouseNumber(const std::size_t nTargetPresents, const std::size_t nPresentsPerHouse, const std::size_t nHousesPerElf = MAX_SIZE_T)
{
    if (nTargetPresents <= nPresentsPerHouse)
    {
        return 1;
    }

    const std::size_t        nNormalisedTargetPresents = nTargetPresents / nPresentsPerHouse;
    std::vector<std::size_t> kTotalCounts(nNormalisedTargetPresents+1, nPresentsPerHouse);

    // The Basic Algorithm to process all elves in turn, and increment the present count
    // for all possible houses they can deliver to
    for (std::size_t nElf = 2; nElf <= nNormalisedTargetPresents; ++nElf)
    {
        // Cache the Present Count
        const std::size_t nElfPresents = nElf * nPresentsPerHouse;

        // For each house the elf can visit
        for (std::size_t nHouse = 1; nHouse <= std::min(nNormalisedTargetPresents/nElf, nHousesPerElf); ++nHouse)
        {
            // Increment the Presents for the current house
            kTotalCounts.at(nHouse * nElf) += nElfPresents;
            if (kTotalCounts.at(nElf) >= nTargetPresents)
            {
                return nElf;
            }
        }
    }

    assert(false);
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            std::cout << "Part 1: " << findLowestHouseNumber(std::stoull(kLine, NULL, 10), 10)     << std::endl;
            std::cout << "Part 2: " << findLowestHouseNumber(std::stoull(kLine, NULL, 10), 11, 50) << std::endl;
        }
   }

    return 0;
}
