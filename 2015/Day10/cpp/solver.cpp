#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <assert.h>

#define HANDLE_LARGE_REPETITIONS

#ifdef HANDLE_LARGE_REPETITIONS
void storeSeeSay(const std::size_t nRepetitionDigit, const std::size_t nRepetitionCount, std::vector<std::size_t>& kSeeSay)
{
    if (nRepetitionCount > 9)
    {
                 long nPower        = (unsigned long)floor(log10((double)nRepetitionCount));
        unsigned long nRunningTotal = nRepetitionCount;
        while (nPower >= 0)
        {
            const unsigned long nCurrentDivisor = (unsigned long)pow(10.0, nPower);
            kSeeSay.push_back(nRunningTotal / nCurrentDivisor);
            nRunningTotal = nRunningTotal % nCurrentDivisor;
            --nPower;
        }

    }
    else
    {
        kSeeSay.push_back(nRepetitionCount);
    }
    kSeeSay.push_back(nRepetitionDigit);
}
#endif

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            std::size_t              nNewBuffer = 1;
            std::vector<std::size_t> kSeeSayBuffer[2];
            for (std::string::const_iterator it = kLine.cbegin(); it != kLine.cend(); ++it)
            {
                kSeeSayBuffer[0].push_back((std::size_t)((*it) - '0'));
            }
            std::vector<std::size_t>* kSeeSay = &kSeeSayBuffer[0];

            for (std::size_t x = 1; x <= 50; ++x)
            {
                // Initialise Repetitions / Next See Say String
                std::size_t               nRepetitionDigit = kSeeSay->at(0);
                std::size_t               nRepetitionCount = 1;
                std::vector<std::size_t>& kNewSeeSay       = kSeeSayBuffer[nNewBuffer];
                kNewSeeSay.clear();

                // Skip the first digit as we already know the See/Say Number
                for (std::vector<std::size_t>::const_iterator it = kSeeSay->cbegin() + 1; it != kSeeSay->cend(); ++it)
                {
                    if (nRepetitionDigit == *it)
                    {
                        // Increment the Reptition Count
                        ++nRepetitionCount;
                    }
                    else
                    {
                        // Store the Current Repetition
#ifndef HANDLE_LARGE_REPETITIONS
                        kNewSeeSay.push_back(nRepetitionCount);
                        kNewSeeSay.push_back(nRepetitionDigit);
#else
                        storeSeeSay(nRepetitionDigit, nRepetitionCount, kNewSeeSay);
#endif

                        // Setup the next Repetition
                        nRepetitionDigit = *it;
                        nRepetitionCount = 1;
                    }
                }

                // The final repetition needs storing
#ifndef HANDLE_LARGE_REPETITIONS
                kNewSeeSay.push_back(nRepetitionCount);
                kNewSeeSay.push_back(nRepetitionDigit);
#else
                storeSeeSay(nRepetitionDigit, nRepetitionCount, kNewSeeSay);
#endif
                
                // Copy the repetition for the next cycle
                kSeeSay = &kSeeSayBuffer[nNewBuffer];

                // Part 1 - 40 Iterations
                if (40 == x)
                {
                    std::cout << "Part 1: " << kSeeSay->size()  << std::endl;
                }

                nNewBuffer = 1 - nNewBuffer;
            }

            // Part 2 - 50 Iterations
            std::cout << "Part 2: " << kSeeSay->size()  << std::endl;
       }
    }

    return 0;
}
