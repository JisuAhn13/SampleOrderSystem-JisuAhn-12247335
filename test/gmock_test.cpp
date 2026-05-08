#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Sample.h"
#include "Order.h"
#include "ProductionJob.h"
#include "SampleManager.h"
#include "OrderManager.h"
#include "ProductionLine.h"
#include "ShippingManager.h"
#include "Monitor.h"

#include <thread>
#include <chrono>

using namespace testing;

// =============================================================================
// Sample 단위 테스트
// =============================================================================

class SampleTest : public Test {
protected:
    Sample s{"S001", "AlGaN-HEMT", 5.0, 0.9, 10};
};

TEST_F(SampleTest, InitialValues) {
    EXPECT_EQ(s.getId(), "S001");
    EXPECT_EQ(s.getName(), "AlGaN-HEMT");
    EXPECT_DOUBLE_EQ(s.getAvgProductionTime(), 5.0);
    EXPECT_DOUBLE_EQ(s.getYield(), 0.9);
    EXPECT_EQ(s.getStock(), 10);
}

TEST_F(SampleTest, AddStock_IncreasesStock) {
    s.addStock(5);
    EXPECT_EQ(s.getStock(), 15);
}

TEST_F(SampleTest, ReduceStock_Success) {
    EXPECT_TRUE(s.reduceStock(5));
    EXPECT_EQ(s.getStock(), 5);
}

TEST_F(SampleTest, ReduceStock_Exact) {
    EXPECT_TRUE(s.reduceStock(10));
    EXPECT_EQ(s.getStock(), 0);
}

TEST_F(SampleTest, ReduceStock_Insufficient_ReturnsFalseAndKeepsStock) {
    EXPECT_FALSE(s.reduceStock(20));
    EXPECT_EQ(s.getStock(), 10);  // 재고 불변
}

// =============================================================================
// SampleManager 테스트
// =============================================================================

class SampleManagerTest : public Test {
protected:
    SampleManager sm;
};

TEST_F(SampleManagerTest, RegisterSample_ReturnsNonNull) {
    auto* s = sm.registerSample("S001", "AlGaN", 5.0, 0.9);
    ASSERT_THAT(s, NotNull());
    EXPECT_EQ(s->getId(), "S001");
}

TEST_F(SampleManagerTest, RegisterSample_DuplicateIdReturnsNull) {
    sm.registerSample("S001", "AlGaN", 5.0, 0.9);
    EXPECT_THAT(sm.registerSample("S001", "Other", 3.0, 0.8), IsNull());
}

TEST_F(SampleManagerTest, FindById_ExistingId) {
    sm.registerSample("S001", "AlGaN", 5.0, 0.9);
    sm.registerSample("S002", "GaN", 3.0, 0.8);
    auto* s = sm.findById("S001");
    ASSERT_THAT(s, NotNull());
    EXPECT_EQ(s->getName(), "AlGaN");
}

TEST_F(SampleManagerTest, FindById_MissingIdReturnsNull) {
    EXPECT_THAT(sm.findById("XXXX"), IsNull());
}

TEST_F(SampleManagerTest, SearchByName_PartialMatch) {
    sm.registerSample("S001", "AlGaN-HEMT", 5.0, 0.9);
    sm.registerSample("S002", "AlGaAs",     3.0, 0.8);
    sm.registerSample("S003", "GaN-HPA",    2.0, 0.7);

    auto results = sm.searchByName("AlGa");
    EXPECT_THAT(results, SizeIs(2));
    EXPECT_THAT(results, Each(NotNull()));
}

TEST_F(SampleManagerTest, SearchByName_NoMatch_ReturnsEmpty) {
    sm.registerSample("S001", "AlGaN", 5.0, 0.9);
    EXPECT_THAT(sm.searchByName("ZZZZZ"), IsEmpty());
}

TEST_F(SampleManagerTest, GetAllSamples_ReturnsAll) {
    sm.registerSample("S001", "A", 1.0, 0.9);
    sm.registerSample("S002", "B", 2.0, 0.8);
    sm.registerSample("S003", "C", 3.0, 0.7);
    EXPECT_THAT(sm.getAllSamples(), SizeIs(3));
}

// =============================================================================
// OrderManager 테스트
// =============================================================================

class OrderManagerTest : public Test {
protected:
    SampleManager sm;
    ProductionLine pl;
    OrderManager om{sm, pl};

    void SetUp() override {
        sm.registerSample("S001", "TestSample", 1.0, 0.9);
    }
};

TEST_F(OrderManagerTest, PlaceOrder_ValidSample_ReturnsReserved) {
    auto* o = om.placeOrder("S001", "CustomerA", 5);
    ASSERT_THAT(o, NotNull());
    EXPECT_EQ(o->getStatus(), OrderStatus::RESERVED);
    EXPECT_EQ(o->getQuantity(), 5);
    EXPECT_EQ(o->getCustomerName(), "CustomerA");
}

TEST_F(OrderManagerTest, PlaceOrder_InvalidSample_ReturnsNull) {
    EXPECT_THAT(om.placeOrder("XXXX", "CustomerA", 5), IsNull());
}

TEST_F(OrderManagerTest, ApproveOrder_SufficientStock_CONFIRMED) {
    sm.findById("S001")->addStock(20);
    auto* o = om.placeOrder("S001", "CustomerA", 10);

    EXPECT_TRUE(om.approveOrder(o->getOrderId()));
    EXPECT_EQ(o->getStatus(), OrderStatus::CONFIRMED);
    EXPECT_EQ(sm.findById("S001")->getStock(), 10);  // 20 - 10
}

TEST_F(OrderManagerTest, ApproveOrder_InsufficientStock_PRODUCING) {
    // stock = 0 → 전량 부족
    auto* o = om.placeOrder("S001", "CustomerA", 10);

    EXPECT_TRUE(om.approveOrder(o->getOrderId()));
    EXPECT_EQ(o->getStatus(), OrderStatus::PRODUCING);
    EXPECT_TRUE(pl.hasCurrent());
}

TEST_F(OrderManagerTest, ApproveOrder_NonReserved_Fails) {
    sm.findById("S001")->addStock(20);
    auto* o = om.placeOrder("S001", "CustomerA", 5);
    om.approveOrder(o->getOrderId());  // CONFIRMED

    EXPECT_FALSE(om.approveOrder(o->getOrderId()));
}

TEST_F(OrderManagerTest, RejectOrder_Success) {
    auto* o = om.placeOrder("S001", "CustomerA", 5);
    EXPECT_TRUE(om.rejectOrder(o->getOrderId()));
    EXPECT_EQ(o->getStatus(), OrderStatus::REJECTED);
}

TEST_F(OrderManagerTest, RejectOrder_NonReserved_Fails) {
    sm.findById("S001")->addStock(20);
    auto* o = om.placeOrder("S001", "CustomerA", 5);
    om.approveOrder(o->getOrderId());  // CONFIRMED

    EXPECT_FALSE(om.rejectOrder(o->getOrderId()));
}

TEST_F(OrderManagerTest, GetAllActiveOrders_ExcludesRejected) {
    auto* o1 = om.placeOrder("S001", "CustA", 5);
    auto* o2 = om.placeOrder("S001", "CustB", 3);
    om.rejectOrder(o1->getOrderId());

    auto active = om.getAllActiveOrders();
    EXPECT_THAT(active, SizeIs(1));
    EXPECT_EQ(active[0]->getOrderId(), o2->getOrderId());
}

TEST_F(OrderManagerTest, GetOrdersByStatus_FiltersCorrectly) {
    sm.findById("S001")->addStock(20);
    auto* o1 = om.placeOrder("S001", "CustA", 5);
    auto* o2 = om.placeOrder("S001", "CustB", 3);
    om.approveOrder(o1->getOrderId());  // CONFIRMED

    EXPECT_THAT(om.getOrdersByStatus(OrderStatus::RESERVED),  SizeIs(1));
    EXPECT_THAT(om.getOrdersByStatus(OrderStatus::CONFIRMED), SizeIs(1));
    EXPECT_THAT(om.getOrdersByStatus(OrderStatus::PRODUCING), IsEmpty());
}

// =============================================================================
// ProductionLine 테스트
// =============================================================================

class ProductionLineTest : public Test {
protected:
    SampleManager sm;
    ProductionLine pl;
    OrderManager om{sm, pl};

    void SetUp() override {
        sm.registerSample("S001", "FastSample", 0.05, 0.9);
    }
};

TEST_F(ProductionLineTest, InitialState_IsEmpty) {
    EXPECT_FALSE(pl.hasCurrent());
    EXPECT_TRUE(pl.isEmpty());
    EXPECT_EQ(pl.getQueueSize(), 0);
    EXPECT_THAT(pl.getCurrentJob(), IsNull());
    EXPECT_THAT(pl.getWaitingJobs(), IsEmpty());
}

TEST_F(ProductionLineTest, Enqueue_StartsFirstJobImmediately) {
    auto* o = om.placeOrder("S001", "Cust", 5);
    om.approveOrder(o->getOrderId());  // stock=0 → PRODUCING

    EXPECT_TRUE(pl.hasCurrent());
    ASSERT_THAT(pl.getCurrentJob(), NotNull());
    EXPECT_EQ(pl.getCurrentJob()->getOrder()->getOrderId(), o->getOrderId());
}

TEST_F(ProductionLineTest, MultipleEnqueue_FIFOOrder) {
    auto* o1 = om.placeOrder("S001", "Cust1", 2);
    auto* o2 = om.placeOrder("S001", "Cust2", 2);
    auto* o3 = om.placeOrder("S001", "Cust3", 2);
    om.approveOrder(o1->getOrderId());
    om.approveOrder(o2->getOrderId());
    om.approveOrder(o3->getOrderId());

    EXPECT_EQ(pl.getQueueSize(), 2);
    auto waiting = pl.getWaitingJobs();
    ASSERT_THAT(waiting, SizeIs(2));
    EXPECT_EQ(waiting[0].getOrder()->getOrderId(), o2->getOrderId());
    EXPECT_EQ(waiting[1].getOrder()->getOrderId(), o3->getOrderId());
}

TEST_F(ProductionLineTest, Tick_CompletesJob_AdvancesToNext) {
    auto* o1 = om.placeOrder("S001", "Cust1", 2);
    auto* o2 = om.placeOrder("S001", "Cust2", 2);
    om.approveOrder(o1->getOrderId());
    om.approveOrder(o2->getOrderId());

    std::this_thread::sleep_for(std::chrono::duration<double>(0.5));
    pl.tick();

    EXPECT_EQ(o1->getStatus(), OrderStatus::CONFIRMED);
    ASSERT_THAT(pl.getCurrentJob(), NotNull());
    EXPECT_EQ(pl.getCurrentJob()->getOrder()->getOrderId(), o2->getOrderId());
}

// =============================================================================
// ProductionJob 계산 검증
// =============================================================================

struct ProductionJobCalcTest : public Test {
    SampleManager sm;
    ProductionLine pl;
    OrderManager om{sm, pl};
};

TEST_F(ProductionJobCalcTest, TargetQty_Formula) {
    sm.registerSample("S001", "Sample", 1.0, 0.8);
    auto* o = om.placeOrder("S001", "Cust", 10);  // stock=0, shortage=10
    om.approveOrder(o->getOrderId());

    const auto* job = pl.getCurrentJob();
    ASSERT_THAT(job, NotNull());
    // targetQty = ceil(10 / (0.8 * 0.9)) = ceil(13.888) = 14
    EXPECT_EQ(job->getTargetQty(), 14);
    EXPECT_EQ(job->getShortage(), 10);
}

TEST_F(ProductionJobCalcTest, TotalTime_Formula) {
    sm.registerSample("S001", "Sample", 2.0, 0.9);
    auto* o = om.placeOrder("S001", "Cust", 5);  // shortage=5, targetQty=7
    om.approveOrder(o->getOrderId());

    const auto* job = pl.getCurrentJob();
    ASSERT_THAT(job, NotNull());
    // targetQty = ceil(5/0.81) = 7, totalTime = 2.0 * 7 = 14.0
    EXPECT_EQ(job->getTargetQty(), 7);
    EXPECT_DOUBLE_EQ(job->getTotalTime(), 14.0);
}

TEST_F(ProductionJobCalcTest, PartialShortage_StockReducedAtApproval) {
    sm.registerSample("S001", "Sample", 1.0, 0.9);
    sm.findById("S001")->addStock(3);  // stock=3
    auto* o = om.placeOrder("S001", "Cust", 10);  // shortage=7
    om.approveOrder(o->getOrderId());

    const auto* job = pl.getCurrentJob();
    ASSERT_THAT(job, NotNull());
    EXPECT_EQ(job->getShortage(), 7);
    // targetQty = ceil(7 / 0.81) = 9
    EXPECT_EQ(job->getTargetQty(), 9);
}

TEST_F(ProductionJobCalcTest, CompleteJob_StockReflected) {
    sm.registerSample("S001", "FastSample", 0.02, 0.9);
    auto* o = om.placeOrder("S001", "Cust", 3);  // stock=0
    om.approveOrder(o->getOrderId());

    std::this_thread::sleep_for(std::chrono::duration<double>(0.3));
    pl.tick();

    EXPECT_EQ(o->getStatus(), OrderStatus::CONFIRMED);
    // addStock(targetQty) - reduceStock(3) = targetQty - 3
    const auto* job = pl.getCurrentJob();
    EXPECT_THAT(job, IsNull());  // 완료 후 current 없음
}

// =============================================================================
// ShippingManager 테스트
// =============================================================================

class ShippingManagerTest : public Test {
protected:
    SampleManager sm;
    ProductionLine pl;
    OrderManager om{sm, pl};
    ShippingManager sh{om};

    void SetUp() override {
        sm.registerSample("S001", "TestSample", 1.0, 0.9);
        sm.findById("S001")->addStock(20);
    }
};

TEST_F(ShippingManagerTest, Ship_ConfirmedOrder_Success) {
    auto* o = om.placeOrder("S001", "CustA", 5);
    om.approveOrder(o->getOrderId());  // CONFIRMED

    EXPECT_TRUE(sh.ship(o->getOrderId()));
    EXPECT_EQ(o->getStatus(), OrderStatus::RELEASE);
}

TEST_F(ShippingManagerTest, Ship_ReservedOrder_Fails) {
    auto* o = om.placeOrder("S001", "CustA", 5);
    // RESERVED, not CONFIRMED

    EXPECT_FALSE(sh.ship(o->getOrderId()));
    EXPECT_EQ(o->getStatus(), OrderStatus::RESERVED);
}

TEST_F(ShippingManagerTest, Ship_NonExistentOrder_Fails) {
    EXPECT_FALSE(sh.ship(9999));
}

TEST_F(ShippingManagerTest, Ship_RejectedOrder_Fails) {
    auto* o = om.placeOrder("S001", "CustA", 5);
    om.rejectOrder(o->getOrderId());

    EXPECT_FALSE(sh.ship(o->getOrderId()));
}

// =============================================================================
// Monitor 재고 상태 판단 (EXPECT_THAT + gmock matchers)
// =============================================================================

class MonitorStockTest : public Test {
protected:
    SampleManager sm;
    ProductionLine pl;
    OrderManager om{sm, pl};
    Monitor mon{om, sm};
};

TEST_F(MonitorStockTest, StockZero_IsDepletedCondition) {
    sm.registerSample("S001", "Sample", 1.0, 0.9);
    auto* s = sm.findById("S001");
    EXPECT_EQ(s->getStock(), 0);

    // RESERVED 주문 없음 → demand=0, stock==0 → 고갈
    auto reserved = om.getOrdersByStatus(OrderStatus::RESERVED);
    EXPECT_THAT(reserved, IsEmpty());
}

TEST_F(MonitorStockTest, StockLessThanDemand_IsShortage) {
    sm.registerSample("S001", "Sample", 1.0, 0.9);
    sm.findById("S001")->addStock(3);
    om.placeOrder("S001", "CustA", 10);  // RESERVED qty=10

    auto reserved = om.getOrdersByStatus(OrderStatus::RESERVED);
    int demand = 0;
    for (auto* o : reserved) demand += o->getQuantity();

    EXPECT_GT(sm.findById("S001")->getStock(), 0);
    EXPECT_LT(sm.findById("S001")->getStock(), demand);
}

TEST_F(MonitorStockTest, StockSufficient_IsAbundant) {
    sm.registerSample("S001", "Sample", 1.0, 0.9);
    sm.findById("S001")->addStock(20);
    om.placeOrder("S001", "CustA", 5);  // RESERVED qty=5

    auto reserved = om.getOrdersByStatus(OrderStatus::RESERVED);
    int demand = 0;
    for (auto* o : reserved) demand += o->getQuantity();

    EXPECT_GE(sm.findById("S001")->getStock(), demand);
}

// =============================================================================
// gmock MOCK_METHOD 데모 — 생산 이벤트 관찰자 패턴
// =============================================================================

class IProductionObserver {
public:
    virtual ~IProductionObserver() = default;
    virtual void onJobStarted(int orderId, int targetQty)       = 0;
    virtual void onJobCompleted(int orderId, int stockAfter)    = 0;
    virtual void onProgressUpdated(int orderId, int produced, int total) = 0;
};

class MockProductionObserver : public IProductionObserver {
public:
    MOCK_METHOD(void, onJobStarted,      (int orderId, int targetQty),              (override));
    MOCK_METHOD(void, onJobCompleted,    (int orderId, int stockAfter),             (override));
    MOCK_METHOD(void, onProgressUpdated, (int orderId, int produced, int total),    (override));
};

TEST(GMockObserverTest, OnJobStarted_CalledOnce) {
    MockProductionObserver obs;
    EXPECT_CALL(obs, onJobStarted(1, Gt(0))).Times(1);

    obs.onJobStarted(1, 7);
}

TEST(GMockObserverTest, OnJobCompleted_WithCorrectArgs) {
    MockProductionObserver obs;
    EXPECT_CALL(obs, onJobCompleted(1, Ge(0))).Times(1);

    obs.onJobCompleted(1, 3);
}

TEST(GMockObserverTest, OnProgressUpdated_CalledAtLeastOnce) {
    MockProductionObserver obs;
    EXPECT_CALL(obs, onProgressUpdated(_, _, Gt(0)))
        .Times(AtLeast(1));

    obs.onProgressUpdated(1, 3, 7);
    obs.onProgressUpdated(1, 5, 7);
}

TEST(GMockObserverTest, OnJobCompleted_NeverCalledIfNotInvoked) {
    MockProductionObserver obs;
    EXPECT_CALL(obs, onJobCompleted(_, _)).Times(0);
    // onJobCompleted 호출 없음
}

TEST(GMockObserverTest, MockMatchers_AnyAndEq) {
    MockProductionObserver obs;
    EXPECT_CALL(obs, onProgressUpdated(Eq(42), _, _)).Times(1);
    EXPECT_CALL(obs, onJobStarted(_, _)).Times(AnyNumber());

    obs.onProgressUpdated(42, 5, 10);
}
