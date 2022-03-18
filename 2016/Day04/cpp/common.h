#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, const std::string& seperator);
const std::string replace(const std::string& kString, const std::string& kSearch, const std::string& kReplace);

#endif // __COMMON_H__
