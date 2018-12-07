CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(test-client)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic -pthread -std=c++11 -Wl,--no-as-needed")
SET(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES(
    ../include
    ${PROJECT_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(../3rdLib/uvpp/include)
INCLUDE_DIRECTORIES(../3rdLib/libuv/include)

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../3rdLib/libuv/linux)


ADD_EXECUTABLE(${PROJECT_NAME} main.cpp Client.cpp TcpConnection.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} uv)
