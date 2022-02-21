#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cinttypes>
#include <assert.h>

#define MAX_SIZE_T static_cast<std::size_t>(static_cast<uint64_t>(-1))

std::vector<std::string> split(const std::string& s, std::string seperator)
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

void getPermutations(const std::string& kMedicineMolecule, const std::vector< std::vector<std::string> >& kModuleFabricationRules, std::map<std::string, std::size_t>& kPermutations)
{
    const std::size_t nMedicineMolecueLen = kMedicineMolecule.length();
    kPermutations.clear();

    // Reserve enough memory to prevent this function spamming allocations
    std::string kTemp;
    kTemp.reserve(nMedicineMolecueLen*2);

    // For each rule
    for (std::vector< std::vector<std::string> >::const_iterator it = kModuleFabricationRules.cbegin(); it != kModuleFabricationRules.cend(); ++it)
    {
        // Replace all instances of the substring...
        const std::string& kSearch     = it->front();
        const std::size_t  kSearchLen  = kSearch.length();
        const std::string& kSubstitute = it->back();
        std::size_t nNewIndex = kMedicineMolecule.find(kSearch);
        while (nNewIndex != std::string::npos)
        {
            // Extract the Text either side of the current substition
            kTemp.assign(kMedicineMolecule.substr(0, nNewIndex));
            kTemp.append(kSubstitute);
            kTemp.append(kMedicineMolecule.substr(nNewIndex + kSearchLen, nMedicineMolecueLen));

            // Add the Substitution to the unique list of Substituted Strings
            kPermutations[kTemp] = 0;

            // Attempt to find the next substitution...
            nNewIndex = kMedicineMolecule.find(kSearch, nNewIndex + 1);
        }
    }
}

std::size_t findFavouriteMolecule(const std::map<std::string, std::size_t>& kCurrentMolecules, const std::vector< std::vector<std::string> >& kModuleFabricationRules, const std::string& kFavouriteMolecule, const std::size_t nDepth = 1, const std::size_t nBestDepth = MAX_SIZE_T)
{
    // Note: We could work our way forwards, parsing all possible sets at each level until
    //       we reach an answer, but this soon becomes a "heat death of the universe" style
    //       task where each new depth exponentially adds more permutations.
    //       Instead, if we start from the end and work backwards.  The fact the solutions are
    //       reductive helps reduce the overhead of permutations.
    std::map<std::string, std::size_t> kNewMolecules;
    std::size_t                        nBestDepthLocal = nBestDepth;

    for (std::map<std::string, std::size_t>::const_iterator it = kCurrentMolecules.cbegin(); it != kCurrentMolecules.cend(); ++it)
    {
        getPermutations(it->first, kModuleFabricationRules, kNewMolecules);

        if (kNewMolecules.find(kFavouriteMolecule) != kNewMolecules.end())
        {
            return nDepth;
        }
        else if ((nDepth + 1) < nBestDepthLocal)
        {
            // Rather than just randomly iterating through the solutions, sort them by length and parse
            // each group separately...
            std::vector<std::size_t>           kNewMoleculesSizes;
            for (std::map<std::string, std::size_t>::const_iterator itNew = kNewMolecules.cbegin(); itNew != kNewMolecules.cend(); ++itNew)
            {
                const std::string& kNewMolecule = itNew->first;
                if (std::find(kNewMoleculesSizes.begin(), kNewMoleculesSizes.end(), kNewMolecule.length()) == kNewMoleculesSizes.end())
                {
                    kNewMoleculesSizes.push_back(kNewMolecule.length());
                }
            }

            // Parse each group from shortest to longest
            std::sort(kNewMoleculesSizes.begin(), kNewMoleculesSizes.end());
            for (std::vector<std::size_t>::const_iterator itSize = kNewMoleculesSizes.cbegin(); itSize != kNewMoleculesSizes.cend(); ++itSize)
            {
                std::map<std::string, std::size_t> kNewMoleculesSizeSorted;
                for (std::map<std::string, std::size_t>::const_iterator itNew = kNewMolecules.cbegin(); itNew != kNewMolecules.cend(); ++itNew)
                {
                    const std::string& kNewMolecule = itNew->first;
                    if (kNewMolecule.length() == *itSize)
                    {
                        kNewMoleculesSizeSorted[kNewMolecule] = 0;
                    }
                    nBestDepthLocal = findFavouriteMolecule(kNewMoleculesSizeSorted, kModuleFabricationRules, kFavouriteMolecule, nDepth + 1, nBestDepthLocal);

                    // Assume that once we've found any solution, it's the best one.
                    // Whether this stands up to scrutiny or not for other inputs is something
                    // I'd be interested to find out...
                    if (nBestDepthLocal != MAX_SIZE_T)
                    {
                        return nBestDepthLocal;
                    }
                }
            }
        }
    }

    return nBestDepthLocal;
}

void swapRules(std::vector< std::vector<std::string> >& kModuleFabricationRules)
{
    // For each rule
    for (std::vector< std::vector<std::string> >::iterator it = kModuleFabricationRules.begin(); it != kModuleFabricationRules.end(); ++it)
    {
        it->front().swap(it->back());

        // This assumes that we are always reductive, so assert that theory
        assert(it->front().length() >= it->back().length());
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        // Containers
        std::vector< std::vector<std::string> > kModuleFabricationRules;
        std::string                             kMedicineMolecule;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine != "")
            {
                kModuleFabricationRules.push_back(split(kLine, " => "));
            }
            else
            {
                std::getline(kFile, kMedicineMolecule);
            }
        }

        std::map<std::string, std::size_t> kPermutations;
        getPermutations(kMedicineMolecule, kModuleFabricationRules, kPermutations);
        std::cout << "Part 1: " << kPermutations.size() << std::endl;

        kPermutations.clear();
        kPermutations[kMedicineMolecule] = 0;
        swapRules(kModuleFabricationRules);
        std::cout << "Part 2: " << findFavouriteMolecule(kPermutations, kModuleFabricationRules, "e") << std::endl;
   }

    return 0;
}
