//
// Created by chris on 23/11/2024.
//

#ifndef BESTMOVE_H
#define BESTMOVE_H
#include <vector>
#include "Tile.h"


class BestMove {
    int potentials[6] = {};
    const int neighbour_count[6] = {};
    const int move_count;
    const int boardSize;
    const std::vector<std::vector<Tile>> board;
    const std::string colour;
    double potential
};



#endif //BESTMOVE_H
