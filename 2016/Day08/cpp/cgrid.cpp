#include "cgrid.h"
#include <iostream>

CGrid::CGrid(std::size_t WIDTH, std::size_t HEIGHT) :
    m_kGrid()
{
    while (m_kGrid.size() < HEIGHT)
    {
        m_kGrid.push_back(std::vector<bool>(WIDTH, false));
    }
}

void CGrid::setPixel(const std::size_t X, const std::size_t Y, const bool ENABLE)
{
    assert(Y < m_kGrid.size());
    assert(X < m_kGrid.at(Y).size());
    m_kGrid.at(Y).at(X) = ENABLE;
}

void CGrid::setPixels(const std::size_t LEFT, const std::size_t TOP, const std::size_t RIGHT, const std::size_t BOTTOM, const bool ENABLE)
{
    assert(RIGHT >= LEFT);
    assert(BOTTOM >= TOP);
    assert(BOTTOM < m_kGrid.size());
    assert(RIGHT < m_kGrid.at(BOTTOM).size());

    for (std::size_t Y = TOP; Y <= BOTTOM; ++Y)
    {
        for (std::size_t X = LEFT; X <= RIGHT; ++X)
        {
            m_kGrid.at(Y).at(X) = ENABLE;
        }
    }
}

void CGrid::shiftX(const std::size_t ROW, const int64_t SHIFT, const bool WRAP)
{
    assert(ROW < m_kGrid.size());

    const std::vector<bool> kBuffer = m_kGrid.at(ROW);
    const int64_t WIDTH            = static_cast<int64_t>(m_kGrid.at(ROW).size());
    const int64_t SHIFT_NORMALISED = (SHIFT < 0) ? (SHIFT % -WIDTH) : (SHIFT % WIDTH);

    for (std::size_t X = 0; X < kBuffer.size(); ++X)
    {
        int64_t nGridX = static_cast<int64_t>(X) + SHIFT;

        if (WRAP)
        {
            if (nGridX < 0) nGridX += WIDTH;
            nGridX = nGridX % WIDTH;
            m_kGrid.at(ROW).at(static_cast<std::size_t>(nGridX)) = kBuffer.at(X);
        }
        else if ((nGridX >= 0) && ((nGridX < m_kGrid.at(ROW).size())))
        {
            m_kGrid.at(ROW).at(static_cast<std::size_t>(nGridX)) = kBuffer.at(X);
        }
        else
        {
            if (nGridX < 0) nGridX += WIDTH;
            nGridX = nGridX % WIDTH;
            m_kGrid.at(ROW).at(static_cast<std::size_t>(nGridX)) = false;
        }
    }
}

void CGrid::shiftY(const std::size_t COL, const int64_t SHIFT, const bool WRAP)
{
    assert(m_kGrid.size() > 0);

    std::vector<bool> kBuffer;
    for (std::size_t Y = 0; Y < m_kGrid.size(); ++Y)
    {
        assert(COL < m_kGrid.at(Y).size());
        kBuffer.push_back(m_kGrid.at(Y).at(COL));
    }

    const int64_t HEIGHT           = static_cast<int64_t>(m_kGrid.size());
    const int64_t SHIFT_NORMALISED = (SHIFT < 0) ? (SHIFT % -HEIGHT) : (SHIFT % HEIGHT);

    for (std::size_t Y = 0; Y < kBuffer.size(); ++Y)
    {
        int64_t nGridY = static_cast<int64_t>(Y) + SHIFT;

        if (WRAP)
        {
            if (nGridY < 0) nGridY += HEIGHT;
            nGridY = nGridY % HEIGHT;
            m_kGrid.at(static_cast<std::size_t>(nGridY)).at(COL) = kBuffer.at(Y);
        }
        else if ((nGridY >= 0) && ((nGridY < m_kGrid.size())))
        {
            m_kGrid.at(static_cast<std::size_t>(nGridY)).at(COL) = kBuffer.at(Y);
        }
        else
        {
            if (nGridY < 0) nGridY += HEIGHT;
            nGridY = nGridY % HEIGHT;
            m_kGrid.at(static_cast<std::size_t>(nGridY)).at(COL) = false;
        }
    }
}

void CGrid::print(const char OFF, const char ON) const
{
    for (std::vector< std::vector<bool> >::const_iterator itRow = m_kGrid.cbegin(); itRow != m_kGrid.cend(); ++itRow)
    {
        for (std::vector<bool>::const_iterator itCol = itRow->cbegin(); itCol != itRow->cend(); ++itCol)
        {
            if (*itCol)
            {
                std::cout << ON;
            }
            else
            {
                std::cout << OFF;
            }
        }
        std::cout << std::endl;
    }
}

std::size_t CGrid::count(void) const
{
    std::size_t nCount = 0;
    for (std::vector< std::vector<bool> >::const_iterator itRow = m_kGrid.cbegin(); itRow != m_kGrid.cend(); ++itRow)
    {
        for (std::vector<bool>::const_iterator itCol = itRow->cbegin(); itCol != itRow->cend(); ++itCol)
        {
            if (*itCol)
            {
                ++nCount;
            }
        }
    }

    return nCount;
}
