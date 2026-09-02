#pragma once
#include "board.h"
#include "move_handler.h"
#include <atomic>
#include <string>


struct SearchContext {
  Board &board;
  Move_Handler &moveHandler;
  std::atomic<bool> &stopRequested;
  int depth;                                  // Used by fixed-depth algos, ignored by others
  int timeLimitMs;                            // Used by iterative deepening; not currently needed
};


class SearchAlgo {
  public:
    virtual Move findBestMove(SearchContext &ctx) = 0;
    virtual Move findBestMove(SearchContext &ctx, std::vector<Move> &legalMoves) = 0;
    virtual std::string name() const = 0;
    virtual ~SearchAlgo() = default;
};

class RandomSearch : public SearchAlgo {
  public :
    Move findBestMove(SearchContext &ctx) override;
    Move findBestMove(SearchContext &ctx, std::vector<Move> &legalMoves) override;
    std::string name() const override;
};



