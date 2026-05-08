#pragma once

#include <string>

class Sample {
public:
    Sample() = default;
    Sample(int id, const std::string& name, double avgProductionTime, double yield, int stock);

    int getId() const;
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
    int m_id = 0;
    std::string m_name;
    double m_avgProductionTime = 0.0;
    double m_yield = 0.0;
    int m_stock = 0;
};
