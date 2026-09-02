cmake_minimum_required(VERSION 3.20)

project(Chessie
  VERSION 0.0.1
  DESCRIPTION "My awesome Chess Engine"
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(chessie_core STATIC
  core/src/board.cpp
  core/src/game.cpp
  core/src/globals.cpp
  core/src/move_handler.cpp
  core/src/perft.cpp
  core/src/search.cpp
)


add_executable(${PROJECT_NAME}
  main.cpp frontends/uci/src/uci.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    core/include
    frontends/uci/include
)

target_link_libraries(Chessie PRIVATE chessie_core)
