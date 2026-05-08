#include "Monitor.h"
#include "OrderManager.h"
#include "SampleManager.h"
#include "Order.h"
#include "Sample.h"
#include "ConsoleUI.h"

#include <iostream>
#include <iomanip>
#include <sstream>

Monitor::Monitor(OrderManager& orderMgr, SampleManager& sampleMgr)
    : m_orderMgr(orderMgr), m_sampleMgr(sampleMgr)
{
}

void Monitor::showOrderStatus() const
{
    struct Section { OrderStatus status; const char* label; const char* icon; };
    const Section sections[] = {
        { OrderStatus::RESERVED,  "RESERVED",  "◇" },
        { OrderStatus::PRODUCING, "PRODUCING", "◉" },
        { OrderStatus::CONFIRMED, "CONFIRMED", "✔" },
        { OrderStatus::RELEASE,   "RELEASE",   "►" },
    };

    for (const auto& sec : sections) {
        std::string header = std::string("  ") + sec.icon + "  [ " + sec.label + " ]";
        ConsoleUI::printBoxLine(header);
        auto orders = m_orderMgr.getOrdersByStatus(sec.status);
        if (orders.empty()) {
            ConsoleUI::printBoxLine("       (없음)");
        } else {
            for (const auto* o : orders) {
                std::ostringstream row;
                row << "     " << std::left << std::setw(6) << o->getOrderId()
                    << std::setw(14) << o->getSample()->getName()
                    << std::setw(12) << o->getCustomerName()
                    << std::right << o->getQuantity() << " 개";
                ConsoleUI::printBoxLine(row.str());
            }
        }
    }
}

void Monitor::showStockStatus() const
{
    auto samples = m_sampleMgr.getAllSamples();
    if (samples.empty()) {
        ConsoleUI::printBoxLine("   등록된 시료가 없습니다.");
        return;
    }

    ConsoleUI::printBoxLine("   ID           이름           재고  대기주문  상태");
    ConsoleUI::printBoxMid();

    auto reservedOrders = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);

    for (const auto* s : samples) {
        int stock = s->getStock();

        int reservedDemand = 0;
        for (const auto* o : reservedOrders) {
            if (o->getSample()->getId() == s->getId())
                reservedDemand += o->getQuantity();
        }

        const char* state;
        const char* icon;
        if (stock == 0) {
            state = "고갈"; icon = "✕";
        } else if (stock < reservedDemand) {
            state = "부족"; icon = "!";
        } else {
            state = "여유"; icon = "✔";
        }

        std::ostringstream row;
        row << "   " << std::left << std::setw(12) << s->getId()
            << std::setw(14) << s->getName()
            << std::right << std::setw(5) << stock
            << std::setw(9) << reservedDemand
            << "   " << icon << " " << state;
        ConsoleUI::printBoxLine(row.str());
    }
}
