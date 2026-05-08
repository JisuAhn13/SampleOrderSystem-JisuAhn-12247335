# Phase 3. 비즈니스 로직 구현

## 목표

5개의 Manager/Controller 클래스를 구현한다.
각 클래스는 단일 책임을 가지며, 데이터 모델을 조합하여 시스템 흐름을 처리한다.

---

## 3-1. SampleManager (시료 관리)

### 역할
Sample 객체의 생성·저장·검색을 담당한다. Sample 의 소유권을 가진다.

### 클래스 설계

```cpp
class SampleManager {
    std::vector<Sample> m_samples;  // Sample 소유
public:
    SampleManager();

    Sample* registerSample(const std::string& id,
                           const std::string& name,
                           double avgTime, double yield);  // 중복 ID 시 nullptr 반환

    std::vector<Sample*> getAllSamples();
    Sample*              findById(const std::string& id);
    std::vector<Sample*> searchByName(const std::string& keyword);
};
```

### 구현 주의 사항
- `m_samples` 는 `std::vector<Sample>` (값 저장) → 반환 시 반드시 **포인터** 반환
- `push_back` / `emplace_back` 모두 재할당 시 기존 포인터 무효화 위험 (차이 없음)
  - 해결: `reserve` 로 충분한 공간 확보하거나 `std::deque`/`std::list` 사용 검토
- `searchByName` : `std::string::find` 로 부분 일치 검색
- `getWaitingJobs()` : `std::queue`는 반복자를 제공하지 않으므로, 큐를 임시 복사한 뒤 순회하여 `std::vector`로 변환

> **포인터 무효화 주의**: `m_samples.emplace_back()` 호출 시 벡터가 재할당되면
> 이전에 반환한 `Sample*` 포인터가 무효화됨.
> → `reserve(충분한 크기)` 로 예방하거나, 인덱스 기반으로 `findById` 를 통해 재탐색

---

## 3-2. OrderManager (주문 관리)

### 역할
주문의 생성·승인·거절을 처리한다. Order 객체의 소유권을 가진다.

### 클래스 설계

```cpp
class OrderManager {
    std::vector<Order> m_orders;  // Order 소유
    int m_nextId;
    SampleManager&  m_sampleMgr;
    ProductionLine& m_productionLine;
public:
    OrderManager(SampleManager& sm, ProductionLine& pl);

    Order* placeOrder(const std::string& sampleId,
                      const std::string& customer, int qty);
    bool   approveOrder(int orderId);
    bool   rejectOrder(int orderId);

    std::vector<Order*> getOrdersByStatus(OrderStatus status);
    std::vector<Order*> getAllActiveOrders();  // REJECTED 제외
    Order*              findById(int orderId);
};
```

### 승인 로직 흐름

```
approveOrder(orderId):
  1. findById → 없거나 RESERVED 아니면 false 반환
  2. stock = sample->getStock()
  3. if stock >= quantity:
       sample->reduceStock(quantity)   ← 즉시 재고 차감
       order->setStatus(CONFIRMED)
     else:
       shortage = quantity - stock
       productionLine.enqueue(order, shortage)
       order->setStatus(PRODUCING)
  4. return true
```

### 재고 차감 정책

| 경로 | 차감 시점 | 비고 |
|------|-----------|------|
| 재고 충분 (CONFIRMED) | 승인 시 즉시 | `reduceStock(quantity)` |
| 재고 부족 (PRODUCING) | 생산 완료 시 | `ProductionJob::complete()` 에서 처리 |

---

## 3-3. ProductionLine (생산 라인)

### 역할
생산 큐(FIFO)를 관리하고, 시간 기반으로 생산 진행 상태를 갱신한다.

### 클래스 설계

```cpp
#include <optional>
#include <queue>

class ProductionLine {
    std::optional<ProductionJob> m_current;     // 현재 생산 중인 Job
    std::queue<ProductionJob>    m_queue;        // 대기 중인 Job (FIFO)
    std::vector<ProductionJob>   m_history;      // 생산 완료 이력
public:
    void enqueue(Order* order, int shortage);
    void tick();  // 생산 상태 갱신 (메뉴 진입 시마다 호출)

    bool hasCurrent()  const;
    bool isEmpty()     const;  // current 없고 queue 도 비어 있음
    int  getQueueSize() const;

    const ProductionJob*              getCurrentJob()    const;
    std::vector<ProductionJob>        getWaitingJobs()   const;
};
```

### tick() 동작 흐름

```
tick():
  if m_current 없음 → return

  m_current->update()
  if 완료:
    m_current->complete()       ← 재고 반영, CONFIRMED 전환
    m_history.push_back(*m_current)
    m_current.reset()

    if m_queue 비어있지 않음:
      m_current = m_queue.front()
      m_queue.pop()
      m_current->start()        ← 새 Job 생산 시작 시각 기록
```

### enqueue() 동작 흐름

```
enqueue(order, shortage):
  job = ProductionJob(order, shortage)
  if m_current 없음:
    m_current = job
    m_current->start()          ← 즉시 생산 시작
  else:
    m_queue.push(job)           ← 대기열 추가
```

### tick() 호출 시점
- MenuUI 의 **모든 서브메뉴 진입 직전** 에 호출
- 사용자가 어떤 메뉴를 선택하든 생산 상태가 최신화됨

---

## 3-4. Monitor (모니터링)

### 역할
주문 현황과 재고 현황을 조회·출력한다. 데이터를 수정하지 않는다.

### 클래스 설계

```cpp
class Monitor {
    OrderManager&  m_orderMgr;
    SampleManager& m_sampleMgr;
public:
    Monitor(OrderManager& om, SampleManager& sm);

    void showOrderStatus();  // 상태별 주문 목록
    void showStockStatus();  // 시료별 재고 + 상태
};
```

### showOrderStatus() 표시 대상

| 표시 O | 표시 X |
|--------|--------|
| RESERVED | REJECTED |
| PRODUCING | |
| CONFIRMED | |
| RELEASE | |

### showStockStatus() 재고 상태 판단 기준

```
reservedDemand = RESERVED 상태인 해당 시료 주문 수량의 합계

if stock == 0                  → "고갈"
elif stock < reservedDemand   → "부족"
else                           → "여유"
(reservedDemand == 0이고 stock > 0이면 항상 "여유"로 판단)
```

---

## 3-5. ShippingManager (출고 처리)

### 역할
CONFIRMED 상태의 주문을 출고 처리(RELEASE 전환)한다.

### 클래스 설계

```cpp
class ShippingManager {
    OrderManager& m_orderMgr;
public:
    explicit ShippingManager(OrderManager& om);

    void showConfirmedOrders() const;
    bool ship(int orderId);
};
```

### ship() 동작 흐름

```
ship(orderId):
  1. findById → 없거나 CONFIRMED 아니면 false 반환
  2. order->setStatus(RELEASE)
  3. return true
```

### 주의 사항
- 재고 차감은 이미 승인/생산 완료 시점에 처리됨 → 출고 시 재고 변경 없음
- CONFIRMED 가 아닌 주문 ID 입력 시 오류 메시지 출력 후 재입력 유도 (MenuUI 에서 처리)
