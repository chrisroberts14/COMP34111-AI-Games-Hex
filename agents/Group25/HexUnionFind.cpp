//
// Created by chris on 25/11/24.
//

#include "HexUnionFind.h"

#include "HexUnionFind.h"
#include <stdexcept>
#include <set>
#include <iostream>

HexUnionFind::HexUnionFind(const int board_size, const std::set<std::pair<int, int> >& red_moves,
                           const std::set<std::pair<int, int> >& blue_moves)
    : board_size(board_size), uf(board_size * board_size + 4), red_moves(red_moves), blue_moves(blue_moves) {
    const int total_nodes = board_size * board_size + 4;
    bottom = total_nodes - 1;
    top = total_nodes - 2;
    right = total_nodes - 3;
    left = total_nodes - 4;

    // For each move run the add_move function this will update the union find data structure
    for (const auto &[x, y]: red_moves) {
        add_move(x, y, "R");
    }
    for (const auto &[x, y]: blue_moves) {
        add_move(x, y, "B");
    }
}

int HexUnionFind::position_to_index(int x, int y) const {
    return x * board_size + y;
}

std::vector<std::pair<int, int> > HexUnionFind::get_neighbors(int x, int y) const {
    static const std::vector<std::pair<int, int> > directions = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 0}, {1, -1}, {0, -1}
    };

    std::vector<std::pair<int, int> > neighbors;
    for (auto [dr, dc]: directions) {
        const int new_y = y + dc;
        if (int new_x = x + dr; new_x >= 0 && new_x < board_size && new_y >= 0 && new_y < board_size) {
            neighbors.emplace_back(new_x, new_y);
        }
    }
    return neighbors;
}

bool HexUnionFind::add_move(int x, int y, const std::string& player) {
    const int index = position_to_index(x, y);
    if (player == "R") {
        red_moves.insert({x, y});
    } else {
        blue_moves.insert({x, y});
    }

    if (player == "R") {
        if (x == 0) uf.union_sets(index, top);
        if (x == board_size - 1) uf.union_sets(index, bottom);
    } else if (player == "B") {
        if (y == 0) uf.union_sets(index, left);
        if (y == board_size - 1) uf.union_sets(index, right);
    }

    auto neighbors = get_neighbors(x, y);
    for (auto [nr, nc]: neighbors) {
        const int neighborIndex = position_to_index(nr, nc);
        // Only union sets if the neighbor is the same player
        if (player == "R" && red_moves.count({nr, nc})) {
            uf.union_sets(index, neighborIndex);
        } else if (player == "B" && blue_moves.count({nr, nc})) {
            uf.union_sets(index, neighborIndex);
        }
    }
    return check_winner(player);
}

bool HexUnionFind::check_winner(const std::string& player) {
    if (player == "R") return uf.connected(top, bottom);
    if (player == "B") return uf.connected(left, right);
    return false;
}
