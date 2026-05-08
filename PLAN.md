# PLAN.md — SampleOrderSystem 개발 계획

## 목표

C++ 콘솔 기반 반도체 시료 주문 관리 시스템 구현.
메뉴 선택 방식으로 시료 관리 → 주문 → 생산 → 출고 흐름을 처리한다.

---

## 개발 단계

### Phase 1. 프로젝트 기반 설정
- [ ] 디렉토리 구조 생성 (`include/`, `src/`)
- [ ] `CMakeLists.txt` 작성 (C++17, 실행 파일 타겟)
- [ ] `main.cpp` 뼈대 작성 (메뉴 루프 진입점)

---

### Phase 2. 데이터 모델 구현

#### 2-1. `Sample` (시료)
```
필드: id (int), name (string), avgProductionTime (double), yield (double), stock (int)
```
- [ ] `Sample.h` / `Sample.cpp` 작성
- [ ] getter/setter, 재고 증감 메서드

#### 2-2. `Order` (주문)
```
필드: orderId (int), sample (Sample*), customerName (string),
      quantity (int), status (OrderStatus enum)
```
- [ ] `OrderStatus` enum class 정의
  - `RESERVED`, `CONFIRMED`, `PRODUCING`, `RELEASE`, `REJECTED`
- [ ] `Order.h` / `Order.cpp` 작성

#### 2-3. `ProductionJob` (생산 작업)
```
필드: order (Order*), targetQty (int), producedQty (int), startTime (time_point)
계산: targetQty  = ceil(shortage / (yield * 0.9))
      totalTime  = avgProductionTime * targetQty
      producedQty = floor(경과시간 / avgProductionTime)  ← 조회 시점에 동적 계산
```
- [ ] `ProductionJob.h` / `ProductionJob.cpp` 작성
- [ ] `update()` 메서드: 현재 시각 기준으로 producedQty 갱신, 완료 여부 반환

---

### Phase 3. 비즈니스 로직 구현

#### 3-1. `SampleManager` (시료 관리)
- [ ] 시료 등록 — ID 자동 채번
- [ ] 시료 전체 조회 (재고 포함)
- [ ] 시료 이름 검색

#### 3-2. `OrderManager` (주문 관리)
- [ ] 주문 접수 — 입력: 시료 ID, 고객명, 수량 → `RESERVED` 생성
- [ ] `RESERVED` 목록 조회
- [ ] 주문 승인 처리
  - 재고 충분 → `CONFIRMED` 전환, 재고 차감
  - 재고 부족 → `ProductionLine`에 Job 등록, `PRODUCING` 전환
- [ ] 주문 거절 → `REJECTED` 전환

#### 3-3. `ProductionLine` (생산 라인)
- [ ] `std::queue<ProductionJob>` 기반 FIFO 생산 큐
- [ ] **순차 처리**: 큐의 첫 번째 Job만 생산 진행, 완료 후 다음 Job 자동 시작
- [ ] `tick()` 메서드: 현재 생산 중 Job의 `update()` 호출 → 완료 시 `complete()`로 재고 반영 및 `CONFIRMED` 전환 → 다음 Job 자동 시작
  - 메뉴 진입 시마다 `tick()` 호출하여 상태 최신화
- [ ] 생산 현황 표시: 현재 Job의 시료명, targetQty, producedQty, 잔여 시간 등
- [ ] 대기 큐 목록 표시 (FIFO 순서)

#### 3-4. `Monitor` (모니터링)
- [ ] 주문량 현황: 상태별(`RESERVED` / `CONFIRMED` / `PRODUCING` / `RELEASE`) 목록 출력 (`REJECTED` 제외)
- [ ] 재고 현황: 시료별 재고 수량 + 상태 표기 (여유 / 부족 / 고갈)
  - 여유: 재고 ≥ 대기 주문 수량 합계
  - 부족: 0 < 재고 < 대기 주문 수량 합계
  - 고갈: 재고 == 0

#### 3-5. `ShippingManager` (출고 처리)
- [ ] `CONFIRMED` 상태 주문 목록 표시
- [ ] 출고 실행 → `RELEASE` 전환

---

### Phase 4. UI 레이어 구현

#### `MenuUI` (콘솔 메뉴)
- [ ] 메인 메뉴 루프 (1~5 선택 + 0 종료)
- [ ] 각 서브메뉴 진입 및 입력 처리
- [ ] 입력 검증 (잘못된 입력 시 재입력 유도)
- [ ] 출력 포맷 통일 (구분선, 헤더 등)

---

### Phase 5. 통합 테스트 및 검증

- [ ] 시나리오 1: 시료 등록 → 주문 → 재고 충분 → 승인 → 출고
- [ ] 시나리오 2: 시료 등록 → 주문 → 재고 부족 → 승인 → 생산 → 출고
- [ ] 시나리오 3: 주문 거절 흐름 및 모니터링 미표시 확인
- [ ] 시나리오 4: 생산 큐 FIFO 순서 검증 (다중 주문)
- [ ] 시나리오 5: 생산 진행률 중간 확인 (producedQty 정확성 검증)
- [ ] 엣지 케이스: 잘못된 입력 처리 (존재하지 않는 ID, 음수 수량, 수율 범위 초과 등)

---

## 클래스 의존 관계

```
MenuUI
 ├── SampleManager  ──→ Sample
 ├── OrderManager   ──→ Order       ──→ Sample
 │              └──→ ProductionLine ──→ ProductionJob
 ├── Monitor        ──→ OrderManager, SampleManager
 └── ShippingManager──→ OrderManager
```

---

## 구현 우선순위

| 순위 | 항목 | 이유 |
|------|------|------|
| 1 | Sample, Order, OrderStatus | 모든 기능의 기반 |
| 2 | SampleManager, OrderManager | 핵심 비즈니스 로직 |
| 3 | ProductionLine, ProductionJob | 재고 부족 흐름 처리 |
| 4 | Monitor, ShippingManager | 조회·출고 부가 기능 |
| 5 | MenuUI | 전체 통합 후 연결 |

---

## 확정된 설계 결정

| 항목 | 결정 내용 |
|------|-----------|
| 생산 방식 | 시간 기반 순차 처리 (FIFO, std::chrono 사용) |
| 생산량 공식 | `ceil(부족분 / (수율 * 0.9))` |
| 총 생산 시간 | `평균 생산시간 × 실 생산량` |
| 생산 완료 조건 | `producedQty >= targetQty` 시 자동 CONFIRMED 전환 |
| 큐 처리 | 첫 번째 Job 완료 후 다음 Job 자동 시작 |
| ID 채번 | 1부터 시작하는 자동 증가 정수 |
| 재고 차감 시점 | 재고 충분 경로: 승인 시 즉시 / 생산 경로: 생산 완료 시 |
