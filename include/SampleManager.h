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
                           double avgTime, double yield);

    std::vector<Sample*> getAllSamples();
    Sample*              findById(const std::string& id);
    std::vector<Sample*> searchByName(const std::string& keyword);
};
