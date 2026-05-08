#include "MenuUI.h"
#include "ConsoleUI.h"
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
#include <sstream>
#include <limits>
#include <string>
#include <cstdlib>

MenuUI::MenuUI(SampleManager& sm, OrderManager& om,
               ProductionLine& pl, Monitor& mo, ShippingManager& sh)
    : m_sampleMgr(sm)
    , m_orderMgr(om)
    , m_productionLine(pl)
    , m_monitor(mo)
    , m_shippingMgr(sh)
{
}

// ─── 입출력 유틸 ──────────────────────────────────────────────────────────────

void MenuUI::clearScreen()  { system("cls"); }

void MenuUI::printProgressBar(int current, int total, int barWidth)
{
    int filled = (total > 0) ? (current * barWidth / total) : 0;
    if (filled > barWidth) filled = barWidth;
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
        std::cout << (i < filled ? "█" : "░");
    std::cout << "]";
}

void MenuUI::printHeader(const std::string& title)
{
    ConsoleUI::printBoxMid();
    ConsoleUI::printBoxLine("  " + title);
    ConsoleUI::printBoxMid();
}

void MenuUI::printSeparator()
{
    ConsoleUI::printBoxMid();
}

int MenuUI::getIntInput(const std::string& prompt)
{
    int val;
    while (true) {
        std::cout << std::string(ConsoleUI::LEFT_PAD, ' ') << prompt;
        if (std::cin >> val) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << std::string(ConsoleUI::LEFT_PAD, ' ') << "  숫자를 입력해주세요.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

double MenuUI::getDoubleInput(const std::string& prompt)
{
    double val;
    while (true) {
        std::cout << std::string(ConsoleUI::LEFT_PAD, ' ') << prompt;
        if (std::cin >> val) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << std::string(ConsoleUI::LEFT_PAD, ' ') << "  숫자를 입력해주세요.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

std::string MenuUI::getStringInput(const std::string& prompt)
{
    std::string val;
    std::cout << std::string(ConsoleUI::LEFT_PAD, ' ') << prompt;
    std::getline(std::cin, val);
    return val;
}

// ─── 메인 루프 ────────────────────────────────────────────────────────────────

void MenuUI::run()
{
    while (true) {
        clearScreen();
        showMainMenu();
        int choice = getIntInput("> ");
        switch (choice) {
            case 1: handleSampleManagement(); break;
            case 2: handleOrderManagement();  break;
            case 3: handleMonitoring();       break;
            case 4: handleShipping();         break;
            case 5: handleProductionLine();   break;
            case 0:
                clearScreen();
                std::cout << "\n" << std::string(ConsoleUI::LEFT_PAD, ' ')
                          << "  종료합니다. 감사합니다.\n\n";
                return;
            default:
                ConsoleUI::printBoxLine("  잘못된 입력입니다.");
                break;
        }
    }
}

void MenuUI::showMainMenu() const
{
    std::cout << "\n\n";
    ConsoleUI::printBoxTop();
    ConsoleUI::printBoxLine("       SampleOrderSystem  주문 관리 시스템");
    ConsoleUI::printBoxLine("          반도체 시료 생산 · 출고 플랫폼");
    ConsoleUI::printBoxMid();
    ConsoleUI::printBoxEmpty();
    ConsoleUI::printBoxLine("   [ 1 ]   ◈  시료 관리      Sample Management");
    ConsoleUI::printBoxLine("   [ 2 ]   ◇  주문 관리      Order  Management");
    ConsoleUI::printBoxLine("   [ 3 ]   ◎  모니터링       Monitoring");
    ConsoleUI::printBoxLine("   [ 4 ]   ►  출고 처리      Shipping");
    ConsoleUI::printBoxLine("   [ 5 ]   ◉  생산 라인      Production Line");
    ConsoleUI::printBoxEmpty();
    ConsoleUI::printBoxMid();
    ConsoleUI::printBoxLine("   [ 0 ]   ✕  종료");
    ConsoleUI::printBoxBot();
    std::cout << "\n";
}

// ─── 시료 관리 ────────────────────────────────────────────────────────────────

void MenuUI::handleSampleManagement()
{
    while (true) {
        m_productionLine.tick();
        clearScreen();
        std::cout << "\n";
        ConsoleUI::printBoxTop();
        ConsoleUI::printBoxLine("                ◈  시료 관리");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 1 ]   새 시료 등록");
        ConsoleUI::printBoxLine("   [ 2 ]   시료 목록 조회");
        ConsoleUI::printBoxLine("   [ 3 ]   시료 이름 검색");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        ConsoleUI::printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◈  시료 등록");
                ConsoleUI::printBoxMid();
                std::string id = getStringInput("  시료 ID       : ");
                if (id.empty()) {
                    ConsoleUI::printBoxLine("  ! ID를 입력해주세요.");
                    ConsoleUI::printBoxBot();
                    std::cin.get();
                    continue;
                }
                std::string name = getStringInput("  시료 이름     : ");
                if (name.empty()) {
                    ConsoleUI::printBoxLine("  ! 시료 이름을 입력해주세요.");
                    ConsoleUI::printBoxBot();
                    std::cin.get();
                    continue;
                }
                double avgTime = getDoubleInput("  평균 생산시간(s) : ");
                if (avgTime <= 0) {
                    ConsoleUI::printBoxLine("  ! 0보다 큰 값을 입력해주세요.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                double yield = getDoubleInput("  수율 (0.0~1.0)   : ");
                if (yield <= 0.0 || yield > 1.0) {
                    ConsoleUI::printBoxLine("  ! 0.0 초과 1.0 이하 값을 입력해주세요.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                Sample* sample = m_sampleMgr.registerSample(id, name, avgTime, yield);
                ConsoleUI::printBoxMid();
                if (sample == nullptr) {
                    ConsoleUI::printBoxLine("  ! 이미 존재하는 ID입니다.");
                } else {
                    ConsoleUI::printBoxLine("  ✔ 시료 등록 성공  ID: " + sample->getId());
                }
                ConsoleUI::printBoxBot();
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◈  시료 목록");
                ConsoleUI::printBoxMid();
                auto samples = m_sampleMgr.getAllSamples();
                if (samples.empty()) {
                    ConsoleUI::printBoxLine("   등록된 시료가 없습니다.");
                } else {
                    ConsoleUI::printBoxLine("   ID           이름            생산시간   수율   재고");
                    ConsoleUI::printBoxMid();
                    for (Sample* s : samples) {
                        std::ostringstream oss;
                        oss << "   " << std::left << std::setw(12) << s->getId()
                            << std::setw(14) << s->getName()
                            << std::right << std::setw(6) << std::fixed
                            << std::setprecision(1) << s->getAvgProductionTime()
                            << "s   " << std::setprecision(2) << s->getYield()
                            << "   " << s->getStock();
                        ConsoleUI::printBoxLine(oss.str());
                    }
                }
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 3: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◈  시료 검색");
                ConsoleUI::printBoxMid();
                std::string keyword = getStringInput("  검색 키워드: ");
                auto results = m_sampleMgr.searchByName(keyword);
                ConsoleUI::printBoxMid();
                if (results.empty()) {
                    ConsoleUI::printBoxLine("   검색 결과가 없습니다.");
                } else {
                    ConsoleUI::printBoxLine("   ID           이름            생산시간   수율   재고");
                    ConsoleUI::printBoxMid();
                    for (Sample* s : results) {
                        std::ostringstream oss;
                        oss << "   " << std::left << std::setw(12) << s->getId()
                            << std::setw(14) << s->getName()
                            << std::right << std::setw(6) << std::fixed
                            << std::setprecision(1) << s->getAvgProductionTime()
                            << "s   " << std::setprecision(2) << s->getYield()
                            << "   " << s->getStock();
                        ConsoleUI::printBoxLine(oss.str());
                    }
                }
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 0: return;
            default:
                ConsoleUI::printBoxLine("  ! 잘못된 입력입니다.");
                break;
        }
    }
}

// ─── 주문 관리 ────────────────────────────────────────────────────────────────

void MenuUI::handleOrderManagement()
{
    while (true) {
        m_productionLine.tick();
        clearScreen();
        std::cout << "\n";
        ConsoleUI::printBoxTop();
        ConsoleUI::printBoxLine("                ◇  주문 관리");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 1 ]   새 주문 접수");
        ConsoleUI::printBoxLine("   [ 2 ]   접수 주문 목록 (RESERVED)");
        ConsoleUI::printBoxLine("   [ 3 ]   주문 승인");
        ConsoleUI::printBoxLine("   [ 4 ]   주문 거절");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        ConsoleUI::printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◇  주문 접수");
                ConsoleUI::printBoxMid();
                std::string sampleId = getStringInput("  시료 ID  : ");
                if (m_sampleMgr.findById(sampleId) == nullptr) {
                    ConsoleUI::printBoxLine("  ! 존재하지 않는 시료 ID입니다.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                std::string customer = getStringInput("  고객명   : ");
                if (customer.empty()) {
                    ConsoleUI::printBoxLine("  ! 고객명을 입력해주세요.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                int qty = getIntInput("  주문 수량: ");
                if (qty <= 0) {
                    ConsoleUI::printBoxLine("  ! 1 이상의 수량을 입력해주세요.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                Order* order = m_orderMgr.placeOrder(sampleId, customer, qty);
                ConsoleUI::printBoxMid();
                std::ostringstream oss;
                oss << "  ✔ 주문 접수 완료  주문 ID: " << order->getOrderId();
                ConsoleUI::printBoxLine(oss.str());
                ConsoleUI::printBoxBot();
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◇  접수 주문 목록");
                ConsoleUI::printBoxMid();
                auto orders = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
                if (orders.empty()) {
                    ConsoleUI::printBoxLine("   접수된 주문이 없습니다.");
                } else {
                    ConsoleUI::printBoxLine("   주문ID   시료명            고객명         수량");
                    ConsoleUI::printBoxMid();
                    for (Order* o : orders) {
                        std::ostringstream row;
                        row << "   " << std::left << std::setw(7) << o->getOrderId()
                            << std::setw(16) << o->getSample()->getName()
                            << std::setw(13) << o->getCustomerName()
                            << std::right << o->getQuantity();
                        ConsoleUI::printBoxLine(row.str());
                    }
                }
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 3: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◇  주문 승인");
                ConsoleUI::printBoxMid();
                auto reserved = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
                if (reserved.empty()) {
                    ConsoleUI::printBoxLine("   접수된 주문이 없습니다.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                ConsoleUI::printBoxLine("   주문ID   시료명            고객명         수량");
                ConsoleUI::printBoxMid();
                for (Order* o : reserved) {
                    std::ostringstream row;
                    row << "   " << std::left << std::setw(7) << o->getOrderId()
                        << std::setw(16) << o->getSample()->getName()
                        << std::setw(13) << o->getCustomerName()
                        << std::right << o->getQuantity();
                    ConsoleUI::printBoxLine(row.str());
                }
                ConsoleUI::printBoxMid();
                int orderId = getIntInput("  승인할 주문 ID: ");
                if (m_orderMgr.approveOrder(orderId)) {
                    ConsoleUI::printBoxLine("  ✔ 주문 승인되었습니다.");
                } else {
                    ConsoleUI::printBoxLine("  ! 승인 실패: 존재하지 않거나 접수 상태가 아닙니다.");
                }
                ConsoleUI::printBoxBot();
                break;
            }
            case 4: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◇  주문 거절");
                ConsoleUI::printBoxMid();
                int orderId = getIntInput("  거절할 주문 ID: ");
                if (m_orderMgr.rejectOrder(orderId)) {
                    ConsoleUI::printBoxLine("  ✔ 주문이 거절되었습니다.");
                } else {
                    ConsoleUI::printBoxLine("  ! 거절 실패: 존재하지 않거나 접수 상태가 아닙니다.");
                }
                ConsoleUI::printBoxBot();
                break;
            }
            case 0: return;
            default:
                ConsoleUI::printBoxLine("  ! 잘못된 입력입니다.");
                break;
        }
    }
}

// ─── 모니터링 ────────────────────────────────────────────────────────────────

void MenuUI::handleMonitoring()
{
    while (true) {
        m_productionLine.tick();
        clearScreen();
        std::cout << "\n";
        ConsoleUI::printBoxTop();
        ConsoleUI::printBoxLine("                ◎  모니터링");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 1 ]   주문 현황  (상태별)");
        ConsoleUI::printBoxLine("   [ 2 ]   재고 현황");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        ConsoleUI::printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◎  주문 현황");
                ConsoleUI::printBoxMid();
                m_monitor.showOrderStatus();
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ◎  재고 현황");
                ConsoleUI::printBoxMid();
                m_monitor.showStockStatus();
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 0: return;
            default:
                ConsoleUI::printBoxLine("  ! 잘못된 입력입니다.");
                break;
        }
    }
}

// ─── 출고 처리 ────────────────────────────────────────────────────────────────

void MenuUI::handleShipping()
{
    while (true) {
        m_productionLine.tick();
        clearScreen();
        std::cout << "\n";
        ConsoleUI::printBoxTop();
        ConsoleUI::printBoxLine("                ►  출고 처리");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 1 ]   출고 대기 목록 조회");
        ConsoleUI::printBoxLine("   [ 2 ]   출고 실행");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        ConsoleUI::printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ►  출고 대기 목록");
                ConsoleUI::printBoxMid();
                m_shippingMgr.showConfirmedOrders();
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("              ►  출고 실행");
                ConsoleUI::printBoxMid();
                auto confirmed = m_orderMgr.getOrdersByStatus(OrderStatus::CONFIRMED);
                if (confirmed.empty()) {
                    ConsoleUI::printBoxLine("   출고 대기 주문이 없습니다.");
                    ConsoleUI::printBoxBot();
                    continue;
                }
                m_shippingMgr.showConfirmedOrders();
                ConsoleUI::printBoxMid();
                int orderId = getIntInput("  출고할 주문 ID: ");
                if (m_shippingMgr.ship(orderId)) {
                    ConsoleUI::printBoxLine("  ✔ 출고 처리되었습니다.");
                } else {
                    ConsoleUI::printBoxLine("  ! 출고 실패: 승인 상태 주문이 아닙니다.");
                }
                ConsoleUI::printBoxBot();
                break;
            }
            case 0: return;
            default:
                ConsoleUI::printBoxLine("  ! 잘못된 입력입니다.");
                break;
        }
    }
}

// ─── 생산 라인 ────────────────────────────────────────────────────────────────

void MenuUI::handleProductionLine()
{
    while (true) {
        m_productionLine.tick();
        clearScreen();
        std::cout << "\n";
        ConsoleUI::printBoxTop();
        ConsoleUI::printBoxLine("                ◉  생산 라인");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 1 ]   생산 현황 (현재 진행 중인 Job)");
        ConsoleUI::printBoxLine("   [ 2 ]   대기 큐 목록");
        ConsoleUI::printBoxMid();
        ConsoleUI::printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        ConsoleUI::printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("           ◉  생산 라인 - 생산 현황");
                ConsoleUI::printBoxMid();
                if (m_productionLine.hasCurrent()) {
                    const ProductionJob* job = m_productionLine.getCurrentJob();
                    int    qty  = job->getOrder()->getQuantity();
                    int    sht  = job->getShortage();
                    int    tgt  = job->getTargetQty();
                    int    cur  = job->getProducedQty();
                    double tot  = job->getTotalTime();
                    double rem  = job->getRemainingTime();

                    int remMin = static_cast<int>(rem) / 60;
                    int remSec = static_cast<int>(rem) % 60;

                    ConsoleUI::printBoxLine("   [현재 생산 중]");
                    ConsoleUI::printBoxEmpty();
                    {
                        std::ostringstream r;
                        r << "   주문 ID   :  " << job->getOrder()->getOrderId();
                        ConsoleUI::printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   시료      :  " << job->getOrder()->getSample()->getName();
                        ConsoleUI::printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   주문 정보 :  수량 " << qty << " 개  /  부족분 " << sht << " 개";
                        ConsoleUI::printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   현재 생산 :  " << cur << " 개";
                        ConsoleUI::printBoxLine(r.str());
                    }
                    ConsoleUI::printBoxEmpty();
                    {
                        std::ostringstream bar;
                        bar << "   진행률   :  ";
                        int barW = 24;
                        int filled = (tgt > 0) ? (cur * barW / tgt) : 0;
                        if (filled > barW) filled = barW;
                        bar << "[";
                        for (int i = 0; i < barW; ++i)
                            bar << (i < filled ? "█" : "░");
                        bar << "]  " << cur << " / " << tgt;
                        ConsoleUI::printBoxLine(bar.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   잔여 시간 :  " << remMin << "분 "
                          << std::setw(2) << std::setfill('0') << remSec << "초";
                        ConsoleUI::printBoxLine(r.str());
                    }
                    ConsoleUI::printBoxEmpty();
                } else {
                    ConsoleUI::printBoxEmpty();
                    ConsoleUI::printBoxLine("      현재 생산 중인 작업이 없습니다.");
                    ConsoleUI::printBoxEmpty();
                }
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                ConsoleUI::printBoxTop();
                ConsoleUI::printBoxLine("             ◉  생산 대기 큐");
                ConsoleUI::printBoxMid();
                auto waiting = m_productionLine.getWaitingJobs();
                if (waiting.empty()) {
                    ConsoleUI::printBoxLine("   대기 중인 작업이 없습니다.");
                } else {
                    ConsoleUI::printBoxLine("   순서   주문ID   시료명              목표수량");
                    ConsoleUI::printBoxMid();
                    for (std::size_t i = 0; i < waiting.size(); ++i) {
                        const ProductionJob& job = waiting[i];
                        std::ostringstream row;
                        row << "   " << std::left << std::setw(6) << (i + 1)
                            << std::setw(8) << job.getOrder()->getOrderId()
                            << std::setw(18) << job.getOrder()->getSample()->getName()
                            << std::right << job.getTargetQty() << " 개";
                        ConsoleUI::printBoxLine(row.str());
                    }
                }
                ConsoleUI::printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 0: return;
            default:
                ConsoleUI::printBoxLine("  ! 잘못된 입력입니다.");
                break;
        }
    }
}
