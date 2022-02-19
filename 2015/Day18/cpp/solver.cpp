#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <array>
#include <cinttypes>
#include <assert.h>

#if 0

#define MAKE_POSITION(X, Y) {Y, X}
#define EXTRACT_Y(POS)      POS[0]
#define EXTRACT_X(POS)      POS[1]
typedef std::array<std::size_t, 2> positionType;

#else

#define MAKE_POSITION(X,Y)  ((((uint64_t)X&0x00000000FFFFFFFFull) << 0ull)|(((uint64_t)Y&0x00000000FFFFFFFFull) << 32ull))
#define EXTRACT_Y(POS)     (((uint64_t)POS&0xFFFFFFFF00000000ull) >> 32ull)
#define EXTRACT_X(POS)     (((uint64_t)POS&0x00000000FFFFFFFFull) >> 0ull)
typedef uint64_t positionType;

#endif

void initialiseConwayCandidates(const std::map<positionType, std::size_t>& kLiveConways, std::map<positionType, std::size_t>& kConwayNeighbourCount, const std::size_t nWidth, const std::size_t nHeight)
{
    kConwayNeighbourCount.clear();

    for (std::map<positionType, std::size_t>::const_iterator it = kLiveConways.cbegin(); it != kLiveConways.cend(); ++it)
    {
        // Add/Nop the Current Conway Candidate
        if (kConwayNeighbourCount.find(it->first) == kConwayNeighbourCount.end())
        {
            kConwayNeighbourCount[it->first] = 0;
        }

        // Extract X/Y
        const std::size_t Y = EXTRACT_Y(it->first);
        const std::size_t X = EXTRACT_X(it->first);

        // Set the Y Bounds
        const std::size_t tYStart = (Y >             1) ? Y - 1 : 0;
        const std::size_t tYEnd   = (Y < (nHeight - 2)) ? Y + 1 : nHeight - 1;

        // Set the X Bounds
        const std::size_t tXStart = (X >             1) ? X - 1 : 0;
        const std::size_t tXEnd   = (X < (nWidth  - 2)) ? X + 1 : nWidth - 1;

        for (std::size_t tY = tYStart; tY <= tYEnd; ++tY)
        {
            for (std::size_t tX = tXStart; tX <= tXEnd; ++tX)
            {
                if ((tY != Y) || (tX != X))
                {
                    positionType kPos = MAKE_POSITION(tX, tY);
                    if (kConwayNeighbourCount.find(kPos) == kConwayNeighbourCount.end())
                    {
                        kConwayNeighbourCount[kPos] = 0;
                    }
                    kConwayNeighbourCount[kPos] += 1;
                }
            }
        }
    }
}

std::size_t processConways(const std::map<positionType, std::size_t>& kInitialLiveConways, const std::size_t nIterations, const std::size_t nWidth, const std::size_t nHeight, const bool bForceCorners = false)
{
    std::map<positionType, std::size_t> kLiveConways            = kInitialLiveConways;
    std::map<positionType, std::size_t> kConwayNeighbourCount;
    const std::array<positionType, 4>   kConwayCorners          =
    {{
        MAKE_POSITION(0,                    0),
        MAKE_POSITION(0,          nHeight - 1),
        MAKE_POSITION(nWidth - 1,           0),
        MAKE_POSITION(nWidth - 1, nHeight - 1)
    }};

    for (std::size_t i = 0; i < nIterations; ++i)
    {
        if (bForceCorners)
        {
            for (std::array<positionType, 4>::const_iterator it = kConwayCorners.cbegin(); it != kConwayCorners.cend(); ++it)
            {
                kLiveConways[*it] = 0;
            }
        }

        initialiseConwayCandidates(kLiveConways, kConwayNeighbourCount, nWidth, nHeight);

        for (std::map<positionType, std::size_t>::const_iterator it = kConwayNeighbourCount.cbegin(); it != kConwayNeighbourCount.cend(); ++it)
        {
            if (kLiveConways.find(it->first) != kLiveConways.cend())
            {
                if ((2 != it->second) && (3 != it->second))
                {
                    kLiveConways.erase(it->first);
                }
            }
            else
            {
                if (3 == it->second)
                {
                    kLiveConways[it->first] = 0;
                }
            }
        }
    }

    if (bForceCorners)
    {
        for (std::array<positionType, 4>::const_iterator it = kConwayCorners.cbegin(); it != kConwayCorners.cend(); ++it)
        {
            kLiveConways[*it] = 0;
        }
    }

    return kLiveConways.size();
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        // Containers
        std::map<positionType, std::size_t> kLiveConways;
        std::size_t                         nHeight         = 0;
        std::size_t                         nWidth          = 0;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            for (std::size_t nX = 0; nX < kLine.size(); ++nX)
            {
                if ('#' == kLine.at(nX))
                {
                    positionType kPos   = MAKE_POSITION(nX, nHeight);
                    kLiveConways[kPos]  = 0;
                }
                if (nX >= nWidth)
                {
                    nWidth = nX + 1;
                }
            }
            ++nHeight;
        }

        std::cout << "Part 1: " << processConways(kLiveConways, 100, nWidth, nHeight)       << std::endl;
        std::cout << "Part 2: " << processConways(kLiveConways, 100, nWidth, nHeight, true) << std::endl;
   }

    return 0;
}
