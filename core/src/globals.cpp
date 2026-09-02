#include "../include/globals.h"
#include <random>
#include <iostream>



void showBoard(const Bitboard &board) {
  Bitboard mask;
  for(int rank = 7; rank >= 0; rank--) {
    mask = Bitboard(1) << (rank * 8);
    for(int file = 0; file < 8; file++) {
      cout << ((mask & board) ? 1 : 0) << " ";
      mask <<= 1;
    }
    cout << endl;
  }
  cout << endl << endl;
}

string rankFileToString(const uint8_t &rank, const uint8_t &file) {
  string result = "";
  result += char('a' + file);
  result += char('1' + rank);
  return result;
}

string squareToString(const uint8_t &square) {
  return rankFileToString(square / 8, square % 8);
}

string squareToString(const Bitboard &square) {
  for(int i = 0; i < 64; i++) {
    if(square & (1ULL << i)) return squareToString(uint8_t(i));
  }
  return "Invalid square";
}



// Move accessor functions
uint8_t moveFrom(const Move& move) {return move & 0x3F;}
uint8_t moveTo(const Move& move) {return (move >> 6) & 0x3F;}
uint8_t movingSide(const Move& move) {return (move >> 12) & 0x1;}
uint8_t movingPieceType(const Move& move) {return (move >> 13) & 0x7;}
bool    isCapturing(const Move& move) {return (move >> 16) & 0x1;}
uint8_t capturedPieceType(const Move &move) {return (move >> 17) & 0x7;}
bool    isPromoting(const Move &move) {return (move >> 20) & 0x1;}
uint8_t promotionPieceType(const Move &move) {return (move >> 21) & 0x3;}
bool    isDoublePush(const Move &move) {return (move >> 23) & 0x1;}
uint8_t enPassantSquare(const Move &move) {return (move >> 24) & 0x3F;}
bool    isCastling(const Move &move) {return (move >> 30) & 0x1;}
uint8_t castleSide(const Move &move) {return (move >> 31) & 0x1;}


uint64_t getRandomInclusive(const uint64_t &from, const uint64_t &to) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> dist(from, to);
  return dist(gen);
}