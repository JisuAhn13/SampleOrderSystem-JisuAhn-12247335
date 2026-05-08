#pragma once
#include <string>

class SampleManager;
class OrderManager;
class ProductionLine;
class Monitor;
class ShippingManager;

class MenuUI {
    SampleManager&   m_sampleMgr;
    OrderManager&    m_orderMgr;
    ProductionLine&  m_productionLine;
    Monitor&         m_monitor;
    ShippingManager& m_shippingMgr;
public:
    MenuUI(SampleManager& sm, OrderManager& om,
           ProductionLine& pl, Monitor& mo, ShippingManager& sh);

    void run();

    // ── 박스 레이아웃 상수 (Monitor, ShippingManager에서도 사용) ──────────────
    static const int SCREEN_W = 80;
    static const int BOX_W    = 58;
    static const int LEFT_PAD = (SCREEN_W - BOX_W - 2) / 2;  // = 10

    // ── 박스 레이아웃 유틸 (public: 타 클래스의 출력에서 재사용) ─────────────
    static void printBoxTop();
    static void printBoxMid();
    static void printBoxBot();
    static void printBoxLine(const std::string& content);
    static void printBoxEmpty();
    static int  displayWidth(const std::string& s);

private:
    void showMainMenu() const;
    void handleSampleManagement();
    void handleOrderManagement();
    void handleMonitoring();
    void handleShipping();
    void handleProductionLine();

    static int         getIntInput(const std::string& prompt);
    static double      getDoubleInput(const std::string& prompt);
    static std::string getStringInput(const std::string& prompt);
    static void        printHeader(const std::string& title);
    static void        printSeparator();
    static void        clearScreen();
    static void        printProgressBar(int current, int total, int barWidth = 30);
};
