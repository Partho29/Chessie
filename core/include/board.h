#pragma once
#include "globals.h"
#include "move_handler.h"
#include <cstdint>
#include <vector>


struct Board {
  Bitboard pieces[2][6];  // in order : king, queen, bishop, knight, rook, pawn
  Bitboard occupancy[2];
  Bitboard occupancy_all;

  uint8_t mailbox[64]; // bit 0 : square filled or not, bit 1 : color, bit 2-7 : Piece type (bit 7 : king, bit 6 : queen, bit 5 : bishop, bit 4 : knight, bit 3 : rook, bit 2 : pawn)
  uint8_t sideToMove;
  uint8_t castlingRights; // left to right (0 to 3) bit 0 : white king side, bit 1 : white queen side, bit 2 : black king side, bit 3 : black queen size
  uint8_t enPassantSq;
  uint8_t halfmoveClock;
  uint8_t fullmoveClock;

  uint64_t zobristHash;

  struct ZobristKeys {
    Bitboard pieceSquare[2][6][64];
    Bitboard sideToMove;
    Bitboard castlingRights[16];
    Bitboard enPassantFile[8];
  };

  ZobristKeys zobristKeys;

  struct UndoState {
    uint8_t castlingRights;
    uint8_t enPassantSq;
    uint16_t halfmoveClock;
    uint16_t fullmoveClock;
    uint64_t zobristHash;
  };

  std::vector<UndoState> undoStack;




  void validate();


  Board();
  void initZobrist();
  void showBoard();
  string toFEN();
  void fromFEN(const string&);

  uint8_t mailboxSqToPieceTypeNoColor(const uint8_t&) const;
};

uint64_t computeZobristHash(const Board&, const Board::ZobristKeys&);
string mailboxSqToStrForTUI(const uint8_t&);
string mailboxSqToStrForFEN(const uint8_t&);
uint8_t fenChToMailboxSq(const char&);
string sqIndexToBoardIndex(const uint8_t&);
uint8_t boardIndexToSqIndex(const string&);


