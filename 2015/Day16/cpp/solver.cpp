#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>

std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

std::size_t whichSue(const std::map<std::string, std::size_t>&                         kWrappingPaperCompounds,
                     const std::map<std::size_t, std::map<std::string, std::size_t> >& kSues,
                     const std::vector<std::string>&                                   kGreaterThan = std::vector<std::string>(),
                     const std::vector<std::string>&                                   kFewerThan   = std::vector<std::string>())
{
    // Iterate over each Sue
    for (std::map<std::size_t, std::map<std::string, std::size_t> >::const_iterator itSue = kSues.cbegin(); itSue != kSues.cend(); ++itSue)
    {
        const std::map<std::string, std::size_t>& kSue = itSue->second;

        // Initially assume this is our Sue
        bool bValidSue = true;

        // Iterate over each compound you remember about Sue
        for (std::map<std::string, std::size_t>::const_iterator itCompound = kSue.cbegin(); bValidSue && (itCompound != kSue.cend()); ++itCompound)
        {
            const std::string& kCompound = itCompound->first;
            const std::size_t& nQuantity = itCompound->second;

            // This should always be the case...
            assert(kWrappingPaperCompounds.find(kCompound) != kWrappingPaperCompounds.cend());

            // Determine how to compare the compound
            // Greater Than, Fewer Than or Equal To
            const bool bGreaterThan = std::find(kGreaterThan.cbegin(), kGreaterThan.cend(), kCompound) != kGreaterThan.cend();
            const bool bFewerThan   = std::find(kFewerThan.cbegin(),   kFewerThan.cend(),   kCompound) != kFewerThan.cend();

            if (bGreaterThan)
            {
                bValidSue = nQuantity > kWrappingPaperCompounds.at(kCompound);
            }
            else if (bFewerThan)
            {
                bValidSue = nQuantity < kWrappingPaperCompounds.at(kCompound);
            }
            else
            {
                bValidSue = nQuantity == kWrappingPaperCompounds.at(kCompound);
            }
        }

        // If everything we remembered about Sue matches, we've found our Sue!
        if (bValidSue)
        {
            return itSue->first;
        }
    }

    // Shouldn't get here unless all the Sue's were wrong?!?
    assert(false);
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        // Create the Wrapping Paper Compounds
        std::map<std::string, std::size_t> kWrappingPaper;
        kWrappingPaper["children"]      = 3;
        kWrappingPaper["cats"]          = 7;
        kWrappingPaper["samoyeds"]      = 2;
        kWrappingPaper["pomeranians"]   = 3;
        kWrappingPaper["akitas"]        = 0;
        kWrappingPaper["vizslas"]       = 0;
        kWrappingPaper["goldfish"]      = 5;
        kWrappingPaper["trees"]         = 3;
        kWrappingPaper["cars"]          = 2;
        kWrappingPaper["perfumes"]      = 1;

        // Sues
        std::map<std::size_t, std::map<std::string, std::size_t> > kSues;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);
            while (kLine.back() == '.')
            {
                kLine.pop_back();
            }

            // Extract Sue
            std::vector<std::string> kSueMemory = split(kLine, ' ');
            std::map<std::string, std::size_t> kSue;

            for (std::size_t i = 2; i < kSueMemory.size(); i += 2)
            {
                if (':' == kSueMemory.at(i).back())
                {
                    kSueMemory.at(i).pop_back();
                }
                assert(kSue.find(kSueMemory.at(i)) == kSue.end());
                kSue[kSueMemory.at(i)] = std::stoull(kSueMemory.at(i+1));
            }

            // Append Sue to the list of Sues
            assert(kSues.find(std::stoull(kSueMemory.at(1))) == kSues.end());
            kSues[std::stoull(kSueMemory.at(1))] = kSue;
        }

        std::vector<std::string> kGreaterThan;
        kGreaterThan.push_back("cats");
        kGreaterThan.push_back("trees");

        std::vector<std::string> kLessThan;
        kLessThan.push_back("pomeranians");
        kLessThan.push_back("goldfish");

        std::cout << "Part 1: " << whichSue(kWrappingPaper, kSues)                          << std::endl;
        std::cout << "Part 2: " << whichSue(kWrappingPaper, kSues, kGreaterThan, kLessThan) << std::endl;
    }

    return 0;
}
