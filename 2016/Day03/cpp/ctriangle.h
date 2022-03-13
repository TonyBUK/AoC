#ifndef __CTRIANGLE_H__
#define __CTRIANGLE_H__

#include <cstddef>

class CTriangle
{
    public:

        typedef std::size_t triangleType[3];

                            CTriangle();
                            CTriangle(const std::size_t x, const std::size_t y, const std::size_t z);
        void                set(const std::size_t x, const std::size_t y, const std::size_t z);
        const triangleType& get() const;
        bool                valid() const;

    private:

        triangleType m_kTriangle;
};

#endif // __CTRIANGLE_H__
