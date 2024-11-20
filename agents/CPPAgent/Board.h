//
// Created by chris on 20/11/24.
//

#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>
#include "Tile.h"

class Board {
private:
    std::vector<std::vector<Tile>> state;
    int size;
    std::string winner;

public:

    Board(std::vector<std::vector<Tile>> state_vec, const int& size = 11, std::string winner = "");
    Board(const Board& board, const int& size = 11, std::string winner = "");
    void make_move(std::pair<int, int>& move, std::string colour);
    std::vector<std::pair<int, int>> get_moves() const;
    std::vector<std::vector<Tile>>& get_state();
};

#endif //BOARD_H
