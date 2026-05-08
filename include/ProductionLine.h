#pragma once

#include "ProductionJob.h"
#include <optional>
#include <queue>
#include <vector>

class Order;

class ProductionLine {
    std::optional<ProductionJob> m_current;
    std::queue<ProductionJob>    m_queue;
    std::vector<ProductionJob>   m_history;
public:
    ProductionLine() = default;

    void enqueue(Order* order, int shortage);
    void tick();

    bool hasCurrent()   const;
    bool isEmpty()      const;
    int  getQueueSize() const;

    const ProductionJob*       getCurrentJob()  const;
    std::vector<ProductionJob> getWaitingJobs() const;
};
