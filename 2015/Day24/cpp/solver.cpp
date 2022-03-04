#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <cinttypes>
#include <assert.h>

#define MAX_SIZE_T static_cast<std::size_t>(static_cast<uint64_t>(-1))

std::size_t calculateQEFromMask(const std::vector<std::size_t>& kPackages, const std::size_t nMask)
{
    std::size_t nProduct = 1;

    for (std::size_t i = 0; i < kPackages.size(); ++i)
    {
        if ((1 << i) & nMask)
        {
            nProduct *= kPackages.at(i);
        }
    }
    return nProduct;
}

std::size_t calculateCountFromMask(const std::size_t nMask)
{
    std::size_t nLength    = 0;
    std::size_t nLocalMask = nMask;

    while (nLocalMask > 0)
    {
        if (nLocalMask & 1)
        {
            ++nLength;
        }
        nLocalMask /= 2;
    }
    return nLength;
}

struct PermutationType
{
    std::size_t nCount;
    std::size_t nQuantumEntanglement;
    std::size_t nMask;
};

void getPermutationIndexes(const std::vector<std::size_t>& kPackages, const std::size_t nTargetWeight, std::vector<PermutationType>& kPermutations, const std::size_t nCurrentIndex = 0, const std::size_t nCurrentWeight = 0, const std::size_t nCurrentMask = 0)
{
    for (std::size_t i = nCurrentIndex; i < kPackages.size(); ++i)
    {
        const std::size_t nDeltaMask     = 1 << i;
        const std::size_t nPackageWeight = kPackages.at(i);

        if ((nCurrentWeight + nPackageWeight) < nTargetWeight)
        {
            // Recurse...
            getPermutationIndexes(kPackages, nTargetWeight, kPermutations, i + 1, nCurrentWeight + nPackageWeight, nCurrentMask | nDeltaMask);
        }
        else if ((nCurrentWeight + nPackageWeight) == nTargetWeight)
        {
            const std::size_t nNewMask = nCurrentMask | nDeltaMask;

            // Store the Quantity, Quantum Entanglement and Mask
            // This allows for the packages to be sorted, which means the first viable solution
            // found when iterating will *always* have the best package in terms of Quantity/QE.
            PermutationType kPermutation = {calculateCountFromMask(nNewMask), calculateQEFromMask(kPackages, nNewMask), nNewMask};
            kPermutations.push_back(kPermutation);

            // The Weights are sorted so there's no more possible solutions at this depth
            break;
        }
        else
        {
            // The Weights are sorted so there's no more possible solutions at this depth
            break;
        }
    }
}

bool comparePermutations(const PermutationType& a, const PermutationType& b)
{
    // smallest comes first
    if (a.nCount < b.nCount)
    {
        return true;
    }
    else if (a.nCount == b.nCount)
    {
        return a.nQuantumEntanglement < b.nQuantumEntanglement;
    }
    return false;
}

std::size_t getLowestQE(const std::vector<PermutationType>& kPermutations, const std::size_t nCompartmentCount, const std::size_t nLowestQE = MAX_SIZE_T, const std::size_t nCurrentIndex = 0, const std::size_t nCombinedMask = 0, const std::size_t nDepth = 1, const std::size_t nCandidateQE = 0)
{
    std::size_t nLocalLowestQE = nLowestQE;

    for (std::size_t i = nCurrentIndex; i < kPermutations.size(); ++i)
    {
        // An easy way of telling whether two masks have no bits in common is to compare the OR to the ADD.
        // If there's no bits in common, the result should be equal, if not, then they share bits.
        const std::size_t nDeltaMask = kPermutations.at(i).nMask | nCombinedMask;
        if (nDeltaMask == (kPermutations.at(i).nMask + nCombinedMask))
        {
            if (nDepth == nCompartmentCount)
            {
                return nCandidateQE;
            }
            else
            {
                nLocalLowestQE = getLowestQE(kPermutations, nCompartmentCount, nLocalLowestQE, i+1, nDeltaMask, nDepth + 1, (nDepth == 1) ? kPermutations[i].nQuantumEntanglement : nCandidateQE);
                if (nLocalLowestQE != MAX_SIZE_T)
                {
                    return nLocalLowestQE;
                }
            }
        }
    }

    return nLocalLowestQE;
}

std::size_t getPermutationWithLowestQE(const std::vector<std::size_t>& kPackages, const std::size_t nCompartmentCount)
{
    const std::size_t               nGroupTarget    = std::accumulate(kPackages.cbegin(), kPackages.cend(), 0ull) / nCompartmentCount;
    std::vector<PermutationType>    kPermutations;

    getPermutationIndexes(kPackages, nGroupTarget, kPermutations);
    std::sort(kPermutations.begin(), kPermutations.end(), comparePermutations);

    return getLowestQE(kPermutations, nCompartmentCount);
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<std::size_t> kPackages;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            kPackages.push_back(std::stoull(kLine));
        }

        std::sort(kPackages.begin(), kPackages.end());

        std::cout << "Part 1: " << getPermutationWithLowestQE(kPackages, 3) << std::endl;
        std::cout << "Part 2: " << getPermutationWithLowestQE(kPackages, 4) << std::endl;
   }

    return 0;
}
