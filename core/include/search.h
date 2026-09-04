#pragma once
#include "game.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>

using Evaluation_Function = std::function<int(const Board&)>;

int materialOnlyEvaluation(const Board&);

std::string evalFnToStr(const std::shared_ptr<Evaluation_Function>&);

struct SearchContext {
  Game &game;
  std::atomic<bool> &stopRequested;
  int depth;                                  // Used by fixed-depth algos, ignored by others
  int timeLimitMs;                            // Used by iterative deepening; not currently needed
};


class SearchAlgo {
  public:
    virtual Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) = 0;
    virtual std::string name() const = 0;
    virtual ~SearchAlgo() = default;
};

class RandomSearch : public SearchAlgo {
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;
};

class NegamaxSearch : public SearchAlgo {
  private :
    std::shared_ptr<Evaluation_Function> evalFn = std::make_shared<Evaluation_Function>(materialOnlyEvaluation);
    int negamax(Game&, int, std::atomic<bool>&);
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;

};

class AlphaBetaNegamax : public SearchAlgo {
  private :
    std::shared_ptr<Evaluation_Function> evalFn = std::make_shared<Evaluation_Function>(materialOnlyEvaluation);
    int alphaBetaNegamax(Game&, int, int, int, std::atomic<bool>&);
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;
};






