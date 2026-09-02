#pragma once
#include "../../../core/include/game.h"
#include <atomic>
#include <mutex>
#include <search.h>
#include <string>
#include <thread>
#include <vector>

class UCI {
  public :
    UCI();
    ~UCI();

    void loop();

  private :

    // member data
    Game game;
    std::string currentFen;
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> searching{false};
    std::thread searchThread;                      // the thread that will do the searching
    std::mutex outputMutex;                        // protects stdout
    std::mutex stateMutex;                         // protects curretFen / other engine state info
    std::atomic<bool> quitting{false};          // used to distinguish between stopping and quitting

    

    
    // UCI command handling
    void handleCmd(const std::string&);

    void handleUci();
    void handleIsReady();
    void handlePosition(const std::vector<std::string>&);
    void handleGo(const std::vector<std::string>&);
    void handleStop();
    void handleQuit();
    void handleNewGame();
    void handleSetOption(const std::vector<std::string>&);
    void handlePonderHit();

    // Search thread management
    void startSearch();
    void stopSearch(bool sendBestMove);

    void searchWorker();

    // UCI output
    void send(const std::string&);

};