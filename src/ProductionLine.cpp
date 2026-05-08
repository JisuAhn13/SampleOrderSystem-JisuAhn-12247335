#include "ProductionLine.h"
#include "Order.h"

void ProductionLine::enqueue(Order* order, int shortage)
{
    ProductionJob job(order, shortage);
    if (!m_current.has_value()) {
        m_current = job;
        m_current->start();
    } else {
        m_queue.push(job);
    }
}

void ProductionLine::tick()
{
    if (!m_current.has_value()) return;

    if (m_current->update()) {
        m_current->complete();
        m_history.push_back(*m_current);
        m_current.reset();

        if (!m_queue.empty()) {
            m_current = m_queue.front();
            m_queue.pop();
            m_current->start();
        }
    }
}

bool ProductionLine::hasCurrent() const
{
    return m_current.has_value();
}

bool ProductionLine::isEmpty() const
{
    return !m_current.has_value() && m_queue.empty();
}

int ProductionLine::getQueueSize() const
{
    return static_cast<int>(m_queue.size());
}

const ProductionJob* ProductionLine::getCurrentJob() const
{
    return m_current.has_value() ? &m_current.value() : nullptr;
}

std::vector<ProductionJob> ProductionLine::getWaitingJobs() const
{
    std::vector<ProductionJob> result;
    std::queue<ProductionJob> temp = m_queue;
    while (!temp.empty()) {
        result.push_back(temp.front());
        temp.pop();
    }
    return result;
}
