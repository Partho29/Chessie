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

  Move_Handler moveHandler;

  struct UndoState {
    uint8_t castlingRights;
    uint8_t enPassantSq;
    uint16_t halfmoveClock;
    uint16_t fullmoveClock;
  };

  std::vector<UndoState> undoStack;

  void validate();

  

  // End state
  /*
    bit 0 : black king in check (we'll set this after receiving a move from the opponent)
    bit 1 : white king in check (we'll set this after receiving a move from the opponent)
    bit 2 : game won flag (we'll set this after finding out that our king was in check and we don't have any legal moves)
    bit 3 : who won (0 for black, 1 for white; we'll set this after game won flag is set)
    bit 4 : stalemate flag (we'll set this after finding out that we don't have any legal moves but our king is not in check)
    bit 5 : three fold repetition flag (to do later)
    bit 6 : five fold repetition flag (to do later)
    bit 7 : 50 move draw availability flag (we'll set this before making our move by checking Board.fullmoveClock)
  */
  uint8_t endState = 0;

  Board();
  void showBoard();
  string toFEN();
  void fromFEN(const string&);

  uint8_t mailboxSqToPieceTypeNoColor(const uint8_t&) const;
};


string mailboxSqToStrForTUI(const uint8_t&);
string mailboxSqToStrForFEN(const uint8_t&);
uint8_t fenChToMailboxSq(const char&);
string sqIndexToBoardIndex(const uint8_t&);
uint8_t boardIndexToSqIndex(const string&);



void init();