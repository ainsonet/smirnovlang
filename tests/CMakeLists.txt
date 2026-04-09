cmake_minimum_required(VERSION 3.15)
project(SmirnovLangTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ../src)

# Test executable
add_executable(smirnovTests
    test_main.cpp
)

target_link_libraries(smirnovTests
    smirnovLang
    ${GTEST_LIBRARIES}
    pthread
)

add_test(NAME SmirnovLangTests COMMAND smirnovTests)
