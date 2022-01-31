#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::string kString;
        while (!kFile.eof())
        {
            std::getline(kFile, kString);
            const std::size_t nOpen  = std::count(kString.cbegin(), kString.cend(), '(');
            const std::size_t nClose = std::count(kString.cbegin(), kString.cend(), ')');
            std::cout << "Part 1: " << (nOpen-nClose) << std::endl;

            std::size_t nPos;
            int         nFloor = 0;
            for (std::string::const_iterator it = kString.cbegin(); it != kString.cend(); ++it)
            {
                if ('(' == *it)
                {
                    ++nFloor;
                }
                else if (')' == *it)
                {
                    --nFloor;
                }

                if (-1 == nFloor)
                {
                    nPos = (it - kString.cbegin()) + 1;
                    break;
                }
            }
            std::cout << "Part 2: " << nPos << std::endl;
        }
    }

    return 0;
}
