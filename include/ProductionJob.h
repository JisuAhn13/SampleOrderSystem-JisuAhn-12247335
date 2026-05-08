#pragma once

#include <chrono>

class Order;

class ProductionJob {
    using Clock     = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    ProductionJob() = default;
    ProductionJob(Order* order, int shortage);

    void   start();
    bool   update();
    void   complete();

    Order* getOrder()         const;
    int    getTargetQty()     const;
    double getTotalTime()     const;
    int    getProducedQty()   const;
    double getElapsedTime()   const;
    double getRemainingTime() const;
    bool   isStarted()        const;

private:
    Order*     m_order       = nullptr;
    int        m_targetQty   = 0;
    double     m_totalTime   = 0.0;
    TimePoint  m_startTime;
    int        m_producedQty = 0;
    bool       m_started     = false;
};
