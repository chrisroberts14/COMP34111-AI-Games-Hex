//
// Created by chris on 30/11/24.
//

#include "DFS.h"

DFS::DFS(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves)
    : red_moves(std::move(red_moves)), blue_moves(std::move(blue_moves)) {}

std::string DFS::get_winner() {
    for (const auto &move : red_moves) {
        if (move.first == 0) {
        visited.insert(move);
        if (dfs(move, "R")) {
            return "R";
        }
        }
    }

    visited.clear();

    for (const auto &move : blue_moves) {
        if (move.second == 0) {
        visited.insert(move);
        if (dfs(move, "B")) {
            return "B";
        }
        }
    }

    return "0";
}

bool DFS::dfs(const std::pair<int, int> &move, const std::string &player) {
    if (player == "R" && move.first == 10) {
        return true;
    }
    if (player == "B" && move.second == 10) {
        return true;
    }

    for (const auto &direction : directions) {
        std::pair<int, int> new_move = {move.first + direction.first, move.second + direction.second};
        if (new_move.first >= 0 && new_move.first < 11 && new_move.second >= 0 && new_move.second < 11) {
            if (player == "R" && red_moves.find(new_move) != red_moves.end() &&  visited.find(new_move) == visited.end()) {
                visited.insert(new_move);
                if (dfs(new_move, player)) {
                    return true;
                }
            }
            if (player == "B" && blue_moves.find(new_move) != blue_moves.end() && visited.find(new_move) == visited.end()) {
                visited.insert(new_move);
                if (dfs(new_move, player)) {
                    return true;
                }
            }
        }
    }

    return false;
}
