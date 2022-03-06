#ifndef __CPOSITION_H__
#define __CPOSITION_H__

#include <cstddef>
#include <cinttypes>

class CPosition
{
    public:

        struct PointType
        {
            int64_t posX;
            int64_t posY;
        };

                            CPosition           ();
                            CPosition           (const int64_t x, const int64_t y);
                            CPosition           (const PointType& pos);
        void                setPos              (const int64_t x, const int64_t y);
        void                setPos              (const PointType& pos);
        const PointType&    getPos              () const;

        void                clamp               (const int64_t left, const int64_t top, const int64_t right, const int64_t bottom);

        CPosition           operator+           (const CPosition& pos) const;
        CPosition           operator+           (const int64_t    n) const;

        CPosition           operator*           (const CPosition& pos) const;
        CPosition           operator*           (const int64_t    n) const;

        CPosition&          operator+=          (const CPosition& pos);
        CPosition&          operator+=          (const int64_t    n);

        CPosition&          operator*=          (const CPosition& pos);
        CPosition&          operator*=          (const int64_t    n);

        bool                operator<           (const CPosition& pos) const;

        std::size_t         manhattenDistance   () const;

    private:

        PointType   m_position;
};

#endif
