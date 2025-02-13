cmake_minimum_required(VERSION 3.31)

set(THIS Frontier)

project(${THIS} CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++17 -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

add_library(PriorityQueue INTERFACE)
target_include_directories(PriorityQueue INTERFACE ${LIB_DIR}/PriorityQueue)

add_library(FrontierInterface STATIC ${LIB_DIR}/FrontierInterface/FrontierInterface.cpp)
target_include_directories(FrontierInterface PUBLIC ${LIB_DIR}/FrontierInterface)

add_executable(${THIS} src/Frontier.cpp)
target_link_libraries(${THIS} PUBLIC FrontierInterface spdlog::spdlog)
target_link_libraries(${THIS} PRIVATE PriorityQueue)

add_executable(Tests tests/test.cpp)
target_link_libraries(Tests PRIVATE PriorityQueue GTest::gtest_main)

add_executable(FrontierInterfaceTests tests/FrontierInterfaceTests.cpp)
target_link_libraries(FrontierInterfaceTests PRIVATE FrontierInterface GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(Tests)
gtest_discover_tests(FrontierInterfaceTests)

