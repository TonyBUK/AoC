#include "ctriangle.h"
#include <algorithm>

CTriangle::CTriangle()
{
    set(0,0,0);
}

CTriangle::CTriangle(const std::size_t x, const std::size_t y, const std::size_t z)
{
    set(x,y,z);
}

void CTriangle::set(const std::size_t x, const std::size_t y, const std::size_t z)
{
    m_kTriangle[0] = x;
    m_kTriangle[1] = y;
    m_kTriangle[2] = z;
}

bool CTriangle::valid() const
{
    std::size_t kTriangle[] = {m_kTriangle[0],
                               m_kTriangle[1],
                               m_kTriangle[2]};
    std::sort(std::begin(kTriangle), std::end(kTriangle));

    return (kTriangle[0] + kTriangle[1]) > kTriangle[2];
}

const CTriangle::triangleType& CTriangle::get() const
{
    return m_kTriangle;
}