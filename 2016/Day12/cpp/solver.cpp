#include <iostream>
#include <fstream>
#include <string>
#include "helper.h"
#include "CSuperAssembunny.h"
#include <assert.h>

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        CSuperAssembunny kAssembunny;
        CRegisters&      kRegisters = kAssembunny.registers();
        while (!kFile.eof())
        {
            std::string         kLine;
            std::getline(kFile, kLine);

            const std::vector<std::string> kTokens = split(kLine, " ", false);
            
            switch (kTokens.size())
            {
                case 2:
                {
                    if ("inc" == kTokens.at(0))
                    {
                        assert(kRegisters.legal(kTokens.at(1)));
                        kAssembunny.Add(CAssembunny::E_INC, kTokens.at(1).front());
                    }
                    else if ("dec" == kTokens.at(0))
                    {
                        assert(kRegisters.legal(kTokens.at(1)));
                        kAssembunny.Add(CAssembunny::E_DEC, kTokens.at(1).front());
                    }
                    else
                    {
                        assert(false);
                    }
                } break;

                case 3:
                {
                    if ("cpy" == kTokens.at(0))
                    {
                        assert(kRegisters.legal(kTokens.at(2)));
                        if (!kRegisters.legal(kTokens.at(1)))
                        {
                            kAssembunny.Add(CAssembunny::E_CPY, std::stoll(kTokens.at(1)), kTokens.at(2).front());
                        }
                        else
                        {
                            kAssembunny.Add(CAssembunny::E_CPY, kTokens.at(1).front(), kTokens.at(2).front());
                        }
                    }
                    else if ("jnz" == kTokens.at(0))
                    {
                        kAssembunny.Add(CAssembunny::E_JNZ, kTokens.at(1), kTokens.at(2));
                    }
                    else
                    {
                        assert(false);
                    }
                } break;

                default:
                {
                    assert(false);
                }
            }
        }

        kAssembunny.Patch();

        kAssembunny.Process();
        std::cout << "Part 1: " << kRegisters.get('a') << std::endl;

        kRegisters.reset();
        kRegisters.set('c', 1);
        kAssembunny.Process();
        std::cout << "Part 2: " << kRegisters.get('a') << std::endl;
    }

    return 0;
}
