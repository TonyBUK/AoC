#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

unsigned long min(const std::vector<unsigned long>& kArray)
{
    unsigned long minValue = 0xFFFFFFFF;
    for (std::vector<unsigned long>::const_iterator it = kArray.cbegin(); it != kArray.cend(); ++it)
    {
        if (*it < minValue)
        {
            minValue = *it;
        }
    }
    return minValue;
}

std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

unsigned long calculateWrappingPaper(const std::vector<unsigned long>& kBox)
{
    const int                        L                   = 0;
    const int                        W                   = 1;
    const int                        H                   = 2;
    const unsigned long              SIDES[]             = {kBox.at(L) * kBox.at(W),
                                                            kBox.at(W) * kBox.at(H),
                                                            kBox.at(H) * kBox.at(L)};
    unsigned long                    sideArea            = 0;
    const std::vector<unsigned long> kSides(SIDES, SIDES + sizeof(SIDES)/sizeof(const unsigned long));

    for (std::vector<unsigned long>::const_iterator it = kSides.cbegin(); it != kSides.cend(); ++it)
    {
        sideArea += 2 * *it;
    }

    return min(kSides) + sideArea;
}

unsigned long calculateRibbon(const std::vector<unsigned long>& kBox)
{
    std::vector<unsigned long> kSortedBox = kBox;
    std::sort(kSortedBox.begin(), kSortedBox.end());
    return (2 * kSortedBox.at(0)) + (2 * kSortedBox.at(1)) + (kBox.at(0) * kBox.at(1) * kBox.at(2));
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        unsigned long wrappingPaper = 0;
        unsigned long ribbon        = 0;

        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);
            const std::vector<std::string> kBoxString = split(kLine, 'x');
            std::vector<unsigned long> kBox;
            for (std::vector<std::string>::const_iterator it = kBoxString.cbegin(); it != kBoxString.cend(); ++it)
            {
                const unsigned long X = strtoul(it->c_str(), NULL, 10);
                kBox.push_back(X);
            }
            assert(kBox.size() == 3);
            wrappingPaper += calculateWrappingPaper(kBox);
            ribbon        += calculateRibbon(kBox);
        }

        std::cout << "Part 1: " << wrappingPaper << std::endl;
        std::cout << "Part 2: " << ribbon        << std::endl;
    }

    return 0;
}
