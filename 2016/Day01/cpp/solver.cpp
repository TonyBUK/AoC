#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <assert.h>

std::vector<std::string> split(const std::string& s, const std::string& seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        pos     += seperator.length();
        prev_pos = pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

enum DirectionType
{
    E_NORTH  = 0,
    E_EAST,
    E_SOUTH,
    E_WEST
};

enum TurnType
{
    E_LEFT  = 0,
    E_RIGHT
};

struct MovesType
{
    TurnType    eTurn;
    std::size_t nDistance;
};

class CPosition
{
    public:

        struct PointType
        {
            int64_t posX;
            int64_t posY;
        };

                    CPosition   ();
                    CPosition   (const int64_t x, const int64_t y);
                    CPosition   (const PointType& pos);
        const PointType&  getPos      () const;

        CPosition   operator+   (const CPosition& pos) const;
        CPosition   operator+   (const int64_t    n) const;

        CPosition   operator*   (const CPosition& pos) const;
        CPosition   operator*   (const int64_t    n) const;

        CPosition&  operator+=  (const CPosition& pos);
        CPosition&  operator+=  (const int64_t    n);

        CPosition&  operator*=  (const CPosition& pos);
        CPosition&  operator*=  (const int64_t    n);

        bool        operator<   (const CPosition& pos) const;

        std::size_t manhattenDistance() const;

    private:

        PointType   m_position;
};

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

const CPosition::PointType& CPosition::getPos() const
{
    return m_position;
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

DirectionType updateDirection(const DirectionType eCurrent, const TurnType eTurn)
{
    const DirectionType DIRECTION_CHANGE[][2] =
    {
        // E_NORTH
        {
            E_WEST,     // E_LEFT
            E_EAST      // E_RIGHT
        },
        // E_EAST
        {
            E_NORTH,    // E_LEFT
            E_SOUTH     // E_RIGHT
        },
        // E_SOUTH
        {
            E_EAST,     // E_LEFT
            E_WEST      // E_RIGHT
        },
        // E_WEST
        {
            E_SOUTH,    // E_LEFT
            E_NORTH     // E_RIGHT
        }
    };

    return DIRECTION_CHANGE[eCurrent][eTurn];
}

const CPosition::PointType& getVector(const DirectionType eDirection)
{
    static const CPosition::PointType kVectors[] =
    {
        { 0,  1},   // E_NORTH
        { 1,  0},   // E_EAST
        { 0, -1},   // E_SOUTH
        {-1,  0}    // E_WEST
    };
    return kVectors[eDirection];
}

CPosition processMovesAllowRepeatedPositions(const std::vector<MovesType>& kMoves)
{
    CPosition     kPosition(0, 0);
    DirectionType eDirection = E_NORTH;

    for (std::vector<MovesType>::const_iterator it = kMoves.cbegin(); it != kMoves.cend(); ++it)
    {
        // Update the Direction
        eDirection = updateDirection(eDirection, it->eTurn);

        // Update the Position
        kPosition += (CPosition(getVector(eDirection)) * static_cast<int64_t>(it->nDistance));
    }

    return kPosition;
}

CPosition processMovesDisallowRepeatedPositions(const std::vector<MovesType>& kMoves)
{
    CPosition                   kPosition(0, 0);
    DirectionType               eDirection = E_NORTH;
    std::set<CPosition>         kVisited;
    kVisited.insert(kPosition);

    for (std::vector<MovesType>::const_iterator it = kMoves.cbegin(); it != kMoves.cend(); ++it)
    {
        // Update the Direction
        eDirection = updateDirection(eDirection, it->eTurn);

        // Update the Position
        CPosition kDelta(getVector(eDirection));

        for (std::size_t i = 0; i < it->nDistance; ++i)
        {
            kPosition += kDelta;
            if (kVisited.find(kPosition) != kVisited.end())
            {
                return kPosition;
            }
            kVisited.insert(kPosition);
        }
    }

    return kPosition;
}

CPosition processMoves(const std::vector<MovesType>& kMoves, const bool bfinishOnFirstRepetition)
{
    if (!bfinishOnFirstRepetition)
    {
        return processMovesAllowRepeatedPositions(kMoves);
    }
    else
    {
        return processMovesDisallowRepeatedPositions(kMoves);
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<MovesType> kMoves;
        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            std::vector<std::string> kTokens = split(kLine, ", ");
            for (std::vector<std::string>::const_iterator it = kTokens.cbegin(); it != kTokens.cend(); ++it)
            {
                MovesType kCurrentMove;
                const std::string& kToken = *it;
                if (kToken.at(0) == 'L')
                {
                    kCurrentMove.eTurn = E_LEFT;
                }
                else if (kToken.at(0) == 'R')
                {
                    kCurrentMove.eTurn = E_RIGHT;
                }
                else
                {
                    assert(false);
                }
                kCurrentMove.nDistance = std::stoull(kToken.substr(1));
                kMoves.push_back(kCurrentMove);
            }
        }

        std::cout << "Part 1: " << processMoves(kMoves, false).manhattenDistance() << std::endl;
        std::cout << "Part 2: " << processMoves(kMoves, true ).manhattenDistance() << std::endl;
   }

    return 0;
}
