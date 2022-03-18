#include "common.h"

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

const std::string replace(const std::string& kString, const std::string& kSearch, const std::string& kReplace)
{
    std::string kNewString;
    std::size_t nPos = 0;

    kNewString = kString;
    do
    {
        nPos = kNewString.find(kSearch, nPos);
        if (std::string::npos != nPos)
        {
            kNewString.replace(nPos, kSearch.length(), kReplace);
            nPos += kReplace.length();
        }
    } while (std::string::npos != nPos);

    return kNewString;
}
