#pragma once

struct SearchConfig {
    int maxDepth = 12;
    unsigned threads = 4;
    int splitMinDepth = 4;
    int splitMinMoves = 2;
};
