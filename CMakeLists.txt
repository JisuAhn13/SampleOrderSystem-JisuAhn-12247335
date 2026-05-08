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

# 테스트 타겟: src/main.cpp 제외한 모든 src/*.cpp + test/test_main.cpp
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
