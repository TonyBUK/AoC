#include <iostream>
#include <fstream>
#include <string>
#include "cfloors.h"
#include "helper.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        CFloors kFloors(4);
        std::size_t nFloor = 0;

        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            const std::vector<std::string> kTokens = split(kLine, ", -", false);
            for (std::size_t i = 0; i < kTokens.size(); ++i)
            {
                if ("a" == kTokens.at(i))
                {
                    assert(kTokens.size() > (i + 2));
                    const std::string& kName = kTokens.at(i + 1);
                    const std::string& kType = kTokens.at(i + 2);
                    kFloors.Add(nFloor, ("generator" == kType) ? CFloors::E_GENERATOR : CFloors::E_MICROCHIP, kName);
                }
            }

            ++nFloor;
        }

        std::cout << "Part 1: " << kFloors.Solve() << std::endl;
        kFloors.Add(0, 0, "elerium");
        kFloors.Add(0, 0, "dilithium");
        std::cout << "Part 2: " << kFloors.Solve() << std::endl;
    }

    return 0;
}
