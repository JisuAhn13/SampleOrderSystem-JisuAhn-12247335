#include "ShippingManager.h"
#include "OrderManager.h"
#include "MenuUI.h"
#include "Order.h"
#include "Sample.h"

#include <iostream>
#include <iomanip>
#include <sstream>

ShippingManager::ShippingManager(OrderManager& orderMgr)
    : m_orderMgr(orderMgr)
{
}

void ShippingManager::showConfirmedOrders() const
{
    auto orders = m_orderMgr.getOrdersByStatus(OrderStatus::CONFIRMED);
    if (orders.empty()) {
        MenuUI::printBoxLine("   출고 대기 주문이 없습니다.");
        return;
    }

    MenuUI::printBoxLine("   주문ID   시료명            고객명         수량");
    MenuUI::printBoxMid();
    for (const auto* o : orders) {
        std::ostringstream row;
        row << "   " << std::left << std::setw(7) << o->getOrderId()
            << std::setw(16) << o->getSample()->getName()
            << std::setw(13) << o->getCustomerName()
            << std::right << o->getQuantity() << " 개";
        MenuUI::printBoxLine(row.str());
    }
}

bool ShippingManager::ship(int orderId)
{
    Order* order = m_orderMgr.findById(orderId);
    if (order == nullptr || order->getStatus() != OrderStatus::CONFIRMED) return false;

    order->setStatus(OrderStatus::RELEASE);
    return true;
}
