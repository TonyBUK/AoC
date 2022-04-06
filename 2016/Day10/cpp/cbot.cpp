#include "cbot.h"

CBot::CBot(void) :
    m_kChips    (),
    m_kActions  ()
{
}

void CBot::receiveChip(const int64_t nChip)
{
    assert(m_kChips.size() < 2);

    m_kChips.push_back(nChip);
    std::sort(m_kChips.begin(), m_kChips.end());
}

std::size_t CBot::countChips(void) const
{
    return m_kChips.size();
}

void CBot::storeAction(const TargetType eTargetLow,  const int64_t nTargetLow,
                       const TargetType eTargetHigh, const int64_t nTargetHigh)
{
    assert(m_kActions.empty());

    m_kActions.assign(2, ActionType());
    m_kActions[0].eTarget   = eTargetLow;
    m_kActions[0].nTargetId = nTargetLow;    
    m_kActions[1].eTarget   = eTargetHigh;
    m_kActions[1].nTargetId = nTargetHigh;    
}

void CBot::process(std::map<int64_t, CBot>& kBots, std::map<int64_t, COutputBin>& kOutputs, std::vector<int64_t>& kQueue)
{
    for (std::vector<ActionType>::const_iterator it = m_kActions.cbegin(); it != m_kActions.cend(); ++it)
    {
        if (E_BOT == it->eTarget)
        {
            kBots.at(it->nTargetId).receiveChip(m_kChips.at(it - m_kActions.cbegin()));
            if (2 == kBots.at(it->nTargetId).countChips())
            {
                kQueue.push_back(it->nTargetId);
            }
        }
        else
        {
            kOutputs.at(it->nTargetId).receiveChip(m_kChips.at(it - m_kActions.cbegin()));
        }
    }

    m_kChips.clear();
}

bool CBot::compare(const std::vector<int64_t> kCompare) const
{
    if (kCompare.size() != m_kChips.size())
    {
        return false;
    }
    else
    {
        std::vector<int64_t> kCompareSorted = kCompare;
        std::sort(kCompareSorted.begin(), kCompareSorted.end());

        return m_kChips == kCompareSorted;
    }
}