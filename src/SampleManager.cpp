#include "SampleManager.h"

SampleManager::SampleManager()
{
    // vector 재할당에 의한 포인터 무효화 방지
    m_samples.reserve(1000);
}

Sample* SampleManager::registerSample(const std::string& id,
                                       const std::string& name,
                                       double avgTime, double yield)
{
    if (findById(id) != nullptr) return nullptr;  // 중복 ID 거부
    m_samples.emplace_back(id, name, avgTime, yield, 0);
    return &m_samples.back();
}

std::vector<Sample*> SampleManager::getAllSamples()
{
    std::vector<Sample*> result;
    result.reserve(m_samples.size());
    for (auto& s : m_samples) {
        result.push_back(&s);
    }
    return result;
}

Sample* SampleManager::findById(const std::string& id)
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
