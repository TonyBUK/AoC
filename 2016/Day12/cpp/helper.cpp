#include "helper.h"

std::vector<std::string> split(const std::string& s, const std::string& kSeperators, const bool bIncludeSeparators)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find_first_of(kSeperators, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);

        if (bIncludeSeparators)
        {
            output.push_back(std::string(1, s.at(pos)));
        }

        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}
