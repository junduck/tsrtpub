cmake_minimum_required(VERSION 3.0.0)
project(tsrtpub VERSION 0.1.0 LANGUAGES CXX)

set(USE_TLS TRUE)
add_subdirectory(IXWebSocket)

set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED ON)

set( tsrt_src
  tsrt/sub.cpp
  main.cpp
)

set( tsrt_header_dir
  tsrt
)

find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)

add_executable(${PROJECT_NAME} ${tsrt_src})

target_include_directories(${PROJECT_NAME} PRIVATE ${tsrt_header_dir} )
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket cppzmq )
