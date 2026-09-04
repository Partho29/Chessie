#pragma once
#include "game.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <chrono>

using Evaluation_Function = std::function<int(const Board&)>;

int materialOnlyEvaluation(const Board&);

std::string evalFnToStr(const std::shared_ptr<Evaluation_Function>&);

struct SearchContext {
  Game &game;
  std::atomic<bool> &stopRequested;
  int maxDepth;                                      // hard ceiling — 64 by default, or user-specified via "go depth N"
  std::chrono::steady_clock::time_point deadline;    // wall-clock time the search must stop by
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

// The basis is Negamax for all the below algos...

class Base : public SearchAlgo {
  private :
    long long nodeCount = 0;
    std::chrono::steady_clock::time_point deadline;
    std::shared_ptr<Evaluation_Function> evalFn = std::make_shared<Evaluation_Function>(materialOnlyEvaluation);
    int negamax(Game&, int, std::atomic<bool>&);
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;
};

class AB : public SearchAlgo {
  private :
    long long nodeCount = 0;
    std::chrono::steady_clock::time_point deadline;
    std::shared_ptr<Evaluation_Function> evalFn = std::make_shared<Evaluation_Function>(materialOnlyEvaluation);
    int negamax(Game&, int, int, int, std::atomic<bool>&);
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;
};

class ID : public SearchAlgo {
  private :
    long long nodeCount = 0;
    std::chrono::steady_clock::time_point deadline;
    std::shared_ptr<Evaluation_Function> evalFn = std::make_shared<Evaluation_Function>(materialOnlyEvaluation);
    int negamax(Game&, int, std::atomic<bool>&);
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;

};

class IDAB : public SearchAlgo {
  private:
    long long nodeCount = 0;
    std::chrono::steady_clock::time_point deadline;
    std::shared_ptr<Evaluation_Function> evalFn = std::make_shared<Evaluation_Function>(materialOnlyEvaluation);
    int negamax(Game&, int, int, int, std::atomic<bool>&);
  public :
    Move findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) override;
    std::string name() const override;
};






