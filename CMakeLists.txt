cmake_minimum_required(VERSION 3.0.0)
project(tsrtpub VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 14)
set (CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshorten-64-to-32")
endif()

set( tsrt_src
  tsrt_pub/tsrt_pub.cpp
  main.cpp
)

set( tsrt_header_dir
  tsrt_pub
)

find_package(ixwebsocket REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(cppzmq REQUIRED)

add_executable(${PROJECT_NAME} ${tsrt_src})

target_include_directories(${PROJECT_NAME} PRIVATE ${tsrt_header_dir} )
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json ixwebsocket::ixwebsocket cppzmq )
