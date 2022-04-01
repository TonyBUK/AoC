#ifndef __C_GRID_H__
#define __C_GRID_H__

#include <vector>
#include <cstddef>

class CGrid
{
    public:
    
                    CGrid       (std::size_t WIDTH, std::size_t HEIGHT);
        void        setPixel    (const std::size_t X, const std::size_t Y, const bool ENABLE);
        void        setPixels   (const std::size_t LEFT, const std::size_t TOP, const std::size_t RIGHT, const std::size_t BOTTOM, const bool ENABLE);
        void        shiftX      (const std::size_t ROW, const int64_t SHIFT, const bool WRAP);
        void        shiftY      (const std::size_t COL, const int64_t SHIFT, const bool WRAP);
        void        print       (const char OFF, const char ON) const;
        std::size_t count       (void) const;

    private:

        std::vector< std::vector<bool> >    m_kGrid;
};

#endif // __C_GRID_H__