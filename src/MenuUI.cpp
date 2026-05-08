#include "MenuUI.h"
#include "SampleManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
#include "ProductionJob.h"
#include "Monitor.h"
#include "ShippingManager.h"
#include "Sample.h"
#include "Order.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

MenuUI::MenuUI(SampleManager& sm, OrderManager& om,
               ProductionLine& pl, Monitor& mo, ShippingManager& sh)
    : m_sampleMgr(sm)
    , m_orderMgr(om)
    , m_productionLine(pl)
    , m_monitor(mo)
    , m_shippingMgr(sh)
{
}

// ─── 공통 유틸 ────────────────────────────────────────────────────────────────

void MenuUI::printHeader(const std::string& title)
{
    std::cout << "\n========================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================\n";
}

void MenuUI::printSeparator()
{
    std::cout << "----------------------------------------\n";
}

int MenuUI::getIntInput(const std::string& prompt)
{
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  숫자를 입력해주세요.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

double MenuUI::getDoubleInput(const std::string& prompt)
{
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  숫자를 입력해주세요.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

std::string MenuUI::getStringInput(const std::string& prompt)
{
    std::string val;
    std::cout << prompt;
    std::getline(std::cin, val);
    return val;
}

// ─── 메인 루프 ────────────────────────────────────────────────────────────────

void MenuUI::run()
{
    while (true) {
        showMainMenu();
        int choice = getIntInput("> ");
        switch (choice) {
            case 1: handleSampleManagement(); break;
            case 2: handleOrderManagement();  break;
            case 3: handleMonitoring();       break;
            case 4: handleShipping();         break;
            case 5: handleProductionLine();   break;
            case 0:
                std::cout << "종료합니다.\n";
                return;
            default:
                std::cout << "  잘못된 입력입니다.\n";
                break;
        }
    }
}

void MenuUI::showMainMenu() const
{
    printHeader("SampleOrderSystem - 메인 메뉴");
    std::cout << "  1. 시료 관리\n";
    std::cout << "  2. 주문 관리\n";
    std::cout << "  3. 모니터링\n";
    std::cout << "  4. 출고 처리\n";
    std::cout << "  5. 생산 라인\n";
    std::cout << "  0. 종료\n";
}

// ─── 시료 관리 ────────────────────────────────────────────────────────────────

void MenuUI::handleSampleManagement()
{
    while (true) {
        m_productionLine.tick();
        printHeader("시료 관리");
        std::cout << "  1. 시료 등록\n";
        std::cout << "  2. 시료 목록 조회\n";
        std::cout << "  3. 시료 이름 검색\n";
        std::cout << "  0. 이전 메뉴\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                std::string id = getStringInput("시료 ID: ");
                if (id.empty()) {
                    std::cout << "  ID를 입력해주세요.\n";
                    continue;
                }
                std::string name = getStringInput("시료 이름: ");
                if (name.empty()) {
                    std::cout << "  시료 이름을 입력해주세요.\n";
                    continue;
                }
                double avgTime = getDoubleInput("평균 생산시간(초): ");
                if (avgTime <= 0) {
                    std::cout << "  0보다 큰 값을 입력해주세요.\n";
                    continue;
                }
                double yield = getDoubleInput("수율 (0.0 초과 ~ 1.0 이하): ");
                if (yield <= 0.0 || yield > 1.0) {
                    std::cout << "  0.0 초과 1.0 이하 값을 입력해주세요.\n";
                    continue;
                }
                Sample* sample = m_sampleMgr.registerSample(id, name, avgTime, yield);
                if (sample == nullptr) {
                    std::cout << "  이미 존재하는 ID입니다.\n";
                    continue;
                }
                std::cout << "  [완료] 시료 등록 성공. ID: " << sample->getId() << "\n";
                break;
            }
            case 2: {
                auto samples = m_sampleMgr.getAllSamples();
                if (samples.empty()) {
                    std::cout << "  등록된 시료가 없습니다.\n";
                } else {
                    printSeparator();
                    std::cout << std::setw(4) << "ID" << "  "
                              << std::setw(16) << std::left  << "이름"
                              << std::setw(12) << std::right << "생산시간(초)"
                              << std::setw(8)               << "수율"
                              << std::setw(8)               << "재고" << "\n";
                    printSeparator();
                    for (Sample* s : samples) {
                        std::cout << std::setw(4) << s->getId() << "  "
                                  << std::setw(16) << std::left  << s->getName()
                                  << std::setw(12) << std::right << std::fixed << std::setprecision(1) << s->getAvgProductionTime()
                                  << std::setw(8)               << std::setprecision(2) << s->getYield()
                                  << std::setw(8)               << s->getStock() << "\n";
                    }
                }
                break;
            }
            case 3: {
                std::string keyword = getStringInput("검색 키워드: ");
                auto results = m_sampleMgr.searchByName(keyword);
                if (results.empty()) {
                    std::cout << "  검색 결과가 없습니다.\n";
                } else {
                    printSeparator();
                    std::cout << std::setw(4) << "ID" << "  "
                              << std::setw(16) << std::left  << "이름"
                              << std::setw(12) << std::right << "생산시간(초)"
                              << std::setw(8)               << "수율"
                              << std::setw(8)               << "재고" << "\n";
                    printSeparator();
                    for (Sample* s : results) {
                        std::cout << std::setw(4) << s->getId() << "  "
                                  << std::setw(16) << std::left  << s->getName()
                                  << std::setw(12) << std::right << std::fixed << std::setprecision(1) << s->getAvgProductionTime()
                                  << std::setw(8)               << std::setprecision(2) << s->getYield()
                                  << std::setw(8)               << s->getStock() << "\n";
                    }
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "  잘못된 입력입니다.\n";
                break;
        }
    }
}

// ─── 주문 관리 ────────────────────────────────────────────────────────────────

void MenuUI::handleOrderManagement()
{
    while (true) {
        m_productionLine.tick();
        printHeader("주문 관리");
        std::cout << "  1. 주문 접수\n";
        std::cout << "  2. 접수 주문 목록\n";
        std::cout << "  3. 주문 승인\n";
        std::cout << "  4. 주문 거절\n";
        std::cout << "  0. 이전 메뉴\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                std::string sampleId = getStringInput("시료 ID: ");
                if (m_sampleMgr.findById(sampleId) == nullptr) {
                    std::cout << "  존재하지 않는 시료 ID입니다.\n";
                    continue;
                }
                std::string customer = getStringInput("고객명: ");
                if (customer.empty()) {
                    std::cout << "  고객명을 입력해주세요.\n";
                    continue;
                }
                int qty = getIntInput("주문 수량: ");
                if (qty <= 0) {
                    std::cout << "  1 이상의 수량을 입력해주세요.\n";
                    continue;
                }
                Order* order = m_orderMgr.placeOrder(sampleId, customer, qty);
                std::cout << "  [완료] 주문 접수 성공. 주문 ID: " << order->getOrderId() << "\n";
                break;
            }
            case 2: {
                auto orders = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
                if (orders.empty()) {
                    std::cout << "  접수된 주문이 없습니다.\n";
                } else {
                    printSeparator();
                    std::cout << std::setw(6)  << "주문ID" << " | "
                              << std::setw(16) << std::left  << "시료명" << " | "
                              << std::setw(12) << std::left  << "고객명" << " | "
                              << std::right    << "수량" << "\n";
                    printSeparator();
                    for (Order* o : orders) {
                        std::cout << std::setw(6)  << o->getOrderId() << " | "
                                  << std::setw(16) << std::left  << o->getSample()->getName() << " | "
                                  << std::setw(12) << std::left  << o->getCustomerName() << " | "
                                  << std::right    << o->getQuantity() << "\n";
                    }
                }
                break;
            }
            case 3: {
                // RESERVED 목록 먼저 표시
                auto reserved = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
                if (reserved.empty()) {
                    std::cout << "  접수된 주문이 없습니다.\n";
                } else {
                    printSeparator();
                    std::cout << std::setw(6)  << "주문ID" << " | "
                              << std::setw(16) << std::left  << "시료명" << " | "
                              << std::setw(12) << std::left  << "고객명" << " | "
                              << std::right    << "수량" << "\n";
                    printSeparator();
                    for (Order* o : reserved) {
                        std::cout << std::setw(6)  << o->getOrderId() << " | "
                                  << std::setw(16) << std::left  << o->getSample()->getName() << " | "
                                  << std::setw(12) << std::left  << o->getCustomerName() << " | "
                                  << std::right    << o->getQuantity() << "\n";
                    }
                }
                int orderId = getIntInput("승인할 주문 ID: ");
                if (m_orderMgr.approveOrder(orderId)) {
                    std::cout << "  [완료] 주문 승인되었습니다.\n";
                } else {
                    std::cout << "  승인 실패: 존재하지 않거나 접수 상태가 아닙니다.\n";
                }
                break;
            }
            case 4: {
                int orderId = getIntInput("거절할 주문 ID: ");
                if (m_orderMgr.rejectOrder(orderId)) {
                    std::cout << "  [완료] 주문이 거절되었습니다.\n";
                } else {
                    std::cout << "  거절 실패: 존재하지 않거나 접수 상태가 아닙니다.\n";
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "  잘못된 입력입니다.\n";
                break;
        }
    }
}

// ─── 모니터링 ────────────────────────────────────────────────────────────────

void MenuUI::handleMonitoring()
{
    while (true) {
        m_productionLine.tick();
        printHeader("모니터링");
        std::cout << "  1. 주문 현황 (상태별)\n";
        std::cout << "  2. 재고 현황\n";
        std::cout << "  0. 이전 메뉴\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1:
                m_monitor.showOrderStatus();
                break;
            case 2:
                m_monitor.showStockStatus();
                break;
            case 0:
                return;
            default:
                std::cout << "  잘못된 입력입니다.\n";
                break;
        }
    }
}

// ─── 출고 처리 ────────────────────────────────────────────────────────────────

void MenuUI::handleShipping()
{
    while (true) {
        m_productionLine.tick();
        printHeader("출고 처리");
        std::cout << "  1. 출고 대기 목록 조회\n";
        std::cout << "  2. 출고 실행\n";
        std::cout << "  0. 이전 메뉴\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1:
                m_shippingMgr.showConfirmedOrders();
                break;
            case 2: {
                auto confirmed = m_orderMgr.getOrdersByStatus(OrderStatus::CONFIRMED);
                if (confirmed.empty()) {
                    std::cout << "  출고 대기 주문이 없습니다.\n";
                    continue;
                }
                m_shippingMgr.showConfirmedOrders();
                int orderId = getIntInput("출고할 주문 ID: ");
                if (m_shippingMgr.ship(orderId)) {
                    std::cout << "  [완료] 출고 처리되었습니다.\n";
                } else {
                    std::cout << "  출고 실패: 승인 상태 주문이 아닙니다.\n";
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "  잘못된 입력입니다.\n";
                break;
        }
    }
}

// ─── 생산 라인 ────────────────────────────────────────────────────────────────

void MenuUI::handleProductionLine()
{
    while (true) {
        m_productionLine.tick();
        printHeader("생산 라인");
        std::cout << "  1. 생산 현황\n";
        std::cout << "  2. 대기 주문 목록\n";
        std::cout << "  0. 이전 메뉴\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                printHeader("생산 라인 - 생산 현황");
                if (m_productionLine.hasCurrent()) {
                    const ProductionJob* job = m_productionLine.getCurrentJob();
                    printSeparator();
                    std::cout << "[현재 생산 중]\n";
                    std::cout << "  주문 ID     : " << job->getOrder()->getOrderId() << "\n";
                    std::cout << "  시료        : " << job->getOrder()->getSample()->getName() << "\n";
                    std::cout << "  실 생산량   : " << job->getTargetQty() << " 개\n";
                    std::cout << "  현재 생산량 : " << job->getProducedQty() << " 개\n";
                    std::cout << "  총 생산시간 : " << std::fixed << std::setprecision(1) << job->getTotalTime() << " 초\n";
                    std::cout << "  잔여 시간   : " << job->getRemainingTime() << " 초\n";
                    printSeparator();
                } else {
                    std::cout << "  현재 생산 중인 작업이 없습니다.\n";
                }
                break;
            }
            case 2: {
                auto waiting = m_productionLine.getWaitingJobs();
                if (waiting.empty()) {
                    std::cout << "  대기 중인 작업이 없습니다.\n";
                } else {
                    printSeparator();
                    std::cout << std::setw(8) << "대기순서" << " | "
                              << std::setw(6) << "주문ID" << " | "
                              << std::setw(16) << std::left << "시료명" << " | "
                              << std::right << "목표수량" << "\n";
                    printSeparator();
                    for (std::size_t i = 0; i < waiting.size(); ++i) {
                        const ProductionJob& job = waiting[i];
                        std::cout << std::setw(8) << (i + 1) << " | "
                                  << std::setw(6) << job.getOrder()->getOrderId() << " | "
                                  << std::setw(16) << std::left << job.getOrder()->getSample()->getName() << " | "
                                  << std::right << job.getTargetQty() << "\n";
                    }
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "  잘못된 입력입니다.\n";
                break;
        }
    }
}
