# CLAUDE.md — SampleOrderSystem 작업 가이드

## 참조 문서

| 문서 | 내용 |
|------|------|
| `PRD.md` | 제품 요구사항 (도메인 용어, 기능 명세, 상태 흐름) |
| `PLAN.md` | Phase별 개발 계획 및 확정된 설계 결정 |
| `docs/design/` | Phase별 세부 구현 설계 |

---

## 기술 스택

- 언어: **C++17** / 빌드: **CMake** / 외부 라이브러리 없음
- 실행: 콘솔 기반 메뉴 인터페이스
- 시간 처리: `std::chrono::steady_clock`

---

## 아키텍처 원칙

- **레이어 분리**: 데이터 모델 / 비즈니스 로직 / UI 분리
- **단일 책임**: 클래스 하나당 하나의 역할
- 파일 입출력 없음 — 모든 데이터는 런타임 메모리에서만 관리
- 생산 큐: **FIFO** (`std::queue`)

---

## 클래스 구조

```
[데이터 모델]
Sample          — 시료 정보 (ID, 이름, 평균 생산시간, 수율, 재고)
Order           — 주문 정보 (주문 ID, 시료, 고객명, 수량, 상태)
ProductionJob   — 생산 작업 (대상 Order, 목표 생산량, 현재 생산량, 시작 시각)

[비즈니스 로직]
SampleManager   — 시료 CRUD, 재고 관리
OrderManager    — 주문 접수, 승인, 거절
ProductionLine  — 생산 큐(FIFO) 관리, 시간 기반 생산 진행
Monitor         — 주문 현황, 재고 현황 조회
ShippingManager — 출고 처리

[UI]
MenuUI          — 콘솔 메뉴 출력 및 입력 처리
ConsoleUI       — 박스 테두리 출력 유틸리티 (namespace, MenuUI/Monitor/ShippingManager 공용)
```

---

## 코딩 컨벤션

| 대상 | 규칙 |
|------|------|
| 클래스명 | `PascalCase` |
| 멤버 변수 | `m_camelCase` |
| 함수명 | `camelCase` |
| 상수/열거형 | `UPPER_SNAKE_CASE` |
| 헤더 가드 | `#pragma once` |
| 헤더/구현 | `.h` / `.cpp` |

---

## 파일 구조

```
SampleOrderSystem/
├── CMakeLists.txt
├── PRD.md / CLAUDE.md / PLAN.md / README.md
├── .claude/agents/     ← pm, developer, qa 에이전트
├── docs/design/        ← phase1~5 세부 설계
├── include/            ← 헤더 파일
└── src/                ← 구현 파일
```

---

## 핵심 설계 결정

| 항목 | 결정 내용 |
|------|-----------|
| 생산 방식 | 시간 기반 순차 처리 (FIFO, `std::chrono`) |
| 현재 생산량 | `tick()` 호출 시 `update()`에서 `floor(경과시간 / avgProductionTime)`으로 `m_producedQty` 갱신; `getProducedQty()`는 캐시된 값 반환 |
| 완료 조건 | `producedQty >= targetQty` 시 자동 `CONFIRMED` 전환 |
| 재고 차감 | 재고 충분 경로: 승인 시 / 생산 경로: 생산 완료 시 |
| ID 입력 | 사용자가 직접 입력 (중복 불가) |

---

## 구현 주의 사항

- `ProductionJob::start()` 는 큐 대기 중 호출 금지 (생산 시작 시점에만)
- `Sample*` 반환 후 `vector` 재할당 시 포인터 무효화 주의
- `addStock()` 후 `reduceStock()` 순서 보장 필요
- Windows 한글 출력: `SetConsoleOutputCP(CP_UTF8)` 및 `SetConsoleCP(CP_UTF8)` 모두 적용 (출력 + 입력 인코딩)
- `tick()` 은 모든 서브메뉴 진입 시 첫 번째로 호출
