#include <iostream>
#include <fstream>
#include <string>
#include "cdecompressor.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            CDecompressor kPart1(kLine, CDecompressor::E_CALCULATE_LENGTH, false);
            CDecompressor kPart2(kLine, CDecompressor::E_CALCULATE_LENGTH, true);

            std::cout << "Part 1: " << kPart1.length() << std::endl;
            std::cout << "Part 2: " << kPart2.length() << std::endl;
        }
    }

    return 0;
}
