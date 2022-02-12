#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

void incrementPassword(std::vector<std::size_t>& kPassword, const std::size_t nPasswordBase)
{
    // Note: This could be a bit cleverer if we didn't just increment by 1 but instead
    //       calculated for each failed rule what the first valid value that meets the
    //       rule is.

    // Incrementing the password is now add and carry, i.e. the way
    // you probably used to add numbers as a child before getting a
    // calculator or memorising.
    //
    // i.e. 0,1,2,3 + 22 is:
    //
    // 3 + 22 = 25, but 25 > 23 so subtract 23 to leave 2, and carry the 1 (23/23 = 1)
    // leaving 0,1,2,2 + 1,0, giving a final answer of 0,1,3,2
    for (std::vector<std::size_t>::reverse_iterator it = kPassword.rbegin(); it != kPassword.rend(); ++it)
    {
        ++(*it);
        if (*it < nPasswordBase)
        {
            return;
        }
        *it = 0;
    }
}

// Rule 1: Passwords must include one increasing straight of at least three letters,
//         like abc, bcd, cde, and so on, up to xyz. They cannot skip letters; abd
//         doesn't count.
//
//         But there's a big gotcha here... because we've stripped i, o and l, that
//         means technically hjk now passes this rule, which doesn't seem right,
//         so I've re-indexed the values back into base 26 first before doing the
//         comparison, that way hjk would now no longer be consecutive.
bool rule1(const std::vector<std::size_t>& kPassword, const std::vector<std::size_t>& kBase26Mappings)
{
    for (std::size_t i = 2; i < kPassword.size(); ++i)
    {
        const size_t a = kBase26Mappings.at(kPassword.at(i-2));
        const size_t b = kBase26Mappings.at(kPassword.at(i-1));
        const size_t c = kBase26Mappings.at(kPassword.at(i));
        if ((a == (b-1)) && (b == (c-1)))
        {
            return true;
        }
    }

    return false;
}

// Rule 3 : Passwords must contain at least two different, non-overlapping pairs of
//          letters, like aa, bb, or zz.
//
// Note: Because of the "different" part, it's impossible to overlap, i.e. aa can't
//       overlap bb.  If the different part wasn't a rule, then we'd need to consdier
//       aaa and ensure it's not detected as two pairs, but since it is, we don't.
bool rule3(const std::vector<std::size_t>& kPassword)
{
    std::vector<std::size_t> kUniquePairs;
    for (std::size_t i = 1; i < kPassword.size(); ++i)
    {
        const size_t a = kPassword.at(i-1);
        const size_t b = kPassword.at(i);
        if (a == b)
        {
            if (std::find(kUniquePairs.begin(), kUniquePairs.end(), a) == kUniquePairs.end())
            {
                kUniquePairs.push_back(a);
                if (2 == kUniquePairs.size())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

const std::string solvePassword(const std::string& kPasswordString)
{
    // Full Digits including illegal charactersx
    const std::string kFullDigits = "abcdefghijklmnopqrstuvwxyz";

    // Allowed Digits
    const std::string kAllowedDigits = "abcdefghjkmnpqrstuvwxyz";
    //                                  00000000001111111111222
    //                                  01234567890123456789012

    // Password in Base 23
    std::vector<std::size_t> kPassword;
    const std::size_t        nPasswordBase = kAllowedDigits.size();

    // The password itself is really just a base 23 number in that each
    // position has 23 possible values based upon the rules that it's
    // a lower case letter and excludes i, o and l.
    // "aaaa" would be 0,0,0,0
    // "abcd" would be 0,1,2,3
    // "wxyz" would be 19,20,21,22
    // So step 1 is to convert the string into an array of digits.
    for (std::string::const_iterator it = kPasswordString.cbegin(); it != kPasswordString.cend(); ++it)
    {
        kPassword.push_back(kAllowedDigits.find(*it));
    }

    // Mapping Array to convert from Base 23 to Base 26 (Full Alphabet)
    std::vector<std::size_t> kBase26Mappings;
    for (std::string::const_iterator it = kAllowedDigits.cbegin(); it != kAllowedDigits.cend(); ++it)
    {
        kBase26Mappings.push_back(kFullDigits.find(*it));
    }

    // Force a single increment to get things moving...
    incrementPassword(kPassword, nPasswordBase);

    // The core principle here is to keep incrementing the password until
    // we pass a given rule, then move onto the next rule and repeat.  We've
    // found our password when all the rules passed without incrementing the
    // password.
    while (1)
    {
        bool bAnyIncrements = false;

        // Validate the Current Password

        // Rule 1: See above
        while (false == rule1(kPassword, kBase26Mappings))
        {
            incrementPassword(kPassword, nPasswordBase);
            bAnyIncrements = true;
        }

        // Rule 2: Passwords may not contain the letters i, o, or l, as these letters can be
        //         mistaken for other characters and are therefore confusing.

        // This is impossible to achieve because of how we've already stripped out these letters
        // when moving to Base 23, meaning we always pass this rule

        // Rule 3 : See above
        while (false == rule3(kPassword))
        {
            incrementPassword(kPassword, nPasswordBase);
            bAnyIncrements = true;
        }

        if (false == bAnyIncrements) break;
    }

    // Re-encode the String
    std::string kResult;
    for (std::vector<size_t>::const_iterator it = kPassword.cbegin(); it != kPassword.cend(); ++it)
    {
        kResult.push_back(kAllowedDigits.at(*it));
    }
    return kResult;
}

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

            // Part 1 - First Password
            const std::string kPassword = solvePassword(kLine);
            std::cout << "Part 1: " << kPassword                << std::endl;
            
            // Part 2 - Second Password
            std::cout << "Part 2: " << solvePassword(kPassword) << std::endl;
       }
    }

    return 0;
}
