#include <iostream>
#include <fstream>
#include <string>
#include "cip7.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::size_t nSupportsTLS = 0;
        std::size_t nSupportsSSL = 0;
        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            CIP7    kIP7Address(kLine);
            nSupportsTLS += kIP7Address.supportsTLS() ? 1 : 0;
            nSupportsSSL += kIP7Address.supportsSSL() ? 1 : 0;
        }

        std::cout << "Part 1: " << nSupportsTLS << std::endl;
        std::cout << "Part 2: " << nSupportsSSL << std::endl;
    }

    return 0;
}
