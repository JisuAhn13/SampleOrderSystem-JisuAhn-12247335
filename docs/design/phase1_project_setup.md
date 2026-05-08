# Phase 1. 프로젝트 기반 설정

## 목표

빌드 환경 구성과 전체 프로그램 진입점(main.cpp) 뼈대를 작성한다.

---

## 1-1. 디렉토리 구조

```
SampleOrderSystem/
├── CMakeLists.txt
├── docs/design/
├── include/          ← 헤더 파일 (.h)
└── src/              ← 구현 파일 (.cpp)
```

---

## 1-2. CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(SampleOrderSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(SampleOrderSystem ${SOURCES})
target_include_directories(SampleOrderSystem PRIVATE include)

if(MSVC)
    target_compile_options(SampleOrderSystem PRIVATE /utf-8)
endif()
```

- `file(GLOB_RECURSE)` 로 src/ 하위 모든 .cpp 자동 수집
- MSVC 환경에서 한글 소스 파일 인코딩을 UTF-8로 강제

---

## 1-3. main.cpp 구조

```
역할: 모든 Manager 객체를 생성하고 MenuUI에 주입한 뒤 실행
```

### 객체 생성 순서 (의존 관계 기반)

```
1. SampleManager     ← 의존 없음
2. ProductionLine    ← 의존 없음
3. OrderManager      ← SampleManager, ProductionLine 필요
4. Monitor           ← OrderManager, SampleManager 필요
5. ShippingManager   ← OrderManager 필요
6. MenuUI            ← 위 5개 모두 필요
```

### 의사 코드

```cpp
int main() {
    // Windows 한글 출력 설정
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    SampleManager  sampleMgr;
    ProductionLine productionLine;
    OrderManager   orderMgr(sampleMgr, productionLine);
    Monitor        monitor(orderMgr, sampleMgr);
    ShippingManager shippingMgr(orderMgr);
    MenuUI ui(sampleMgr, orderMgr, productionLine, monitor, shippingMgr);

    ui.run();
    return 0;
}
```

---

## 1-4. 주의 사항

| 항목 | 내용 |
|------|------|
| 한글 출력 | Windows에서 `SetConsoleOutputCP(65001)` 필요, `#ifdef _WIN32` 로 플랫폼 분기 |
| 객체 수명 | 모든 Manager는 main 스택에서 생성 → 포인터/참조 전달 시 수명 보장됨 |
| 순환 의존 | OrderManager ↔ ProductionLine 간 순환 없도록 헤더에서 전방 선언 사용 |
