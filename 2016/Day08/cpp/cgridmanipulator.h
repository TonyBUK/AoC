#ifndef __C_GRID_MANIPULATOR_H__
#define __C_GRID_MANIPULATOR_H__

#include "cgrid.h"

class CGridManipulator
{
    public:

                CGridManipulator    (const std::string& RAW);
        void    Process             (CGrid& kGrid) const;

    private:

        enum CommandType {E_NONE, E_RECT /* No Laughing! */, E_ROTATE_ROW, E_ROTATE_COLUMN};

        CommandType                 m_eCommand;
        std::vector<std::size_t>    m_kParameters;
};

#endif // __C_GRID_MANIPULATOR_H__