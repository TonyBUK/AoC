#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>
#include <assert.h>
#include "ctriangle.h"

std::vector<std::string> split(const std::string& s, const std::string& seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        pos     += seperator.length();
        prev_pos = pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

std::size_t countValid(const std::vector<CTriangle> kTriangles)
{
    std::size_t nValidCount = 0;
    for (std::vector<CTriangle>::const_iterator it = kTriangles.cbegin(); it != kTriangles.cend(); ++it)
    {
        if (it->valid())
        {
            ++nValidCount;
        }
    }

    return nValidCount;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<CTriangle> kTriangles;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            // Erase Duplicate Spaces
            std::size_t nDuplicateSpacePos;
            do
            {
                nDuplicateSpacePos = kLine.find("  ");
                if (std::string::npos != nDuplicateSpacePos)
                {
                    kLine.replace(nDuplicateSpacePos, 2, " ");
                }
            } while (std::string::npos != nDuplicateSpacePos);

            // Erase the Leading Space
            if (' ' == kLine.front())
            {
                kLine.erase(kLine.begin());
            }

            const std::vector<std::string> kTriangleString = split(kLine, " ");
            assert(kTriangleString.size() == 3);

            kTriangles.push_back(CTriangle(std::strtoull(kTriangleString.at(0).c_str(), NULL, 10),
                                           std::strtoull(kTriangleString.at(1).c_str(), NULL, 10),
                                           std::strtoull(kTriangleString.at(2).c_str(), NULL, 10)));
        }

        std::cout << "Part 1: " << countValid(kTriangles) << std::endl;

        assert((kTriangles.size() % 3) == 0);
        std::vector<CTriangle> kRearrangedTriangles;

        for (std::size_t Y = 0; Y < kTriangles.size(); Y += 3)
        {
            const CTriangle::triangleType& kTriangle1 = kTriangles.at(Y + 0).get();
            const CTriangle::triangleType& kTriangle2 = kTriangles.at(Y + 1).get();
            const CTriangle::triangleType& kTriangle3 = kTriangles.at(Y + 2).get();

            for (std::size_t X = 0; X < (sizeof(kTriangle1)/sizeof(kTriangle1[0])); ++X)
            {
                kRearrangedTriangles.push_back(CTriangle(kTriangle1[X], kTriangle2[X], kTriangle3[X]));
            }
        }

        std::cout << "Part 2: " << countValid(kRearrangedTriangles) << std::endl;
   }

    return 0;
}
