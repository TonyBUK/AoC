#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

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

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        const std::size_t WIDTH  = 1000;
        const std::size_t HEIGHT = 1000;

        // Whilst std::array might make more sense here, it's the classic stack vs. heap
        // issue.  std::array uses the stack, but given the shear size of the data, it'd
        // mean tweaking the build file, whereas heap will just build as-is.
        std::vector< std::vector<std::size_t> > kLight      (HEIGHT, std::vector<std::size_t>(WIDTH, 0));
        std::vector< std::vector<std::size_t> > kLightDimmer(HEIGHT, std::vector<std::size_t>(WIDTH, 0));

        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);
            const std::vector<std::string> kCommand = split(kLine, ' ');

            std::vector<std::string> kStart;
            std::vector<std::string> kEnd;
            if ("toggle" == kCommand[0])
            {
                kStart = split(kCommand[1], ',');
                kEnd   = split(kCommand[3], ',');
            }
            else if ("turn" == kCommand[0])
            {
                kStart = split(kCommand[2], ',');
                kEnd   = split(kCommand[4], ',');
            }
            else
            {
                assert(false);
            }

            for (std::size_t Y = std::stoul(kStart[1]); Y <= std::stoul(kEnd[1]); ++Y)
            {
                for (std::size_t X = std::stoul(kStart[0]); X <= std::stoul(kEnd[0]); ++X)
                {
                    if ("toggle" == kCommand[0])
                    {
                        kLight.at(Y).at(X)        = 1 - kLight.at(Y).at(X);
                        kLightDimmer.at(Y).at(X) += 2;
                    }
                    else if ("off" == kCommand[1])
                    {
                        kLight.at(Y).at(X)        = 0;
                        if (kLightDimmer.at(Y).at(X) > 0) kLightDimmer.at(Y).at(X) -= 1;
                    }
                    else if ("on" == kCommand[1])
                    {
                        kLight.at(Y).at(X)        = 1;
                        kLightDimmer.at(Y).at(X) += 1;
                    }
                    else
                    {
                        assert(false);
                    }
                }
            }
        }

        std::size_t nPart1 = 0;
        std::size_t nPart2 = 0;
        for (std::size_t Y = 0; Y < HEIGHT; ++Y)
        {
            for (std::size_t X = 0; X < WIDTH; ++X)
            {
                nPart1 += kLight.at(Y).at(X);
                nPart2 += kLightDimmer.at(Y).at(X);
            }
        }

        std::cout << "Part 1: " << nPart1 << std::endl;
        std::cout << "Part 2: " << nPart2 << std::endl;
    }

    return 0;
}
