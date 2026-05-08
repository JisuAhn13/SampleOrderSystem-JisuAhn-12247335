#include <windows.h>
#include "SampleManager.h"
#include "ProductionLine.h"
#include "OrderManager.h"
#include "Monitor.h"
#include "ShippingManager.h"
#include "Order.h"
#include "Sample.h"
#include "ProductionJob.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cassert>
#include <cmath>

static int g_pass = 0, g_fail = 0;

void check(bool condition, const std::string& desc) {
    if (condition) {
        std::cout << "  [PASS] " << desc << "\n";
        ++g_pass;
    } else {
        std::cout << "  [FAIL] " << desc << "\n";
        ++g_fail;
    }
}

void waitFor(double seconds) {
    std::this_thread::sleep_for(
        std::chrono::duration<double>(seconds));
}

// =============================================================
// 시나리오 1: 재고 충분 경로
// =============================================================
void scenario1() {
    std::cout << "\n=== 시나리오 1: 재고 충분 경로 ===\n";
    SampleManager sm;
    ProductionLine pl;
    OrderManager om(sm, pl);
    Monitor mon(om, sm);

    // 준비: SiC-MOSFET, avgTime=5, yield=0.9, stock=20
    Sample* s = sm.registerSample("SiC-MOSFET", 5.0, 0.9, 20);
    check(s != nullptr, "시료 등록 성공");
    check(s->getStock() == 20, "초기 재고 20");

    // 주문 접수
    Order* o = om.placeOrder(s->getId(), "A사", 10);
    check(o != nullptr, "주문 접수 성공");
    check(o->getStatus() == OrderStatus::RESERVED, "접수 후 RESERVED");

    // 주문 승인 (재고 충분: 20 >= 10)
    bool approved = om.approveOrder(o->getOrderId());
    check(approved, "주문 승인 성공");
    check(o->getStatus() == OrderStatus::CONFIRMED, "승인 후 CONFIRMED");
    check(s->getStock() == 10, "승인 후 재고 10 (20-10)");

    // 출고
    ShippingManager sh(om);
    bool shipped = sh.ship(o->getOrderId());
    check(shipped, "출고 성공");
    check(o->getStatus() == OrderStatus::RELEASE, "출고 후 RELEASE");
}

// =============================================================
// 시나리오 2: 재고 부족 경로 (생산 포함)
// shortage = 10 - 3 = 7
// targetQty = ceil(7 / (0.8 * 0.9)) = ceil(9.722) = 10
// totalTime = 0.05 * 10 = 0.5초
// complete(): addStock(10) → 3+10=13, reduceStock(10) → 3
// =============================================================
void scenario2() {
    std::cout << "\n=== 시나리오 2: 재고 부족 경로 ===\n";
    SampleManager sm;
    ProductionLine pl;
    OrderManager om(sm, pl);

    // 준비: GaN-HPA, avgTime=0.05(테스트용), yield=0.8, stock=3
    Sample* s = sm.registerSample("GaN-HPA", 0.05, 0.8, 3);
    Order* o = om.placeOrder(s->getId(), "B연구소", 10);
    check(o->getStatus() == OrderStatus::RESERVED, "접수 후 RESERVED");

    om.approveOrder(o->getOrderId());
    check(o->getStatus() == OrderStatus::PRODUCING, "승인 후 재고 부족 → PRODUCING");
    check(pl.hasCurrent(), "생산 큐에 Job 있음");

    // 생산 완료까지 대기 (0.5초 + 여유 0.2초)
    waitFor(0.7);
    pl.tick();
    check(o->getStatus() == OrderStatus::CONFIRMED, "생산 완료 후 CONFIRMED");
    // 재고: 3 + 10 - 10 = 3
    check(s->getStock() == 3, "생산 완료 후 재고 3 (3+10-10)");
    check(!pl.hasCurrent(), "생산 완료 후 큐 비어있음");

    // 출고
    ShippingManager sh(om);
    sh.ship(o->getOrderId());
    check(o->getStatus() == OrderStatus::RELEASE, "출고 후 RELEASE");
}

// =============================================================
// 시나리오 3: 주문 거절 경로
// =============================================================
void scenario3() {
    std::cout << "\n=== 시나리오 3: 주문 거절 경로 ===\n";
    SampleManager sm;
    ProductionLine pl;
    OrderManager om(sm, pl);

    Sample* s = sm.registerSample("InP-HBT", 3.0, 0.85, 5);
    Order* o = om.placeOrder(s->getId(), "C기관", 2);

    bool rejected = om.rejectOrder(o->getOrderId());
    check(rejected, "주문 거절 성공");
    check(o->getStatus() == OrderStatus::REJECTED, "거절 후 REJECTED");

    // 모니터링: REJECTED 주문은 활성 목록에 없음
    auto active = om.getAllActiveOrders();
    check(active.empty(), "활성 주문 목록에 REJECTED 주문 없음");

    // RESERVED 목록에도 없음
    auto reserved = om.getOrdersByStatus(OrderStatus::RESERVED);
    check(reserved.empty(), "RESERVED 목록 비어있음");
}

// =============================================================
// 시나리오 4: FIFO 큐 순서 검증
// stock=0, yield=0.9, avgTime=0.05
// 주문1: qty=5, shortage=5, targetQty=ceil(5/0.81)=7, totalTime=0.35초
// 주문2: qty=3, shortage=3, targetQty=ceil(3/0.81)=4, totalTime=0.20초
// 주문3: qty=4, shortage=4, targetQty=ceil(4/0.81)=5, totalTime=0.25초
// =============================================================
void scenario4() {
    std::cout << "\n=== 시나리오 4: 생산 큐 FIFO 순서 검증 ===\n";
    SampleManager sm;
    ProductionLine pl;
    OrderManager om(sm, pl);

    Sample* s = sm.registerSample("SiGe-BiCMOS", 0.05, 0.9, 0);
    Order* o1 = om.placeOrder(s->getId(), "고객1", 5);
    Order* o2 = om.placeOrder(s->getId(), "고객2", 3);
    Order* o3 = om.placeOrder(s->getId(), "고객3", 4);

    om.approveOrder(o1->getOrderId());
    om.approveOrder(o2->getOrderId());
    om.approveOrder(o3->getOrderId());

    check(o1->getStatus() == OrderStatus::PRODUCING, "주문1 PRODUCING");
    check(o2->getStatus() == OrderStatus::PRODUCING, "주문2 PRODUCING");
    check(o3->getStatus() == OrderStatus::PRODUCING, "주문3 PRODUCING");
    check(pl.getQueueSize() == 2, "대기 큐 크기 2 (주문2, 주문3)");

    // 현재 생산 중인 것이 주문1인지 확인
    const ProductionJob* cur = pl.getCurrentJob();
    check(cur != nullptr && cur->getOrder()->getOrderId() == o1->getOrderId(),
          "현재 생산 중: 주문1");

    // 대기 큐 순서 확인 (주문2 → 주문3)
    auto waiting = pl.getWaitingJobs();
    check(waiting.size() == 2, "대기 목록 크기 2");
    check(waiting[0].getOrder()->getOrderId() == o2->getOrderId(), "대기 1번: 주문2");
    check(waiting[1].getOrder()->getOrderId() == o3->getOrderId(), "대기 2번: 주문3");

    // 주문1 완료 대기 (0.35초 + 여유)
    waitFor(0.5);
    pl.tick();
    check(o1->getStatus() == OrderStatus::CONFIRMED, "주문1 생산 완료 → CONFIRMED");
    cur = pl.getCurrentJob();
    check(cur != nullptr && cur->getOrder()->getOrderId() == o2->getOrderId(),
          "주문1 완료 후 주문2 생산 시작");

    // 주문2 완료 대기
    waitFor(0.4);
    pl.tick();
    check(o2->getStatus() == OrderStatus::CONFIRMED, "주문2 생산 완료 → CONFIRMED");
    cur = pl.getCurrentJob();
    check(cur != nullptr && cur->getOrder()->getOrderId() == o3->getOrderId(),
          "주문2 완료 후 주문3 생산 시작");
}

// =============================================================
// 시나리오 5: 생산 진행률 중간 확인
// stock=0, yield=0.9, avgTime=0.05
// shortage=9, targetQty=ceil(9/0.81)=12, totalTime=0.6초
// 0.1초 후: producedQty = floor(0.1/0.05) = 2
// 완료 후: addStock(12)-reduceStock(9) → stock=3
// =============================================================
void scenario5() {
    std::cout << "\n=== 시나리오 5: 생산 진행률 중간 확인 ===\n";
    SampleManager sm;
    ProductionLine pl;
    OrderManager om(sm, pl);

    Sample* s = sm.registerSample("GaAs-pHEMT", 0.05, 0.9, 0);
    Order* o = om.placeOrder(s->getId(), "D사", 9);
    om.approveOrder(o->getOrderId());

    check(o->getStatus() == OrderStatus::PRODUCING, "승인 후 PRODUCING");

    const ProductionJob* job = pl.getCurrentJob();
    check(job != nullptr, "getCurrentJob() 반환 성공");
    check(job->getTargetQty() == 12, "targetQty = 12");
    // totalTime = 0.05 * 12 = 0.6
    double eps = 0.01;
    check(std::abs(job->getTotalTime() - 0.6) < eps, "totalTime = 0.6초");

    // 0.1초 후 진행률 확인
    waitFor(0.12);
    pl.tick();
    // 완료되지 않았을 때 확인
    if (o->getStatus() == OrderStatus::PRODUCING) {
        job = pl.getCurrentJob();
        int pqty = job->getProducedQty();
        check(pqty >= 2, "0.1초 후 producedQty >= 2");
    }

    // 0.6초 후 완료 확인
    waitFor(0.6);
    pl.tick();
    check(o->getStatus() == OrderStatus::CONFIRMED, "0.6초 후 생산 완료 → CONFIRMED");
    // 재고: 0 + 12 - 9 = 3
    check(s->getStock() == 3, "완료 후 재고 3 (0+12-9)");
}

// =============================================================
// 엣지 케이스
// =============================================================
void edgeCases() {
    std::cout << "\n=== 엣지 케이스 ===\n";
    SampleManager sm;
    ProductionLine pl;
    OrderManager om(sm, pl);

    Sample* s = sm.registerSample("TestSample", 1.0, 0.9, 5);

    // 1. 존재하지 않는 시료 ID로 주문 접수
    Order* badOrder = om.placeOrder(9999, "고객", 1);
    check(badOrder == nullptr, "존재하지 않는 시료 ID → null 반환");

    // 2. RESERVED 아닌 주문 승인 시도 (재고 충분으로 즉시 CONFIRMED)
    Order* o = om.placeOrder(s->getId(), "고객A", 1);
    om.approveOrder(o->getOrderId()); // CONFIRMED
    bool reapprove = om.approveOrder(o->getOrderId());
    check(!reapprove, "CONFIRMED 주문 재승인 → false");

    // 3. CONFIRMED 아닌 주문 출고 시도 (RESERVED 상태)
    Order* o2 = om.placeOrder(s->getId(), "고객B", 1);
    // o2는 RESERVED 상태
    ShippingManager sh(om);
    bool badShip = sh.ship(o2->getOrderId());
    check(!badShip, "RESERVED 주문 출고 시도 → false");

    // 4. 생산 중 없을 때 현황 조회
    ProductionLine emptyPl;
    check(!emptyPl.hasCurrent(), "비어있는 ProductionLine.hasCurrent() = false");
    check(emptyPl.isEmpty(), "비어있는 ProductionLine.isEmpty() = true");
    check(emptyPl.getCurrentJob() == nullptr, "getCurrentJob() on empty → nullptr");

    // 5. reduceStock 재고 부족 시 false 반환 및 재고 유지
    Sample* s2 = sm.registerSample("LowStock", 1.0, 0.9, 2);
    bool reduced = s2->reduceStock(5);
    check(!reduced, "재고 부족 시 reduceStock false 반환");
    check(s2->getStock() == 2, "reduceStock 실패 시 재고 유지");

    // 6. searchByName 부분 일치
    sm.registerSample("AlGaN", 1.0, 0.9, 0);
    sm.registerSample("AlGaAs", 1.0, 0.9, 0);
    auto results = sm.searchByName("AlGa");
    check(results.size() == 2, "searchByName('AlGa') → 2건 반환");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    scenario1();
    scenario2();
    scenario3();
    scenario4();
    scenario5();
    edgeCases();

    std::cout << "\n========================================\n";
    std::cout << "  결과: " << g_pass << " PASS / " << g_fail << " FAIL\n";
    std::cout << "========================================\n";

    return g_fail > 0 ? 1 : 0;
}
