#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <assert.h>

std::string SSTR(const int64_t Number)
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

class JSONNode
{
    public:

        enum jsonType
        {
            E_NOT_INITIALISED,
            E_OBJECT,
            E_ARRAY,
            E_STRING,
            E_NUMBER_INT,
            E_NUMBER_DBL,
            E_BOOLEAN,
            E_NULL
        };

        JSONNode () :
            m_eType     (E_NOT_INITIALISED)
        {
        }

        JSONNode (const jsonType eType) :
            m_eType     (eType)
        {
        }

        void SetType(const jsonType eType)
        {
            m_eType = eType;
        }

        bool Add(const JSONNode& kNode)
        {
            switch (m_eType)
            {
                case E_ARRAY:
                {
                    m_kArray.push_back(kNode);
                } break;

                default:
                {
                    return false;
                } break;
            };

            return true;
        }

        bool Set(const JSONNode& kKeyNode, const JSONNode& kValueNode)
        {
            switch (m_eType)
            {
                case E_OBJECT:
                {
                    // The key value isn't actually important to this challenge,
                    // otherwise I'd make it a cut-down version of the JSON Node Type
                    std::string kKey;
                    if (kKeyNode.GetType() == E_STRING)
                    {
                        kKeyNode.Get(kKey);
                    }
                    else
                    {
                        int64_t x;
                        kKeyNode.Get(x);
                        kKey = SSTR(x);
                    }
                    m_kObject.insert(std::pair<std::string, JSONNode>(kKey, kValueNode));
                } break;

                default:
                {
                    return false;
                } break;
            };

            return true;
        }

        bool Set(const std::string& kValue)
        {
            switch (m_eType)
            {
                case E_STRING:
                {
                    m_kString = kValue;
                } break;

                default:
                {
                    return false;
                } break;
            };

            return true;
        }

        bool Set(const int64_t nValue)
        {
            switch (m_eType)
            {
                case E_NUMBER_INT:
                {
                    m_nInt = nValue;
                } break;

                default:
                {
                    return false;
                } break;
            };

            return true;
        }

        bool Set(const double nValue)
        {
            switch (m_eType)
            {
                case E_NUMBER_DBL:
                {
                    m_nInt = nValue;
                } break;

                default:
                {
                    return false;
                } break;
            };

            return true;
        }

        bool Set(const bool bValue)
        {
            switch (m_eType)
            {
                case E_BOOLEAN:
                {
                    m_bBool = bValue;
                } break;

                default:
                {
                    return false;
                } break;
            };

            return true;
        }

        jsonType GetType() const
        {
            return m_eType;
        }

        bool Get(int64_t& nNumber) const
        {
            if (m_eType != E_NUMBER_INT)
            {
                return false;
            }
            nNumber = m_nInt;
            return true;
        }

        bool Get(std::string& kString) const
        {
            if (m_eType != E_STRING)
            {
                return false;
            }
            kString = m_kString;
            return true;
        }

        std::size_t GetSize() const
        {
            if (m_eType != E_ARRAY)
            {
                return 0;
            }
            return m_kArray.size();
        }
        bool GetKeys(std::vector<std::string>& kKeys) const
        {
            if (m_eType != E_OBJECT)
            {
                return false;
            }

            // Create a list of all the keys
            for (std::map<std::string, JSONNode>::const_iterator it = m_kObject.cbegin(); it != m_kObject.cend(); ++it)
            {
                kKeys.push_back(it->first);
            }
            return true;
        }

        const JSONNode& GetNode(const std::string& kKey) const
        {
            assert(m_eType == E_OBJECT);
            assert(m_kObject.find(kKey) != m_kObject.cend());
            return m_kObject.at(kKey);
        }

        const JSONNode& GetNode(const std::size_t nElement) const
        {
            assert(m_eType == E_ARRAY);
            assert(nElement < m_kArray.size());
            return m_kArray.at(nElement);
        }

    private:

        jsonType                        m_eType;
        std::vector<JSONNode>           m_kArray;
        std::map<std::string, JSONNode> m_kObject;
        std::string                     m_kString;
        int64_t                         m_nInt;
        double                          m_nDouble;
        bool                            m_bBool;
};

// We can simplify this based on puzzle inputs...
// 1. No Escape Sequences
// 2. No NULL Values
// 3. No Floating Point values
// 4. No "empty" items
void Parse(const std::string& kJSONText, JSONNode& kJSONNode, std::size_t& nPos)
{
    if (kJSONText.at(nPos) == '[')
    {
        kJSONNode.SetType(JSONNode::E_ARRAY);
        ++nPos;

        while (kJSONText.at(nPos) != ']')
        {
            JSONNode kNode;
            Parse(kJSONText, kNode, nPos);
            kJSONNode.Add(kNode);
            assert((kJSONText.at(nPos) == ',') || (kJSONText.at(nPos) == ']'));
            if (kJSONText.at(nPos) == ',')
            {
                ++nPos;
            }
        }
        ++nPos;
    }
    else if (kJSONText.at(nPos) == '{')
    {
        kJSONNode.SetType(JSONNode::E_OBJECT);
        ++nPos;

        bool        bProcessingKey = true;
        std::string kKey;
        while (kJSONText.at(nPos) != '}')
        {
            JSONNode kKeyNode;
            Parse(kJSONText, kKeyNode, nPos);
            assert(kJSONText.at(nPos) == ':');
            ++nPos;

            JSONNode kValueNode;
            Parse(kJSONText, kValueNode, nPos);
            assert((kJSONText.at(nPos) == ',') || (kJSONText.at(nPos) == '}'));

            kJSONNode.Set(kKeyNode, kValueNode);
            if (kJSONText.at(nPos) == ',')
            {
                ++nPos;
            }
        }
        ++nPos;
    }
    else if (kJSONText.at(nPos) == '\"')
    {
        kJSONNode.SetType(JSONNode::E_STRING);
        ++nPos;

        std::string kString;
        while (kJSONText.at(nPos) != '\"')
        {
            kString.push_back(kJSONText.at(nPos));
            ++nPos;
        }
        kJSONNode.Set(kString);
        ++nPos;
    }
    else
    {
        const std::string kNumeric = "-0123456789";
        kJSONNode.SetType(JSONNode::E_NUMBER_INT);

        std::string kString;
        while (kNumeric.find(kJSONText.at(nPos)) != std::string::npos)
        {
            kString.push_back(kJSONText.at(nPos));
            ++nPos;
        }
        kJSONNode.Set((int64_t)std::stoi(kString));
    }
}

std::size_t getJSONTotal(const JSONNode& kJSONRoot, const std::string& kFilter = "", const std::size_t nTotal = 0)
{
    std::size_t nLocalTotal = nTotal;

    switch(kJSONRoot.GetType())
    {
        case JSONNode::E_ARRAY:
        {
            for (std::size_t i = 0; i < kJSONRoot.GetSize(); ++i)
            {
                const JSONNode& kJSON = kJSONRoot.GetNode(i);
                nLocalTotal = getJSONTotal(kJSON, kFilter, nLocalTotal);
            }
        } break;

        case JSONNode::E_OBJECT:
        {
            std::vector<std::string> kKeys;
            kJSONRoot.GetKeys(kKeys);
            if ("" != kFilter)
            {
                for (std::vector<std::string>::const_iterator it = kKeys.cbegin(); it != kKeys.cend(); ++it)
                {
                    const JSONNode& kJSON = kJSONRoot.GetNode(*it);
                    if (JSONNode::E_STRING == kJSON.GetType())
                    {
                        std::string kValue;
                        kJSON.Get(kValue);
                        if (kValue == kFilter)
                        {
                            return nLocalTotal;
                        }
                    }
                }
            }

            for (std::vector<std::string>::const_iterator it = kKeys.cbegin(); it != kKeys.cend(); ++it)
            {
                const JSONNode& kJSON = kJSONRoot.GetNode(*it);
                nLocalTotal = getJSONTotal(kJSON, kFilter, nLocalTotal);
            }
        } break;

        case JSONNode::E_NUMBER_INT:
        {
            int64_t nValue;
            kJSONRoot.Get(nValue);
            nLocalTotal += static_cast<std::size_t>(nValue);
        } break;

        default:
        {
        } break;
    }

    return nLocalTotal;
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
       
            std::size_t nPos = 0;
            JSONNode    kJSON;
            Parse(kLine, kJSON, nPos);

            std::cout << "Part 1: " << getJSONTotal(kJSON)        << std::endl;
            std::cout << "Part 2: " << getJSONTotal(kJSON, "red") << std::endl;
       }
    }

    return 0;
}
