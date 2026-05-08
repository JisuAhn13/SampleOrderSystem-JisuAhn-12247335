#include "Monitor.h"
#include "OrderManager.h"
#include "SampleManager.h"
#include "Order.h"
#include "Sample.h"

#include <iostream>

Monitor::Monitor(OrderManager& orderMgr, SampleManager& sampleMgr)
    : m_orderMgr(orderMgr), m_sampleMgr(sampleMgr)
{
}

void Monitor::showOrderStatus() const
{
    std::cout << "=== 주문 현황 ===\n";

    struct Section {
        OrderStatus   status;
        const char*   label;
    };

    const Section sections[] = {
        { OrderStatus::RESERVED,  "[접수]"  },
        { OrderStatus::PRODUCING, "[생산중]" },
        { OrderStatus::CONFIRMED, "[승인]"  },
        { OrderStatus::RELEASE,   "[출고]"  },
    };

    for (const auto& sec : sections) {
        std::cout << sec.label << "\n";
        auto orders = m_orderMgr.getOrdersByStatus(sec.status);
        if (orders.empty()) {
            std::cout << "  (없음)\n";
        } else {
            for (const auto* o : orders) {
                std::cout << "  " << o->getOrderId()
                          << " | " << o->getSample()->getName()
                          << " | " << o->getCustomerName()
                          << " | " << o->getQuantity() << "\n";
            }
        }
    }
}

void Monitor::showStockStatus() const
{
    std::cout << "=== 재고 현황 ===\n";

    auto samples = m_sampleMgr.getAllSamples();
    if (samples.empty()) {
        std::cout << "  (등록된 시료 없음)\n";
        return;
    }

    for (const auto* s : samples) {
        int stock = s->getStock();

        // reservedDemand: 해당 시료의 RESERVED 상태 주문 수량 합계
        int reservedDemand = 0;
        auto reservedOrders = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
        for (const auto* o : reservedOrders) {
            if (o->getSample()->getId() == s->getId()) {
                reservedDemand += o->getQuantity();
            }
        }

        const char* state;
        if (stock == 0) {
            state = "고갈";
        } else if (stock < reservedDemand) {
            state = "부족";
        } else {
            state = "여유";
        }

        std::cout << "  " << s->getId()
                  << " | " << s->getName()
                  << " | " << stock
                  << " | " << state << "\n";
    }
}
