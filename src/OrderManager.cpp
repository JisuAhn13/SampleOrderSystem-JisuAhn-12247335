#include "OrderManager.h"
#include "SampleManager.h"
#include "ProductionLine.h"
#include "Sample.h"

OrderManager::OrderManager(SampleManager& sampleMgr, ProductionLine& productionLine)
    : m_sampleMgr(sampleMgr), m_productionLine(productionLine)
{
    m_orders.reserve(1000);
}

Order* OrderManager::placeOrder(const std::string& sampleId, const std::string& customerName, int quantity)
{
    Sample* sample = m_sampleMgr.findById(sampleId);
    if (sample == nullptr) return nullptr;

    m_orders.emplace_back(m_nextId++, sample, customerName, quantity);
    return &m_orders.back();
}

bool OrderManager::approveOrder(int orderId)
{
    Order* order = findById(orderId);
    if (order == nullptr || order->getStatus() != OrderStatus::RESERVED) return false;

    Sample* sample = order->getSample();
    int stock    = sample->getStock();
    int quantity = order->getQuantity();

    if (stock >= quantity) {
        // 재고 충분: 즉시 차감 후 CONFIRMED
        sample->reduceStock(quantity);
        order->setStatus(OrderStatus::CONFIRMED);
    } else {
        // 재고 부족: shortage를 그대로 enqueue (ProductionJob 생성자가 targetQty 계산)
        int shortage = quantity - stock;
        order->setStatus(OrderStatus::PRODUCING);
        m_productionLine.enqueue(order, shortage);
    }

    return true;
}

bool OrderManager::rejectOrder(int orderId)
{
    Order* order = findById(orderId);
    if (order == nullptr || order->getStatus() != OrderStatus::RESERVED) return false;

    order->setStatus(OrderStatus::REJECTED);
    return true;
}

Order* OrderManager::findById(int orderId)
{
    for (auto& o : m_orders) {
        if (o.getOrderId() == orderId) return &o;
    }
    return nullptr;
}

std::vector<Order*> OrderManager::getOrdersByStatus(OrderStatus status)
{
    std::vector<Order*> result;
    for (auto& o : m_orders) {
        if (o.getStatus() == status) result.push_back(&o);
    }
    return result;
}

std::vector<Order*> OrderManager::getAllActiveOrders()
{
    std::vector<Order*> result;
    for (auto& o : m_orders) {
        if (o.getStatus() != OrderStatus::REJECTED) result.push_back(&o);
    }
    return result;
}
