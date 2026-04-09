cmake_minimum_required(VERSION 3.15)
project(SmirnovLang VERSION 0.7.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/ast/ast.cpp
    src/vm/vm.cpp
    src/runtime/runtime.cpp
)

add_executable(smirnovLang ${SOURCES})

# Enable warnings
if(MSVC)
    target_compile_options(smirnovLang PRIVATE /W4)
else()
    target_compile_options(smirnovLang PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Testing
enable_testing()
find_package(GTest)
if(GTest_FOUND)
    add_subdirectory(tests)
endif()
