cmake_minimum_required(VERSION 3.20)

project(Chessie
  VERSION 0.0.1
  DESCRIPTION "My awesome Chess Engine"
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
  src/main.cpp
  src/move_handler.cpp
  src/globals.cpp
  src/game.cpp
  src/board.cpp
  src/uci.cpp
  src/perft.cpp
)

add_executable(${PROJECT_NAME}
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    include
)
