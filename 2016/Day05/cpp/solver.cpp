#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include "md5.h"

int main(int argc, char** argv)
{
    srand((unsigned)time(NULL));

    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            const std::string   kMovieOS        = "0123456789abcdefghijklmnopqrstuvwxyz";
            const std::string   kValidPositions = "01234567";
            std::string         kLine;
            std::string         kPasswordPart1;
            std::string         kPasswordPart2(8, ' ');
            std::vector<bool>   kPasswordPart2Validity(kPasswordPart2.size(), false);
            std::getline(kFile, kLine);

            unsigned long i      = 0;

            while ((kPasswordPart1.size() < 8) || (std::find(kPasswordPart2Validity.begin(), kPasswordPart2Validity.end(), false) != kPasswordPart2Validity.end()))
            {
                const std::string kHash = md5(kLine + std::to_string(i));

                if (kHash.find("00000") == 0)
                {
                    if (kPasswordPart1.size() < 8)
                    {
                        kPasswordPart1.push_back(kHash.at(5));
                    }

                    const std::size_t nPosition = kValidPositions.find(kHash.at(5));
                    if (std::string::npos != nPosition)
                    {
                        if (!kPasswordPart2Validity.at(nPosition))
                        {
                            kPasswordPart2Validity[nPosition] = true;
                            kPasswordPart2[nPosition]         = kHash.at(6);
                        }
                    }
                }

                i += 1;

                // Movie OS
                // This will rapidly print a garbled password containing a mix of:
                // 1. Known values which never change
                // 2. Random alphanumeric values for unknown entries
                //
                // The rate of change is unknown but acts after a large number of permutations
                // have been evaluated.
                //
                // Unfortunately going into higher than second level time precision tends to get
                // into the domain of OS specific operations, and has been deliberately excluded
                // for portability.
                if (0 == (i % 25000))
                {
                    for (std::size_t j = 0; j < kPasswordPart2.size(); ++j)
                    {
                        if (!kPasswordPart2Validity.at(j))
                        {
                            kPasswordPart2[j] = kMovieOS.at(rand() % kMovieOS.size());
                        }
                    }
                    std::cerr << "Hacking FBI Mainframe... " << kPasswordPart2 << "\r" << std::flush;
                }
            }
            std::cerr << "Follow the white rabbit...                " << std::endl;

            std::cout << "Part 1: " << kPasswordPart1 << std::endl;
            std::cout << "Part 2: " << kPasswordPart2 << std::endl;
        }
    }

    return 0;
}
