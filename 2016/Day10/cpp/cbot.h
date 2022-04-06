#ifndef __C_BOT_H__
#define __C_BOT_H__

#include "cchipdevice.h"
#include "coutputbin.h"
#include <vector>
#include <map>

class CBot : public CChipDevice
{
    public:

        enum TargetType {E_BOT, E_OUTPUT};

                    CBot        (void);
        void        receiveChip (const int64_t nChip);
        std::size_t countChips  (void) const;
        void        storeAction (const TargetType eTargetLow,  const int64_t nTargetLow,
                                 const TargetType eTargetHigh, const int64_t nTargetHigh);
        void        process     (std::map<int64_t, CBot>& kBots, std::map<int64_t, COutputBin>& kOutputs, std::vector<int64_t>& kQueue);
        bool        compare     (const std::vector<int64_t> kCompare) const;

    private:

        struct ActionType
        {
            TargetType  eTarget;
            int64_t     nTargetId;
        };

        std::vector<int64_t>    m_kChips;
        std::vector<ActionType> m_kActions;
};

#endif // __C_BOT_H__