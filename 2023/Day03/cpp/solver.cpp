#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <assert.h>

struct SPosType
{
    std::int64_t nX;
    std::int64_t nY;

    bool operator<(const SPosType& kOther) const
    {
        if (nY < kOther.nY)
        {
            return true;
        }
        else if (nY > kOther.nY)
        {
            return false;
        }
        else
        {
            return (nX < kOther.nX);
        }
    }
};

struct SGearType
{
    std::uint64_t nValue;
    std::uint64_t nId;

    bool operator<(const SGearType& kOther) const
    {
        if (nValue < kOther.nValue)
        {
            return true;
        }
        else if (nValue > kOther.nValue)
        {
            return false;
        }
        else
        {
            return (nId < kOther.nId);
        }
    }
};

class CGrid
{
    public:

                        CGrid       (const char cOutofBounds);
        bool            AddRow      (const std::string& kRow);
        char            getAt       (const std::int64_t nX, const std::int64_t nY) const;
        std::uint64_t   getWidth    () const;
        std::uint64_t   getHeight   () const;

    private:

        const char                      m_cOutOfBounds;
              std::uint64_t             m_nWidth;
              std::uint64_t             m_nHeight;
              std::vector<std::string>  m_kGrid;
};

CGrid::CGrid(const char cOutofBounds)
    : m_cOutOfBounds(cOutofBounds)
    , m_nWidth(0)
    , m_nHeight(0)
{
}

bool CGrid::AddRow(const std::string& kRow)
{
    bool bValid = true;

    if (0 == m_nWidth)
    {
        m_nWidth = kRow.length();
    }
    else
    {
        bValid = (m_nWidth == kRow.length());
    }

    if (bValid)
    {
        m_kGrid.push_back(kRow);
        ++m_nHeight;
    }

    return bValid;
}

char CGrid::getAt(const std::int64_t nX, const std::int64_t nY) const
{
    if ((nX >= 0) && (nX < m_nWidth) && (nY >= 0) && (nY < m_nHeight))
    {
        return m_kGrid.at(nY).at(nX);
    }
    else
    {
        return m_cOutOfBounds;
    }
}

std::uint64_t CGrid::getWidth() const
{
    return m_nWidth;
}

std::uint64_t CGrid::getHeight() const
{
    return m_nHeight;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        const char kSpaceValue = '.';
        // As a Cheap Way of handling OoB, we'll declare a safe character
        // to return that will 'artificially' extend the grid as if nothing
        // is present.
        CGrid                         kGearsGrid(kSpaceValue);
        std::map<SPosType, SGearType> kPossiblePartNumbers;

        // Load the Grid
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);
            if (kLine.length())
            {
                const bool bValid = kGearsGrid.AddRow(kLine);
                assert(bValid);
            }
        }

        // Note: We don't need to identify that the numbers at this point *are* gears,
        //       we just need to know that they are numbers.  We can reduce this in
        //       a second pass.
        std::uint64_t nGearId = 0;
        for (std::uint64_t nY = 0; nY < kGearsGrid.getHeight(); ++nY)
        {
            std::vector<SPosType> kGearsOnRow;
            bool                  bFoundGear = false;
            std::uint64_t         nGearValue;

            // Little bit of a cheat, the Gears Class will handle invalid reads,
            // and we need to guarentee that we can process the row if a gear
            // is at the end without any special logic, this will forcibly ensure
            // that occurs.
            for (std::uint64_t nX = 0; nX <= kGearsGrid.getWidth(); ++nX)
            {
                const char kGear = kGearsGrid.getAt(nX, nY);

                // Create a list of all possible locations for each gear
                if (std::isdigit(kGear))
                {
                    const SPosType      kPos    = { static_cast<int64_t>(nX), static_cast<int64_t>(nY) };
                    const std::uint64_t nValue  = std::stoull(std::string(1, kGear));
                    kGearsOnRow.push_back(kPos);

                    if (!bFoundGear)
                    {
                        bFoundGear = true;
                        nGearValue = nValue;
                    }
                    else
                    {
                        nGearValue = (nGearValue * 10) + nValue;
                    }
                }
                else if (bFoundGear)
                {
                    // We can add the part numbers at this point, and use set unique properties
                    // to identify unique parts.
                    bFoundGear = false;
                    while (kGearsOnRow.size() > 0)
                    {
                        const SPosType  kPos   = kGearsOnRow.back();
                        const SGearType kValue = { nGearValue, nGearId };
                        kGearsOnRow.pop_back();
                        kPossiblePartNumbers[kPos] = kValue;
                    }
                    ++nGearId;
                }
            }
        }

        // Here's where we really solve this...
        //
        // Part 1 wants all parts.
        //
        // Part 2 wants only the parts that are in pairs around a gear.
        std::set<SGearType> kPartOneUniqueValues;
        std::uint64_t       nPartOneSum          = 0;
        std::uint64_t       nPartTwoGearRatioSum = 0;
        for (std::uint64_t nY = 0; nY < kGearsGrid.getHeight(); ++nY)
        {
            for (std::uint64_t nX = 0; nX < kGearsGrid.getWidth(); ++nX)
            {
                const char kGear = kGearsGrid.getAt(nX, nY);
                if (!std::isdigit(kGear) && (kGear != kSpaceValue))
                {
                    std::set<SGearType> kPartUniqueValues;

                    for (std::int64_t tY = static_cast<std::int64_t>(nY) - 1; tY <= static_cast<std::int64_t>(nY) + 1; ++tY)
                    {
                        for (std::int64_t tX = static_cast<std::int64_t>(nX) - 1; tX <= static_cast<std::int64_t>(nX) + 1; ++tX)
                        {
                            // Add this to the list of unique parts for this gear, and the unique parts overall.
                            const SPosType kPos = { tX, tY };
                            if (kPossiblePartNumbers.find(kPos) != kPossiblePartNumbers.end())
                            {
                                kPartUniqueValues.insert(kPossiblePartNumbers[kPos]);
                                kPartOneUniqueValues.insert(kPossiblePartNumbers[kPos]);
                            }
                        }
                    }

                    // We're only interested in pairs
                    if (kPartUniqueValues.size() == 2)
                    {
                        std::uint64_t nGearProduct = 1;
                        for (std::set<SGearType>::const_iterator it = kPartUniqueValues.cbegin(); it != kPartUniqueValues.cend(); ++it)
                        {
                            nGearProduct *= it->nValue;
                        }
                        nPartTwoGearRatioSum += nGearProduct;
                    }
                }
            }
        }

        for (std::set<SGearType>::const_iterator it = kPartOneUniqueValues.cbegin(); it != kPartOneUniqueValues.cend(); ++it)
        {
            nPartOneSum += it->nValue;
        }

        std::cout << "Part 1: " << nPartOneSum          << std::endl;
        std::cout << "Part 2: " << nPartTwoGearRatioSum << std::endl;
   }

    return 0;
}
