#include <iostream>
#include <fstream>
#include <string>

bool isNicePart1(const std::string& kName)
{
    const std::string VOWELS        = "aeiou";
    const std::string BAD_WORDS[]   = {"ab", "cd", "pq", "xy"};
    const std::size_t NEEDED_VOWELS = 3;
    std::size_t       nVowelCount   = 0;
    bool              bRepeat       = false;

    // Eliminate the Bad Words
    for (std::size_t i = 0; i < (sizeof(BAD_WORDS) / sizeof(const std::string)); ++i)
    {
        if (kName.find(BAD_WORDS[i]) != std::string::npos)
        {
            return false;
        }
    }

    // Find Repetitions and Vowels
    for (std::string::const_iterator it = kName.cbegin(); it != kName.cend(); ++it)
    {
        if (nVowelCount < NEEDED_VOWELS)
        {
            if (VOWELS.find(*it) != std::string::npos)
            {
                ++nVowelCount;
            }
        }

        if (!bRepeat)
        {
            if (it != kName.cbegin())
            {
                if (*it == *(it-1))
                {
                    bRepeat = true;
                }
            }
        }

        if ((nVowelCount >= NEEDED_VOWELS) && bRepeat)
        {
            break;
        }
    }

    return (nVowelCount >= NEEDED_VOWELS) && bRepeat;
}

bool isNicePart2(const std::string& kName)
{
    // Repeated Words
    bool bFound = false;
    for (std::size_t i = 0; !bFound && (i < kName.size()); ++i)
    {
        for (std::size_t j = i + 2; !bFound && (j < kName.size()); ++j)
        {
            bFound = kName.substr(i,2) == kName.substr(j,2);
        }
    }

    // Sandwiched Letter
    if (bFound)
    {
        bFound = false;
        for (std::size_t i = 1; !bFound && (i < (kName.size()-1)); ++i)
        {
            bFound = kName.at(i-1) == kName.at(i+1);
        }
    }

    return bFound;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        unsigned long nPart1 = 0;
        unsigned long nPart2 = 0;

        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (isNicePart1(kLine)) ++nPart1;
            if (isNicePart2(kLine)) ++nPart2;
        }

        std::cout << "Part 1: " << nPart1 << std::endl;
        std::cout << "Part 2: " << nPart2 << std::endl;
    }

    return 0;
}
