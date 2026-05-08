#include "SampleManager.h"

SampleManager::SampleManager()
{
    // vector 재할당에 의한 포인터 무효화 방지
    m_samples.reserve(1000);
}

Sample* SampleManager::registerSample(const std::string& name,
                                       double avgTime, double yield,
                                       int initStock)
{
    m_samples.emplace_back(m_nextId++, name, avgTime, yield, initStock);
    return &m_samples.back();
}

std::vector<Sample*> SampleManager::getAllSamples() const
{
    std::vector<Sample*> result;
    result.reserve(m_samples.size());
    for (auto& s : m_samples) {
        result.push_back(const_cast<Sample*>(&s));
    }
    return result;
}

Sample* SampleManager::findById(int id)
{
    for (auto& s : m_samples) {
        if (s.getId() == id) return &s;
    }
    return nullptr;
}

std::vector<Sample*> SampleManager::searchByName(const std::string& keyword)
{
    std::vector<Sample*> result;
    for (auto& s : m_samples) {
        if (s.getName().find(keyword) != std::string::npos) {
            result.push_back(&s);
        }
    }
    return result;
}
