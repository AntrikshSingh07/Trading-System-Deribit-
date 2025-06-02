cmake_minimum_required(VERSION 3.10)

project(HFT_WebSocket)
set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT "C:/boost_1_87_0")

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(deribit_trader
    deribit_trader.cpp
    websocket_handler.cpp
    trade_execution.cpp
    latency_module.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(deribit_trader PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(deribit_trader
    PRIVATE
    ${Boost_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    ws2_32
)
