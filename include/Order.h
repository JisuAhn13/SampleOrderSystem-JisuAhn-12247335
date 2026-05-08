#pragma once

#include <string>

class Sample;

enum class OrderStatus {
    RESERVED,
    CONFIRMED,
    PRODUCING,
    RELEASE,
    REJECTED
};

std::string statusToString(OrderStatus s);

class Order {
public:
    Order() = default;
    Order(int orderId, Sample* sample, const std::string& customerName, int quantity);

    int getOrderId() const;
    Sample* getSample() const;
    const std::string& getCustomerName() const;
    int getQuantity() const;
    OrderStatus getStatus() const;

    void setStatus(OrderStatus status);

private:
    int m_orderId = 0;
    Sample* m_sample = nullptr;
    std::string m_customerName;
    int m_quantity = 0;
    OrderStatus m_status = OrderStatus::RESERVED;
};
