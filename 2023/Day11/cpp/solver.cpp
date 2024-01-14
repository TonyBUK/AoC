#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <assert.h>

struct SPosition
{
    std::uint64_t nX;
    std::uint64_t nY;
};

class CGalaxies
{
    public :
    
        CGalaxies();
        void AddRow(const std::string& kLine);
        std::size_t getWidth() const;
        std::size_t getHeight() const;
        std::uint64_t sumDistances(const std::uint64_t nDistanceScaling) const;
        void calculateExpandedGalaxyLocations(std::uint64_t& nPointX, std::uint64_t& nPointY, std::uint64_t nDistanceScaling) const;

    private:

        std::uint64_t calculateManhattenDistance(const std::size_t nX1, const std::size_t nY1, const std::size_t nX2, const std::size_t nY2) const;

        std::vector<std::string>            m_kMap;
        mutable std::vector<std::uint64_t>  m_kMapEmptyRows;
        mutable std::vector<std::uint64_t>  m_kMapEmptyColumns;
        mutable bool                        m_bRecalculateColumns;
};

CGalaxies::CGalaxies() :
    m_kMap                  (),
    m_kMapEmptyRows         (),
    m_kMapEmptyColumns      (),
    m_bRecalculateColumns   (true)
{
}

void CGalaxies::AddRow(const std::string& kLine)
{
    m_kMap.push_back(kLine);
    assert(m_kMap.back().length() == m_kMap.front().length());
    m_bRecalculateColumns = true;
}

std::size_t CGalaxies::getWidth() const
{
    return m_kMap.front().length();
}

std::size_t CGalaxies::getHeight() const
{
    return m_kMap.size();
}

std::uint64_t CGalaxies::calculateManhattenDistance(const std::size_t nX1, const std::size_t nY1, const std::size_t nX2, const std::size_t nY2) const
{
    return static_cast<std::uint64_t>(llabs(static_cast<std::int64_t>(nX1) - static_cast<std::int64_t>(nX2)) +
                                      llabs(static_cast<std::int64_t>(nY1) - static_cast<std::int64_t>(nY2)));
}

std::uint64_t CGalaxies::sumDistances(const std::uint64_t nDistanceScaling) const
{
    const std::uint64_t nWidth     = getWidth();
    const std::uint64_t nHeight    = getHeight();

    // Cache the Columns for Speed on subsequent calls
    if (m_bRecalculateColumns)
    {
        m_kMapEmptyRows.clear();
        for (std::size_t nY = 0; nY < nHeight; ++nY)
        {
            m_kMapEmptyRows.push_back((m_kMap[nY].find('#') == std::string::npos) ? 1 : 0);
        }

        m_kMapEmptyColumns.clear();
        for (std::size_t nX = 0; nX < getWidth(); ++nX)
        {
            bool bEmpty = true;
            for (std::size_t nY = 0; nY < nWidth; ++nY)
            {
                if (m_kMap[nY][nX] != '.')
                {
                    bEmpty = false;
                    break;
                }
            }
            m_kMapEmptyColumns.push_back(bEmpty ? 1 : 0);
        }
        m_bRecalculateColumns = false;
    }

    // Calculate the Expanded Galaxy Locations
    std::vector<SPosition> kGalaxyExpandedLocations;
    for (std::uint64_t nY = 0; nY < nHeight; ++nY)
    {
        for (std::uint64_t nX = 0; nX < nWidth; ++nX)
        {
            if (m_kMap[nY][nX] == '#')
            {
                SPosition kPosition = { nX, nY };
                calculateExpandedGalaxyLocations(kPosition.nX, kPosition.nY, nDistanceScaling);
                kGalaxyExpandedLocations.push_back(kPosition);
            }
        }
    }

    std::uint64_t nSum = 0;

    for (std::vector<SPosition>::const_iterator it1 = kGalaxyExpandedLocations.cbegin(); it1 != kGalaxyExpandedLocations.cend(); ++it1)
    {
        for (std::vector<SPosition>::const_iterator it2 = it1 + 1; it2 != kGalaxyExpandedLocations.cend(); ++it2)
        {
            nSum += calculateManhattenDistance(it1->nX, it1->nY, it2->nX, it2->nY);
        }
    }

    return nSum;
}

void CGalaxies::calculateExpandedGalaxyLocations(std::uint64_t& nPointX, std::uint64_t& nPointY, std::uint64_t nDistanceScaling) const
{
    nPointX = nPointX + (std::accumulate(m_kMapEmptyColumns.cbegin(), m_kMapEmptyColumns.cbegin() + nPointX, 0) * (nDistanceScaling - 1));
    nPointY = nPointY + (std::accumulate(m_kMapEmptyRows.cbegin(),    m_kMapEmptyRows.cbegin()    + nPointY, 0) * (nDistanceScaling - 1));
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        CGalaxies kGalaxies;

        // Read the Pipe Maze
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                std::string::size_type nXPosition;

                kGalaxies.AddRow(kLine);
            }
        }

        std::cout << "Part 1: " << kGalaxies.sumDistances(2ull)       << std::endl;
        std::cout << "Part 2: " << kGalaxies.sumDistances(1000000ull) << std::endl;
   }

    return 0;
}
