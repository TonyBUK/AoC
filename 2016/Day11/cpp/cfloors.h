#ifndef __CFLOORS_H__
#define __CFLOORS_H__

#include <string>
#include <set>
#include <map>
#include <vector>

class CFloors
{
    public:

        enum ItemType {E_GENERATOR = 0, E_MICROCHIP};

                        CFloors (const std::size_t nFloorCount);
        void            Add     (const std::size_t nGeneratorFloor, const std::size_t nMicrochipFloor, const std::string& kName);
        void            Add     (const std::size_t nFloor, const ItemType eType, const std::string& kName);
        std::size_t     Solve   (void);

    private:

        struct FloorType
        {
            std::set<std::string> kGenerators;
            std::set<std::string> kMicrochips;
        };

        struct FloorStateType
        {
            std::size_t                             nFloor;
            std::vector< std::vector<std::size_t> > kGenerators;
            std::vector< std::vector<std::size_t> > kMicrochips;

            bool operator<(const FloorStateType &x) const
            {
                if (nFloor < x.nFloor)
                {
                    return true;
                }
                else if (nFloor == x.nFloor)
                {
                    if (kGenerators < x.kGenerators)
                    {
                        return true;
                    }
                    else if (kGenerators == x.kGenerators)
                    {
                       return kMicrochips < x.kMicrochips;
                    }
                }
                return false;
            }
        };

        struct FloorProcessType
        {
            std::string kElement;
            ItemType    eType;
        };

        std::size_t     Solve   (const std::size_t nFloor, const std::size_t nDepth, const std::size_t nBestSolution, std::map<FloorStateType, std::size_t>& kCache);
        bool            Valid   (void) const;
        bool            Solved  (void) const;
        void            Move    (const std::size_t nSourceFloor, const std::size_t nTargetFloor, const FloorProcessType& kItem);
        std::size_t     Min     (void) const;
        FloorStateType  State   (const std::size_t nFloor) const;
        void            State   (const std::set<std::string>& kItems, const ItemType eType, std::vector<std::size_t>& kState) const;
        std::size_t     Find    (const std::string& kName, const ItemType eType) const;
        void            Add     (const std::set<std::string>& kItems, const std::set<std::string>& kIgnoreList, const ItemType eType, std::vector<FloorProcessType>& kFloorLookup) const;

        std::vector<FloorType> m_kFloors;
};

#endif // __CFLOORS_H__