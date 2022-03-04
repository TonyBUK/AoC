#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>
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

std::size_t triangleNumber(const size_t n)
{
    return (n * (n+1)) / 2;
}

std::size_t getCodeAlgorithmically(const std::size_t nRow, const std::size_t nCol, const std::size_t nInitial)
{
    std::size_t nCurrentRow = 1;
    std::size_t nCurrentCol = 1;
    std::size_t nCurrent    = nInitial;

    while ((nRow != nCurrentRow) || (nCol != nCurrentCol))
    {
        if (1 == nCurrentRow)
        {
            nCurrentRow = nCurrentCol + 1;
            nCurrentCol = 1;
        }
        else
        {
            --nCurrentRow;
            ++nCurrentCol;
        }
        nCurrent = (nCurrent * 252533) % 33554393;
    }
    return nCurrent;
}

std::size_t getCodeHybrid(const std::size_t nRow, const std::size_t nCol, const std::size_t nInitial)
{
    const std::size_t nIterations = triangleNumber(nRow + nCol - 2) + nCol - 1;
    std::size_t nCurrent    = nInitial;
    for (std::size_t i = 0; i < nIterations; ++i)
    {
        nCurrent = (nCurrent * 252533) % 33554393;
    }
    return nCurrent;
}

std::size_t modPow(std::size_t nBase, std::size_t nExp, std::size_t nModulus)
{
  std::size_t nResult = 1;
  nBase %= nModulus;
  while (nExp > 0)
  {
    if (nExp & 1)
    {
        nResult = (nResult * nBase) % nModulus;
    }
    nBase = (nBase * nBase) % nModulus;
    nExp >>= 1;
  }
  return nResult;
}

std::size_t getCodeMathematically(const std::size_t nRow, const std::size_t nCol, const std::size_t nInitial)
{
    const std::size_t nIterations = triangleNumber(nRow + nCol - 2) + nCol - 1;
    return (modPow(252533, nIterations, 33554393) * nInitial) % 33554393;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::size_t nRow;
        std::size_t nCol;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            std::size_t nPos;
            do
            {
                nPos = kLine.find_first_of(",.");
                if (nPos != std::string::npos)
                {
                    kLine.erase(kLine.begin()+nPos);
                }
            } while (nPos != std::string::npos);

            std::vector<std::string> kTokens = split(kLine, " ");
            assert(kTokens.size() == 19);
            nRow = std::stoull(kTokens.at(16));
            nCol = std::stoull(kTokens.at(18));
        }

        std::cout << "Part 1 (Algo): " << getCodeAlgorithmically(nRow, nCol, 20151125) << std::endl;
        std::cout << "Part 1 (Hyb) : " << getCodeHybrid         (nRow, nCol, 20151125) << std::endl;
        std::cout << "Part 1 (Math): " << getCodeMathematically (nRow, nCol, 20151125) << std::endl;
   }
    std::cout << "Ho ho ho!" << std::endl;

    return 0;
}
