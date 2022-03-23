#ifndef __CSTRING_METRICS_H__
#define __CSTRING_METRICS_H__

#include <string>
#include <map>
#include <vector>
#include <cstdint>

class CStringMetrics
{
    public:

                    CStringMetrics  (void);
        void        Add             (const std::string& kString);
        void        Calculate       (void);

        std::string Min             (void) const;
        std::string Max             (void) const;

    private:

        struct MetricsType
        {
            std::size_t nLowest;
            std::size_t nHighest;
        };
        

        std::vector< std::map<char, std::size_t> >          m_kCharacterFrequencyColumns;
        std::vector< std::map<std::size_t, std::string> >   m_kCharacterFrequencyColumnsInverse;
        std::vector<MetricsType>                            m_kCharacterFrequencyColumnsMinMax;
};

#endif // __CSTRING_METRICS_H__