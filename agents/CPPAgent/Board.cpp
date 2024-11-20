//
// Created by chris on 20/11/24.
//

#include <sstream>
#include "Board.h"
#include "Tile.h"

Board::Board(std::vector<std::vector<Tile>> state_vec, const int& size, std::string winner)
    : state(state_vec), size(size), winner(winner) {}

Board::Board(const Board& board, const int& size, std::string winner)
    : state(board.state), size(size), winner(winner) {}

std::vector<std::vector<Tile>>& Board::get_state() {
    return this->state;
}

void Board::make_move(std::pair<int, int>& move, std::string colour) {
    this->state.at(move.first).at(move.second).setColour(colour);
}

std::vector<std::pair<int, int>> Board::get_moves() const {
    std::vector<std::pair<int, int>> choices;
    for (int i = 0; i < this->size; ++i) {
        for (int j = 0; j < this->size; ++j) {
            if (this->state[i][j].getColour() == "0") {
                choices.emplace_back(i, j);
            }
        }
    }
    return choices;
}
