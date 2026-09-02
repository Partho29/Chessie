#include "../include/board.h"
#include "../include/globals.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>


Board::Board() {

  for (int sq = 16; sq < 48; ++sq) {
    this -> mailbox[sq] = 0;
  }
  // Black
  this -> pieces[0][0] = Bitboard(1) << 60;  // king
  this -> mailbox[60] = 0b10000001;
  this -> pieces[0][1] = Bitboard(1) << 59; // queen
  this -> mailbox[59] = 0b01000001;
  this -> pieces[0][2] = Bitboard(9) << 58; // both bishops
  this -> mailbox[58] = 0b00100001;
  this -> mailbox[61] = 0b00100001;
  this -> pieces[0][3] = Bitboard(33) << 57; // both knights
  this -> mailbox[57] = 0b00010001;
  this -> mailbox[62] = 0b00010001;
  this -> pieces[0][4] = Bitboard(129) << 56; // both rooks
  this -> mailbox[56] = 0b00001001;
  this -> mailbox[63] = 0b00001001;
  this -> pieces[0][5] = Bitboard(255) << 48; // all pawns
  this -> mailbox[55] = 0b00000101;
  this -> mailbox[54] = 0b00000101;
  this -> mailbox[53] = 0b00000101;
  this -> mailbox[52] = 0b00000101;
  this -> mailbox[51] = 0b00000101;
  this -> mailbox[50] = 0b00000101;
  this -> mailbox[49] = 0b00000101;
  this -> mailbox[48] = 0b00000101;

  this -> occupancy[0] = Bitboard(65535) << 48;

  // White
  this -> pieces[1][0] = Bitboard(1) << 4;  // king
  this -> mailbox[4] = 0b10000011;
  this -> pieces[1][1] = Bitboard(1) << 3; // queen
  this -> mailbox[3] = 0b01000011;
  this -> pieces[1][2] = Bitboard(9) << 2; // both bishops
  this -> mailbox[2] = 0b00100011;
  this -> mailbox[5] = 0b00100011;
  this -> pieces[1][3] = Bitboard(33) << 1; // both knights
  this -> mailbox[1] = 0b00010011;
  this -> mailbox[6] = 0b00010011;
  this -> pieces[1][4] = Bitboard(129); // both rooks
  this -> mailbox[0] = 0b00001011;
  this -> mailbox[7] = 0b00001011;
  this -> pieces[1][5] = Bitboard(255) << 8; // all pawns
  this -> mailbox[8] = 0b00000111;
  this -> mailbox[9] = 0b00000111;
  this -> mailbox[10] = 0b00000111;
  this -> mailbox[11] = 0b00000111;
  this -> mailbox[12] = 0b00000111;
  this -> mailbox[13] = 0b00000111;
  this -> mailbox[14] = 0b00000111;
  this -> mailbox[15] = 0b00000111;

  this -> occupancy[1] = Bitboard(65535);


  this -> occupancy_all = occupancy[0] | occupancy[1];

  this -> sideToMove = 1;

  this -> castlingRights = 0b00001111;

  this -> enPassantSq = 0;

  this -> halfmoveClock = 0;
  this -> fullmoveClock = 1;

  

  this -> initZobrist();
  this -> zobristHash = computeZobristHash(*this, this -> zobristKeys);
}

void Board::initZobrist() {
  for (int side = 0; side < 2; side++)
    for (int piece = 0; piece < 6; piece++)
      for (int sq = 0; sq < 64; sq++)
        zobristKeys.pieceSquare[side][piece][sq] = getRandomInclusive(0, UINT64_MAX);

  zobristKeys.sideToMove = getRandomInclusive(0, UINT64_MAX);
  for (int i = 0; i < 16; i++) zobristKeys.castlingRights[i] = getRandomInclusive(0, UINT64_MAX);
  for (int i = 0; i < 8; i++) zobristKeys.enPassantFile[i] = getRandomInclusive(0, UINT64_MAX);
}

uint64_t computeZobristHash(const Board &board, const Board::ZobristKeys &zobrist) {
  uint64_t hash = 0;
  for (int side = 0; side < 2; side++) {
    for (int piece = 0; piece < 6; piece++) {
      Bitboard pieces = board.pieces[side][piece];
      while (pieces) {
        int sq = __builtin_ctzll(pieces);
        pieces &= (pieces - 1);
        hash ^= zobrist.pieceSquare[side][piece][sq];
      }
    }
  }

  if (board.sideToMove == 0)   // pick whichever side gets XORed in — Black, say (but always stick to one convention)
    hash ^= zobrist.sideToMove;

  hash ^= zobrist.castlingRights[board.castlingRights]; 

  if (board.enPassantSq != 0)  // our no-ep sentinel is the value 0
    hash ^= zobrist.enPassantFile[board.enPassantSq % 8];   // file, not full square — see below

  return hash;
}


void Board::showBoard() {
  std::cout << "    Aa   Bb   Cc   Dd   Ee   Ff   Gg   Hh" << std::endl;
  std::cout << "   ____ ____ ____ ____ ____ ____ ____ ____" << std::endl;
  std::cout << "8 / " << mailboxSqToStrForTUI(this -> mailbox[56]) << " | " << mailboxSqToStrForTUI(this -> mailbox[57]) << " | " << mailboxSqToStrForTUI(this -> mailbox[58]) << " | " << mailboxSqToStrForTUI(this -> mailbox[59]) << " | " << mailboxSqToStrForTUI(this -> mailbox[60]) << " | " << mailboxSqToStrForTUI(this -> mailbox[61]) << " | " << mailboxSqToStrForTUI(this -> mailbox[62]) << " | " << mailboxSqToStrForTUI(this -> mailbox[63]) << " \\ 8" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "7 | " << mailboxSqToStrForTUI(this -> mailbox[48]) << " | " << mailboxSqToStrForTUI(this -> mailbox[49]) << " | " << mailboxSqToStrForTUI(this -> mailbox[50]) << " | " << mailboxSqToStrForTUI(this -> mailbox[51]) << " | " << mailboxSqToStrForTUI(this -> mailbox[52]) << " | " << mailboxSqToStrForTUI(this -> mailbox[53]) << " | " << mailboxSqToStrForTUI(this -> mailbox[54]) << " | " << mailboxSqToStrForTUI(this -> mailbox[55]) << " | 7" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "6 | " << mailboxSqToStrForTUI(this -> mailbox[40]) << " | " << mailboxSqToStrForTUI(this -> mailbox[41]) << " | " << mailboxSqToStrForTUI(this -> mailbox[42]) << " | " << mailboxSqToStrForTUI(this -> mailbox[43]) << " | " << mailboxSqToStrForTUI(this -> mailbox[44]) << " | " << mailboxSqToStrForTUI(this -> mailbox[45]) << " | " << mailboxSqToStrForTUI(this -> mailbox[46]) << " | " << mailboxSqToStrForTUI(this -> mailbox[47]) << " | 6" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "5 | " << mailboxSqToStrForTUI(this -> mailbox[32]) << " | " << mailboxSqToStrForTUI(this -> mailbox[33]) << " | " << mailboxSqToStrForTUI(this -> mailbox[34]) << " | " << mailboxSqToStrForTUI(this -> mailbox[35]) << " | " << mailboxSqToStrForTUI(this -> mailbox[36]) << " | " << mailboxSqToStrForTUI(this -> mailbox[37]) << " | " << mailboxSqToStrForTUI(this -> mailbox[38]) << " | " << mailboxSqToStrForTUI(this -> mailbox[39]) << " | 5" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "4 | " << mailboxSqToStrForTUI(this -> mailbox[24]) << " | " << mailboxSqToStrForTUI(this -> mailbox[25]) << " | " << mailboxSqToStrForTUI(this -> mailbox[26]) << " | " << mailboxSqToStrForTUI(this -> mailbox[27]) << " | " << mailboxSqToStrForTUI(this -> mailbox[28]) << " | " << mailboxSqToStrForTUI(this -> mailbox[29]) << " | " << mailboxSqToStrForTUI(this -> mailbox[30]) << " | " << mailboxSqToStrForTUI(this -> mailbox[31]) << " | 4" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "3 | " << mailboxSqToStrForTUI(this -> mailbox[16]) << " | " << mailboxSqToStrForTUI(this -> mailbox[17]) << " | " << mailboxSqToStrForTUI(this -> mailbox[18]) << " | " << mailboxSqToStrForTUI(this -> mailbox[19]) << " | " << mailboxSqToStrForTUI(this -> mailbox[20]) << " | " << mailboxSqToStrForTUI(this -> mailbox[21]) << " | " << mailboxSqToStrForTUI(this -> mailbox[22]) << " | " << mailboxSqToStrForTUI(this -> mailbox[23]) << " | 3" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "2 | " << mailboxSqToStrForTUI(this -> mailbox[8]) << " | " << mailboxSqToStrForTUI(this -> mailbox[9]) << " | " << mailboxSqToStrForTUI(this -> mailbox[10]) << " | " << mailboxSqToStrForTUI(this -> mailbox[11]) << " | " << mailboxSqToStrForTUI(this -> mailbox[12]) << " | " << mailboxSqToStrForTUI(this -> mailbox[13]) << " | " << mailboxSqToStrForTUI(this -> mailbox[14]) << " | " << mailboxSqToStrForTUI(this -> mailbox[15]) << " | 2" << std::endl;
  std::cout << "  |____|____|____|____|____|____|____|____|" << std::endl;
  std::cout << "1 | " << mailboxSqToStrForTUI(this -> mailbox[0]) << " | " << mailboxSqToStrForTUI(this -> mailbox[1]) << " | " << mailboxSqToStrForTUI(this -> mailbox[2]) << " | " << mailboxSqToStrForTUI(this -> mailbox[3]) << " | " << mailboxSqToStrForTUI(this -> mailbox[4]) << " | " << mailboxSqToStrForTUI(this -> mailbox[5]) << " | " << mailboxSqToStrForTUI(this -> mailbox[6]) << " | " << mailboxSqToStrForTUI(this -> mailbox[7]) << " | 1" << std::endl;
  std::cout << "  \\____|____|____|____|____|____|____|____/" << std::endl;
  std::cout << "    Aa   Bb   Cc   Dd   Ee   Ff   Gg   Hh" << std::endl;
}

string Board::toFEN() {
  
  string fenStr = "";
  char emptySpaces = '0';
  uint8_t pos;
  for(int rank = 7; rank >= 0; rank--) {
    for(int file = 0; file < 8u; file++) {
      pos = rank * 8 + file;
      if(this -> mailbox[pos] & 1) {
        if(emptySpaces != '0') {
          fenStr += emptySpaces;
          emptySpaces = '0';
        }
        fenStr += mailboxSqToStrForFEN(this -> mailbox[pos]);
      }
      else emptySpaces++;
    }
    if(emptySpaces != '0') {
      fenStr += emptySpaces;
      emptySpaces = '0';
    }
    if(rank != 0) fenStr += '/';
  }
  
  fenStr += (this -> sideToMove ? " w " : " b ");
  if(this -> castlingRights == 0) fenStr += "-";
  else {
    fenStr += (this -> castlingRights & 8 ? "K" : "");
    fenStr += (this -> castlingRights & 4 ? "Q" : "");
    fenStr += (this -> castlingRights & 2 ? "k" : "");
    fenStr += (this -> castlingRights & 1 ? "q" : "");
  }
  
  fenStr += (this -> enPassantSq == 0 ? " - " : (" " + sqIndexToBoardIndex(this -> enPassantSq) + " "));
  fenStr += to_string(this -> halfmoveClock);
  fenStr += " ";
  fenStr += to_string(this -> fullmoveClock);

  return fenStr;
}

void Board::fromFEN(const string &_fen) {
  string fen(_fen);
  uint8_t pos;
  uint16_t strIndex = 0;
  for(int rank = 7; rank >= 0; rank--) {
    for(int file = 0; file < 8; file++) {
      pos = rank * 8 + file;
      if(fen[strIndex] > '0' && fen[strIndex] <= '9') (this -> mailbox[pos] = 0, fen[strIndex]--);
      else if(fen[strIndex] == '0' || fen[strIndex] == '/') (strIndex++, file--);
      else {
        this -> mailbox[pos] = fenChToMailboxSq(fen[strIndex++]);
      }
    }
  }

  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 6; j++) {
      this -> pieces[i][j] = 0;
    }
  }
  this -> occupancy_all = 0;
  this -> occupancy[0] = 0;
  this -> occupancy[1] = 0;
  for(int i = 0; i < 64; i++) {
    if(this -> mailbox[i] & 1) {
      Bitboard toOr = Bitboard(1) << i;
      this -> occupancy_all |= toOr;
      if(this -> mailbox[i] & 2) {  // White piece
        this -> occupancy[1] |= toOr;
        if(this -> mailbox[i] & 128) this -> pieces[1][0] |= toOr;
        else if(this -> mailbox[i] & 64) this -> pieces[1][1] |= toOr;
        else if(this -> mailbox[i] & 32) this -> pieces[1][2] |= toOr;
        else if(this -> mailbox[i] & 16) this -> pieces[1][3] |= toOr;
        else if(this -> mailbox[i] & 8) this -> pieces[1][4] |= toOr;
        else if(this -> mailbox[i] & 4) this -> pieces[1][5] |= toOr;
      }
      else {                        // Black piece
        this -> occupancy[0] |= toOr;
        if(this -> mailbox[i] & 128) this -> pieces[0][0] |= toOr;
        else if(this -> mailbox[i] & 64) this -> pieces[0][1] |= toOr;
        else if(this -> mailbox[i] & 32) this -> pieces[0][2] |= toOr;
        else if(this -> mailbox[i] & 16) this -> pieces[0][3] |= toOr;
        else if(this -> mailbox[i] & 8) this -> pieces[0][4] |= toOr;
        else if(this -> mailbox[i] & 4) this -> pieces[0][5] |= toOr;
      }
    }
  }

  while(fen[strIndex] != ' ') strIndex++;
  while(fen[strIndex] == ' ') strIndex++;

  this -> sideToMove = (fen[strIndex] == 'w' ? 1 : 0);
  while(fen[strIndex] != ' ') strIndex++;
  while(fen[strIndex] == ' ') strIndex++;

  if(fen[strIndex] == '-') this -> castlingRights = 0;
  else {
    string castleStr = fen.substr(strIndex, fen.find(" ", strIndex) - strIndex);
    this -> castlingRights = 0;
    if(castleStr.find("K") != string::npos) this -> castlingRights |= 8;
    if(castleStr.find("Q") != string::npos) this -> castlingRights |= 4;
    if(castleStr.find("k") != string::npos) this -> castlingRights |= 2;
    if(castleStr.find("q") != string::npos) this -> castlingRights |= 1;
  }
  while(fen[strIndex] != ' ') strIndex++;
  while(fen[strIndex] == ' ') strIndex++;

  if(fen[strIndex] == '-') this -> enPassantSq = 0;
  else {
    this -> enPassantSq = boardIndexToSqIndex(fen.substr(strIndex, 2));
  }
  while(fen[strIndex] != ' ') strIndex++;
  while(fen[strIndex] == ' ') strIndex++;

  string halfMoveclk = fen.substr(strIndex, fen.find(" ", strIndex) - strIndex);
  this -> halfmoveClock = stoi(halfMoveclk);
  while(fen[strIndex] != ' ') strIndex++;
  while(fen[strIndex] == ' ') strIndex++;

  string fullMoveClk = fen.substr(strIndex, fen.size() - strIndex);
  this -> fullmoveClock = stoi(fullMoveClk);

  this -> zobristHash = computeZobristHash(*this, this -> zobristKeys);

  this -> undoStack.clear();

  assert(this -> toFEN() == _fen);
}

string mailboxSqToStrForTUI(const uint8_t &sq) {
  switch (sq) {
    case 0b10000001 :
      return "BK";
    case 0b01000001 :
      return "BQ";
    case 0b00100001 :
      return "BB";
    case 0b00010001 :
      return "BN";
    case 0b00001001 :
      return "BR";
    case 0b00000101 :
      return "BP";
    case 0b10000011 :
      return "WK";
    case 0b01000011 :
      return "WQ";
    case 0b00100011 :
      return "WB";
    case 0b00010011 :
      return "WN";
    case 0b00001011 :
      return "WR";
    case 0b00000111 :
      return "WP";
    default : return "::";
  }
}

string mailboxSqToStrForFEN(const uint8_t &sq) {
  switch (sq) {
    case 0b10000001 :
      return "k";
    case 0b01000001 :
      return "q";
    case 0b00100001 :
      return "b";
    case 0b00010001 :
      return "n";
    case 0b00001001 :
      return "r";
    case 0b00000101 :
      return "p";
    case 0b10000011 :
      return "K";
    case 0b01000011 :
      return "Q";
    case 0b00100011 :
      return "B";
    case 0b00010011 :
      return "N";
    case 0b00001011 :
      return "R";
    case 0b00000111 :
      return "P";
    default : return " ";
  }
}

uint8_t fenChToMailboxSq(const char &ch) {
  switch (ch) {
    case 'r' :
      return 0b00001001;
    case 'n' :
      return 0b00010001;
    case 'b' :
      return 0b00100001;
    case 'q' :  
      return 0b01000001;
    case 'k' :
      return 0b10000001;
    case 'p' :
      return 0b00000101;
    case 'R' :
      return 0b00001011;
    case 'N' :
      return 0b00010011;
    case 'B' :
      return 0b00100011;
    case 'Q' :
      return 0b01000011;
    case 'K' :
      return 0b10000011;
    case 'P' :
      return 0b00000111;
    default : return 0;
  }
}

string sqIndexToBoardIndex(const uint8_t &pos) {
  switch (pos) {
    case 0 : return "a1";
    case 1 : return "b1";
    case 2 : return "c1";
    case 3 : return "d1";
    case 4 : return "e1";
    case 5 : return "f1";
    case 6 : return "g1";
    case 7 : return "h1";
    case 8 : return "a2";
    case 9 : return "b2";
    case 10 : return "c2";
    case 11 : return "d2";
    case 12 : return "e2";
    case 13 : return "f2";
    case 14 : return "g2";
    case 15 : return "h2";
    case 16 : return "a3";
    case 17 : return "b3";
    case 18 : return "c3";
    case 19 : return "d3";
    case 20 : return "e3";
    case 21 : return "f3";
    case 22 : return "g3";
    case 23 : return "h3";
    case 24 : return "a4";
    case 25 : return "b4";
    case 26 : return "c4";
    case 27 : return "d4";
    case 28 : return "e4";
    case 29 : return "f4";
    case 30 : return "g4";
    case 31 : return "h4";
    case 32 : return "a5";
    case 33 : return "b5";
    case 34 : return "c5";
    case 35 : return "d5";
    case 36 : return "e5";
    case 37 : return "f5";
    case 38 : return "g5";
    case 39 : return "h5";
    case 40 : return "a6";
    case 41 : return "b6";
    case 42 : return "c6";
    case 43 : return "d6";
    case 44 : return "e6";
    case 45 : return "f6";
    case 46 : return "g6";
    case 47 : return "h6";
    case 48 : return "a7";
    case 49 : return "b7";
    case 50 : return "c7";
    case 51 : return "d7";  
    case 52 : return "e7";
    case 53 : return "f7";
    case 54 : return "g7";
    case 55 : return "h7";
    case 56 : return "a8";
    case 57 : return "b8";
    case 58 : return "c8";
    case 59 : return "d8";
    case 60 : return "e8";
    case 61 : return "f8";
    case 62 : return "g8";
    case 63 : return "h8";
    default : return "Invalid square";
  }
}

uint8_t boardIndexToSqIndex(const string &id) {
  if(id == "a1") return 0;
  else if(id == "b1") return 1;
  else if(id == "c1") return 2;
  else if(id == "d1") return 3;
  else if(id == "e1") return 4;
  else if(id == "f1") return 5;
  else if(id == "g1") return 6;
  else if(id == "h1") return 7;
  else if(id == "a2") return 8;
  else if(id == "b2") return 9;
  else if(id == "c2") return 10;
  else if(id == "d2") return 11;
  else if(id == "e2") return 12;
  else if(id == "f2") return 13;
  else if(id == "g2") return 14;
  else if(id == "h2") return 15;
  else if(id == "a3") return 16;
  else if(id == "b3") return 17;
  else if(id == "c3") return 18;
  else if(id == "d3") return 19;
  else if(id == "e3") return 20;
  else if(id == "f3") return 21;
  else if(id == "g3") return 22;
  else if(id == "h3") return 23;
  else if(id == "a4") return 24;
  else if(id == "b4") return 25;
  else if(id == "c4") return 26;
  else if(id == "d4") return 27;
  else if(id == "e4") return 28;
  else if(id == "f4") return 29;
  else if(id == "g4") return 30;
  else if(id == "h4") return 31;
  else if(id == "a5") return 32;
  else if(id == "b5") return 33;
  else if(id == "c5") return 34;
  else if(id == "d5") return 35;
  else if(id == "e5") return 36;
  else if(id == "f5") return 37;
  else if(id == "g5") return 38;
  else if(id == "h5") return 39;
  else if(id == "a6") return 40;
  else if(id == "b6") return 41;
  else if(id == "c6") return 42;
  else if(id == "d6") return 43;
  else if(id == "e6") return 44;
  else if(id == "f6") return 45;
  else if(id == "g6") return 46;
  else if(id == "h6") return 47;
  else if(id == "a7") return 48;
  else if(id == "b7") return 49;
  else if(id == "c7") return 50;
  else if(id == "d7") return 51;
  else if(id == "e7") return 52;
  else if(id == "f7") return 53;
  else if(id == "g7") return 54;
  else if(id == "h7") return 55;
  else if(id == "a8") return 56;
  else if(id == "b8") return 57;
  else if(id == "c8") return 58;
  else if(id == "d8") return 59;
  else if(id == "e8") return 60;
  else if(id == "f8") return 61;
  else if(id == "g8") return 62;
  else if(id == "h8") return 63;
  else (cerr << "Something bad happened...\n", exit(1));
}


uint8_t Board::mailboxSqToPieceTypeNoColor(const uint8_t &pos) const {
  if((this -> mailbox[pos] & 1) == 0) return ((1 << 8) - 1);
  else {
    uint8_t sq = this -> mailbox[pos];
    while(__builtin_popcount(sq) != 1) sq &= (sq - 1);
    return (7 - __builtin_ctzll(sq));
  }
}

// void Board::validate()
// {
//     Bitboard rebuilt[2][6] = {};
//     Bitboard occ[2] = {};

//     for (int sq = 0; sq < 64; ++sq)
//     {
//         uint8_t piece = mailbox[sq];

//         if (!(piece & 1))
//             continue;

//         int side = (piece >> 1) & 1;
//         int type = mailboxSqToPieceTypeNoColor(sq);

//         rebuilt[side][type] |= 1ULL << sq;
//         occ[side] |= 1ULL << sq;
//     }

//     for (int side = 0; side < 2; ++side)
//     {
//         for (int type = 0; type < 6; ++type)
//         {
//             cerr << side << " " << type << endl;
//             assert(rebuilt[side][type] ==
//                    pieces[side][type]);
//         }

//         assert(occ[side] == occupancy[side]);
//     }

//     assert((occupancy[0] | occupancy[1]) ==
//            occupancy_all);
// }


void Board::validate()
{
    
    Bitboard rebuilt[2][6] = {};
    Bitboard occ[2] = {};

    for (int sq = 0; sq < 64; ++sq)
    {
        uint8_t piece = mailbox[sq];

        if (!(piece & 1))
            continue;

        int side = (piece >> 1) & 1;
        int type = mailboxSqToPieceTypeNoColor(sq);

        rebuilt[side][type] |= 1ULL << sq;
        occ[side] |= 1ULL << sq;
    }
    // ::showBoard(rebuilt[0][4]);
    // ::showBoard(pieces[0][4]);
    // cout << "Castling Rights : " << int(this -> castlingRights) << endl;
    // ::showBoard(occupancy_all);
    // this -> showBoard();

    for (int side = 0; side < 2; ++side)
    {
        for (int type = 0; type < 6; ++type)
        {
            
            if (rebuilt[side][type] != pieces[side][type])
            {
                std::cerr
                    << "\nMISMATCH!\n"
                    << "side = " << side << '\n'
                    << "type = " << type << '\n'
                    << std::hex
                    << "rebuilt = 0x" << rebuilt[side][type] << '\n'
                    << "actual   = 0x" << pieces[side][type] << '\n'
                    << "diff     = 0x"
                    << (rebuilt[side][type] ^ pieces[side][type])
                    << std::dec
                    << '\n';

                std::abort();
            }
        }

        if (occ[side] != occupancy[side])
        {
            std::cerr
                << "\nOCCUPANCY MISMATCH!\n"
                << "side = " << side << '\n'
                << std::hex
                << "rebuilt = 0x" << occ[side] << '\n'
                << "actual   = 0x" << occupancy[side] << '\n';

            std::abort();
        }
    }

    assert((occupancy[0] | occupancy[1]) ==
           occupancy_all);
}