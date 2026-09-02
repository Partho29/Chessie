#pragma once
#include "globals.h"
#include <vector>
using namespace std;

struct Board;


class Move_Handler {
  public : 
    Bitboard pawnMoveSingle[2][64] = {0};
    Bitboard pawnMoveDouble[2][64] = {0};
    Bitboard pawnAttack[2][64] = {0};
    Bitboard knightMove[64] = {0};
    Bitboard kingMove[64] = {0};
    Bitboard kingCastle[2][64] = {0};

    struct MagicEntry {
      Bitboard mask;
      Bitboard magic;
      uint8_t relevantBits;
      vector<Bitboard> attacks;
    };
    MagicEntry rookMagics[64];
    MagicEntry bishopMagics[64];

    void generatePawnMoves();
    void generateKnightMoves();
    void generateKingMoves();

    Bitboard getPossibleRookMoves(const Bitboard&, const Bitboard&);
    Bitboard getPossibleBishopMoves(const Bitboard&, const Bitboard&);
    void generateRookMoves();
    void generateBishopMoves();
    void printMagics();
    void hardcodeMagics();
    Bitboard getRookMoves(const uint8_t&, const Bitboard&);
    Bitboard getBishopMoves(const uint8_t&, const Bitboard&);

    vector<Move> generateLegalMoves(Board&);
    vector<Move> generatePseudoLegalMoves(const Board&);
    std::string legalMoveToString(const Move&);
    Move makeMove(uint8_t from, uint8_t to, uint8_t movingSide, uint8_t movingPieceType, bool captureFlag, uint8_t capturedPieceType, bool promotionFlag, uint8_t promotionPieceType, bool doublePushFlag, uint8_t enPassSq, bool castleFlag, uint8_t castlingSide);
    void applyMove(const Move&, Board&);
    void revertMove(const Move&, Board&);
    bool isSquareAttacked(const Bitboard&, const uint8_t&, const Board&);

};



