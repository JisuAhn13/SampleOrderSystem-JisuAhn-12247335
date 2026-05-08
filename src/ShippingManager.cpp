#include "ShippingManager.h"
#include "OrderManager.h"
#include "ConsoleUI.h"
#include "Order.h"
#include "Sample.h"

#include <iostream>
#include <iomanip>
#include <sstream>

ShippingManager::ShippingManager(OrderManager& orderMgr)
    : m_orderMgr(orderMgr)
{
}

void ShippingManager::showConfirmedOrders()
{
    auto orders = m_orderMgr.getOrdersByStatus(OrderStatus::CONFIRMED);
    if (orders.empty()) {
        ConsoleUI::printBoxLine("   출고 대기 주문이 없습니다.");
        return;
    }

    ConsoleUI::printBoxLine("   주문ID   시료명            고객명         수량");
    ConsoleUI::printBoxMid();
    for (const auto* o : orders) {
        std::ostringstream row;
        row << "   " << std::left << std::setw(7) << o->getOrderId()
            << std::setw(16) << o->getSample()->getName()
            << std::setw(13) << o->getCustomerName()
            << std::right << o->getQuantity() << " 개";
        ConsoleUI::printBoxLine(row.str());
    }
}

bool ShippingManager::ship(int orderId)
{
    Order* order = m_orderMgr.findById(orderId);
    if (order == nullptr || order->getStatus() != OrderStatus::CONFIRMED) return false;

    order->setStatus(OrderStatus::RELEASE);
    return true;
}
