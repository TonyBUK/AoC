#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

struct SConversionRange
{
    std::uint64_t nDestinationStart;
    std::uint64_t nSourceStart;
    std::uint64_t nRange;
};

struct SSeedRange
{
    std::uint64_t nStart;
    std::uint64_t nRange;

    bool operator<(const SSeedRange& kOther) const
    {
        return nStart < kOther.nStart;
    }
};

class CAlmanacEntry
{
    public:

             CAlmanacEntry  ();
        void AddRange       (const SConversionRange& kRange);
        void MapRange       (std::vector<SSeedRange>& kRanges) const;
        void MapRange       (std::uint64_t& nSeed) const;

    private:

        std::vector<SConversionRange> m_kRanges;
};

CAlmanacEntry::CAlmanacEntry()
{
}

void CAlmanacEntry::AddRange(const SConversionRange& kRange)
{
    m_kRanges.push_back(kRange);
}

void CAlmanacEntry::MapRange(std::vector<SSeedRange>& kRanges) const
{
    std::vector<SSeedRange> kRangesToProcess = kRanges;
    std::vector<SSeedRange> kSkippedRanges;
    bool                    bAny;

    kRanges.clear();

    do
    {
        bAny = false;
        for (std::vector<SSeedRange>::const_iterator itSeed = kRangesToProcess.cbegin(); itSeed != kRangesToProcess.cend(); ++itSeed)
        {
            bool bAnyThisPass = false;
            for (std::vector<SConversionRange>::const_iterator itConversion = m_kRanges.cbegin(); itConversion != m_kRanges.cend(); ++itConversion)
            {
                // Detect Overlap
                if (((itSeed->nStart + itSeed->nRange - 1) >= itConversion->nSourceStart) &&
                    (itSeed->nStart                        <  (itConversion->nSourceStart + itConversion->nRange)))
                {
                    // Check for any non-overlapped parts of the Range
                    std::uint64_t nStart = itSeed->nStart;
                    std::uint64_t nRange = itSeed->nRange;

                    // Pre
                    if (itSeed->nStart < itConversion->nSourceStart)
                    {
                        const SSeedRange kRange = { itSeed->nStart, itConversion->nSourceStart - itSeed->nStart };
                        kSkippedRanges.push_back(kRange);
                        nStart = itConversion->nSourceStart;
                    }

                    // Post
                    if ((itSeed->nStart + itSeed->nRange) > (itConversion->nSourceStart + itConversion->nRange))
                    {
                        const SSeedRange kRange = { itConversion->nSourceStart + itConversion->nRange, (itSeed->nStart + itSeed->nRange - 1) - (itConversion->nSourceStart + itConversion->nRange) };
                        kSkippedRanges.push_back(kRange);
                        nRange = itConversion->nSourceStart + itConversion->nRange - nStart;
                    }

                    // Middle
                    const SSeedRange kRange = { nStart - itConversion->nSourceStart + itConversion->nDestinationStart, nRange };
                    kRanges.push_back(kRange);

                    bAny         = true;
                    bAnyThisPass = true;

                    // We can overlap multiple ranges so we need to be very careful about preventing
                    // the same range being processed multiple times.
                    // The danger of doing so is that we could end up processing part of a range as being mapped
                    // and then later on process the same range as being unmapped.  So we split now, and process
                    // the split parts in another pass.
                    break;
                }
            }

            if (!bAnyThisPass)
            {
                kRanges.push_back(*itSeed);
            }
        }

        if (bAny)
        {
            if (kSkippedRanges.size() == 0)
            {
                break;
            }
            kRangesToProcess = kSkippedRanges;
            kSkippedRanges.clear();
        }
    } while (bAny);

    kRanges.insert(kRanges.end(), kSkippedRanges.begin(), kSkippedRanges.end());
}

void CAlmanacEntry::MapRange(std::uint64_t& nSeed) const
{
    // Very iniefficent, but it does funnel all the functionality into
    // a single method.
    std::vector<SSeedRange> kSeeds;
    SSeedRange              kSeedRange = { nSeed, 1 };
    kSeeds.push_back(kSeedRange);

    MapRange(kSeeds);

    assert(kSeeds.size() == 1);
    assert(kSeeds.front().nRange == 1);

    nSeed = kSeeds.front().nStart;
}

std::vector<std::string> split(const std::string& kString, const std::string& kSeperators, const bool bIncludeSeparators)
{
    std::vector<std::string> kOutput;
    std::string::size_type   nPrevPos = 0, nPos = 0;

    while((nPos = kString.find_first_of(kSeperators, nPos)) != std::string::npos)
    {
        std::string kSubString( kString.substr(nPrevPos, nPos-nPrevPos) );
        kOutput.push_back(kSubString);

        if (bIncludeSeparators)
        {
            kOutput.push_back(std::string(1, kString.at(nPos)));
        }

        nPrevPos = ++nPos;
    }
    kOutput.push_back(kString.substr(nPrevPos, nPos-nPrevPos)); // Last word
    return kOutput;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<CAlmanacEntry>  kAlamanacs;
        std::vector<std::uint64_t>  kSeeds;
        std::vector<SSeedRange>     kSeedRanges;
        bool                        bSeeds           = true;
        bool                        bNewAlmanacEntry = false;

        // Store the Almanacs/Seeds/Seed Ranges
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                if (bSeeds)
                {
                    const std::vector<std::string> kTokens = split(kLine, " ", false);

                    for (std::vector<std::string>::const_iterator it = kTokens.begin() + 1; it != kTokens.end(); ++it)
                    {
                        kSeeds.push_back(std::stoull(*it));
                        if (0 == (kSeeds.size() % 2))
                        {
                            const SSeedRange kRange = { kSeeds[kSeeds.size()-2], kSeeds[kSeeds.size()-1] };
                            kSeedRanges.push_back(kRange);
                        }
                    }

                    assert((kSeedRanges.size() * 2) == kSeeds.size());

                    bSeeds = false;
                }
                else if (bNewAlmanacEntry)
                {
                    bNewAlmanacEntry = false;
                    kAlamanacs.push_back(CAlmanacEntry());
                }
                else
                {
                    const std::vector<std::string> kTokens = split(kLine, " ", false);
                    assert(kTokens.size() == 3);
                    const SConversionRange kRange = { std::stoull(kTokens[0]), std::stoull(kTokens[1]), std::stoull(kTokens[2]) };
                    kAlamanacs.back().AddRange(kRange);
                }
            }
            else
            {
                bNewAlmanacEntry = true;
            }
        }

        // Solve Both Parts
        for (std::vector<CAlmanacEntry>::const_iterator it = kAlamanacs.cbegin(); it != kAlamanacs.cend(); ++it)
        {
            for (std::vector<std::uint64_t>::iterator itSeed = kSeeds.begin(); itSeed != kSeeds.end(); ++itSeed)
            {
                it->MapRange(*itSeed);
            }
            it->MapRange(kSeedRanges);
        }

        std::cout << "Part 1: " << *std::min_element(kSeeds.begin(), kSeeds.end())                  << std::endl;
        std::cout << "Part 2: " << std::min_element(kSeedRanges.begin(), kSeedRanges.end())->nStart << std::endl;
   }

    return 0;
}
