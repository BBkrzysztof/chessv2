#pragma once
#include "../Bitboard.h"

using namespace Bitboards;
using namespace std;

namespace Tests {

    static BitBoard ray_to_edge(int r, int c, int dr, int dc) {
        BitBoard bb = 0;
        int rr = r, cc = c;
        while (true) {
            rr += dr;
            cc += dc;
            if (rr < 0 || rr > 7 || cc < 0 || cc > 7) break;
            bb |= bit(getPos(rr, cc));
        }
        return bb;
    }

    static BitBoard beyond_blocker(int br, int bc, int dr, int dc) {
        BitBoard bb = 0;
        int rr = br, cc = bc;
        while (true) {
            rr += dr;
            cc += dc;
            if (rr < 0 || rr > 7 || cc < 0 || cc > 7) break;
            bb |= bit(getPos(rr, cc));
        }
        return bb;
    }

    static void printTestCase(const BitBoard& ) {
        cout<<"===Test case===\n"<<endl;


    }

}
