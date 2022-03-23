#include "cstringmetrics.h"

CStringMetrics::CStringMetrics(void) :
    m_kCharacterFrequencyColumns()
{
}

void CStringMetrics::Add(const std::string& kString)
{
    while (m_kCharacterFrequencyColumns.size() < kString.size())
    {
        m_kCharacterFrequencyColumns.push_back(MetricsType());
    }

    for (std::size_t i = 0; i < kString.size(); ++i)
    {
        std::map<char, std::size_t>& kFrequencies = m_kCharacterFrequencyColumns.at(i).kCharacterFrequency;

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
    for (std::vector<MetricsType>::iterator it = m_kCharacterFrequencyColumns.begin(); it != m_kCharacterFrequencyColumns.end(); ++it)
    {
        it->kCharacterFrequencyInverse.clear();

        MetricsType& kMinMax = *it;
        kMinMax.nLowest  = static_cast<std::size_t>(-1);
        kMinMax.nHighest = 0;

        std::map<std::size_t, std::string>& kCharacterFrequencyInverse = kMinMax.kCharacterFrequencyInverse;

        for (std::map<char, std::size_t>::const_iterator itFreq = it->kCharacterFrequency.cbegin(); itFreq != it->kCharacterFrequency.cend(); ++itFreq)
        {
            if (kCharacterFrequencyInverse.find(itFreq->second) == kCharacterFrequencyInverse.end())
            {
                kCharacterFrequencyInverse[itFreq->second] = std::string(1, itFreq->first);
            }
            else
            {
                kCharacterFrequencyInverse[itFreq->second] += itFreq->first;
            }

            if (itFreq->second < kMinMax.nLowest)
            {
                kMinMax.nLowest = itFreq->second;
            }
            if (itFreq->second > kMinMax.nHighest)
            {
                kMinMax.nHighest = itFreq->second;
            }
        }
    }
}

std::string CStringMetrics::Min(void) const
{
    std::string kResult;

    for (std::vector<MetricsType>::const_iterator it = m_kCharacterFrequencyColumns.cbegin(); it != m_kCharacterFrequencyColumns.cend(); ++it)
    {
        kResult += it->kCharacterFrequencyInverse.at(it->nLowest);
    }

    return kResult;
}

std::string CStringMetrics::Max(void) const
{
    std::string kResult;

    for (std::vector<MetricsType>::const_iterator it = m_kCharacterFrequencyColumns.cbegin(); it != m_kCharacterFrequencyColumns.cend(); ++it)
    {
        kResult += it->kCharacterFrequencyInverse.at(it->nHighest);
    }

    return kResult;
}
