#include "Sample.h"

Sample::Sample(const std::string& id, const std::string& name, double avgProductionTime, double yield, int stock)
    : m_id(id), m_name(name), m_avgProductionTime(avgProductionTime), m_yield(yield), m_stock(stock)
{
}

const std::string& Sample::getId() const { return m_id; }
const std::string& Sample::getName() const { return m_name; }
double Sample::getAvgProductionTime() const { return m_avgProductionTime; }
double Sample::getYield() const { return m_yield; }
int Sample::getStock() const { return m_stock; }

void Sample::setName(const std::string& name) { m_name = name; }
void Sample::setAvgProductionTime(double avgProductionTime) { m_avgProductionTime = avgProductionTime; }
void Sample::setYield(double yield) { m_yield = yield; }

void Sample::addStock(int qty) { m_stock += qty; }

bool Sample::reduceStock(int qty)
{
    if (m_stock < qty) return false;
    m_stock -= qty;
    return true;
}
