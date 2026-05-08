#include "ProductionJob.h"
#include "Order.h"
#include "Sample.h"

#include <cmath>

ProductionJob::ProductionJob(Order* order, int shortage)
    : m_order(order)
{
    double yield = order->getSample()->getYield();
    m_targetQty  = static_cast<int>(std::ceil(static_cast<double>(shortage) / (yield * 0.9)));
    m_totalTime  = order->getSample()->getAvgProductionTime() * m_targetQty;
}

Order* ProductionJob::getOrder()       const { return m_order; }
int    ProductionJob::getTargetQty()   const { return m_targetQty; }
double ProductionJob::getTotalTime()   const { return m_totalTime; }
int    ProductionJob::getProducedQty() const { return m_producedQty; }
bool   ProductionJob::isStarted()      const { return m_started; }

double ProductionJob::getElapsedTime() const
{
    if (!m_started) return 0.0;
    auto now = Clock::now();
    return std::chrono::duration<double>(now - m_startTime).count();
}

double ProductionJob::getRemainingTime() const
{
    double remaining = m_totalTime - getElapsedTime();
    return remaining > 0.0 ? remaining : 0.0;
}

void ProductionJob::start()
{
    m_startTime = Clock::now();
    m_started   = true;
}

bool ProductionJob::update()
{
    if (!m_started || m_order == nullptr) return false;

    double avgTime = m_order->getSample()->getAvgProductionTime();
    double elapsed = getElapsedTime();

    m_producedQty = static_cast<int>(std::floor(elapsed / avgTime));
    if (m_producedQty > m_targetQty) m_producedQty = m_targetQty;

    return m_producedQty >= m_targetQty;
}

void ProductionJob::complete()
{
    m_producedQty = m_targetQty;
    Sample* s = m_order->getSample();
    s->addStock(m_targetQty);
    s->reduceStock(m_order->getQuantity());
    m_order->setStatus(OrderStatus::CONFIRMED);
}
