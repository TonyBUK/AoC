#include "cstringmetrics.h"

CStringMetrics::CStringMetrics(void) :
    m_kCharacterFrequencyColumns(),
    m_kCharacterFrequencyColumnsInverse(),
    m_kCharacterFrequencyColumnsMinMax()
{
}

void CStringMetrics::Add(const std::string& kString)
{
    while (m_kCharacterFrequencyColumns.size() < kString.size())
    {
        m_kCharacterFrequencyColumns.push_back(std::map<char, std::size_t>());
        m_kCharacterFrequencyColumnsMinMax.push_back(MetricsType());
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
        MetricsType& kMinMax = m_kCharacterFrequencyColumnsMinMax.at(i);
        kMinMax.nLowest  = static_cast<std::size_t>(-1);
        kMinMax.nHighest = 0;

        m_kCharacterFrequencyColumnsInverse.push_back(std::map<std::size_t, std::string>());
        std::map<std::size_t, std::string>& kCharacterFrequencyInverse = m_kCharacterFrequencyColumnsInverse.back();

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

    for (std::size_t i = 0; i < m_kCharacterFrequencyColumns.size(); ++i)
    {
        kResult += m_kCharacterFrequencyColumnsInverse.at(i).at(m_kCharacterFrequencyColumnsMinMax.at(i).nLowest);
    }

    return kResult;
}

std::string CStringMetrics::Max(void) const
{
    std::string kResult;

    for (std::size_t i = 0; i < m_kCharacterFrequencyColumns.size(); ++i)
    {
        kResult += m_kCharacterFrequencyColumnsInverse.at(i).at(m_kCharacterFrequencyColumnsMinMax.at(i).nHighest);
    }

    return kResult;
}
