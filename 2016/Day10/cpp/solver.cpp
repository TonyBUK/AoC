#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "cbot.h"
#include "coutputbin.h"
#include "helper.h"

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        std::map<int64_t, CBot>         kBots;
        std::map<int64_t, COutputBin>   kOutputs;
        std::vector<int64_t>            kQueue;

        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            const std::vector<std::string>  kTokens = split(kLine, " ");

            assert(!kTokens.empty());
            if ("value" == kTokens.at(0))
            {
                assert(kTokens.size() == 6);

                const int64_t nBotId = std::stoll(kTokens.at(5));
                const int64_t nValue = std::stoll(kTokens.at(1));

                if (kBots.find(nBotId) == kBots.cend())
                {
                    kBots[nBotId] = CBot();
                }
                kBots[nBotId].receiveChip(nValue);
                
                if (2 == kBots[nBotId].countChips())
                {
                    kQueue.push_back(nBotId);
                }
            }
            else if ("bot" == kTokens.at(0))
            {
                assert(kTokens.size() == 12);

                const int64_t           nTargetIds[]  = {std::stoll(kTokens.at(1)),
                                                         std::stoll(kTokens.at(6)),
                                                         std::stoll(kTokens.at(11))};
                const CBot::TargetType  eTargetType[] = {CBot::E_BOT,
                                                         ("bot" == kTokens.at(5))  ? CBot::E_BOT : CBot::E_OUTPUT,
                                                         ("bot" == kTokens.at(10)) ? CBot::E_BOT : CBot::E_OUTPUT};
                for (std::size_t i = 0; i < (sizeof(nTargetIds) / sizeof(nTargetIds[0])); ++i)
                {
                    if (CBot::E_BOT == eTargetType[i])
                    {
                        if (kBots.find(nTargetIds[i]) == kBots.cend())
                        {
                            kBots[nTargetIds[i]] = CBot();
                        }
                    }
                    else
                    {
                        if (kOutputs.find(nTargetIds[i]) == kOutputs.cend())
                        {
                            kOutputs[nTargetIds[i]] = COutputBin();
                        }
                    }
                }

                kBots[nTargetIds[0]].storeAction(eTargetType[1], nTargetIds[1], eTargetType[2], nTargetIds[2]);
            }
            else
            {
                assert(false);
            }
        }

        const int64_t               kChipComparisonArray[]   = {17, 61};
        const std::vector<int64_t>  kChipComparison(kChipComparisonArray, kChipComparisonArray + 2);
        int64_t                     nPart1BotId;

        while (!kQueue.empty())
        {
            const int64_t nBotId = kQueue.back();
            kQueue.pop_back();

            if (kBots[nBotId].compare(kChipComparison))
            {
                nPart1BotId = nBotId;
            }

            kBots[nBotId].process(kBots, kOutputs, kQueue);
        }

        std::cout << "Part 1: " << nPart1BotId << std::endl;
        std::cout << "Part 2: " << kOutputs[0].value() * kOutputs[1].value() * kOutputs[2].value() << std::endl;
    }

    return 0;
}
