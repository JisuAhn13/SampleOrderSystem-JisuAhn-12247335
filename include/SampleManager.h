#pragma once

#include "Sample.h"
#include <vector>
#include <string>

class SampleManager {
    std::vector<Sample> m_samples;
public:
    SampleManager();

    Sample* registerSample(const std::string& id,
                           const std::string& name,
                           double avgTime, double yield);  // 중복 ID 시 nullptr 반환

    std::vector<Sample*> getAllSamples() const;
    Sample*              findById(const std::string& id);
    std::vector<Sample*> searchByName(const std::string& keyword);
};
