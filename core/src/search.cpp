#include "../include/search.h"
#include "../include/globals.h"
#include <vector>


Move RandomSearch::findBestMove(SearchContext &ctx) {
  std::vector<Move> legalMoves = ctx.moveHandler.generateLegalMoves(ctx.board);
  int randIndex = getRandomInclusive(0, legalMoves.size() - 1);
  return legalMoves[randIndex];
}

Move RandomSearch::findBestMove(SearchContext &ctx, std::vector<Move> &legalMoves) {
  int randIndex = getRandomInclusive(0, legalMoves.size() - 1);
  return legalMoves[randIndex];
}



std::string RandomSearch::name() const {
  return "Random";
}