#include <iostream>
#include <fstream>
#include <string>
#include <cinttypes>
#include <assert.h>
#include "ckioskdata.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        int64_t nSectorSum       = 0;
        int64_t nNorthPoleSector = 0;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            CKioskData  kSector(kLine);
            if (kSector.Valid())
            {
                nSectorSum += kSector.SectorId();
                if (std::string::npos != kSector.Decrypt().find("north"))
                {
                    nNorthPoleSector = kSector.SectorId();
                }
            }
        }

        std::cout << "Part 1: " << nSectorSum       << std::endl;
        std::cout << "Part 2: " << nNorthPoleSector << std::endl;
   }

    return 0;
}
