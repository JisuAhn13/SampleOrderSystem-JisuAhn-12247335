#include "ShippingManager.h"
#include "OrderManager.h"
#include "Order.h"
#include "Sample.h"

#include <iostream>

ShippingManager::ShippingManager(OrderManager& orderMgr)
    : m_orderMgr(orderMgr)
{
}

void ShippingManager::showConfirmedOrders() const
{
    std::cout << "=== 출고 대기 주문 ===\n";

    auto orders = m_orderMgr.getOrdersByStatus(OrderStatus::CONFIRMED);
    if (orders.empty()) {
        std::cout << "  (출고 대기 주문 없음)\n";
        return;
    }

    for (const auto* o : orders) {
        std::cout << "  " << o->getOrderId()
                  << " | " << o->getSample()->getName()
                  << " | " << o->getCustomerName()
                  << " | " << o->getQuantity() << "\n";
    }
}

bool ShippingManager::ship(int orderId)
{
    Order* order = m_orderMgr.findById(orderId);
    if (order == nullptr || order->getStatus() != OrderStatus::CONFIRMED) return false;

    order->setStatus(OrderStatus::RELEASE);
    return true;
}
