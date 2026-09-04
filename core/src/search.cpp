#include "../include/search.h"
#include "../include/globals.h"
#include <vector>


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


Move RandomSearch::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  int randIndex = getRandomInclusive(0, legalMoves.size() - 1);
  return legalMoves[randIndex];
}

std::string RandomSearch::name() const {
  return "Random";
}


Move NegamaxSearch::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  Move bestMove = legalMoves[0];
  int bestScore = -INFINITY;
  for(const Move &move : legalMoves) {
    ctx.game.moveHandler -> applyMove(move, *ctx.game.board);
    int score = -1 * this -> negamax(ctx.game, ctx.depth - 1, ctx.stopRequested);
    ctx.game.moveHandler -> revertMove(move, *ctx.game.board);
    if(score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }

  return bestMove;
}

int NegamaxSearch::negamax(Game &game, int depth, std::atomic<bool> &stopRequested) {
  if(stopRequested.load()) return 0;

  if(depth == 0) return (*this -> evalFn)(*game.board);

  game.determineGameState();
  switch (game.gameState) {
    case Game::GameState::CHECKMATE : return -INFINITY;
    case Game::GameState::ONGOING : {
      int bestScore = -INFINITY;
      std::vector<Move> legalMoves = game.legalMoves;
      for(const Move &move : legalMoves) {
        game.moveHandler -> applyMove(move, *game.board);
        int score = -this -> negamax(game, depth - 1, stopRequested);
        game.moveHandler -> revertMove(move, *game.board);
        bestScore = std::max(bestScore, score);
      }
      return bestScore;
    }
    default : return 0;
  }
}

std::string NegamaxSearch::name() const {
  return "Negamax : " + evalFnToStr(this -> evalFn);
}


Move AlphaBetaNegamax::findBestMove(SearchContext &ctx, std::vector<Move> legalMoves) {
  Move bestMove = legalMoves[0];
  int bestScore = -INFINITY;
  for(const Move &move : legalMoves) {
    ctx.game.moveHandler -> applyMove(move, *ctx.game.board);
    int score = -1 * this -> alphaBetaNegamax(ctx.game, ctx.depth - 1, -INFINITY, INFINITY, ctx.stopRequested);
    ctx.game.moveHandler -> revertMove(move, *ctx.game.board);
    if(score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }
  return bestMove;
}

int AlphaBetaNegamax::alphaBetaNegamax(Game &game, int depth, int alpha, int beta, std::atomic<bool> &stopRequested) {
  if(stopRequested.load()) return 0;

  if(depth == 0) return (*this -> evalFn)(*game.board);

  game.determineGameState();
  switch (game.gameState) {
    case Game::GameState::CHECKMATE : return -INFINITY;
    case Game::GameState::ONGOING : {
      int bestScore = -INFINITY;
      std::vector<Move> legalMoves = game.legalMoves;
      for(const Move &move : legalMoves) {
        game.moveHandler -> applyMove(move, *game.board);
        int score = -this -> alphaBetaNegamax(game, depth - 1, -beta, -alpha, stopRequested);
        game.moveHandler -> revertMove(move, *game.board);
        bestScore = std::max(bestScore, score);
        alpha = max(alpha, bestScore);

        if(alpha >= beta) break;
      }
      return bestScore;
    }
    default : return 0;
  }
}


std::string AlphaBetaNegamax::name() const {
  return "Alpha Beta Negamax : " + evalFnToStr(this -> evalFn);
}