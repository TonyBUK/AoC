#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cinttypes>
#include <cmath>
#include <assert.h>
#include "cposition.h"

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
    E_UP    = 0,
    E_RIGHT,
    E_DOWN,
    E_LEFT
};

const CPosition::PointType& getVector(const DirectionType eDirection)
{
    static const CPosition::PointType kVectors[] =
    {
        { 0, -1},   // E_UP
        { 1,  0},   // E_RIGHT
        { 0,  1},   // E_DOWN
        {-1,  0}    // E_LEFT
    };
    return kVectors[eDirection];
}

std::string getPasscode(const std::vector<std::string>& kKeypad, const std::vector< std::vector<DirectionType> >& kMoveSet, const char cStart)
{
    CPosition   kPosition(0,0);

    // Find the Start Position, and extract the Height/Width of the Keypad
    bool        bFound    = false;

    const std::size_t nHeight = kKeypad.size();
    std::size_t       nWidth;

    for (std::vector<std::string>::const_iterator itY = kKeypad.cbegin(); !bFound && (itY != kKeypad.cend()); ++itY)
    {
        const std::string& kRow = *itY;
        nWidth                  = kRow.size();

        for (std::string::const_iterator itX = kRow.cbegin(); itX != kRow.cend(); ++itX)
        {
            if (cStart == *itX)
            {
                kPosition.setPos(itX - kRow.cbegin(), itY - kKeypad.cbegin());
                bFound    = true;
                break;
            }
        }
    }

    // Make sure we actually found the start position...
    assert(bFound);

    // Initialise the Passcode
    std::string kPasscode = "";

    // For each set of moves...
    for (std::vector< std::vector<DirectionType> >::const_iterator itMoves = kMoveSet.cbegin(); itMoves != kMoveSet.cend(); ++itMoves)
    {
        const std::vector<DirectionType>& kCurrentMoveset = *itMoves;

        // Process all moves in the current set
        for (std::vector<DirectionType>::const_iterator itCurrentMove = kCurrentMoveset.cbegin(); itCurrentMove != kCurrentMoveset.cend(); ++itCurrentMove)
        {
            // Store the potential new position
            CPosition kCandidatePosition = kPosition + getVector(*itCurrentMove);
            
            // Ensure the new position is on the Keypad Grid
            kCandidatePosition.clamp(0, 0, nWidth-1, nHeight-1);
            
            // Ensure the new position is on the Keypad Itself
            const CPosition::PointType& kPoint = kCandidatePosition.getPos();
            if (kKeypad.at(kPoint.posY).at(kPoint.posX) != '.')
            {
                kPosition = kCandidatePosition;
            }
        }

        // Add the current keypad value to the Passcode
        const CPosition::PointType& kPoint = kPosition.getPos();
        kPasscode += kKeypad.at(kPoint.posY).at(kPoint.posX);
    }

    return kPasscode;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<std::string> kStandardKeypad;
        kStandardKeypad.push_back("123");
        kStandardKeypad.push_back("456");
        kStandardKeypad.push_back("789");

        std::vector<std::string> kBathroomKeypad;
        kBathroomKeypad.push_back("..1..");
        kBathroomKeypad.push_back(".234.");
        kBathroomKeypad.push_back("56789");
        kBathroomKeypad.push_back(".ABC.");
        kBathroomKeypad.push_back("..D..");

        std::vector< std::vector<DirectionType> > kMoveSet;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            std::vector<DirectionType> kCurrentMoveSet;

            for (std::string::const_iterator it = kLine.cbegin(); it != kLine.cend(); ++it)
            {
                switch(*it)
                {
                    case 'U':
                    {
                        kCurrentMoveSet.push_back(E_UP);
                    } break;

                    case 'R':
                    {
                        kCurrentMoveSet.push_back(E_RIGHT);
                    } break;

                    case 'D':
                    {
                        kCurrentMoveSet.push_back(E_DOWN);
                    } break;

                    case 'L':
                    {
                        kCurrentMoveSet.push_back(E_LEFT);
                    } break;

                    default:
                    {
                        assert(false);
                    }
                }
            }

            kMoveSet.push_back(kCurrentMoveSet);
        }

        std::cout << "Part 1: " << getPasscode(kStandardKeypad, kMoveSet, '5') << std::endl;
        std::cout << "Part 2: " << getPasscode(kBathroomKeypad, kMoveSet, '5') << std::endl;
   }

    return 0;
}
