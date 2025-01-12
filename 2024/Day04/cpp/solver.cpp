#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class WordSearch
{
    public:

        WordSearch(const std::vector<std::string>& kWordSearch);

    protected:

        const std::vector<std::string>& m_kWordSearch;
        size_t                          m_nWidth;
        size_t                          m_nHeight;
};

WordSearch::WordSearch(const std::vector<std::string>& kWordSearch) :
    m_kWordSearch(kWordSearch)    
{
    m_nWidth  = 0;
    m_nHeight = 0;

    for (const std::string& kLine : kWordSearch)
    {
        m_nWidth  = std::max(m_nWidth, kLine.size());
        ++m_nHeight;
    }
}

class XMasWordSearch : public WordSearch
{
    public:

        XMasWordSearch(const std::vector<std::string>& kWordSearch);
        uint64_t count(const size_t X, const size_t Y) const;

    private:

        std::string across(const size_t X, const size_t Y) const;
        std::string down(const size_t X, const size_t Y) const;
        std::string diagonalBackward(const size_t X, const size_t Y) const;
        std::string diagonalForward(const size_t X, const size_t Y) const;
};

XMasWordSearch::XMasWordSearch(const std::vector<std::string>& kWordSearch) :
    WordSearch(kWordSearch)
{
}

uint64_t XMasWordSearch::count(const size_t X, const size_t Y) const
{
    const bool bX = m_kWordSearch[Y][X] == 'X';
    const bool bS = m_kWordSearch[Y][X] == 'S';
    if (!bX && !bS)
    {
        return 0;
    }

    uint64_t nCount = 0;

    // Across/Down/Diagonals
    if (bX)
    {
        if (across(X, Y)           == "XMAS") ++nCount;
        if (down(X, Y)             == "XMAS") ++nCount;
        if (diagonalBackward(X, Y) == "XMAS") ++nCount;
        if (diagonalForward(X, Y)  == "XMAS") ++nCount;
    }
    else
    {
        if (across(X, Y)           == "SAMX") ++nCount;
        if (down(X, Y)             == "SAMX") ++nCount;
        if (diagonalBackward(X, Y) == "SAMX") ++nCount;
        if (diagonalForward(X, Y)  == "SAMX") ++nCount;
    }

    return nCount;
}

std::string XMasWordSearch::across(const size_t X, const size_t Y) const
{
    // Across Pattern
    if ((X + 4) <= m_nWidth)
    {
        return m_kWordSearch[Y].substr(X, 4);
    }
    return "";
}

std::string XMasWordSearch::down(const size_t X, const size_t Y) const
{
    // Down Pattern
    std::string kDown;
    if ((Y + 4) <= m_nHeight)
    {
        for (size_t nY = Y; nY < (Y + 4); ++nY)
        {
            kDown += m_kWordSearch[nY][X];
        }
        return kDown;
    }
    return "";
}

std::string XMasWordSearch::diagonalBackward(const size_t X, const size_t Y) const
{
    // Diagonal Pattern 1 (Down and Left)

    std::string kDiagonal;
    if ((X >= 3) && (Y + 4) <= m_nHeight)
    {
        for (size_t nY = Y, nX = X; nY < (Y + 4); ++nY, --nX)
        {
            kDiagonal += m_kWordSearch[nY][nX];
        }
        return kDiagonal;
    }
    return "";
}

std::string XMasWordSearch::diagonalForward(const size_t X, const size_t Y) const
{
    // Diagonal Pattern 1 (Down and Right)

    std::string kDiagonal;
    if ((X + 4) <= m_nWidth && (Y + 4) <= m_nHeight)
    {
        for (size_t nY = Y, nX = X; nY < (Y + 4); ++nY, ++nX)
        {
            kDiagonal += m_kWordSearch[nY][nX];
        }
        return kDiagonal;
    }
    return "";
}

class CrossMasWordSearch : public WordSearch
{
    public:

        CrossMasWordSearch(const std::vector<std::string>& kWordSearch);
        uint64_t count(const size_t X, const size_t Y) const;
};

CrossMasWordSearch::CrossMasWordSearch(const std::vector<std::string>& kWordSearch) :
    WordSearch(kWordSearch)
{
}

uint64_t CrossMasWordSearch::count(const size_t X, const size_t Y) const
{
    // Part Two's Pattern is actually much simpler...
    // Find a Middle A, then check the Diagonals for MS/SM

    uint64_t nCount = 0;
    if ((X >= 1) && (X < (m_nWidth - 1)) && (Y >= 1) && (Y < (m_nHeight - 1)))
    {
        if (m_kWordSearch[Y][X] == 'A')
        {
            const std::string kDiagonal1 = {m_kWordSearch[Y-1][X-1], m_kWordSearch[Y+1][X+1]};
            const std::string kDiagonal2 = {m_kWordSearch[Y-1][X+1], m_kWordSearch[Y+1][X-1]};

            if (kDiagonal1 == "MS" || kDiagonal1 == "SM")
            {
                if (kDiagonal2 == "MS" || kDiagonal2 == "SM")
                {
                    ++nCount;
                }
            }
        }
    }

    return nCount;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    std::uint64_t nPartOne = 0;
    std::uint64_t nPartTwo = 0;
    size_t        nWidth   = 0;
    size_t        nHeight  = 0;

    std::vector<std::string> kWordSearch;

    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            // Parse through the File
            std::string kLine;
            std::getline(kFile, kLine);
            kWordSearch.push_back(kLine);
            nWidth = std::max(nWidth, kLine.size());
            ++nHeight;
        }
        kFile.close();

        XMasWordSearch     kXMasWordSearch(kWordSearch);
        CrossMasWordSearch kCrossMasWordSearch(kWordSearch);

        // Perform the Word Search for Both Parts
        for (size_t Y = 0; Y < nHeight; ++Y)
        {
            for (size_t X = 0; X < nWidth; ++X)
            {
                nPartOne += kXMasWordSearch.count(X, Y);
                nPartTwo += kCrossMasWordSearch.count(X, Y);
            }
        }

        std::cout << "Part 1: " << nPartOne << std::endl;
        std::cout << "Part 2: " << nPartTwo << std::endl;
   }

    return 0;
}
