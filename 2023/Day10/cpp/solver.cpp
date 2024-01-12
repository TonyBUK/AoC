#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <assert.h>

struct SPosition
{
    std::int64_t nX;
    std::int64_t nY;

    bool operator<(const SPosition& kOther) const
    {
        return nX < kOther.nX || (nX == kOther.nX && nY < kOther.nY);
    }

    bool operator==(const SPosition& kOther) const
    {
        return (nX == kOther.nX) && (nY == kOther.nY);
    }
};

class CPipeMap
{
    public :
    
        typedef std::vector<SPosition> TNeighbours;

        CPipeMap();
        void AddLine(const std::string& kLine, std::string::size_type& nXPosition);
        void Replace(const SPosition& kPosition, const char cNewValue);
        void GetNeighbours(const SPosition& kPosition, TNeighbours& kNeighbours) const;
        char GetTypeFromNeighbours(const SPosition& kPosition) const;
        void Clean(const std::set<SPosition>& kPipes);
        char Get(const std::size_t nX, const std::size_t nY) const;
        char empty() const;
        std::size_t getWidth() const;
        std::size_t getHeight() const;

    private:

        typedef std::map<char, TNeighbours> TNeighboursMap;

        char Get(const SPosition& kPosition) const;

        const char                  m_cEmpty;
        std::vector<std::string>    m_kMap;
        TNeighboursMap              m_kNeighbours;
        bool                        m_bAllowStart;
};

CPipeMap::CPipeMap() :
    m_cEmpty('.'),
    m_kMap(),
    m_kNeighbours(),
    m_bAllowStart(true)
{
    m_kNeighbours.insert(std::make_pair('|', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('-', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('L', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('J', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('7', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('F', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('.', std::vector<SPosition>()));
    m_kNeighbours.insert(std::make_pair('S', std::vector<SPosition>()));

    m_kNeighbours['|'].push_back(SPosition{  0, -1 });
    m_kNeighbours['|'].push_back(SPosition{  0,  1 });
    m_kNeighbours['-'].push_back(SPosition{ -1,  0 });
    m_kNeighbours['-'].push_back(SPosition{  1,  0 });
    m_kNeighbours['L'].push_back(SPosition{  1,  0 });
    m_kNeighbours['L'].push_back(SPosition{  0, -1 });
    m_kNeighbours['J'].push_back(SPosition{ -1,  0 });
    m_kNeighbours['J'].push_back(SPosition{  0, -1 });
    m_kNeighbours['7'].push_back(SPosition{ -1,  0 });
    m_kNeighbours['7'].push_back(SPosition{  0,  1 });
    m_kNeighbours['F'].push_back(SPosition{  1,  0 });
    m_kNeighbours['F'].push_back(SPosition{  0,  1 });
    m_kNeighbours['S'].push_back(SPosition{  0, -1 });
    m_kNeighbours['S'].push_back(SPosition{  0,  1 });
    m_kNeighbours['S'].push_back(SPosition{ -1,  0 });
    m_kNeighbours['S'].push_back(SPosition{  1,  0 });
}

void CPipeMap::AddLine(const std::string& kLine, std::string::size_type& nXPosition)
{
    m_kMap.push_back(kLine);
    assert(m_kMap.back().length() == m_kMap.front().length());
    nXPosition = m_kMap.back().find('S');
}

void CPipeMap::Replace(const SPosition& kPosition, const char cNewValue)
{
    assert(kPosition.nX >= 0 && kPosition.nX < static_cast<std::int64_t>(m_kMap.front().length()));
    assert(kPosition.nY >= 0 && kPosition.nY < static_cast<std::int64_t>(m_kMap.size()));
    assert('S' != cNewValue);

    if ('S' == m_kMap[kPosition.nY][kPosition.nX])
    {
        m_bAllowStart = false;
        m_kNeighbours.erase('S');
    }
    m_kMap[kPosition.nY][kPosition.nX] = cNewValue;
}

std::size_t CPipeMap::getWidth() const
{
    return m_kMap.front().length();
}

std::size_t CPipeMap::getHeight() const
{
    return m_kMap.size();
}

void CPipeMap::GetNeighbours(const SPosition& kPosition, TNeighbours& kNeighbours) const
{
    kNeighbours.clear();
    const char kPipe = Get(kPosition);
    assert(m_kNeighbours.find(kPipe) != m_kNeighbours.cend());

    for (TNeighbours::const_iterator kIt = m_kNeighbours.at(kPipe).cbegin(); kIt != m_kNeighbours.at(kPipe).cend(); ++kIt)
    {
        const SPosition kNeighbour = { kPosition.nX + kIt->nX, kPosition.nY + kIt->nY };

        if ('S' == kPipe)
        {
            const char      kNeighbourPipe = Get(kNeighbour);

            for (TNeighbours::const_iterator kNeighbourIt = m_kNeighbours.at(kNeighbourPipe).cbegin(); kNeighbourIt != m_kNeighbours.at(kNeighbourPipe).cend(); ++kNeighbourIt)
            {
                const SPosition kNeighbourNeighbour = { kNeighbour.nX + kNeighbourIt->nX, kNeighbour.nY + kNeighbourIt->nY };
                if (kNeighbourNeighbour == kPosition)
                {
                    kNeighbours.push_back(kNeighbour);
                    break;
                }
            }
        }
        else
        {
            kNeighbours.push_back(kNeighbour);
        }
    }
}

char CPipeMap::GetTypeFromNeighbours(const SPosition& kPosition) const
{
    const char kPipe = Get(kPosition);
    assert(m_kNeighbours.find(kPipe) != m_kNeighbours.cend());

    TNeighbours kNeighbours;

    for (TNeighbours::const_iterator kIt = m_kNeighbours.at(kPipe).cbegin(); kIt != m_kNeighbours.at(kPipe).cend(); ++kIt)
    {
        const SPosition kNeighbour = { kPosition.nX + kIt->nX, kPosition.nY + kIt->nY };
        const char      kNeighbourPipe = Get(kNeighbour);

        for (TNeighbours::const_iterator kNeighbourIt = m_kNeighbours.at(kNeighbourPipe).cbegin(); kNeighbourIt != m_kNeighbours.at(kNeighbourPipe).cend(); ++kNeighbourIt)
        {
            const SPosition kNeighbourNeighbour = { kNeighbour.nX + kNeighbourIt->nX, kNeighbour.nY + kNeighbourIt->nY };
            if (kNeighbourNeighbour == kPosition)
            {
                kNeighbours.push_back(*kIt);
            }
        }
    }

    assert(kNeighbours.size() == 2);

    for (TNeighboursMap::const_iterator kIt = m_kNeighbours.cbegin(); kIt != m_kNeighbours.cend(); ++kIt)
    {
        if (kIt->second.size() != 2)
        {
            continue;
        }

        if (kIt->second.front() == kNeighbours.front() && kIt->second.back() == kNeighbours.back())
        {
            return kIt->first;
        }
        else if (kIt->second.front() == kNeighbours.back() && kIt->second.back() == kNeighbours.front())
        {
            return kIt->first;
        }
    }

    assert(false);
}

void CPipeMap::Clean(const std::set<SPosition>& kPipes)
{
    const std::size_t nWidth  = getWidth();
    const std::size_t nHeight = getHeight();
    std::vector<std::string> kCleanMap;
    for (std::size_t nY = 0; nY < nHeight; ++nY)
    {
        kCleanMap.push_back(std::string(nWidth, m_cEmpty));
    }

    for (std::set<SPosition>::const_iterator kIt = kPipes.cbegin(); kIt != kPipes.cend(); ++kIt)
    {
        kCleanMap[kIt->nY][kIt->nX] = m_kMap[kIt->nY][kIt->nX];
    }

    m_kMap = kCleanMap;
}

char CPipeMap::Get(const std::size_t nX, const std::size_t nY) const
{
    assert(nX < m_kMap.front().length());
    assert(nY < m_kMap.size());
    return m_kMap[nY][nX];
}

char CPipeMap::Get(const SPosition& kPosition) const
{
    if ((kPosition.nX < 0) || (kPosition.nX >= static_cast<std::int64_t>(m_kMap.front().length())))
    {
        return m_cEmpty;
    }
    else if ((kPosition.nY < 0) || (kPosition.nY >= static_cast<std::int64_t>(m_kMap.size())))
    {
        return m_cEmpty;
    }

    return m_kMap[kPosition.nY][kPosition.nX];
}

char CPipeMap::empty() const
{
    return m_cEmpty;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        CPipeMap kPipeMap;
        std::set<SPosition> kPipes;
        SPosition kStartPosition;

        // Read the Pipe Maze
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                std::string::size_type nXPosition;

                kPipeMap.AddLine(kLine, nXPosition);

                if (nXPosition != std::string::npos)
                {
                    assert(kPipes.size() == 0);

                    kStartPosition.nX = static_cast<std::int64_t>(nXPosition);
                    kStartPosition.nY = static_cast<std::int64_t>(kPipeMap.getHeight()) - 1;
                    kPipes.insert(kStartPosition);
                }
            }
        }

        kFile.close();

        assert(kPipes.size() == 1);

        // Part 1

        // Walk the Pipe Maze
        SPosition   kPosition   = kStartPosition;
        while (1)
        {
            CPipeMap::TNeighbours kNeighbours;
            kPipeMap.GetNeighbours(kPosition, kNeighbours);

            assert(kNeighbours.size() == 2);

            if (kPipes.find(kNeighbours.front()) == kPipes.cend())
            {
                kPosition = kNeighbours.front();
            }
            else if (kPipes.find(kNeighbours.back()) == kPipes.cend())
            {
                kPosition = kNeighbours.back();
            }
            else
            {
                break;
            }
            kPipes.insert(kPosition);
        }

        // We should have now walked the entire pipe system, and given it's a loop, we should have an even number of pipes.
        assert((kPipes.size() % 2) == 0);
        std::cout << "Part 1: " << kPipes.size() / 2 << std::endl;

        // Part 2

        // Update the Start Position with the correct Pipe Type
        kPipeMap.Replace(kStartPosition, kPipeMap.GetTypeFromNeighbours(kStartPosition));

        // Clean the Map
        kPipeMap.Clean(kPipes);

        // We now need to walk the map and figure out what is inside the map and what is outside
        const std::size_t nWidth           = kPipeMap.getWidth();
        const std::size_t nHeight          = kPipeMap.getHeight();
        const char        kEmpty           = kPipeMap.empty();
        // https://en.wikipedia.org/wiki/Point_in_polygon#Ray_casting_algorithm
        const std::string kVerticalPipes   = "|F7"; // "|LJ" woyld also work.
        std::size_t       nInsideCount     = 0;

        for (std::size_t nY = 0; nY < nHeight; ++nY)
        {
            bool bInside = false;
            for (std::size_t nX = 0; nX < nWidth; ++nX)
            {
                const char kPipe = kPipeMap.Get(nX, nY);

                if (kEmpty == kPipe)
                {
                    if (bInside)
                    {
                        ++nInsideCount;
                    }
                }
                else if (kVerticalPipes.find(kPipe) != std::string::npos)
                {
                    bInside = !bInside;
                }
            }
        }

        std::cout << "Part 2: " << nInsideCount << std::endl;
   }

    return 0;
}
