#include "cfloors.h"
#include <assert.h>

CFloors::CFloors(const std::size_t nFloorCount)
{
    m_kFloors.assign(nFloorCount, FloorType());
}

void CFloors::Add(const std::size_t nGeneratorFloor, const std::size_t nMicrochipFloor, const std::string& kName)
{
    assert(nGeneratorFloor < m_kFloors.size());
    assert(nMicrochipFloor < m_kFloors.size());

    for (std::size_t i = 0; i < m_kFloors.size(); ++i)
    {
        const std::set<std::string>& kGenerator = m_kFloors.at(i).kGenerators;
        const std::set<std::string>& kMicrochip = m_kFloors.at(i).kMicrochips;
        assert(kGenerator.find(kName) == kGenerator.cend());
        assert(kMicrochip.find(kName) == kMicrochip.cend());
    }

    m_kFloors.at(nGeneratorFloor).kGenerators.insert(kName);
    m_kFloors.at(nMicrochipFloor).kMicrochips.insert(kName);
}

void CFloors::Add(const std::size_t nFloor, const ItemType eType, const std::string& kName)
{
    assert(nFloor < m_kFloors.size());

    for (std::size_t i = 0; i < m_kFloors.size(); ++i)
    {
        const std::set<std::string>& kItem = (E_GENERATOR == eType) ? m_kFloors.at(i).kGenerators : m_kFloors.at(i).kMicrochips;
        assert(kItem.find(kName) == kItem.cend());
    }
    std::set<std::string>& kItem = (E_GENERATOR == eType) ? m_kFloors.at(nFloor).kGenerators : m_kFloors.at(nFloor).kMicrochips;
    kItem.insert(kName);
}

std::size_t CFloors::Solve(void)
{
    std::map<FloorStateType, std::size_t> kCache;

    return Solve(0, 0, static_cast<std::size_t>(-1), kCache);
}

std::size_t CFloors::Solve(const std::size_t nFloor, const std::size_t nDepth, const std::size_t nBestSolution, std::map<FloorStateType, std::size_t>& kCache)
{
    std::size_t nBestSolutionLocal = nBestSolution;

    // Have we Failed?
    if (!Valid())
    {
        return nBestSolutionLocal;
    }

    // Are there enough moves remaining to solve?
    if ((nDepth + Min()) >= nBestSolutionLocal)
    {
        return nBestSolutionLocal;
    }

    // Have we encountered this game state before?
    const FloorStateType kState = State(nFloor);
    if (kCache.find(kState) != kCache.end())
    {
        if (nDepth >= kCache.at(kState))
        {
            return nBestSolutionLocal;
        }
    }
    kCache[kState] = nDepth;

    // Have we Solved the Puzzle?
    if (Solved())
    {
        return nDepth;
    }

    // Otherwise... carry on solving the Puzzle

    // One thing we can optimize is on floors where multiple pairs exist, ignore all but one of the pairs
    // We can do this because the pairs are essentially interchangeable
    std::set<std::string> kIgnoreList;
    for (std::set<std::string>::const_iterator it = m_kFloors.at(nFloor).kGenerators.cbegin(); it != m_kFloors.at(nFloor).kGenerators.cend(); ++it)
    {
        if (m_kFloors.at(nFloor).kMicrochips.find(*it) != m_kFloors.at(nFloor).kMicrochips.cend())
        {
            kIgnoreList.insert(*it);
        }
    }

    // Ahem... all but *one* of the pairs
    if (!kIgnoreList.empty())
    {
        kIgnoreList.erase(kIgnoreList.begin());
    }

    // Create the List of Microchips/Generators on each Floor as something we can trivially iterate on and filter
    std::vector<FloorProcessType> kFloorLookup;
    Add(m_kFloors.at(nFloor).kGenerators, kIgnoreList, E_GENERATOR, kFloorLookup);
    Add(m_kFloors.at(nFloor).kMicrochips, kIgnoreList, E_MICROCHIP, kFloorLookup);

    const int64_t kTests[4][2] =
    {
        {    1, 2   },
        {   -1, 1   },
        {    1, 1   },
        {   -1, 2   }
    };

    for (std::size_t nTest = 0; nTest < (sizeof(kTests) / sizeof(kTests[0])); ++nTest)
    {
        const int64_t* kTest     = kTests[nTest];
        const int64_t  nNewFloor = static_cast<int64_t>(nFloor) + kTest[0];
        if (nNewFloor < 0)                 continue;
        if (nNewFloor >= m_kFloors.size()) continue;

        for (std::size_t i = 0; i < kFloorLookup.size(); ++i)
        {
            Move(nFloor, nNewFloor, kFloorLookup.at(i));

            if (2 == kTest[1])
            {
                for (std::size_t j = i + 1; j < kFloorLookup.size(); ++j)
                {
                    Move(nFloor, nNewFloor, kFloorLookup.at(j));
                    nBestSolutionLocal = Solve(nNewFloor, nDepth + 1, nBestSolutionLocal, kCache);
                    Move(nNewFloor, nFloor, kFloorLookup.at(j));
                }
            }
            else if (1 == kTest[1])
            {
                nBestSolutionLocal = Solve(nNewFloor, nDepth + 1, nBestSolutionLocal, kCache);
            }
            else
            {
                assert(false);
            }

            Move(nNewFloor, nFloor, kFloorLookup.at(i));
        }
    }

    return nBestSolutionLocal;
}

bool CFloors::Valid(void) const
{
    for (std::vector<FloorType>::const_iterator it = m_kFloors.cbegin(); it != m_kFloors.cend(); ++it)
    {
        if (it->kGenerators.size() > 0)
        {
            for (std::set<std::string>::const_iterator itChip = it->kMicrochips.cbegin(); itChip != it->kMicrochips.cend(); ++itChip)
            {
                if (it->kGenerators.find(*itChip) == it->kGenerators.cend())
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CFloors::Solved(void) const
{
    for (std::size_t i = 0; (i < m_kFloors.size() - 1); ++i)
    {
        if (!m_kFloors.at(i).kGenerators.empty() ||
            !m_kFloors.at(i).kMicrochips.empty())
        {
            return false;
        }
    }

    assert(m_kFloors.back().kGenerators.size() == m_kFloors.back().kMicrochips.size());

    return true;
}

void CFloors::Move(const std::size_t nSourceFloor, const std::size_t nTargetFloor, const FloorProcessType& kItem)
{
    assert(nSourceFloor < m_kFloors.size());
    assert(nTargetFloor < m_kFloors.size());

    std::set<std::string>& kSource = (E_GENERATOR == kItem.eType) ? m_kFloors.at(nSourceFloor).kGenerators : m_kFloors.at(nSourceFloor).kMicrochips;
    std::set<std::string>& kTarget = (E_GENERATOR == kItem.eType) ? m_kFloors.at(nTargetFloor).kGenerators : m_kFloors.at(nTargetFloor).kMicrochips;

    assert(kSource.find(kItem.kElement) != kSource.end());
    assert(kTarget.find(kItem.kElement) == kTarget.end());

    kSource.erase(kItem.kElement);
    kTarget.insert(kItem.kElement);
}

std::size_t CFloors::Min(void) const
{
    std::size_t nCount = 0;

    for (std::size_t i = 0; (i < m_kFloors.size() - 1); ++i)
    {
        const std::size_t nFloorCount = m_kFloors.at(i).kGenerators.size() + m_kFloors.at(i).kMicrochips.size();

        if (0 == nFloorCount) continue;
        const std::size_t nFloorMultiplier = m_kFloors.size() - i - 1;
        
        if (nFloorCount <= 2) nCount += nFloorMultiplier;
        else                  nCount += (3 + ((nFloorCount - 3) * 2)) * nFloorMultiplier;

    }

    return nCount;
}

CFloors::FloorStateType CFloors::State(const std::size_t nFloor) const
{
    FloorStateType kState;
    kState.nFloor = nFloor;

    for (std::vector<FloorType>::const_iterator it = m_kFloors.cbegin(); it != m_kFloors.cend(); ++it)
    {
        kState.kGenerators.push_back(std::vector<std::size_t>());
        kState.kMicrochips.push_back(std::vector<std::size_t>());

        State (it->kGenerators, E_GENERATOR, kState.kGenerators.back());
        State (it->kMicrochips, E_MICROCHIP, kState.kMicrochips.back());

        std::sort(kState.kGenerators.back().begin(), kState.kGenerators.back().end());
        std::sort(kState.kMicrochips.back().begin(), kState.kMicrochips.back().end());
    }

    return kState;
}

void CFloors::State(const std::set<std::string>& kItems, const ItemType eType, std::vector<std::size_t>& kState) const
{
    const ItemType OTHER_TYPE[] = {E_MICROCHIP, E_GENERATOR};

    for (std::set<std::string>::const_iterator it = kItems.cbegin(); it != kItems.cend(); ++it)
    {
        kState.push_back(Find(*it, OTHER_TYPE[eType]));
    }
}

std::size_t CFloors::Find(const std::string& kName, const ItemType eType) const
{
    if (eType == E_GENERATOR)
    {
        for (std::size_t i = 0; i < m_kFloors.size(); ++i)
        {
            if (m_kFloors.at(i).kGenerators.find(kName) != m_kFloors.at(i).kGenerators.cend())
            {
                return i;
            }
        }
    }
    else if (eType == E_MICROCHIP)
    {
        for (std::size_t i = 0; i < m_kFloors.size(); ++i)
        {
            if (m_kFloors.at(i).kMicrochips.find(kName) != m_kFloors.at(i).kMicrochips.cend())
            {
                return i;
            }
        }
    }

    assert(false);
}

void CFloors::Add(const std::set<std::string>& kItems, const std::set<std::string>& kIgnoreList, const ItemType eType, std::vector<FloorProcessType>& kFloorLookup) const
{
    for (std::set<std::string>::const_iterator it = kItems.cbegin(); it != kItems.cend(); ++it)
    {
        if (kIgnoreList.find(*it) == kIgnoreList.cend())
        {
            FloorProcessType kEntry = {*it, eType};
            kFloorLookup.push_back(kEntry);
        }
    }
}