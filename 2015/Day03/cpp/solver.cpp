#include <iostream>
#include <fstream>
#include <string>
#include <map>

struct positionType
{
    int X;
    int Y;

    bool const operator==(const positionType &o) const
    {
        return X == o.X && Y == o.Y;
    }

    bool const operator<(const positionType &o) const
    {
        return X < o.X || (X == o.X && Y < o.Y);
    }
};

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::map<positionType, bool>    kPositions;
        positionType                    kPosition                 = {0,0};
        std::map<positionType, bool>    kCombinedPositions;
        positionType                    kCombinedPosition[2]      = {{0,0},{0,0}};
        int                             nCurrent                  = 0;

        while (!kFile.eof())
        {
            const char C = kFile.get();
            if ('<' == C)
            {
                kCombinedPosition[nCurrent].X -= 1;
                kPosition.X                   -= 1;
            }
            else if ('^' == C)
            {
                kCombinedPosition[nCurrent].Y += 1;
                kPosition.Y                   += 1;
            }
            else if ('>' == C)
            {
                kCombinedPosition[nCurrent].X += 1;
                kPosition.X                   += 1;
            }
            else if ('v' == C)
            {
                kCombinedPosition[nCurrent].Y -= 1;
                kPosition.Y                   -= 1;
            }

            kPositions.insert        (std::pair<positionType, bool>(kPosition,                   true));
            kCombinedPositions.insert(std::pair<positionType, bool>(kCombinedPosition[nCurrent], true));
            nCurrent = 1 - nCurrent;
        }

        std::cout << "Part 1: " << kPositions.size()         << std::endl;
        std::cout << "Part 2: " << kCombinedPositions.size() << std::endl;
    }

    return 0;
}
