#pragma once
#include "board.h"
#include "move_handler.h"
#include <cstdint>
#include <string>


struct PerftTest {
  std::string fen;
  std::vector<uint64_t> expected;
};

uint64_t perft(Board&, Move_Handler&, int);
uint64_t perftDivide(Board&, Move_Handler&, int);
