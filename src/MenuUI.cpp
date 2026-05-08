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

// ─── 박스 유틸 ────────────────────────────────────────────────────────────────

static std::string rep(const std::string& s, int n)
{
    std::string r;
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

int MenuUI::displayWidth(const std::string& s)
{
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = (unsigned char)s[i];
        if      (c < 0x80) { w += 1; i += 1; }
        else if (c < 0xE0) { w += 1; i += 2; }
        else if (c < 0xF0) { w += 2; i += 3; }  // 한글·전각(CJK) = 2칸
        else               { w += 2; i += 4; }
    }
    return w;
}

void MenuUI::clearScreen()  { system("cls"); }

void MenuUI::printBoxTop()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╔" << rep("═", BOX_W) << "╗\n";
}
void MenuUI::printBoxMid()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╠" << rep("═", BOX_W) << "╣\n";
}
void MenuUI::printBoxBot()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╚" << rep("═", BOX_W) << "╝\n";
}
void MenuUI::printBoxEmpty()
{
    std::cout << std::string(LEFT_PAD, ' ') << "║" << std::string(BOX_W, ' ') << "║\n";
}
void MenuUI::printBoxLine(const std::string& content)
{
    int pad = BOX_W - displayWidth(content);
    if (pad < 0) pad = 0;
    std::cout << std::string(LEFT_PAD, ' ') << "║" << content
              << std::string(pad, ' ') << "║\n";
}

void MenuUI::printProgressBar(int current, int total, int barWidth)
{
    int filled = (total > 0) ? (current * barWidth / total) : 0;
    if (filled > barWidth) filled = barWidth;
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
        std::cout << (i < filled ? "█" : "░");
    std::cout << "]";
}

// ─── 기존 유틸 (하위 호환) ───────────────────────────────────────────────────

void MenuUI::printHeader(const std::string& title)
{
    printBoxMid();
    printBoxLine("  " + title);
    printBoxMid();
}

void MenuUI::printSeparator()
{
    printBoxMid();
}

int MenuUI::getIntInput(const std::string& prompt)
{
    int val;
    while (true) {
        std::cout << std::string(LEFT_PAD, ' ') << prompt;
        if (std::cin >> val) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << std::string(LEFT_PAD, ' ') << "  숫자를 입력해주세요.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

double MenuUI::getDoubleInput(const std::string& prompt)
{
    double val;
    while (true) {
        std::cout << std::string(LEFT_PAD, ' ') << prompt;
        if (std::cin >> val) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << std::string(LEFT_PAD, ' ') << "  숫자를 입력해주세요.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

std::string MenuUI::getStringInput(const std::string& prompt)
{
    std::string val;
    std::cout << std::string(LEFT_PAD, ' ') << prompt;
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
                std::cout << "\n" << std::string(LEFT_PAD, ' ')
                          << "  종료합니다. 감사합니다.\n\n";
                return;
            default:
                printBoxLine("  잘못된 입력입니다.");
                break;
        }
    }
}

void MenuUI::showMainMenu() const
{
    std::cout << "\n\n";
    printBoxTop();
    printBoxLine("       SampleOrderSystem  주문 관리 시스템");
    printBoxLine("          반도체 시료 생산 · 출고 플랫폼");
    printBoxMid();
    printBoxEmpty();
    printBoxLine("   [ 1 ]   ◈  시료 관리      Sample Management");
    printBoxLine("   [ 2 ]   ◇  주문 관리      Order  Management");
    printBoxLine("   [ 3 ]   ◎  모니터링       Monitoring");
    printBoxLine("   [ 4 ]   ►  출고 처리      Shipping");
    printBoxLine("   [ 5 ]   ◉  생산 라인      Production Line");
    printBoxEmpty();
    printBoxMid();
    printBoxLine("   [ 0 ]   ✕  종료");
    printBoxBot();
    std::cout << "\n";
}

// ─── 시료 관리 ────────────────────────────────────────────────────────────────

void MenuUI::handleSampleManagement()
{
    while (true) {
        m_productionLine.tick();
        clearScreen();
        std::cout << "\n";
        printBoxTop();
        printBoxLine("                ◈  시료 관리");
        printBoxMid();
        printBoxLine("   [ 1 ]   새 시료 등록");
        printBoxLine("   [ 2 ]   시료 목록 조회");
        printBoxLine("   [ 3 ]   시료 이름 검색");
        printBoxMid();
        printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◈  시료 등록");
                printBoxMid();
                std::string id = getStringInput("  시료 ID       : ");
                if (id.empty()) {
                    printBoxLine("  ! ID를 입력해주세요.");
                    printBoxBot();
                    std::cin.get();
                    continue;
                }
                std::string name = getStringInput("  시료 이름     : ");
                if (name.empty()) {
                    printBoxLine("  ! 시료 이름을 입력해주세요.");
                    printBoxBot();
                    std::cin.get();
                    continue;
                }
                double avgTime = getDoubleInput("  평균 생산시간 : ");
                if (avgTime <= 0) {
                    printBoxLine("  ! 0보다 큰 값을 입력해주세요.");
                    printBoxBot();
                    continue;
                }
                double yield = getDoubleInput("  수율 (0~1)    : ");
                if (yield <= 0.0 || yield > 1.0) {
                    printBoxLine("  ! 0.0 초과 1.0 이하 값을 입력해주세요.");
                    printBoxBot();
                    continue;
                }
                Sample* sample = m_sampleMgr.registerSample(id, name, avgTime, yield);
                printBoxMid();
                if (sample == nullptr) {
                    printBoxLine("  ! 이미 존재하는 ID입니다.");
                } else {
                    printBoxLine("  ✔ 시료 등록 성공  ID: " + sample->getId());
                }
                printBoxBot();
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◈  시료 목록");
                printBoxMid();
                auto samples = m_sampleMgr.getAllSamples();
                if (samples.empty()) {
                    printBoxLine("   등록된 시료가 없습니다.");
                } else {
                    printBoxLine("   ID           이름            생산시간   수율   재고");
                    printBoxMid();
                    for (Sample* s : samples) {
                        std::ostringstream oss;
                        oss << "   " << std::left << std::setw(12) << s->getId()
                            << std::setw(14) << s->getName()
                            << std::right << std::setw(6) << std::fixed
                            << std::setprecision(1) << s->getAvgProductionTime()
                            << "s   " << std::setprecision(2) << s->getYield()
                            << "   " << s->getStock();
                        printBoxLine(oss.str());
                    }
                }
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 3: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◈  시료 검색");
                printBoxMid();
                std::string keyword = getStringInput("  검색 키워드: ");
                auto results = m_sampleMgr.searchByName(keyword);
                printBoxMid();
                if (results.empty()) {
                    printBoxLine("   검색 결과가 없습니다.");
                } else {
                    printBoxLine("   ID           이름            생산시간   수율   재고");
                    printBoxMid();
                    for (Sample* s : results) {
                        std::ostringstream oss;
                        oss << "   " << std::left << std::setw(12) << s->getId()
                            << std::setw(14) << s->getName()
                            << std::right << std::setw(6) << std::fixed
                            << std::setprecision(1) << s->getAvgProductionTime()
                            << "s   " << std::setprecision(2) << s->getYield()
                            << "   " << s->getStock();
                        printBoxLine(oss.str());
                    }
                }
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 0: return;
            default:
                printBoxLine("  ! 잘못된 입력입니다.");
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
        printBoxTop();
        printBoxLine("                ◇  주문 관리");
        printBoxMid();
        printBoxLine("   [ 1 ]   새 주문 접수");
        printBoxLine("   [ 2 ]   접수 주문 목록 (RESERVED)");
        printBoxLine("   [ 3 ]   주문 승인");
        printBoxLine("   [ 4 ]   주문 거절");
        printBoxMid();
        printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◇  주문 접수");
                printBoxMid();
                std::string sampleId = getStringInput("  시료 ID  : ");
                if (m_sampleMgr.findById(sampleId) == nullptr) {
                    printBoxLine("  ! 존재하지 않는 시료 ID입니다.");
                    printBoxBot();
                    continue;
                }
                std::string customer = getStringInput("  고객명   : ");
                if (customer.empty()) {
                    printBoxLine("  ! 고객명을 입력해주세요.");
                    printBoxBot();
                    continue;
                }
                int qty = getIntInput("  주문 수량: ");
                if (qty <= 0) {
                    printBoxLine("  ! 1 이상의 수량을 입력해주세요.");
                    printBoxBot();
                    continue;
                }
                Order* order = m_orderMgr.placeOrder(sampleId, customer, qty);
                printBoxMid();
                std::ostringstream oss;
                oss << "  ✔ 주문 접수 완료  주문 ID: " << order->getOrderId();
                printBoxLine(oss.str());
                printBoxBot();
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◇  접수 주문 목록");
                printBoxMid();
                auto orders = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
                if (orders.empty()) {
                    printBoxLine("   접수된 주문이 없습니다.");
                } else {
                    printBoxLine("   주문ID   시료명            고객명         수량");
                    printBoxMid();
                    for (Order* o : orders) {
                        std::ostringstream row;
                        row << "   " << std::left << std::setw(7) << o->getOrderId()
                            << std::setw(16) << o->getSample()->getName()
                            << std::setw(13) << o->getCustomerName()
                            << std::right << o->getQuantity();
                        printBoxLine(row.str());
                    }
                }
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 3: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◇  주문 승인");
                printBoxMid();
                auto reserved = m_orderMgr.getOrdersByStatus(OrderStatus::RESERVED);
                if (reserved.empty()) {
                    printBoxLine("   접수된 주문이 없습니다.");
                    printBoxBot();
                    continue;
                }
                printBoxLine("   주문ID   시료명            고객명         수량");
                printBoxMid();
                for (Order* o : reserved) {
                    std::ostringstream row;
                    row << "   " << std::left << std::setw(7) << o->getOrderId()
                        << std::setw(16) << o->getSample()->getName()
                        << std::setw(13) << o->getCustomerName()
                        << std::right << o->getQuantity();
                    printBoxLine(row.str());
                }
                printBoxMid();
                int orderId = getIntInput("  승인할 주문 ID: ");
                if (m_orderMgr.approveOrder(orderId)) {
                    printBoxLine("  ✔ 주문 승인되었습니다.");
                } else {
                    printBoxLine("  ! 승인 실패: 존재하지 않거나 접수 상태가 아닙니다.");
                }
                printBoxBot();
                break;
            }
            case 4: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◇  주문 거절");
                printBoxMid();
                int orderId = getIntInput("  거절할 주문 ID: ");
                if (m_orderMgr.rejectOrder(orderId)) {
                    printBoxLine("  ✔ 주문이 거절되었습니다.");
                } else {
                    printBoxLine("  ! 거절 실패: 존재하지 않거나 접수 상태가 아닙니다.");
                }
                printBoxBot();
                break;
            }
            case 0: return;
            default:
                printBoxLine("  ! 잘못된 입력입니다.");
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
        printBoxTop();
        printBoxLine("                ◎  모니터링");
        printBoxMid();
        printBoxLine("   [ 1 ]   주문 현황  (상태별)");
        printBoxLine("   [ 2 ]   재고 현황");
        printBoxMid();
        printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◎  주문 현황");
                printBoxMid();
                m_monitor.showOrderStatus();
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ◎  재고 현황");
                printBoxMid();
                m_monitor.showStockStatus();
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 0: return;
            default:
                printBoxLine("  ! 잘못된 입력입니다.");
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
        printBoxTop();
        printBoxLine("                ►  출고 처리");
        printBoxMid();
        printBoxLine("   [ 1 ]   출고 대기 목록 조회");
        printBoxLine("   [ 2 ]   출고 실행");
        printBoxMid();
        printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ►  출고 대기 목록");
                printBoxMid();
                m_shippingMgr.showConfirmedOrders();
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("              ►  출고 실행");
                printBoxMid();
                auto confirmed = m_orderMgr.getOrdersByStatus(OrderStatus::CONFIRMED);
                if (confirmed.empty()) {
                    printBoxLine("   출고 대기 주문이 없습니다.");
                    printBoxBot();
                    continue;
                }
                m_shippingMgr.showConfirmedOrders();
                printBoxMid();
                int orderId = getIntInput("  출고할 주문 ID: ");
                if (m_shippingMgr.ship(orderId)) {
                    printBoxLine("  ✔ 출고 처리되었습니다.");
                } else {
                    printBoxLine("  ! 출고 실패: 승인 상태 주문이 아닙니다.");
                }
                printBoxBot();
                break;
            }
            case 0: return;
            default:
                printBoxLine("  ! 잘못된 입력입니다.");
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
        printBoxTop();
        printBoxLine("                ◉  생산 라인");
        printBoxMid();
        printBoxLine("   [ 1 ]   생산 현황 (현재 진행 중인 Job)");
        printBoxLine("   [ 2 ]   대기 큐 목록");
        printBoxMid();
        printBoxLine("   [ 0 ]   ◀  이전 메뉴");
        printBoxBot();
        std::cout << "\n";

        int choice = getIntInput("> ");
        switch (choice) {
            case 1: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("           ◉  생산 라인 - 생산 현황");
                printBoxMid();
                if (m_productionLine.hasCurrent()) {
                    const ProductionJob* job = m_productionLine.getCurrentJob();
                    int    tgt  = job->getTargetQty();
                    int    cur  = job->getProducedQty();
                    double tot  = job->getTotalTime();
                    double rem  = job->getRemainingTime();

                    printBoxLine("   [현재 생산 중]");
                    printBoxEmpty();
                    {
                        std::ostringstream r;
                        r << "   주문 ID   :  " << job->getOrder()->getOrderId();
                        printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   시료      :  " << job->getOrder()->getSample()->getName();
                        printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   목표 수량 :  " << tgt << " 개";
                        printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << "   현재 생산 :  " << cur << " 개";
                        printBoxLine(r.str());
                    }
                    printBoxEmpty();
                    // 진행률 바
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
                        printBoxLine(bar.str());
                    }
                    {
                        std::ostringstream r;
                        r << std::fixed << std::setprecision(1);
                        r << "   총 생산   :  " << tot << " 초";
                        printBoxLine(r.str());
                    }
                    {
                        std::ostringstream r;
                        r << std::fixed << std::setprecision(1);
                        r << "   잔여 시간 :  " << rem << " 초";
                        printBoxLine(r.str());
                    }
                    printBoxEmpty();
                } else {
                    printBoxEmpty();
                    printBoxLine("      현재 생산 중인 작업이 없습니다.");
                    printBoxEmpty();
                }
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 2: {
                clearScreen();
                std::cout << "\n";
                printBoxTop();
                printBoxLine("             ◉  생산 대기 큐");
                printBoxMid();
                auto waiting = m_productionLine.getWaitingJobs();
                if (waiting.empty()) {
                    printBoxLine("   대기 중인 작업이 없습니다.");
                } else {
                    printBoxLine("   순서   주문ID   시료명              목표수량");
                    printBoxMid();
                    for (std::size_t i = 0; i < waiting.size(); ++i) {
                        const ProductionJob& job = waiting[i];
                        std::ostringstream row;
                        row << "   " << std::left << std::setw(6) << (i + 1)
                            << std::setw(8) << job.getOrder()->getOrderId()
                            << std::setw(18) << job.getOrder()->getSample()->getName()
                            << std::right << job.getTargetQty() << " 개";
                        printBoxLine(row.str());
                    }
                }
                printBoxBot();
                getStringInput("  [Enter = 계속] > ");
                break;
            }
            case 0: return;
            default:
                printBoxLine("  ! 잘못된 입력입니다.");
                break;
        }
    }
}
