#include "frontends/uci/include/uci.h"





int main()
{
    // Game game;
    // game.init();
    // for(int i = 0; i < 6; i++) {
    //   game.board -> fromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    //   cout << perft(*game.board, *game.moveHandler, i) << endl;
    // }
    

    UCI uci;
    uci.loop();

    return 0;
}










