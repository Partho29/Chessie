#pragma once
#include "board.h"
#include "move_handler.h"
#include <memory>

class Game {
  public : 
    std::unique_ptr<Board> board;
    std::unique_ptr<Move_Handler> moveHandler;
    std::vector<uint64_t> positionHistory;
    std::vector<Move> legalMoves;                      // Holds legal moves for the current playing side

    enum class GameState {
      UNINITIALIZED,
      ONGOING,
      CHECKMATE,       // side to move has no moves and is in check — they lost
      STALEMATE,       // side to move has no moves, not in check — draw
      THREEFOLD_REPETITION,
      FIVEFOLD_REPETITION,
      FIFTY_MOVE_RULE,
      INSUFFICIENT_MATERIAL
    };

    GameState gameState;

    void init();
    bool isInsufficientMaterialOnBoard();
    void determineGameState();        // Responsible for checking whether the game reached some form of end state and modifying this -> gameState accordingly; also fills this -> legalMoves 
    std::string gameStateToStr(); 
};