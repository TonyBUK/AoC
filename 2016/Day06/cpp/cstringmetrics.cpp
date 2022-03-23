#include "cstringmetrics.h"

CStringMetrics::CStringMetrics(void) :
    m_kCharacterFrequencyColumns(),
    m_kCharacterFrequencyColumnsInverse()
{
}

void CStringMetrics::Add(const std::string& kString)
{
    while (m_kCharacterFrequencyColumns.size() < kString.size())
    {
        m_kCharacterFrequencyColumns.push_back(std::map<char, std::size_t>());
    }

    for (std::size_t i = 0; i < kString.size(); ++i)
    {
        std::map<char, std::size_t>& kFrequencies = m_kCharacterFrequencyColumns.at(i);

        if (kFrequencies.find(kString.at(i)) == kFrequencies.end())
        {
            kFrequencies[kString.at(i)] = 1;
        }
        else
        {
            ++kFrequencies[kString.at(i)];
        }
    }
}

void CStringMetrics::Calculate(void)
{
    m_kCharacterFrequencyColumnsInverse.clear();

    for (std::size_t i = 0; i < m_kCharacterFrequencyColumns.size(); ++i)
    {
        m_kCharacterFrequencyColumnsInverse.push_back(MetricsType());
        MetricsType& kMinMax = m_kCharacterFrequencyColumnsInverse.back();
        kMinMax.nLowest  = static_cast<std::size_t>(-1);
        kMinMax.nHighest = 0;

        std::map<std::size_t, std::string>& kCharacterFrequencyInverse = kMinMax.kCharacterFrequency;

        for (std::map<char, std::size_t>::const_iterator it = m_kCharacterFrequencyColumns.at(i).cbegin(); it != m_kCharacterFrequencyColumns.at(i).cend(); ++it)
        {
            if (kCharacterFrequencyInverse.find(it->second) == kCharacterFrequencyInverse.end())
            {
                kCharacterFrequencyInverse[it->second] = std::string(1, it->first);
            }
            else
            {
                kCharacterFrequencyInverse[it->second] += it->first;
            }

            if (it->second < kMinMax.nLowest)
            {
                kMinMax.nLowest = it->second;
            }
            if (it->second > kMinMax.nHighest)
            {
                kMinMax.nHighest = it->second;
            }
        }
    }
}

std::string CStringMetrics::Min(void) const
{
    std::string kResult;

    for (std::vector<MetricsType>::const_iterator it = m_kCharacterFrequencyColumnsInverse.cbegin(); it != m_kCharacterFrequencyColumnsInverse.cend(); ++it)
    {
        kResult += it->kCharacterFrequency.at(it->nLowest);
    }

    return kResult;
}

std::string CStringMetrics::Max(void) const
{
    std::string kResult;

    for (std::vector<MetricsType>::const_iterator it = m_kCharacterFrequencyColumnsInverse.cbegin(); it != m_kCharacterFrequencyColumnsInverse.cend(); ++it)
    {
        kResult += it->kCharacterFrequency.at(it->nHighest);
    }

    return kResult;
}
