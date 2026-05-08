#pragma once

class OrderManager;
class SampleManager;

class Monitor {
public:
    Monitor(OrderManager& orderMgr, SampleManager& sampleMgr);

    void showOrderStatus() const;
    void showStockStatus() const;

private:
    OrderManager& m_orderMgr;
    SampleManager& m_sampleMgr;
};
