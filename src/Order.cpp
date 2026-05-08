#include "Order.h"

std::string statusToString(OrderStatus s)
{
    switch (s) {
        case OrderStatus::RESERVED:  return "접수";
        case OrderStatus::CONFIRMED: return "승인";
        case OrderStatus::PRODUCING: return "생산중";
        case OrderStatus::RELEASE:   return "출고";
        case OrderStatus::REJECTED:  return "거절";
        default: return "알 수 없음";
    }
}

Order::Order(int orderId, Sample* sample, const std::string& customerName, int quantity)
    : m_orderId(orderId), m_sample(sample), m_customerName(customerName), m_quantity(quantity)
{
}

int Order::getOrderId() const { return m_orderId; }
Sample* Order::getSample() const { return m_sample; }
const std::string& Order::getCustomerName() const { return m_customerName; }
int Order::getQuantity() const { return m_quantity; }
OrderStatus Order::getStatus() const { return m_status; }

void Order::setStatus(OrderStatus status) { m_status = status; }
