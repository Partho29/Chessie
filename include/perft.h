#pragma once
#include "board.h"
#include <cstdint>
#include <string>


struct PerftTest {
  std::string fen;
  std::vector<uint64_t> expected;
};

uint64_t perft(Board&, int);
uint64_t perftDivide(Board&, int);
