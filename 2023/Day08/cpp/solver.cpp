#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <assert.h>

typedef std::pair<std::string, std::string> NodePairType;

class CNavigator
{
    public:

                      CNavigator    (const std::string& kMoves, const std::string& kStartNode, const std::string& kEndNode, const bool bIsGhost);
        std::uint64_t Navigate      (std::map<std::string, NodePairType>& kNodes) const;

    private:

        std::string             m_kMoves;
        std::string             m_kStartNode;
        std::string             m_kEndNode;
        bool                    m_bIsGhost;
};

CNavigator::CNavigator(const std::string& kMoves, const std::string& kStartNode, const std::string& kEndNode, const bool bIsGhost) :
    m_kMoves(kMoves),
    m_kStartNode(kStartNode),
    m_kEndNode(kEndNode),
    m_bIsGhost(bIsGhost)
{
}

std::uint64_t CNavigator::Navigate(std::map<std::string, NodePairType>& kNodes) const
{
    const std::string::size_type  nLength   = m_kMoves.length();
          std::uint64_t           nDistance = 0;
          std::string::size_type  nPos      = 0;

    std::string kCurrentNode = m_kStartNode;
    bool        bFound;

    do
    {
        bFound = false;
        assert(kNodes.find(kCurrentNode) != kNodes.end());

        // Determine if the Node is Correct
        if (m_bIsGhost)
        {
            bFound = (kCurrentNode.back() == m_kEndNode.back());
        }
        else
        {
            bFound = (kCurrentNode == m_kEndNode);
        }

        if (!bFound)
        {
            // Get the Next Move
            const char kNextMove = m_kMoves.at(nPos);
            nPos = (nPos + 1) % nLength;

            if (kNextMove == 'L')
            {
                kCurrentNode = kNodes[kCurrentNode].first;
            }
            else if (kNextMove == 'R')
            {
                kCurrentNode = kNodes[kCurrentNode].second;
            }
            else
            {
                assert(false);
            }

            ++nDistance;
        }
    } while (!bFound);
    
    return nDistance;
}

std::uint64_t gcd(std::uint64_t a, std::uint64_t b)
{
    if (b == 0)
    {
        return a;
    }
    return gcd(b, a % b);
}

std::uint64_t lcm(std::uint64_t a, std::uint64_t b)
{
    std::uint64_t temp = gcd(a, b);

    return temp ? (a / temp * b) : 0;
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
        bool        bMovesProcessed = false;
        std::string kMoves;
        std::map<std::string, NodePairType> kNodes;
        std::vector<CNavigator> kGhosts;

        // Moves / Nodes
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);

            if (kLine.length())
            {
                if (!bMovesProcessed)
                {
                    kMoves          = kLine;
                    bMovesProcessed = true;
                }
                else
                {
                    std::string::size_type nPos;
                    do
                    {
                        nPos = kLine.find_first_of("=(),");
                        if (std::string::npos != nPos)
                        {
                            kLine.erase(nPos, 1);
                        }
                    } while (nPos != std::string::npos);
                    
                    const std::vector<std::string> kSplit = split(kLine, " ", false);
                    assert(kSplit.size() == 4);
                    assert(kSplit.at(1).length() == 0);
                    assert(kNodes.find(kSplit.at(0)) == kNodes.end());
                    kNodes[kSplit.at(0)] = NodePairType(kSplit.at(2), kSplit.at(3));

                    if (kSplit.at(0).back() == 'A')
                    {
                        kGhosts.push_back(CNavigator(kMoves, kSplit.at(0), "ZZZ", true));
                    }
                }
            }
        }

        // Part 1
        CNavigator kNavigator(kMoves, "AAA", "ZZZ", false);

        // Part 2
        std::vector<std::uint64_t> kPartTwoDistances;
        for (std::vector<CNavigator>::const_iterator it = kGhosts.begin(); it != kGhosts.end(); ++it)
        {
            kPartTwoDistances.push_back(it->Navigate(kNodes));
        }

        std::cout << "Part 1: " << kNavigator.Navigate(kNodes)                                                                         << std::endl;
        std::cout << "Part 2: " << std::accumulate(kPartTwoDistances.begin(), kPartTwoDistances.end(), kPartTwoDistances.front(), lcm) << std::endl;
   }

    return 0;
}
