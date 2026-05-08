#pragma once

#include "Order.h"
#include <vector>
#include <string>

class SampleManager;
class ProductionLine;

class OrderManager {
public:
    OrderManager(SampleManager& sampleMgr, ProductionLine& productionLine);

    Order* placeOrder(const std::string& sampleId, const std::string& customerName, int quantity);
    bool   approveOrder(int orderId);
    bool   rejectOrder(int orderId);

    std::vector<Order*> getOrdersByStatus(OrderStatus status);
    std::vector<Order*> getAllActiveOrders();  // REJECTED 제외
    Order*              findById(int orderId);

private:
    SampleManager&  m_sampleMgr;
    ProductionLine& m_productionLine;
    std::vector<Order> m_orders;
    int m_nextId = 1;
};
