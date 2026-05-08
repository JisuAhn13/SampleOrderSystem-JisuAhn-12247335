# CLAUDE.md — SampleOrderSystem 프로젝트 가이드

## 프로젝트 개요

**반도체 시료 주문 관리 시스템** (C++ 콘솔 애플리케이션)
가상의 반도체 회사 S Semi의 시료 주문·생산·출고 흐름을 관리하는 시스템.

- 언어: **C++ (C++17 이상)**
- 빌드: **CMake** 또는 단일 Makefile
- 실행 방식: **콘솔(터미널) 기반 메뉴 인터페이스**
- 외부 라이브러리: 없음 (표준 라이브러리만 사용)

---

## 도메인 용어

| 용어 | 설명 |
|------|------|
| 시료 (Sample) | 반도체 웨이퍼 공정으로 만들어진 제품 단위 |
| 수율 (Yield) | 정상 시료 수 / 총 생산 시료 수 (0.0 ~ 1.0) |
| 실 생산량 | `ceil(부족분 / (수율 * 0.9))` |
| 총 생산 시간 | `평균 생산시간 × 실 생산량` |

---

## 주문 상태 머신

```
RESERVED → (승인) → 재고 충분 → CONFIRMED → RELEASE
RESERVED → (승인) → 재고 부족 → PRODUCING → CONFIRMED → RELEASE
RESERVED → (거절) → REJECTED
```

- 상태는 `enum class OrderStatus`로 관리
- 상태 전이는 반드시 지정된 경로만 허용

---

## 아키텍처 원칙

- **레이어 분리**: 데이터 모델 / 비즈니스 로직 / UI(콘솔 출력) 분리
- **단일 책임**: 클래스 하나당 하나의 역할
- **전역 상태 최소화**: 싱글턴 남용 금지, 필요한 경우 Manager 클래스로 집중
- 파일 입출력 없음 — 모든 데이터는 메모리(런타임) 상에서만 관리
- 생산 큐는 **FIFO** (std::queue 사용)

---

## 주요 클래스 구조 (예정)

```
Sample          — 시료 정보 (ID, 이름, 평균 생산시간, 수율, 재고)
Order           — 주문 정보 (주문 ID, 시료, 고객명, 수량, 상태)
ProductionJob   — 생산 작업 (대상 Order, 목표 생산량, 현재 생산량, 생산 시작 시각)
SampleManager   — 시료 CRUD, 재고 관리
OrderManager    — 주문 접수, 승인, 거절
ProductionLine  — 생산 큐(FIFO) 관리, 생산 시뮬레이션
Monitor         — 주문 현황, 재고 현황 조회
ShippingManager — 출고 처리
MenuUI          — 콘솔 메뉴 출력 및 입력 처리
```

---

## 코딩 컨벤션

- 클래스명: `PascalCase`
- 멤버 변수: `m_camelCase` (접두사 `m_`)
- 함수명: `camelCase`
- 상수/열거형: `UPPER_SNAKE_CASE`
- 헤더: `.h`, 구현: `.cpp`
- 헤더 가드: `#pragma once`

---

## 파일 구조 (예정)

```
SampleOrderSystem/
├── CMakeLists.txt
├── CLAUDE.md
├── PLAN.md
├── README_update.md
├── include/
│   ├── Sample.h
│   ├── Order.h
│   ├── ProductionJob.h
│   ├── SampleManager.h
│   ├── OrderManager.h
│   ├── ProductionLine.h
│   ├── Monitor.h
│   ├── ShippingManager.h
│   └── MenuUI.h
└── src/
    ├── main.cpp
    ├── Sample.cpp
    ├── Order.cpp
    ├── ProductionJob.cpp
    ├── SampleManager.cpp
    ├── OrderManager.cpp
    ├── ProductionLine.cpp
    ├── Monitor.cpp
    ├── ShippingManager.cpp
    └── MenuUI.cpp
```

---

## 생산 시뮬레이션 방식

- **시간 기반 순차 처리**: 생산 큐(FIFO)의 첫 번째 Job만 진행되며, 완료 후 다음 Job이 시작됨
- `startTime`은 해당 Job이 큐의 맨 앞이 되는 순간 기록
- 사용자가 생산 라인 메뉴를 조회할 때마다 `update()` 호출 → 경과 시간으로 `producedQty` 동적 계산
- `producedQty >= targetQty` 이면 자동으로 `PRODUCING → CONFIRMED` 전환 및 재고 반영
- 다음 대기 Job이 있으면 즉시 해당 Job의 `startTime`을 기록하여 생산 시작

```
producedQty = floor(경과시간(초) / avgProductionTime)
경과시간    = 현재시각 - startTime   (std::chrono 사용)
```

---

## 주의 사항

- 재고 차감은 **승인(CONFIRMED 전환) 시점**에 수행 (재고 충분 경로: 즉시 차감 / 생산 경로: 생산 완료 시 차감)
- `REJECTED` 주문은 모니터링 목록에서 제외
- 수율 및 생산량 계산 시 소수점 처리에 주의 (`double` 사용, 최종값은 `ceil`로 올림)
