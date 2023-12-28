#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <assert.h>

struct SDigitsType
{
    std::string   kDigit;
    std::uint64_t nDigit;
};

std::uint64_t countDigits(const std::string& kDigits, const std::vector<SDigitsType>& kDigitsVector, const std::vector<char>& kFirstOf)
{
    std::uint64_t       nFirst      = 0;
    std::uint64_t       nLast       = 0;
    bool                bFirst      = true;
    std::size_t         nPos        = 0;

    for (nPos = 0; nPos < kDigits.length(); ++nPos)
    {
        const std::size_t nTestPos = kDigits.find_first_of(kFirstOf.data(), nPos);
        if (std::string::npos == nTestPos)
        {
            break;
        }
        nPos = nTestPos;

        for (const auto& kDigit : kDigitsVector)
        {
            if (kDigits.find(kDigit.kDigit, nPos) == nPos)
            {
                if (bFirst)
                {
                    nFirst = kDigit.nDigit;
                    bFirst = false;
                }
                nLast = kDigit.nDigit;
                break;
            }
        }
    }

    return (nFirst * 10) + nLast;
}

int main(int argc, char** argv)
{
    const std::array<SDigitsType,9> kDigitsArray = {
        {{"1", 1}, {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}}
    };

    const std::array<SDigitsType,18> kDigitsArrayWithWords = {
        {{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9},
         {"1", 1}, {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}}
    };
    const std::vector<SDigitsType> kDigitsVector(kDigitsArray.begin(), kDigitsArray.end());
    const std::vector<SDigitsType> kDigitsVectorWithWords(kDigitsArrayWithWords.begin(), kDigitsArrayWithWords.end());

    // Do a quick optimisation, we can derive sensible search areas by ignoring anything that
    // can't be any of the search terms.
    std::vector<char> kFirstOfPartOne;
    for (const auto& kDigit : kDigitsVector)
    {
        kFirstOfPartOne.push_back(kDigit.kDigit[0]);
    }

    std::vector<char> kFirstOfPartTwo;
    for (const auto& kDigit : kDigitsVectorWithWords)
    {
        kFirstOfPartTwo.push_back(kDigit.kDigit[0]);
    }

    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::uint64_t nCalibrationSumPartOne = 0;
        std::uint64_t nCalibrationSumPartTwo = 0;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);
            if (kLine.length())
            {
                nCalibrationSumPartOne += countDigits(kLine, kDigitsVector, kFirstOfPartOne);
                nCalibrationSumPartTwo += countDigits(kLine, kDigitsVectorWithWords, kFirstOfPartTwo);
            }
        }

        std::cout << "Part 1: " << nCalibrationSumPartOne << std::endl;
        std::cout << "Part 2: " << nCalibrationSumPartTwo << std::endl;
   }

    return 0;
}
