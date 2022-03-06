#include "cposition.h"
#include <cmath>

CPosition::CPosition()
{
    m_position.posX = 0;
    m_position.posY = 0;
}

CPosition::CPosition(const int64_t x, const int64_t y)
{
    m_position.posX = x;
    m_position.posY = y;
}

CPosition::CPosition(const PointType& pos) :
    m_position(pos)
{
}

void CPosition::setPos(const int64_t x, const int64_t y)
{
    m_position.posX = x;
    m_position.posY = y;
}

void CPosition::setPos(const PointType& pos)
{
    m_position = pos;
}

const CPosition::PointType& CPosition::getPos() const
{
    return m_position;
}

void CPosition::clamp(const int64_t left, const int64_t top, const int64_t right, const int64_t bottom)
{
    if (m_position.posX < left)
    {
        m_position.posX = left;
    }
    else if (m_position.posX > right)
    {
        m_position.posX = right;
    }

    if (m_position.posY < top)
    {
        m_position.posY = top;
    }
    else if (m_position.posY > bottom)
    {
        m_position.posY = bottom;
    }
}

CPosition CPosition::operator+(const CPosition& pos) const
{
    CPosition::PointType kPosition;
    kPosition.posX = m_position.posX + pos.getPos().posX;
    kPosition.posY = m_position.posY + pos.getPos().posY;

    return CPosition(kPosition);
}

CPosition CPosition::operator+(const int64_t n) const
{
    CPosition::PointType kPosition;
    kPosition.posX = m_position.posX + n;
    kPosition.posY = m_position.posY + n;

    return CPosition(kPosition);
}

CPosition CPosition::operator*(const CPosition& pos) const
{
    CPosition::PointType kPosition;
    kPosition.posX = m_position.posX * pos.getPos().posX;
    kPosition.posY = m_position.posY * pos.getPos().posY;

    return CPosition(kPosition);
}

CPosition CPosition::operator*(const int64_t n) const
{
    CPosition::PointType kPosition;
    kPosition.posX = m_position.posX * n;
    kPosition.posY = m_position.posY * n;

    return CPosition(kPosition);
}

CPosition& CPosition::operator+= (const CPosition& pos)
{
    m_position.posX += pos.getPos().posX;
    m_position.posY += pos.getPos().posY;

    return *this;
}

CPosition& CPosition::operator+= (const int64_t n)
{
    m_position.posX += n;
    m_position.posY += n;

    return *this;
}

CPosition& CPosition::operator*= (const CPosition& pos)
{
    m_position.posX *= pos.getPos().posX;
    m_position.posY *= pos.getPos().posY;

    return *this;
}

CPosition& CPosition::operator*= (const int64_t n)
{
    m_position.posX *= n;
    m_position.posY *= n;

    return *this;
}

bool CPosition::operator< (const CPosition& pos) const
{
    if (m_position.posX == pos.getPos().posX)
    {
        return m_position.posY < pos.getPos().posY;
    }
    return m_position.posX < pos.getPos().posX;
}

std::size_t CPosition::manhattenDistance() const
{
    return abs(m_position.posX) + abs(m_position.posY);
}
