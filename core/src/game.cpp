#include "../include/game.h"
#include "../include/move_handler.h"
#include <algorithm>
#include <memory>
#include <search.h>


void Game::init() {
  this -> gameState = Game::GameState::ONGOING;
  this -> board = std::make_unique<Board>();
  this -> moveHandler = std::make_unique<Move_Handler>();
  this -> currAlgo = std::make_unique<RandomSearch>();
}

void Game::determineGameState() {
  // this -> legalMoves.clear();
  this -> legalMoves = this -> moveHandler -> generateLegalMoves(*this -> board);

  if(legalMoves.empty()) {
    bool isSideToMoveInCheck = this -> moveHandler -> isSquareAttacked(this -> board -> pieces[this -> board -> sideToMove][0], !this -> board -> sideToMove, *this -> board);
    if(isSideToMoveInCheck) this -> gameState = GameState::CHECKMATE;
    else this -> gameState = GameState::STALEMATE;
  }
  else if(this -> isInsufficientMaterialOnBoard()) this -> gameState = GameState::INSUFFICIENT_MATERIAL;
  else {
    int repetitions = std::count(this -> positionHistory.begin(), this -> positionHistory.end(), this -> board -> zobristHash);
    if(repetitions >= 5) this -> gameState = GameState::FIVEFOLD_REPETITION;
    else if(repetitions >= 3) this -> gameState = GameState::THREEFOLD_REPETITION;
    else if(this -> board -> halfmoveClock >= 100) this -> gameState = GameState::FIFTY_MOVE_RULE;
    else this -> gameState = GameState::ONGOING;
  }
}

bool Game::isInsufficientMaterialOnBoard() {
  // TODO
  return false;
}

std::string Game::gameStateToStr() {
  switch (this -> gameState) {
    case GameState::UNINITIALIZED: return "UNINITIALIZED";
    case GameState::ONGOING: return "ONGOING";
    case GameState::CHECKMATE: return "CHECKMATE";
    case GameState::STALEMATE: return "STALEMATE";
    case GameState::THREEFOLD_REPETITION: return "THREEFOLD_REPETITION";
    case GameState::FIVEFOLD_REPETITION: return "FIVEFOLD_REPETITION";
    case GameState::FIFTY_MOVE_RULE: return "FIFTY_MOVE_RULE";
    case GameState::INSUFFICIENT_MATERIAL: return "INSUFFICIENT_MATERIAL";
    default: return "UNKNOWN";
  }
}

