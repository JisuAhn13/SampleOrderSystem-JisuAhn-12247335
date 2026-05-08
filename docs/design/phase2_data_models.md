# Phase 2. 데이터 모델 구현

## 목표

시스템의 핵심 데이터를 담는 3개 클래스를 구현한다.
비즈니스 로직은 포함하지 않고 데이터 보관과 단순 접근만 담당한다.

---

## 2-1. Sample (시료)

### 역할
시료의 속성과 현재 재고 수량을 보관한다.

### 클래스 설계

```cpp
class Sample {
    std::string m_id;
    std::string m_name;
    double      m_avgProductionTime;  // 단위: 초 (1개 생산 시 소요 시간)
    double      m_yield;              // 수율 (0.0 ~ 1.0)
    int         m_stock;              // 현재 재고 수량
public:
    Sample(const std::string& id, const std::string& name,
           double avgTime, double yield, int initStock = 0);

    const std::string& getId()                 const;
    const std::string& getName()               const;
    double             getAvgProductionTime()  const;
    double             getYield()              const;
    int                getStock()              const;

    void addStock(int qty);
    bool reduceStock(int qty);  // 재고 부족 시 false 반환, 차감 안 함
};
```

### 주의 사항
- `avgProductionTime` 단위는 **초(second)** → 시뮬레이션에서 실제 경과 시간과 비교
- `reduceStock` 은 차감 전 검증 후 실패 시 false 반환 (예외 없음)
- ID는 사용자가 직접 입력, SampleManager가 중복 여부 검증 후 등록

---

## 2-2. Order (주문)

### 역할
주문 1건의 정보와 현재 상태를 보관한다.

### OrderStatus 열거형

```cpp
enum class OrderStatus {
    RESERVED,   // 주문 접수됨
    CONFIRMED,  // 승인 완료, 출고 대기
    PRODUCING,  // 생산 중
    RELEASE,    // 출고 완료
    REJECTED    // 거절됨
};

std::string statusToString(OrderStatus s);  // 출력용 문자열 변환
```

### 클래스 설계

```cpp
class Order {
    int         m_orderId;
    Sample*     m_sample;        // 비소유 포인터 (SampleManager 가 소유)
    std::string m_customerName;
    int         m_quantity;
    OrderStatus m_status;
public:
    Order(int id, Sample* sample,
          const std::string& customer, int qty);

    int                getOrderId()      const;
    Sample*            getSample()       const;
    const std::string& getCustomerName() const;
    int                getQuantity()     const;
    OrderStatus        getStatus()       const;

    void setStatus(OrderStatus s);  // OrderManager 만 호출
};
```

### 상태 전이 규칙

```
RESERVED  →  CONFIRMED  (승인, 재고 충분)
RESERVED  →  PRODUCING  (승인, 재고 부족)
RESERVED  →  REJECTED   (거절)
PRODUCING →  CONFIRMED  (생산 완료)
CONFIRMED →  RELEASE    (출고)
```

- 그 외 전이는 허용하지 않음 (OrderManager 에서 검증)

---

## 2-3. ProductionJob (생산 작업)

### 역할
생산 큐에 등록된 작업 1건을 표현한다.
시간 기반으로 현재 생산량을 계산하고 완료 여부를 판단한다.

### 클래스 설계

```cpp
#include <chrono>

class ProductionJob {
    using Clock      = std::chrono::steady_clock;
    using TimePoint  = std::chrono::time_point<Clock>;

    Order*     m_order;
    int        m_targetQty;    // 실 생산량 = ceil(부족분 / (수율 * 0.9))
    double     m_totalTime;    // 총 생산 시간 = avgProductionTime * targetQty (초)
    TimePoint  m_startTime;    // 생산 시작 시각 (start() 호출 시 기록)
    int        m_producedQty;  // 현재까지 생산된 수량
    bool       m_started;      // start() 호출 여부

public:
    ProductionJob(Order* order, int shortage);

    void start();   // 생산 시작 시각 기록 (ProductionLine 이 호출)
    bool update();  // producedQty 갱신, 완료 시 true 반환
    void complete();// 재고 반영 + Order 상태 CONFIRMED 전환

    Order* getOrder()          const;
    int    getTargetQty()      const;
    double getTotalTime()      const;
    int    getProducedQty()    const;
    double getElapsedTime()    const;  // 경과 시간 (초)
    double getRemainingTime()  const;  // 잔여 시간 (초)
    bool   isStarted()         const;
};
```

### 핵심 계산 로직

```
생성 시:
  targetQty = ceil(shortage / (yield * 0.9))
  totalTime = avgProductionTime * targetQty

update() 호출 시:
  elapsed     = 현재시각 - startTime  (초 단위 double)
  producedQty = min(targetQty, (int)floor(elapsed / avgProductionTime))
  완료 조건   = producedQty >= targetQty

complete() 호출 시:
  sample->addStock(targetQty)          // 생산된 수량 재고에 반영
  sample->reduceStock(order.quantity)  // 주문 수량 재고에서 차감
  order->setStatus(CONFIRMED)
```

### 주의 사항
- `start()` 는 ProductionLine 이 Job을 현재 작업으로 설정할 때만 호출
  (큐 대기 중에는 호출 금지 → 대기 시간이 생산 시간에 포함되면 안 됨)
- `complete()` 는 `update()` 가 true 반환한 직후에만 호출
- `addStock` 후 `reduceStock` 순서 보장 → addStock 이 먼저여야 reduceStock 이 성공
