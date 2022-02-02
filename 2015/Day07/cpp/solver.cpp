#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <assert.h>

enum instructionType {E_SET, E_NOT, E_LSHIFT, E_RSHIFT, E_AND, E_OR};

struct dataType
{
    bool        bIsLiteral;
    uint16_t    nLiteral;
    std::string kSignal;
};

struct commandType
{
    instructionType       eInstruction;
    std::vector<dataType> kInputs;
    std::string           kOutput;
};

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

dataType getNumber(const std::string& s)
{
    dataType kNumber;
    kNumber.bIsLiteral = false;
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    if (!s.empty() && it == s.end())
    {
        const int candidateNumber = std::stoi(s);
        kNumber.bIsLiteral = (candidateNumber & 0xFFFF) == candidateNumber;
        if (kNumber.bIsLiteral)
        {
            kNumber.nLiteral = (uint16_t)candidateNumber;
        }
    }
    if (!kNumber.bIsLiteral)
    {
        kNumber.kSignal = s;
    }

    return kNumber;
}

bool isOneOf(const std::string& kString, const std::vector<const char*>& kArrayOfStrings)
{
    for (std::vector<const char*>::const_iterator it = kArrayOfStrings.cbegin(); it != kArrayOfStrings.cend(); ++it)
    {
        if (kString == *it)
        {
            return true;
        }
    }
    return false;
}

bool decodeCommand(const std::string& kLine, commandType& kCommand)
{
    const char* TWO_PARAM_OPCODES[] = {"AND", "OR", "LSHIFT", "RSHIFT"};
    const std::vector<std::string> kTokens = split(kLine, ' ');
    const std::vector<const char*> kTwoParamOpcodes(TWO_PARAM_OPCODES,
                                                    TWO_PARAM_OPCODES + (sizeof(TWO_PARAM_OPCODES)/sizeof(TWO_PARAM_OPCODES[0])));
    assert(kTokens.size() >= 2);
    if (isOneOf(kTokens[1], kTwoParamOpcodes))
    {
        assert(kTokens.size() == 5);
        assert(kTokens[3] == "->");
        if      ("AND"    == kTokens[1]) kCommand.eInstruction = E_AND;
        else if ("OR"     == kTokens[1]) kCommand.eInstruction = E_OR;
        else if ("LSHIFT" == kTokens[1]) kCommand.eInstruction = E_LSHIFT;
        else if ("RSHIFT" == kTokens[1]) kCommand.eInstruction = E_RSHIFT;
        else assert(false);

        kCommand.kInputs.push_back(getNumber(kTokens[0]));
        kCommand.kInputs.push_back(getNumber(kTokens[2]));
        kCommand.kOutput = kTokens[4];

        return true;
    }
    else if ("NOT" == kTokens[0])
    {
        assert(kTokens.size() == 4);
        assert(kTokens[2] == "->");

        kCommand.eInstruction = E_NOT;
        kCommand.kInputs.push_back(getNumber(kTokens[1]));
        kCommand.kOutput = kTokens[3];

        return true;
    }
    else
    {
        assert(kTokens.size() == 3);
        assert(kTokens[1] == "->");

        kCommand.eInstruction = E_SET;
        kCommand.kInputs.push_back(getNumber(kTokens[0]));
        kCommand.kOutput = kTokens[2];

        return true;
    }

    return false;
}

uint16_t getSignalOrLiteral(const dataType& kInput, const std::map<std::string, uint16_t>& kSignals)
{
    if (kInput.bIsLiteral)
    {
        return kInput.nLiteral;
    }
    else if (kSignals.find(kInput.kSignal) != kSignals.end())
    {
        return kSignals.at(kInput.kSignal);
    }
    assert(false);
}

void processCommands(const std::vector<commandType>& kGateCommands, std::map<std::string, uint16_t>& kSignals)
{
    while (kSignals.size() != kGateCommands.size())
    {
        for (std::vector<commandType>::const_iterator it = kGateCommands.cbegin(); it != kGateCommands.cend(); ++it)
        {
            const commandType& kGateCommand = *it;

            // Skip Commands we've already handled
            if (kSignals.find(kGateCommand.kOutput) != kSignals.end()) continue;

            bool bValid = true;
            for (std::vector<dataType>::const_iterator input = kGateCommand.kInputs.cbegin(); input != kGateCommand.kInputs.cend(); ++input)
            {
                if (!input->bIsLiteral)
                {
                    if (kSignals.find(input->kSignal) == kSignals.end())
                    {
                        bValid = false;
                        break;
                    }
                }
            }

            if (!bValid) continue;

            switch(kGateCommand.eInstruction)
            {
                case E_SET:
                {
                    kSignals[kGateCommand.kOutput] = getSignalOrLiteral(kGateCommand.kInputs[0], kSignals);
                } break;

                case E_NOT:
                {
                    kSignals[kGateCommand.kOutput] = (~getSignalOrLiteral(kGateCommand.kInputs[0], kSignals)) & 0xFFFF;
                } break;

                case E_LSHIFT:
                {
                    kSignals[kGateCommand.kOutput] = (getSignalOrLiteral(kGateCommand.kInputs[0], kSignals) <<
                                                      getSignalOrLiteral(kGateCommand.kInputs[1], kSignals));
                } break;
                
                case E_RSHIFT:
                {
                    kSignals[kGateCommand.kOutput] = (getSignalOrLiteral(kGateCommand.kInputs[0], kSignals) >>
                                                      getSignalOrLiteral(kGateCommand.kInputs[1], kSignals));
                } break;

                case E_AND:
                {
                    kSignals[kGateCommand.kOutput] = (getSignalOrLiteral(kGateCommand.kInputs[0], kSignals) &
                                                      getSignalOrLiteral(kGateCommand.kInputs[1], kSignals));
                } break;

                case E_OR:
                {
                    kSignals[kGateCommand.kOutput] = (getSignalOrLiteral(kGateCommand.kInputs[0], kSignals) |
                                                      getSignalOrLiteral(kGateCommand.kInputs[1], kSignals));
                } break;
            
                default:
                {
                    assert(false);
                } break;
            }
        }
    }
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::vector<commandType> kGateCommands;
        while (!kFile.eof())
        {
            std::string kLine;
            std::getline(kFile, kLine);
            commandType kCommand;
            if (!decodeCommand(kLine, kCommand))
            {
                assert(false);
            }

            kGateCommands.push_back(kCommand);
        }

        std::map<std::string, uint16_t> kSignals;
        processCommands(kGateCommands, kSignals);
        std::cout << "Part 1: " << kSignals["a"] << std::endl;

        const uint16_t tmp = kSignals["a"];
        kSignals.clear();
        kSignals["b"] = tmp;
        processCommands(kGateCommands, kSignals);
        std::cout << "Part 2: " << kSignals["a"] << std::endl;
    }

    return 0;
}
