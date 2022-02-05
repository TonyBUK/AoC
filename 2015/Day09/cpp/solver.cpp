#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>

#define addUniqueElementToVector(v, e) if (v.end() == std::find(v.begin(), v.end(), e)) v.push_back(e);
#define createMapAndPairType(base) typedef std::map <base##KeyType, base##ValueType> base##MapType; \
                                   typedef std::pair<base##KeyType, base##ValueType> base##PairType;

// Places Type
typedef std::vector<std::string> placesType;

// Route Type
typedef std::vector<std::string> routeKeyType;
typedef unsigned long routeValueType;
createMapAndPairType(route);

// Visited Type
struct bestKeyType
{
    std::string   kPlace;
    unsigned long nVisitedMask;

    bool const operator< (const bestKeyType& x) const
    {
       if (kPlace < x.kPlace)
       {
           return true;
       }
       else if (kPlace == x.kPlace)
       {
           return nVisitedMask < x.nVisitedMask;
       }
       return false;
    }
};
typedef unsigned long bestValueType;
createMapAndPairType(best);

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

std::size_t getIndex(const placesType& kPlaces, const std::string& kPlace)
{
    return std::find(kPlaces.cbegin(), kPlaces.cend(), kPlace) - kPlaces.cbegin();
}

unsigned long findRoute(const placesType& kPlaces, const routeMapType& kRoutes, const bool bFindShortest,
                        const unsigned long nCurrentScore = 0, const unsigned long nBestScore = 0,
                        bestMapType* kBestScores = NULL,
                        const unsigned long nVisitedMask = 0, const unsigned long nCompleteMask = 0, const std::string& kCurrentPlace = "")
{
    unsigned long nLocalBestScore = nBestScore;

    if (NULL == kBestScores)
    {
        nLocalBestScore = bFindShortest ? (unsigned long)-1 : 0;
        bestMapType         kBestScoresBuffer;
        const unsigned long nCompleteMaskTarget = (1 << kPlaces.size()) - 1;

        for (placesType::const_iterator it1 = kPlaces.cbegin(); it1 != kPlaces.cend(); ++it1)
        {
            const std::string& kPlace = *it1;
            const unsigned long nPlaceMask = 1 << getIndex(kPlaces, kPlace);
            for (placesType::const_iterator it2 = kPlaces.cbegin(); it2 != kPlaces.cend(); ++it2)
            {
                const std::string& kTargetPlace = *it2;
                if (kPlace != kTargetPlace)
                {
                    routeKeyType kRoute;
                    kRoute.push_back(kPlace);
                    kRoute.push_back(kTargetPlace);

                    // Make sure the Route Exists                    
                    if (kRoutes.cend() != kRoutes.find(kRoute))
                    {
                        const unsigned long nTargetMask = 1 << getIndex(kPlaces, kTargetPlace);
                        nLocalBestScore = findRoute(kPlaces, kRoutes, bFindShortest,
                                                    nCurrentScore + kRoutes.at(kRoute), nLocalBestScore,
                                                    &kBestScoresBuffer,
                                                    nVisitedMask | nPlaceMask | nTargetMask, nCompleteMaskTarget,
                                                    kTargetPlace);
                    }
                }
            }
        }
    }
    else
    {
        // If the Task has been completed... return our best score
        if (nVisitedMask == nCompleteMask)
        {
            if (bFindShortest)
            {
                if (nCurrentScore < nLocalBestScore)
                {
                    nLocalBestScore = nCurrentScore;
                }
            }
            else
            {
                if (nCurrentScore > nLocalBestScore)
                {
                    nLocalBestScore = nCurrentScore;
                }
            }
            return nLocalBestScore;
        }

        // Determine if this is a duplicate state, if so, only proceed
        // *IF* we've got a better score
        const bestKeyType kCurrentState = {kCurrentPlace, nVisitedMask};
        if (kBestScores->end() == kBestScores->find(kCurrentState))
        {
            (*kBestScores)[kCurrentState] = nCurrentScore;
        }
        else
        {
            if (bFindShortest)
            {
                if (nCurrentScore >= kBestScores->at(kCurrentState))
                {
                    return nLocalBestScore;
                }
            }
            else
            {
                if (nCurrentScore <= kBestScores->at(kCurrentState))
                {
                    return nLocalBestScore;
                }
            }
        }

        // All subsequent calls have a fixed start position
        for (placesType::const_iterator it = kPlaces.cbegin(); it != kPlaces.cend(); ++it)
        {
            const std::string kTargetPlace = *it;

            // Make sure this isn't a place we've already visited...
            const unsigned long nTargetMask = 1 << getIndex(kPlaces, kTargetPlace);
            if ((nTargetMask & nVisitedMask) == 0)
            {
                // Test this is a Valid Route
                routeKeyType kRoute;
                kRoute.push_back(kCurrentPlace);
                kRoute.push_back(kTargetPlace);
                if (kRoutes.cend() != kRoutes.find(kRoute))
                {
                    nLocalBestScore = findRoute(kPlaces, kRoutes, bFindShortest,
                                                nCurrentScore + kRoutes.at(kRoute), nLocalBestScore,
                                                kBestScores,
                                                nVisitedMask | nTargetMask, nCompleteMask,
                                                kTargetPlace);
                }
            }
        }
    }

    return nLocalBestScore;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        placesType   kPlaces;
        routeMapType kRoutes;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            // Split the Input Line into Tokens for Parsing
            const std::vector<std::string> kTokens = split(kLine, ' ');
            assert(kTokens.size() == 5);

            // Buffer each unique place Santa has to visit
            addUniqueElementToVector(kPlaces, kTokens.at(0));
            addUniqueElementToVector(kPlaces, kTokens.at(2));

            // Buffer each route

            // Store the pair of places for the route
            std::vector<std::string> kRoute;
            kRoute.push_back(kTokens.at(0));
            kRoute.push_back(kTokens.at(2));

            // Decode the Distance
            const unsigned long nDistance = std::stoul(kTokens.at(4));

            // Sort Forwards
            std::sort(kRoute.begin(), kRoute.end());
            kRoutes.insert(routePairType (kRoute, nDistance));

            // Sort Backwards
            std::sort(kRoute.rbegin(), kRoute.rend());
            kRoutes.insert(routePairType (kRoute, nDistance));
        }

        std::cout << "Part 1: " << findRoute(kPlaces, kRoutes, true)  << std::endl;
        std::cout << "Part 2: " << findRoute(kPlaces, kRoutes, false) << std::endl;
    }

    return 0;
}
