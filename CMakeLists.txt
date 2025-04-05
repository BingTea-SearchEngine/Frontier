cmake_minimum_required(VERSION 3.31)

set(THIS Frontier)

project(${THIS} CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

enable_testing()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(
    Gateway
    GIT_REPOSITORY https://github.com/BingTea-SearchEngine/Gateway.git
    GIT_TAG main
)
FetchContent_MakeAvailable(Gateway)

find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL Include Dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_library(PriorityQueue STATIC ${LIB_DIR}/PriorityQueue/PriorityQueue.cpp)
target_include_directories(PriorityQueue INTERFACE ${LIB_DIR}/PriorityQueue)

add_library(FrontierInterface STATIC ${LIB_DIR}/FrontierInterface/FrontierInterface.cpp)
target_include_directories(FrontierInterface PUBLIC ${LIB_DIR}/FrontierInterface)

add_library(BloomFilter INTERFACE)
target_include_directories(BloomFilter INTERFACE ${LIB_DIR}/BloomFilter ${OPENSSL_INCLUDE_DIR})
if(TARGET OpenSSL::Crypto)
    target_link_libraries(BloomFilter INTERFACE OpenSSL::Crypto)
else()
    message(FATAL_ERROR "OpenSSL::Crypto was not found!")
endif()

set(GATEWAY_SOURCE_DIR ${gateway_SOURCE_DIR})
set(GATEWAY_INCLUDE_DIR "${gateway_SOURCE_DIR}/lib")
message(STATUS "Gateway project source directory: ${GATEWAY_SOURCE_DIR}")
message(STATUS "Gateway include directory: ${GATEWAY_INCLUDE_DIR}")

add_executable(${THIS} src/Frontier.cpp)
target_link_libraries(${THIS} PUBLIC FrontierInterface spdlog::spdlog argparse GatewayServer PriorityQueue
    BloomFilter)
target_include_directories(${THIS} PRIVATE ${GATEWAY_INCLUDE_DIR})
# target_link_libraries(${THIS} PRIVATE PriorityQueue BloomFilter)

add_executable(Tests tests/test.cpp)
target_link_libraries(Tests PRIVATE PriorityQueue GTest::gtest_main)

add_definitions(-DPROJECT_ROOT=\"${CMAKE_CURRENT_SOURCE_DIR}/\")
add_executable(FrontierInterfaceTests tests/FrontierInterfaceTests.cpp)
target_link_libraries(FrontierInterfaceTests PRIVATE FrontierInterface GTest::gtest_main)
add_executable(PriorityQueueTests tests/PriorityQueueTests.cpp)
target_link_libraries(PriorityQueueTests PRIVATE PriorityQueue GTest::gtest_main)
add_executable(BloomFilterTests tests/BloomFilterTests.cpp)
target_link_libraries(BloomFilterTests PRIVATE BloomFilter GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(Tests)
gtest_discover_tests(FrontierInterfaceTests)
gtest_discover_tests(PriorityQueueTests)
gtest_discover_tests(BloomFilterTests)

# Specify build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
