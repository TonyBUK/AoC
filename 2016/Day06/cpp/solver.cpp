#include <iostream>
#include <fstream>
#include <string>
#include "cstringmetrics.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        CStringMetrics  kStringMetrics;
        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            kStringMetrics.Add(kLine);
        }

        kStringMetrics.Calculate();

        std::cout << "Part 1: " << kStringMetrics.Max() << std::endl;
        std::cout << "Part 2: " << kStringMetrics.Min() << std::endl;
    }

    return 0;
}
