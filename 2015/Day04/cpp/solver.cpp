#include <iostream>
#include <fstream>
#include <string>
#include "md5.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            unsigned long nPart1 = 0;
            unsigned long nPart2 = 0;
            unsigned long i      = 1;

            while ((0 == nPart1) || (0 == nPart2))
            {
                const std::string kHash = md5(kLine + std::to_string(i));

                if (0 == nPart1)
                {
                    if (kHash.find("00000") == 0)
                    {
                        nPart1 = i;
                    }
                }

                if (0 == nPart2)
                {
                    if (kHash.find("000000") == 0)
                    {
                        nPart2 = i;
                    }
                }

                i += 1;
            }

            std::cout << "Part 1: " << nPart1 << std::endl;
            std::cout << "Part 2: " << nPart2 << std::endl;
        }
    }

    return 0;
}
