#pragma once
#include <string>
#include <vector>

class Sample;
class Order;
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

    void printSampleRows(const std::vector<Sample*>& samples) const;
    void printReservedOrderRows(const std::vector<Order*>& orders) const;
};
