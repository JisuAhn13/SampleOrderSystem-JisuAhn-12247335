# Phase 4. UI 레이어 구현

## 목표

콘솔 메뉴 인터페이스를 구현한다.
입출력과 사용자 흐름만 담당하며, 비즈니스 로직은 Manager에 위임한다.

---

## 4-1. MenuUI 클래스 설계

```cpp
class MenuUI {
    SampleManager&  m_sampleMgr;
    OrderManager&   m_orderMgr;
    ProductionLine& m_productionLine;
    Monitor&        m_monitor;
    ShippingManager& m_shippingMgr;
public:
    MenuUI(SampleManager& sm, OrderManager& om,
           ProductionLine& pl, Monitor& mo, ShippingManager& sh);

    void run();  // 메인 루프

private:
    // 서브메뉴 핸들러
    void handleSampleManagement();
    void handleOrderManagement();
    void handleMonitoring();
    void handleShipping();
    void handleProductionLine();

    // 공통 입력 유틸
    static int         getIntInput(const std::string& prompt);
    static double      getDoubleInput(const std::string& prompt);
    static std::string getStringInput(const std::string& prompt);

    // 공통 출력 유틸
    static void printHeader(const std::string& title);
    static void printSeparator();
};
```

---

## 4-2. 메뉴 트리 구조

```
메인 메뉴
├── 1. 시료 관리
│   ├── 1. 시료 등록
│   ├── 2. 시료 목록 조회
│   ├── 3. 시료 이름 검색
│   └── 0. 이전 메뉴
├── 2. 주문 관리
│   ├── 1. 주문 접수
│   ├── 2. 접수 주문 목록 (RESERVED)
│   ├── 3. 주문 승인
│   ├── 4. 주문 거절
│   └── 0. 이전 메뉴
├── 3. 모니터링
│   ├── 1. 주문 현황 (상태별)
│   ├── 2. 재고 현황
│   └── 0. 이전 메뉴
├── 4. 출고 처리
│   ├── 1. 출고 대기 목록 조회 (CONFIRMED)
│   ├── 2. 출고 실행
│   └── 0. 이전 메뉴
├── 5. 생산 라인
│   ├── 1. 생산 현황 (현재 생산 중인 Job)
│   ├── 2. 대기 주문 목록 (FIFO 큐)
│   └── 0. 이전 메뉴
└── 0. 종료
```

---

## 4-3. tick() 호출 규칙

모든 서브메뉴 핸들러 진입 시 `m_productionLine.tick()` 을 가장 먼저 호출한다.
→ 사용자가 어떤 메뉴를 선택해도 생산 상태가 자동으로 최신화됨.

```
void handleXxx() {
    m_productionLine.tick();  // ← 항상 첫 줄
    ...
}
```

---

## 4-4. 서브메뉴별 입출력 명세

### 시료 등록 입력

| 입력 항목 | 타입 | 검증 조건 |
|-----------|------|-----------|
| 시료 ID | string | 비어있지 않을 것, 중복 불가 |
| 시료 이름 | string | 비어있지 않을 것 |
| 평균 생산시간 | double | > 0 |
| 수율 | double | 0.0 < yield <= 1.0 |

### 주문 접수 입력

| 입력 항목 | 타입 | 검증 조건 |
|-----------|------|-----------|
| 시료 ID | string | 등록된 ID일 것 |
| 고객명 | string | 비어있지 않을 것 |
| 주문 수량 | int | > 0 |

### 출고 실행

- CONFIRMED 목록 먼저 출력 → 주문 ID 입력 → ship() 호출
- CONFIRMED 주문이 없으면 안내 메시지 출력 후 서브메뉴로 복귀

### 생산 현황 출력 항목

| 항목 | 출처 |
|------|------|
| 주문 ID | `job.getOrder()->getOrderId()` |
| 시료명 | `job.getOrder()->getSample()->getName()` |
| 실 생산량 (목표) | `job.getTargetQty()` |
| 현재 생산량 | `job.getProducedQty()` |
| 총 생산 시간 | `job.getTotalTime()` |
| 잔여 시간 | `job.getRemainingTime()` |

---

## 4-5. 입력 검증 방식

```
getIntInput(prompt):
  반복:
    std::cin >> val
    성공 → 반환
    실패 → cin.clear(), 버퍼 비우기, 오류 메시지 출력
```

- 잘못된 타입 입력 시 재입력 요청 (무한 루프 아님, 메뉴 복귀는 0 선택)
- 범위 검증(양수 여부 등)은 각 핸들러에서 추가 처리

---

## 4-6. 출력 포맷 규칙

- 메뉴 헤더: `=` 구분선 + 제목
- 목록 출력: `std::setw` 로 컬럼 정렬 (`<iomanip>` 사용)
- 빈 목록: "해당 항목이 없습니다." 안내 출력
- 성공/실패 메시지: 한 줄로 간결하게

### 출력 예시 (생산 현황)

```
========================================
  생산 라인 - 생산 현황
========================================
[현재 생산 중]
  주문 ID     : 3
  시료        : AlGaN-HEMT
  실 생산량   : 15 개
  현재 생산량 : 9 개
  총 생산시간 : 150.0 초
  잔여 시간   : 60.0 초
========================================
```
