#include "uci.h"
#include "globals.h"
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>


UCI::UCI() {
  this -> currentFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

UCI::~UCI() {
  if(this -> searchThread.joinable()) {
    this -> stopRequested.store(true);
    this -> searchThread.join();
  }
  delete this -> board;
}


// The thread on which the lopp() runs doesn't perform the search itself
// Only handles stdin and stdout for passing and receiving commands with the GUI
void UCI::loop() {
  std::string line;
  
  while(std::getline(std::cin, line)) {
    if(line.empty()) continue;
    this -> handleCmd(line);
    if(this -> quitting.load()) break;
  }

  // If stdin is closed bu GUI explicitly or it was closed unexpectedly, we should call clean up code nonetheless
  if(!this -> quitting.load()) this -> handleQuit();
}


// Parsing the commandd as sent by the GUI
void UCI::handleCmd(const std::string &line) {
  std::cerr << line << std::endl;
  std::istringstream iss(line);

  std::vector<std::string> tokens;
  std::string token;

  while(iss >> token) tokens.push_back(token);
  if(tokens.empty()) return;

  const std::string &command = tokens[0];

  if(command == "uci") {
    this -> handleUci();
  }
  else if(command == "isready") {
    this -> handleIsReady();
  }
  else if (command == "ucinewgame")
  {
      handleNewGame();
  }
  else if (command == "position")
  {
      handlePosition(tokens);
  }
  else if (command == "go")
  {
      handleGo(tokens);
  }
  else if (command == "stop")
  {
      handleStop();
  }
  else if (command == "quit")
  {
      handleQuit();
  }
  else if (command == "setoption")
  {
      handleSetOption(tokens);
  }
  else if (command == "ponderhit")
  {
      handlePonderHit();
  }
  else if (command == "showboard") {
    board -> showBoard();
  }

  // Deferred for later : debug, register, etc.

}

void UCI::handleUci() {
  this -> send("id name Chessie");
  this -> send("id author Pal");

  // We can optionally send options too...

  this -> send("uciok");
}

void UCI::handleIsReady() {
  /*
    Remember that the GUI sent isready means different in different circumstances...
    1. If it is sent when our search threadd is not in execution, then it is sent by the GUI so that we can do all the initializations; the GUI is ready
       to wait for teh response.
    2. If it sent when our search thread is active, then the GUI meant to ping our engine to see if our engine is active and has not crashed. In such a situation,
       the GUI expects our engine to response with "readyok" immediately.
    But since we have separated search thread from our main thread, both the situations are handled automatically...
  */ 
  if(!this -> board) this -> board = new Board();
  this -> send("readyok");
}


void UCI::handleNewGame() {
  // Approach : We stop any existing search before changing the game state

  this -> stopSearch(false);

  std::lock_guard<std::mutex> lock(this -> stateMutex);

  this -> currentFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  // TODO : My engine's reset/new-game function goes here.
  //
  // Example:
  // board.reset();
  // tt.clear();
  // history.clear();
}


void UCI::handlePosition(const std::vector<std::string> &tokens) {
  /*
    We need to handle scenarios like :
      1. position startpos
      2. position fen <fen_str>
      3. position startpos moves ...
      4. position fen <fen_str> moves ...
      etc...
  */

  this -> stopSearch(false);             // We should never change board state while search is using it

  if(tokens.size() < 2) return;                       // The GUI passed only "position"

  std::string fen;

  if(tokens[1] == "startpos") fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  else if(tokens[1] == "fen") {
    /*
      A fen string consists of six fields 
    */

    std::ostringstream fenStream;
    // The fen string starts from index 2 of tokens

    for(int i = 2; i < 8; i++) {
      if(i != 2) fenStream << ' ';
      fenStream << tokens[i];
    }

    fen = fenStream.str();
  }
  else return;

  {
    std::lock_guard<std::mutex> lock(this -> stateMutex);
    this -> currentFen = fen;
    /*
    * Eventually this becomes something like:
    *
    * board.fromFEN(fen);
    */
    board -> fromFEN(fen);
  }

  // Now we have to find "moves" token in tokens
  size_t movesIndex = -1;

  for(size_t i = 0; i < tokens.size(); i++) {
    if(tokens[i] == "moves") {
      movesIndex = i;
      break;
    }
  }

  if(movesIndex == -1) return;
  for(size_t i = movesIndex + 1; i < tokens.size(); i++) {
    const std::string &moveStr = tokens[i];
    /*
    * Eventually:
    *
    * board.parseUciMove(move);
    * board.makeMove(...);
    */
    //cout << "this much ran\n" << endl;
    vector<Move> legalMoves = board -> moveHandler.generateLegalMoves(*board);
    // cout << "size of legal moves : " << board -> legalMoves.size() << endl;
    // exit(1);
    bool found = false;
    for(const auto &generatedLegalMove : legalMoves) {
      if(board -> moveHandler.legalMoveToString(generatedLegalMove) == moveStr) {
        cout << "applying : " << board -> moveHandler.legalMoveToString(generatedLegalMove) << endl;
        board->moveHandler.applyMove(generatedLegalMove, *board);
        this -> board->showBoard();
        //cout << "Castling rights : " << int(this -> board -> castlingRights) << endl;
        //std::cerr << "Found the move\n";
        found = true;
        break;
      }
    }
    if(!found) {
      // board -> showBoard();
      cout << "Didn't find : " << moveStr << endl;
      // cout << "Bishop moves : " << endl;
      // Bitboard bishopAttacks = this->board->moveHandler.getBishopMoves(61, board -> occupancy_all);
      // std::cerr << "f8 bishop attacks g7 = "
      //     << ((bishopAttacks & (1ULL << 54)) != 0)
      //     << '\n';
      //showBoard(this -> board -> moveHandler.getBishopMoves(61, this -> board -> occupancy_all));
      exit(0);
    }
  }
}


void UCI::handleGo(const std::vector<std::string> &tokens) {
  /*
    We have to handle commands like :
     1. go
     2. go depth 11
     3. go movetime 5000
     4. go wtime 60000 btime 60000
     5. go infinite
     etc...
  */ 

  this -> stopSearch(false);          // If GUI sends go while search thread is already searching, we should stop the search thread first.
                                                   // We shouldn't allow two searches to modify the same engine simultaneously.

  //this -> send("info string " + to_string(this -> board -> sideToMove));
  // Parsing the search params
  for(int i = 1; i < tokens.size(); i++) {
    if(tokens[i] == "depth" && i + 1 < tokens.size()) {
      int depth = std::stoi(tokens[++i]);
      std::cerr << "Requested depth : " << depth << std::endl;
    }
    else if(tokens[i] == "movetime" && i + 1 < tokens.size()) {
      int ms = std::stoi(tokens[++i]);
      std::cerr << "Requested movetime : " << ms << "ms" << std::endl;
    }
    else if(tokens[i] == "wtime" && i + 1 < tokens.size()) {
      int ms = std::stoi(tokens[++i]);
      std::cerr << "White time : " << ms << " ms\n";
    }
    else if (tokens[i] == "btime" && i + 1 < tokens.size()) {
        int ms = std::stoi(tokens[++i]);
        std::cerr << "Black time : " << ms << " ms\n";
    }
    else if (tokens[i] == "winc" && i + 1 < tokens.size()) {
      int ms = std::stoi(tokens[++i]);
      std::cerr << "White increment: " << ms << " ms\n";
    }
    else if (tokens[i] == "binc" && i + 1 < tokens.size()) {
      int ms = std::stoi(tokens[++i]);
      std::cerr << "Black increment: " << ms << " ms\n";
    }
    else if(tokens[i] == "infinite") std::cerr << "Infinite search\n";
    else if(tokens[i] == "ponder") std::cerr << "Ponder search\n";
  }
  
  this -> startSearch();
}


void UCI::startSearch() {
  if(this -> searching.load()) return;

  this -> stopRequested.store(false);

   /*
    * The search worker gets its own snapshot of whatever
    * position/state it needs.
    *
    * For now we copy the FEN.
    */
  std::string positionForSearch;

  {
      std::lock_guard<std::mutex> lock(stateMutex);
      positionForSearch = currentFen;
  }

  /*
  * Capture the position by value.
  *
  * This is important:
  *
  *     UCI thread
  *          |
  *          | modifies currentFen
  *          |
  *     search thread
  *
  * should NOT both touch the same Board.
  */
  searchThread = std::thread([this, positionForSearch](){
      (void)positionForSearch;
      this -> searchWorker();
    }
  );

  this -> searching.store(true);

}

void UCI::stopSearch(bool sendBestMove) {
  if(!this -> searchThread.joinable()) {
    this -> searching.store(false);
    return;
  }

  this -> stopRequested.store(true);

  // Wait until searchWorker() has actually finished...
  // Necessary before starting another search or changing shared state.
  this -> searchThread.join();

  this -> searching.store(false);

  /*
  * Normally the search thread itself prints bestmove.
  *
  * We therefore generally don't need this parameter.
  *
  * It is here to make the lifecycle explicit.
  */
  (void)sendBestMove;
}

void UCI::handleStop() {
  this -> stopSearch(true);
}

void UCI::handleQuit() {
  if(this -> quitting.exchange(true)) return;

  this -> stopRequested.store(true);

  if(this -> searchThread.joinable()) this -> searchThread.join();

  this -> searching.store(false);
}

void UCI::handleSetOption(const std::vector<std::string> &tokens) {
  /*
  * UCI syntax:
  *
  * setoption name <name> value <value>
  *
  * Example:
  *
  * setoption name Hash value 128
  */

  size_t nameIndex = 0, valueIndex = 0;

  for(size_t i = 1; i < tokens.size(); i++) {
    if(tokens[i] == "name") nameIndex = i + 1;
    else if(tokens[i] == "value") {
      valueIndex = i + 1;
      break;
    }
  }

  if(nameIndex == 0) return;
  std::string name;

  if(valueIndex != 0) {
    for (size_t i = nameIndex; i < valueIndex - 1; ++i)
      {
        if (!name.empty())
            name += ' ';

        name += tokens[i];
      }
  }
  else {
    for (size_t i = nameIndex; i < tokens.size(); ++i) {
      if (!name.empty())
          name += ' ';

      name += tokens[i];
    }
  }

  std::string value;
  if (valueIndex != 0) {
    for (size_t i = valueIndex; i < tokens.size(); ++i)
    {
      if (!value.empty())
          value += ' ';

      value += tokens[i];
    }
  }

  /*
  * Now:
  *
  * name
  * value
  *
  * can be passed to your engine.
  *
  * Example:
  *
  * if (name == "Hash")
  *     transpositionTable.resize(std::stoi(value));
  */

  std::cerr << "option : " << name << " = " << value << '\n';
}

void UCI::handlePonderHit() {
  /*
  * For now we can leave this empty.
  *
  * Once we implement pondering, this should tell the
  * search that the predicted opponent move actually
  * happened and the search should continue normally.
  */
}

/*
 * ------------------------------------------------------------
 * SEARCH WORKER
 * ------------------------------------------------------------
 *
 * This is intentionally NOT a chess implementation.
 *
 * It demonstrates exactly how your future minimax/alpha-beta
 * search should cooperate with UCI.
 */
void UCI::searchWorker()
{
    /*
     * Example of iterative deepening:
     *
     * depth 1
     * depth 2
     * depth 3
     * ...
     *
     * After every iteration, check stopRequested.
     */

    // for (int depth = 1; depth <= 10; ++depth)
    // {
    //     /*
    //      * ----------------------------------------------------
    //      * YOUR REAL SEARCH GOES HERE
    //      * ----------------------------------------------------
    //      *
    //      * SearchResult result =
    //      *     search(board, depth, stopRequested);
    //      *
    //      * Your recursive search should periodically check:
    //      *
    //      * if (stopRequested.load())
    //      *     return;
    //      */

    //     // Simulate expensive searching.
    //     std::this_thread::sleep_for(
    //         std::chrono::milliseconds(100)
    //     );

    //     /*
    //      * Check whether the UCI thread asked us to stop.
    //      */
    //     if (stopRequested.load())
    //         break;

    //     /*
    //      * UCI "info" output.
    //      *
    //      * This is normally emitted after completed iterations.
    //      */
    //     send(
    //         "info depth " +
    //         std::to_string(depth) +
    //         " score cp 0"
    //     );
    // }
    
    //this -> send("info string " + this -> board->toFEN());

    /*
     * Don't send bestmove when the entire engine is quitting.
     */
    vector<Move> legalMoves = board -> moveHandler.generateLegalMoves(*board);
    cout << legalMoves.size() << endl;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, legalMoves.size() - 1);
    int randIndex = dist(gen);

    // Applying move
    board -> moveHandler.applyMove(legalMoves[randIndex], *board);
    
    if (!quitting.load())
    {
        /*
         * Eventually this comes from your search result:
         *
         * searchResult.bestMove.toUci()
         */
        send("bestmove " + board -> moveHandler.legalMoveToString(legalMoves[randIndex]));
    }

    searching.store(false);
}

/*
 * ------------------------------------------------------------
 * Thread-safe UCI output
 * ------------------------------------------------------------
 */
void UCI::send(const std::string& message)
{
    std::lock_guard<std::mutex> lock(outputMutex);

    std::cout << message << '\n';

    /*
     * UCI communication is line-oriented, and in practice
     * flushing immediately is a good idea.
     */
    std::cout.flush();
}