#include "perft.h"
#include <iostream>
#include <vector>

uint64_t perft(Board &board, int depth) {
  // board.validate();
  if(depth == 0) return 1;

  std::vector<Move> moves = board.moveHandler.generateLegalMoves(board);
  // board.validate();
  uint64_t nodes = 0;
  for(const Move &move : moves) {
    // board.validate();
    // std::cerr
    // << "BEFORE: " << board.moveHandler.legalMoveToString(move)
    // << '\n';
    //cout << "Move : " << board.moveHandler.legalMoveToString(move) << endl;
    board.moveHandler.applyMove(move, board);
    // board.validate();
    nodes += perft(board, depth - 1);
    board.moveHandler.revertMove(move, board);
    // std::cerr
    // << "AFTER REVERT: "
    // << board.moveHandler.legalMoveToString(move)
    // << '\n';
    // board.validate();
  }
  return nodes;
}

uint64_t perftDivide(Board &board, int depth) {
  uint64_t total = 0;
  
  vector<Move> moves = board.moveHandler.generateLegalMoves(board);

    for (const Move& move : moves)
    {
        std::string moveString =
            board.moveHandler.legalMoveToString(move);

        board.moveHandler.applyMove(move, board);

        uint64_t nodes =
            perft(board, depth - 1);

        board.moveHandler.revertMove(move, board);

        std::cout
            << moveString
            << ": "
            << nodes
            << '\n';
        total += nodes;
    }

    std::cout << "\nTotal: " << total << '\n';

    return total;
}