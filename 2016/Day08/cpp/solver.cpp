#include <iostream>
#include <fstream>
#include <string>
#include "cgridmanipulator.h"
#include "cgrid.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        CGrid   kGrid(50, 6);

        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            CGridManipulator kCommand(kLine);
            kCommand.Process(kGrid);
        }

        std::cout << "Part 1: " << kGrid.count() << std::endl;
        std::cout << "Part 2:" << std::endl;
        kGrid.print(' ', '#');
    }

    return 0;
}
