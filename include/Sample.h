#pragma once

#include <string>

class Sample {
public:
    Sample() = default;
    Sample(const std::string& id, const std::string& name, double avgProductionTime, double yield, int stock);

    const std::string& getId() const;
    const std::string& getName() const;
    double getAvgProductionTime() const;
    double getYield() const;
    int getStock() const;

    void setName(const std::string& name);
    void setAvgProductionTime(double avgProductionTime);
    void setYield(double yield);

    void addStock(int qty);
    bool reduceStock(int qty);

private:
    std::string m_id;
    std::string m_name;
    double m_avgProductionTime = 0.0;
    double m_yield = 0.0;
    int m_stock = 0;
};
