#include "cgridmanipulator.h"
#include "helper.h"
#include <iostream>

CGridManipulator::CGridManipulator(const std::string& RAW) :
    m_eCommand(E_NONE),
    m_kParameters()
{
    std::vector<std::string> kTokens = split(RAW, " =");

    if ("rect" == kTokens.at(0))
    {
        assert(kTokens.size() == 2);
        std::vector<std::string> kParameters = split(kTokens.at(1), "x");
        assert(kParameters.size() == 2);

        m_eCommand = E_RECT;
        m_kParameters.push_back(std::stoull(kParameters.at(0)));
        m_kParameters.push_back(std::stoull(kParameters.at(1)));
    }
    else if ("rotate" == kTokens.at(0))
    {
        assert(kTokens.size() == 6);

        if ("row" == kTokens.at(1))
        {
            m_eCommand = E_ROTATE_ROW;
            m_kParameters.push_back(std::stoull(kTokens.at(3)));
            m_kParameters.push_back(std::stoull(kTokens.at(5)));
        }
        else if ("column" == kTokens.at(1))
        {
            m_eCommand = E_ROTATE_COLUMN;
            m_kParameters.push_back(std::stoull(kTokens.at(3)));
            m_kParameters.push_back(std::stoull(kTokens.at(5)));
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
}

void CGridManipulator::Process(CGrid& kGrid) const
{
    switch (m_eCommand)
    {
        case E_RECT:
        {
            kGrid.setPixels(0, 0, m_kParameters.at(0) - 1, m_kParameters.at(1) - 1, true);
        } break;

        case E_ROTATE_ROW:
        {
            kGrid.shiftX(m_kParameters.at(0), m_kParameters.at(1), true);
        } break;

        case E_ROTATE_COLUMN:
        {
            kGrid.shiftY(m_kParameters.at(0), m_kParameters.at(1), true);
        } break;

        default: assert(false);
    }
}