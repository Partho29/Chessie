#include "../include/search.h"
#include "../include/globals.h"
#include <chrono>
#include <vector>



Move RandomSearch::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  int randIndex = getRandomInclusive(0, legalMoves.size() - 1);
  return legalMoves[randIndex];
}
std::string RandomSearch::name() const {
  return "Random";
}




Move Base::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  this -> deadline = ctx.deadline;
  this -> nodeCount = 0;

  Move bestMove = legalMoves[0];
  int bestScore = -INF;
  bool completedFully = true;

  for(const Move &move : legalMoves) {
    ctx.game.moveHandler -> applyMove(move, *ctx.game.board);
    int score = -this -> negamax(ctx.game, ctx.maxDepth - 1, ctx.stopRequested);
    ctx.game.moveHandler -> revertMove(move, *ctx.game.board);

    if (ctx.stopRequested.load()) { completedFully = false; break; }   // time ran out mid-depth — discard this depth

    if(score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }

  if(!completedFully) return legalMoves[getRandomInclusive(0, legalMoves.size() - 1)];   // never trust a partially-searched depth, so we'll return a random move

  return bestMove;
}
int Base::negamax(Game &game, int depth, std::atomic<bool> &stopRequested) {
  if((++this -> nodeCount & 2047) == 0) {           // check every 2048 nodes — cheap bitmask instead of modulo
    if(std::chrono::steady_clock::now() >= this -> deadline) {
      stopRequested.store(true);
    }
  }
  if(stopRequested.load()) return 0;

  if(depth == 0) return (*this -> evalFn)(*game.board);

  game.determineGameState();
  switch (game.gameState) {
    case Game::GameState::CHECKMATE : return -MATE_VAL;
    case Game::GameState::ONGOING : {
      int bestScore = -INF;
      std::vector<Move> legalMoves = game.legalMoves;
      for(const Move &move : legalMoves) {
        game.moveHandler -> applyMove(move, *game.board);
        int score = -this -> negamax(game, depth - 1, stopRequested);
        game.moveHandler -> revertMove(move, *game.board);
        bestScore = std::max(bestScore, score);
        if (stopRequested.load()) break;   // unwind promptly once time's up, don't finish this node's remaining siblings
      }
      return bestScore;
    }
    default : return 0;
  }
}
std::string Base::name() const {
  return "Naive Negamax : " + evalFnToStr(this -> evalFn);
}




Move AB::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  this -> deadline = ctx.deadline;
  this -> nodeCount = 0;

  
    
  Move bestMove = legalMoves[0];
  int bestScore = -INF;
  bool completedFully = true;

  for(const Move &move : legalMoves) {
    ctx.game.moveHandler -> applyMove(move, *ctx.game.board);
    int score = -this -> negamax(ctx.game, ctx.maxDepth - 1, -INF, INF, ctx.stopRequested);
    ctx.game.moveHandler -> revertMove(move, *ctx.game.board);

    if (ctx.stopRequested.load()) { completedFully = false; break; }   // time ran out mid-depth — discard this depth

    if(score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }

  if(!completedFully) return legalMoves[getRandomInclusive(0, legalMoves.size() - 1)];   // never trust a partially-searched depth, so we'll return a random move

  return bestMove;
}
int AB::negamax(Game &game, int depth, int alpha, int beta, std::atomic<bool> &stopRequested) {
  if((++this -> nodeCount & 2047) == 0) {           // check every 2048 nodes — cheap bitmask instead of modulo
    if(std::chrono::steady_clock::now() >= this -> deadline) {
      stopRequested.store(true);
    }
  }
  if(stopRequested.load()) return 0;

  if(depth == 0) return (*this -> evalFn)(*game.board);

  game.determineGameState();
  switch (game.gameState) {
    case Game::GameState::CHECKMATE : return -MATE_VAL;
    case Game::GameState::ONGOING : {
      int bestScore = -INF;
      std::vector<Move> legalMoves = game.legalMoves;
      for(const Move &move : legalMoves) {
        game.moveHandler -> applyMove(move, *game.board);
        int score = -this -> negamax(game, depth - 1, -beta, -alpha, stopRequested);
        game.moveHandler -> revertMove(move, *game.board);
        bestScore = std::max(bestScore, score);
        alpha = max(alpha, bestScore);

        if(alpha >= beta) break;
        if (stopRequested.load()) break;   // unwind promptly once time's up, don't finish this node's remaining siblings
      }
      return bestScore;
    }
    default : return 0;
  }
}
std::string AB::name() const {
  return "Alpha-Beta Pruned Negamax : " + evalFnToStr(this -> evalFn);
}




Move ID::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  this -> deadline = ctx.deadline;
  this -> nodeCount = 0;

  Move overallBestMove = legalMoves[0];

  for(int depth = 1; depth <= ctx.maxDepth; depth++) {
    if(std::chrono::steady_clock::now() >= this -> deadline) break;  // don't start a depth we have no time for

    Move iterationBestMove = legalMoves[0];
    int bestScore = -INF;
    bool completedFully = true;

    for(const Move &move : legalMoves) {
      ctx.game.moveHandler -> applyMove(move, *ctx.game.board);
      int score = -this -> negamax(ctx.game, depth - 1, ctx.stopRequested);
      ctx.game.moveHandler -> revertMove(move, *ctx.game.board);

      if (ctx.stopRequested.load()) { completedFully = false; break; }   // time ran out mid-depth — discard this depth

      if(score > bestScore) {
        bestScore = score;
        iterationBestMove = move;
      }
    }

    if(!completedFully) break;   // never trust a partially-searched depth

    overallBestMove = iterationBestMove;
    // TODO (optional) : send "info depth N score cp X" here — see note below
  }

  return overallBestMove;
}
int ID::negamax(Game &game, int depth, std::atomic<bool> &stopRequested) {

  if((++this -> nodeCount & 2047) == 0) {           // check every 2048 nodes — cheap bitmask instead of modulo
    if(std::chrono::steady_clock::now() >= this -> deadline) {
      stopRequested.store(true);
    }
  }
  if(stopRequested.load()) return 0;

  if(depth == 0) return (*this -> evalFn)(*game.board);

  game.determineGameState();
  switch (game.gameState) {
    case Game::GameState::CHECKMATE : return -MATE_VAL;
    case Game::GameState::ONGOING : {
      int bestScore = -INF;
      std::vector<Move> legalMoves = game.legalMoves;
      for(const Move &move : legalMoves) {
        game.moveHandler -> applyMove(move, *game.board);
        int score = -this -> negamax(game, depth - 1, stopRequested);
        game.moveHandler -> revertMove(move, *game.board);
        bestScore = std::max(bestScore, score);
        if (stopRequested.load()) break;   // unwind promptly once time's up, don't finish this node's remaining siblings
      }
      return bestScore;
    }
    default : return 0;
  }
}
std::string ID::name() const {
  return "Ieratively Deepened Negamax : " + evalFnToStr(this -> evalFn);
}




Move IDAB::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  this -> deadline = ctx.deadline;
  this -> nodeCount = 0;

  Move overallBestMove = legalMoves[0];

  for(int depth = 1; depth <= ctx.maxDepth; depth++) {
    if(std::chrono::steady_clock::now() >= this -> deadline) break;  // don't start a depth we have no time for

    Move iterationBestMove = legalMoves[0];
    int bestScore = -INF;
    bool completedFully = true;

    for(const Move &move : legalMoves) {
      ctx.game.moveHandler -> applyMove(move, *ctx.game.board);
      int score = -this -> negamax(ctx.game, depth - 1, -INF, INF, ctx.stopRequested);
      ctx.game.moveHandler -> revertMove(move, *ctx.game.board);

      if (ctx.stopRequested.load()) { completedFully = false; break; }   // time ran out mid-depth — discard this depth

      if(score > bestScore) {
        bestScore = score;
        iterationBestMove = move;
      }
    }

    if(!completedFully) break;   // never trust a partially-searched depth

    overallBestMove = iterationBestMove;
    // TODO (optional) : send "info depth N score cp X" here — see note below
  }

  return overallBestMove;
}
int IDAB::negamax(Game &game, int depth, int alpha, int beta, std::atomic<bool> &stopRequested) {
  if((++this -> nodeCount & 2047) == 0) {           // check every 2048 nodes — cheap bitmask instead of modulo
    if(std::chrono::steady_clock::now() >= this -> deadline) {
      stopRequested.store(true);
    }
  }
  if(stopRequested.load()) return 0;

  if(depth == 0) return (*this -> evalFn)(*game.board);

  game.determineGameState();
  switch (game.gameState) {
    case Game::GameState::CHECKMATE : return -MATE_VAL;
    case Game::GameState::ONGOING : {
      int bestScore = -INF;
      std::vector<Move> legalMoves = game.legalMoves;
      for(const Move &move : legalMoves) {
        game.moveHandler -> applyMove(move, *game.board);
        int score = -this -> negamax(game, depth - 1, -beta, -alpha, stopRequested);
        game.moveHandler -> revertMove(move, *game.board);
        bestScore = std::max(bestScore, score);
        alpha = max(alpha, bestScore);

        if(alpha >= beta) break;
        if (stopRequested.load()) break;   // unwind promptly once time's up, don't finish this node's remaining siblings
      }
      return bestScore;
    }
    default : return 0;
  }
}
std::string IDAB::name() const {
  return "Iteratively Deepened Alpha-Beta Pruned Negamax : " + evalFnToStr(this -> evalFn);
}































































int materialOnlyEvaluation(const Board &board) {
  static const int pieceValue[6] = {
    0,         // king
    900,       // queen
    300,       // bishop
    300,       // knight
    500,       // rook
    100        // pawn
  };

  int score = 0;
  for (int type = 1; type < 6; type++) {
    int myCount = __builtin_popcountll(board.pieces[board.sideToMove][type]);
    int oppCount = __builtin_popcountll(board.pieces[!board.sideToMove][type]);
    score += pieceValue[type] * (myCount - oppCount);
  }

  return score;
}
std::string evalFnToStr(const std::shared_ptr<Evaluation_Function> &evalFn) {
  if(!evalFn) return "ERROR : No evaluation function detected...";
  else if(*evalFn->target<int (*)(const Board &)>() == &materialOnlyEvaluation) {
    return "Material Only";
  }

  return "Unknown";
}