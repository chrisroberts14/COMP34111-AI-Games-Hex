//
// Created by chris on 20/11/24.
//

#include "Board.h"

#include "Tile.h"
#include <set>
#include <sstream>
#include <stack>
#include <utility>
#include <random>
#include <cmath>

Board::Board(std::vector<std::vector<Tile> > state_vec, const int &size,
             std::string winner)
    : state(std::move(state_vec)), size(size), winner(std::move(winner)) {
}

std::vector<std::vector<Tile> > &Board::get_state() { return this->state; }

void Board::make_move(const std::pair<int, int> &move, const std::string &colour) {
    this->state.at(move.first).at(move.second).setColour(colour);
}

std::vector<std::pair<int, int> > Board::get_moves() const {
    std::vector<std::pair<int, int> > choices;
    for (int i = 0; i < this->size; ++i) {
        for (int j = 0; j < this->size; ++j) {
            if (this->state[i][j].getColour() == "0") {
                choices.emplace_back(i, j);
            }
        }
    }
    return choices;
}

Board Board::duplicate() const {
    return Board(this->state, this->size, this->winner);
}

std::string Board::has_ended() const {
    constexpr int size = 11;
    std::set<std::pair<int, int> > visited;

    std::vector<Tile> red_tiles;
    std::vector<Tile> blue_tiles;

    for (int idx = 0; idx < size; ++idx) {
        if (state[0][idx].getColour() == "R") {
            red_tiles.push_back(state[0][idx]);
        }
        if (state[idx][0].getColour() == "B") {
            blue_tiles.push_back(state[idx][0]);
        }
    }

    // Check red tiles
    for (const auto &tile: red_tiles) {
        if (visited.insert({tile.getX(), tile.getY()}).second) {
            if (DFSColour(0, tile.getY(), "R", visited) == "R") {
                return "R";
            }
        }
    }

    // Check blue tiles
    for (const auto &tile: blue_tiles) {
        if (visited.insert({tile.getX(), tile.getY()}).second) {
            if (DFSColour(tile.getX(), 0, "B", visited) == "B") {
                return "B";
            }
        }
    }

    return "";
}

std::string Board::DFSColour(int x, int y, const std::string &colour,
                             std::set<std::pair<int, int> > &visited) const {
    std::stack<std::pair<int, int> > stack;
    stack.emplace(x, y);
    visited.insert({x, y});

    while (!stack.empty()) {
        constexpr int size = 11;
        auto [fst, snd] = stack.top();
        stack.pop();

        // Win conditions
        if (colour == "R" && fst == size - 1) {
            return "R";
        }
        if (colour == "B" && snd == size - 1) {
            return "B";
        }

        // Visit neighbours
        for (int i = 0; i < Tile::NEIGHBOUR_COUNT; ++i) {
            const int x_n = fst + Tile::I_DISPLACEMENTS[i];

            if (const int y_n = snd + Tile::J_DISPLACEMENTS[i]; x_n >= 0 && x_n < size && y_n >= 0 && y_n < size) {
                if (visited.find({x_n, y_n}) == visited.end() &&
                    state[x_n][y_n].getColour() == colour) {
                    visited.insert({x_n, y_n});
                    stack.emplace(x_n, y_n);
                }
            }
        }
    }
    return "";
}

bool Board::should_swap() const {
    // Should only be called if the player is on turn two
    std::pair<int, int> red_move_coords;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (state[i][j].getColour() == "R") {
                red_move_coords = {i, j};
                break;
            }
        }
    }

    // Don't swap these coordinates
    // Refer to: https://www.hexwiki.net/index.php/Swap_rule#Size_11
    if ((red_move_coords.second == 0 && red_move_coords.first < 10) ||
        (red_move_coords.second == 1 && red_move_coords.first < 9) ||
        (red_move_coords.second == 2 && (red_move_coords.first == 0 || red_move_coords.first == 10)) ||
        (red_move_coords.second == 8 && (red_move_coords.first == 0 || red_move_coords.first == 10)) ||
        (red_move_coords.second == 9 && red_move_coords.first < 2) ||
        (red_move_coords.second == 10 && red_move_coords.first < 1)) {
        return false;
    }
    return true;
}

std::pair<int, int> Board::opening_move() {
    // This is a strong move that is played first
    return {1, 2};
}