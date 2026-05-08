#pragma once

class OrderManager;

class ShippingManager {
public:
    explicit ShippingManager(OrderManager& orderMgr);

    void showConfirmedOrders();
    bool ship(int orderId);

private:
    OrderManager& m_orderMgr;
};
