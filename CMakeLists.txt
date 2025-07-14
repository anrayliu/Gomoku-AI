cmake_minimum_required(VERSION 3.30)
project(Gomoku)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL2/include)
set(SP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SerialPort)
set(SDL2_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL2/lib)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SP_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/assets)

aux_source_directory(src SRC_FILES)

add_executable(Gomoku ${SRC_FILES}
        SerialPort/SerialPort.cpp
        SerialPort/SerialPort.hpp
        src/Core.cpp
        src/Core.h
        src/Piece.cpp
        src/Piece.h
        src/Timer.cpp
        src/Timer.h
        src/SerialHandler.cpp
        src/SerialHandler.h
)

target_link_libraries(${PROJECT_NAME} mingw32 SDL2main SDL2 SDL2_image)
