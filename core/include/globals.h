#pragma once
#include <string>
using namespace std;
#include <cstdint>

#define NOT_A_FILE 0b1111111011111110111111101111111011111110111111101111111011111110
#define NOT_H_FILE 0b0111111101111111011111110111111101111111011111110111111101111111
#define NOT_8_RANK 0b0000000011111111111111111111111111111111111111111111111111111111
#define NOT_1_RANK 0b1111111111111111111111111111111111111111111111111111111100000000

// #define USAGE_M "USAGE : Chessie {uci/play}"

#define MAGIC_NUMBER TODO

#define INF 1000000
#define MATE_VAL 30000

typedef uint64_t Bitboard;

/*
  A Move is a 32 bit unsigned int which packs all information of a move in bit patterns...
  Range boundaries are inclusive
  bits  : Representing
  5-0   : from square
  11-6  : to square
  12    : moving side
  15-13 : moving piece type (0 : king, 1 : queen, 2 : bishop, 3 : knight, 4 : rook, 5 : pawn)
  16    : capture flag
  19-17 : captured piece type (meaningless if capture flag is not 1) (0 : king, 1 : queen, 2 : bishop, 3 : knight, 4 : rook, 5 : pawn)
  20    : promotion flag
  22-21 : promotion piece type (0 : queen, 1 : bishop, 2 : knight, 3 : rook)
  23    : double push flag
  29-24 : en passant square (all 6 bits 0 if no en passant possible)
  30    : castle flag
  31    : castling side (if castle flag is 1) (0 : king side, 1 : queen side)
*/
typedef uint32_t Move;

// Accessor methods for Move
uint8_t moveFrom(const Move&);
uint8_t moveTo(const Move&);
uint8_t movingSide(const Move&);
uint8_t movingPieceType(const Move&);
bool    isCapturing(const Move&);
uint8_t capturedPieceType(const Move&);
bool    isPromoting(const Move&);
uint8_t promotionPieceType(const Move&);
bool    isDoublePush(const Move&);
uint8_t enPassantSquare(const Move&);
bool    isCastling(const Move&);
uint8_t castleSide(const Move&);



// Board utility functions
void showBoard(const Bitboard&);
string rankFileToString(const uint8_t&, const uint8_t&);
string squareToString(const uint8_t&);
string squareToString(const Bitboard&);


// Other utilities
uint64_t getRandomInclusive(const uint64_t&, const uint64_t&);
