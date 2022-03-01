#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cinttypes>
#include <assert.h>

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

#define MAX_SIZE_T static_cast<std::size_t>(static_cast<uint64_t>(-1))

struct coreStatsType
{
    int64_t         nDamage;
    int64_t         nArmor;
};

struct playerType
{
    coreStatsType                   kCoreStats;
    int64_t                         nHealth;
    int64_t                         nMana;
    std::map<std::string, int64_t>  kDuration;
};

struct spellType
{
    int64_t         nManaCost;
    int64_t         nImmediateDamage;
    int64_t         nImmediateHeal;
    int64_t         nDuration;
    int64_t         nBuff;
};

spellType initialiseSpell(const int64_t nManaCost,
                          const int64_t nImmediateDamage, const int64_t nImmediateHeal,
                          const int64_t nDuration, const int64_t nBuff)
{
    const spellType kSpell = {nManaCost, nImmediateDamage, nImmediateHeal, nDuration, nBuff};
    return kSpell;
}

bool checkEndGame(const playerType& kPlayer, const playerType& kBoss, const std::size_t nManaSpend, std::size_t& nMinManaSpend)
{
    if (nManaSpend >= nMinManaSpend)
    {
        return true;
    }

    if (kPlayer.nHealth <= 0)
    {
        return true;
    }

    if (kBoss.nHealth <= 0)
    {
        nMinManaSpend = nManaSpend;
        return true;
    }

    return false;
}

void processCooldowns(playerType& kPlayer, const std::map<std::string, spellType>& kSpells)
{
    // Shield
    if (kPlayer.kDuration.at("Shield") > 0)
    {
        kPlayer.kDuration["Shield"]   -= 1;
        kPlayer.kCoreStats.nArmor      = kSpells.at("Shield").nBuff;
    }
    else
    {
        kPlayer.kCoreStats.nArmor      = 0;
    }

    // Poison
    if (kPlayer.kDuration.at("Poison") > 0)
    {
        kPlayer.kDuration["Poison"]   -= 1;
        kPlayer.kCoreStats.nDamage     = kSpells.at("Poison").nBuff;
    }
    else
    {
        kPlayer.kCoreStats.nDamage    = 0;
    }

    // Recharge
    if (kPlayer.kDuration.at("Recharge") > 0)
    {
        kPlayer.kDuration["Recharge"] -= 1;
        kPlayer.nMana                 += kSpells.at("Recharge").nBuff;
    }
}

std::size_t playGames(const playerType& kPlayer, const playerType& kBoss, const std::map<std::string, spellType>& kSpells, const bool bHardMode, const std::size_t nManaSpend = 0, const std::size_t nMinManaSpend = MAX_SIZE_T)
{
    std::size_t nBestMinManaSpend = nMinManaSpend;

    for (std::map<std::string, spellType>::const_iterator it = kSpells.cbegin(); it != kSpells.cend(); ++it)
    {
        playerType         kLocalPlayer    = kPlayer;
        playerType         kLocalBoss      = kBoss;
        std::size_t        nLocalManaSpend = nManaSpend;
        const std::string& kSpellName      = it->first;
        const spellType&   kSpell          = it->second;

        //////////////////////////////////////////////////////////////////////
        // Hard Mode

        if (bHardMode)
        {
            kLocalPlayer.nHealth -= 1;
            if (checkEndGame(kLocalPlayer, kLocalBoss, nLocalManaSpend, nBestMinManaSpend))
            {
                continue;
            }
        }

        //////////////////////////////////////////////////////////////////////
        // Player Turn

        // Perform all Spells involving Cooldowns
        processCooldowns(kLocalPlayer, kSpells);

        // Determine if the Current Move is Viable
        if (kSpell.nManaCost <= kLocalPlayer.nMana)
        {
            kLocalBoss.nHealth   -= kSpell.nImmediateDamage;
            kLocalPlayer.nHealth += kSpell.nImmediateHeal;

            if (kSpell.nDuration > 0)
            {
                if (0 == kLocalPlayer.kDuration.at(kSpellName))
                {
                    kLocalPlayer.kDuration[kSpellName] = kSpell.nDuration;
                }
            }

            // Spend the Mana
            nLocalManaSpend     += kSpell.nManaCost;
            kLocalPlayer.nMana  -= kSpell.nManaCost;
        }
        else
        {
            continue;
        }

        // Perform any Damage Over Time Actions
        kLocalBoss.nHealth -= kLocalPlayer.kCoreStats.nDamage;

        if (checkEndGame(kLocalPlayer, kLocalBoss, nLocalManaSpend, nBestMinManaSpend))
        {
            continue;
        }

        //////////////////////////////////////////////////////////////////////
        // Boss Turn

        // Perform all Spells involving Cooldowns
        processCooldowns(kLocalPlayer, kSpells);

        // Perform any Damage Over Time Actions
        kLocalBoss.nHealth -= kLocalPlayer.kCoreStats.nDamage;

        if (checkEndGame(kLocalPlayer, kLocalBoss, nLocalManaSpend, nBestMinManaSpend))
        {
            continue;
        }

        // Perform the Boss Attack
        kLocalPlayer.nHealth -= (kLocalBoss.kCoreStats.nDamage - kLocalPlayer.kCoreStats.nArmor);

        if (checkEndGame(kLocalPlayer, kLocalBoss, nLocalManaSpend, nBestMinManaSpend))
        {
            continue;
        }

        //////////////////////////////////////////////////////////////////////
        // Recurse...

        nBestMinManaSpend = playGames(kLocalPlayer, kLocalBoss, kSpells, bHardMode, nLocalManaSpend, nBestMinManaSpend);
    }

    return nBestMinManaSpend;
}

int main(int argc, char** argv)
{
    std::ifstream kFile;
    kFile.open("../input.txt");
    if (kFile.is_open())
    {
        playerType                        kPlayer;
        playerType                        kBoss;
        std::map<std::string, spellType>  kSpells;

        // Define the Spells
        kSpells["Magic Missile"] = initialiseSpell( 53, 4, 0, 0,   0);
        kSpells["Drain"]         = initialiseSpell( 73, 2, 2, 0,   0);
        kSpells["Shield"]        = initialiseSpell(113, 0, 0, 6,   7);
        kSpells["Poison"]        = initialiseSpell(173, 0, 0, 6,   3);
        kSpells["Recharge"]      = initialiseSpell(229, 0, 0, 5, 101);

        // Boss has no Armor
        kBoss.kCoreStats.nArmor = 0;

        while (!kFile.eof())
        {
            // Get the Current Line from the File
            std::string kLine;
            std::getline(kFile, kLine);

            const std::vector<std::string> kSplit = split(kLine, ": ");
            assert(kSplit.size() == 2);
            if ("Hit Points" == kSplit.at(0))
            {
                kBoss.nHealth = std::stold(kSplit.at(1));
            }
            else if ("Damage" == kSplit.at(0))
            {
                kBoss.kCoreStats.nDamage = std::stold(kSplit.at(1));
            }
            else
            {
                assert(false);
            }
        }

        // Solve the Puzzle
        kPlayer.kCoreStats.nDamage      = 0;
        kPlayer.kCoreStats.nArmor       = 0;
        kPlayer.nHealth                 = 50;
        kPlayer.nMana                   = 500;
        kPlayer.kDuration["Shield"]     = 0;
        kPlayer.kDuration["Poison"]     = 0;
        kPlayer.kDuration["Recharge"]   = 0;
        std::cout << "Part 1: " << playGames(kPlayer, kBoss, kSpells, false) << std::endl;
        std::cout << "Part 2: " << playGames(kPlayer, kBoss, kSpells, true)  << std::endl;
   }

    return 0;
}
