cmake_minimum_required(VERSION 3.16)
project(SampleOrderSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── 메인 실행 파일 ───────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(SampleOrderSystem ${SOURCES})
target_include_directories(SampleOrderSystem PRIVATE include)
if(MSVC)
    target_compile_options(SampleOrderSystem PRIVATE /utf-8)
endif()

# ─── 기존 통합 테스트 (test_main.cpp) ────────────────────────────────────────
file(GLOB TEST_SOURCES "src/*.cpp")
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_executable(SampleOrderSystemTest
    ${TEST_SOURCES}
    test/test_main.cpp
)
target_include_directories(SampleOrderSystemTest PRIVATE include)
set_target_properties(SampleOrderSystemTest PROPERTIES CXX_STANDARD 17)
if(MSVC)
    target_compile_options(SampleOrderSystemTest PRIVATE /utf-8)
endif()

# ─── GMock 테스트 ─────────────────────────────────────────────────────────────
set(GMOCK_ROOT "${CMAKE_SOURCE_DIR}/packages/gmock.1.11.0/lib/native")
set(GMOCK_INCLUDE "${GMOCK_ROOT}/include")
set(GMOCK_SRC     "${GMOCK_ROOT}/src")

add_executable(SampleOrderSystemGMockTest
    ${GMOCK_SRC}/gtest/gtest-all.cc
    ${GMOCK_SRC}/gmock/gmock-all.cc
    ${GMOCK_SRC}/gmock/src/gmock_main.cc
    ${TEST_SOURCES}
    test/gmock_test.cpp
)
target_include_directories(SampleOrderSystemGMockTest PRIVATE
    include
    ${GMOCK_INCLUDE}
    ${GMOCK_SRC}/gtest
)
set_target_properties(SampleOrderSystemGMockTest PROPERTIES CXX_STANDARD 17)
if(MSVC)
    target_compile_options(SampleOrderSystemGMockTest PRIVATE /utf-8 /wd4251)
endif()
