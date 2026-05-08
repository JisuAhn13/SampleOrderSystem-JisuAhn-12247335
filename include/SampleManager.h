#pragma once

#include "Sample.h"
#include <vector>
#include <string>

class SampleManager {
    std::vector<Sample> m_samples;
    int m_nextId = 1;
public:
    SampleManager();

    Sample* registerSample(const std::string& name,
                           double avgTime, double yield,
                           int initStock = 0);

    std::vector<Sample*> getAllSamples() const;
    Sample*              findById(int id);
    std::vector<Sample*> searchByName(const std::string& keyword);
};
