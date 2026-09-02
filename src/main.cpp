#include "board.h"
#include "perft.h"
#include <cassert>
#include <iostream>





int main()
{
    for(int i = 0; i < 6; i++) {
      Board board;
      board.fromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

      cout << perft(board, i) << endl;
    }
    

    // UCI uci;
    // uci.loop();




    return 0;
}










