#pragma once

class OrderManager;

class ShippingManager {
public:
    explicit ShippingManager(OrderManager& orderMgr);

    void showConfirmedOrders() const;
    bool ship(int orderId);

private:
    OrderManager& m_orderMgr;
};
