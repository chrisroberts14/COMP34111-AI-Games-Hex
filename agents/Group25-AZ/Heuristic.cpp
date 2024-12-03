//
// Created by chris on 26/11/24.
//

#include "Heuristic.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <limits>
#include <cmath>

static std::vector<std::pair<int, int> > bridge_directions = {
    {-2, 1}, // North
    {-1, 2}, // North East
    {1, 1}, // South East
    {2, -1}, // South
    {1, -2}, // South West
    {-1, -1} // North West
};

void Heuristic::output_board() {
    // Output as a hex board with spaces to offset each row
    // If move is not in either set output a .
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < i; ++j) {
            std::cerr << " ";
        }
        for (int j = 0; j < 11; ++j) {
            if (red_moves.find({i, j}) != red_moves.end()) {
                std::cerr << "R ";
            } else if (blue_moves.find({i, j}) != blue_moves.end()) {
                std::cerr << "B ";
            } else {
                std::cerr << ". ";
            }
        }
        std::cerr << std::endl;
    }
}

void Heuristic::insert_blue_move(const std::pair<int, int> &move) {
    blue_moves.insert(move);
    // Regenerate the candidate moves
    candidate_moves = get_candidate_moves();
}

void Heuristic::insert_red_move(const std::pair<int, int> &move) {
    red_moves.insert(move);
    // Regenerate the candidate moves
    candidate_moves = get_candidate_moves();
}

bool Heuristic::is_valid(const int row, const int col) const {
    return row >= 0 && row < board_size && col >= 0 && col < board_size;
}

bool Heuristic::is_valid(const std::pair<int, int> &node) const {
    return is_valid(node.first, node.second);
}

bool Heuristic::is_free(const int row, const int col) const {
    return red_moves.find({row, col}) == red_moves.end() && blue_moves.find({row, col}) == blue_moves.end();
}

bool Heuristic::is_free(const std::pair<int, int> &node) const {
    return is_free(node.first, node.second);
}

bool Heuristic::is_tile_opponents(const std::pair<int, int> &node) const {
    return (red_moves.find(node) != red_moves.end() && colour == "B") ||
        (blue_moves.find(node) != blue_moves.end() && colour == "R");
}

std::pair<std::pair<int, int>, std::pair<int, int> > Heuristic::get_bridge_intermediates(const std::pair<int, int> &start_tile, const std::pair<int, int> &bridge_tile) {
    const auto direction_pair = std::pair(bridge_tile.first - start_tile.first, bridge_tile.second - start_tile.second);
    if (direction_pair == bridge_directions[0]) {
        return {{start_tile.first - 1, start_tile.second},
            {start_tile.first - 1, start_tile.second + 1}};
    }
    if (direction_pair == bridge_directions[1]) {
        return {{start_tile.first - 1, start_tile.second + 1},
            {start_tile.first, start_tile.second + 1}};
    }
    if (direction_pair == bridge_directions[2]) {
        return {{start_tile.first, start_tile.second + 1},
            {start_tile.first + 1, start_tile.second}};
    }
    if (direction_pair == bridge_directions[3]) {
        return {{start_tile.first + 1, start_tile.second},
            {start_tile.first + 1, start_tile.second - 1}};
    }
    if (direction_pair == bridge_directions[4]) {
        return {{start_tile.first + 1, start_tile.second - 1},
            {start_tile.first, start_tile.second - 1}};
    }
    if (direction_pair == bridge_directions[5]) {
        return {{start_tile.first, start_tile.second - 1},
            {start_tile.first - 1, start_tile.second}};
    }
    throw std::invalid_argument("Invalid bridge direction");
}


std::set<std::pair<int, int>> Heuristic::get_abandoned_moves() const {
    // Don't pick moves which are in opponent bridges
    std::set<std::pair<int, int>> abandoned_moves;
    std::set<std::pair<int, int>> opponent_moves = colour == "R" ? blue_moves : red_moves;
    // Find all tiles which are in opponent bridges
    for (const auto &[row, col] : opponent_moves) {
        // Edge bridges
        // If the opponent is blue and the tile is on the left or right edge
        // abandon the tiles between the node and the edge
        if (colour != "B" && (col == 1 || col == board_size - 2)) {
            if (col == 1) {
                // Check both nodes to the left are free
                const std::set<std::pair<int, int>> left_nodes = {{row + 1, col - 1}, {row, col - 1}};
                if (std::all_of(left_nodes.begin(), left_nodes.end(), [this](const std::pair<int, int> &node) {
                    return is_valid(node) && is_free(node);
                })) {
                    abandoned_moves.insert({row, col});
                }
            } else if (col == board_size - 2) {
                // Check both nodes to the right are free
                const std::set<std::pair<int, int>> right_nodes = {{row - 1, col + 1}, {row, col + 1}};
                if (std::all_of(right_nodes.begin(), right_nodes.end(), [this](const std::pair<int, int> &node) {
                    return is_valid(node) && is_free(node);
                })) {
                    abandoned_moves.insert({row, col});
                }
            }
        } else if (colour != "R" && (row == 1 || row == board_size - 2)) {
            if (col == 1) {
                // Check both nodes above are free
                const std::set<std::pair<int, int>> above_nodes = {{row - 1, col + 1}, {row - 1, col}};
                if (std::all_of(above_nodes.begin(), above_nodes.end(), [this](const std::pair<int, int> &node) {
                    return is_valid(node) && is_free(node);
                })) {
                    abandoned_moves.insert({row, col});
                }
            } else if (col == board_size - 2) {
                // Check both nodes below are free
                const std::set<std::pair<int, int>> below_nodes = {{row + 1, col - 1}, {row + 1, col}};
                if (std::all_of(below_nodes.begin(), below_nodes.end(), [this](const std::pair<int, int> &node) {
                    return is_valid(node) && is_free(node);
                })) {
                    abandoned_moves.insert({row, col});
                }
            }
        }
        // For non edge bridges
        for (const auto &direction : bridge_directions) {
            const auto [dr, dc] = direction;
            if (const std::pair bridge_tile = {row + dr, col + dc}; is_valid(bridge_tile) && is_tile_opponents(bridge_tile)) {
                if (auto [intermediate_1, intermediate_2] = get_bridge_intermediates({row, col}, bridge_tile); is_valid(intermediate_1) && is_valid(intermediate_2) &&
                                                                                                               is_free(intermediate_1) && is_free(intermediate_2)) {
                    abandoned_moves.insert(intermediate_1);
                    abandoned_moves.insert(intermediate_2);
                }
            }
        }
    }
    return abandoned_moves;
}


std::set<std::pair<int, int> > Heuristic::get_candidate_moves() {
    // Find all the moves which are open and not abandoned
    std::set<std::pair<int, int>> abandoned_moves = get_abandoned_moves();
    std::set<std::pair<int, int>> candidate_moves;
    for (int row = 0; row < board_size; ++row) {
        for (int col = 0; col < board_size; ++col) {
            if (red_moves.find({row, col}) == red_moves.end() &&
                blue_moves.find({row, col}) == blue_moves.end() &&
                abandoned_moves.find({row, col}) == abandoned_moves.end()) {
                candidate_moves.insert({row, col});
            }
        }
    }
    return candidate_moves;
}

bool Heuristic::creates_bridge_to_edge(const std::pair<int, int> &move) {
    // Edge check (If the node is one from the edge and both tiles between it and the edge are free it is a bridge)
    // Check it is one row in from the edge this is different for each colour
    if (colour == "R" && (move.first == board_size - 2 || move.first == 1) &&
        move.second != 0 && move.second != board_size - 1) { // Red
        // Top and bottom are edges
        if (move.first == 1) {
            // Check both nodes above are free
            const std::set<std::pair<int, int>> above_nodes = {{move.first - 1, move.second + 1}, {move.first - 1, move.second}};
            if (std::all_of(above_nodes.begin(), above_nodes.end(), [this](const std::pair<int, int> &node) {
                return is_valid(node) && candidate_moves.find(node) != candidate_moves.end();
            })) {
                return true;
            }
        } else if (move.first == board_size - 2) {
            // Check both nodes below are free
            const std::set<std::pair<int, int>> below_nodes = {{move.first + 1, move.second - 1}, {move.first + 1, move.second}};
            if (std::all_of(below_nodes.begin(), below_nodes.end(), [this](const std::pair<int, int> &node) {
                return is_valid(node) && candidate_moves.find(node) != candidate_moves.end();
            })) {
                return true;
            }
        }
        } else if (colour == "B" && (move.second == board_size - 2 || move.second == 1) &&
            move.first != 0 && move.first != board_size - 1) { // Blue
            // Left and right are edges
                if (move.second == 1) {
                    // Left edge
                    // Check both nodes to the left are free
                    const std::set<std::pair<int, int>> left_nodes = {{move.first + 1, move.second - 1}, {move.first, move.second - 1}};
                    if (std::all_of(left_nodes.begin(), left_nodes.end(), [this](const std::pair<int, int> &node) {
                        return is_valid(node) && candidate_moves.find(node) != candidate_moves.end();
                    })) {
                        return true;
                    }
                } else if (move.second == board_size - 2) {
                    // Right edge
                    // Check both nodes to the right are free
                    const std::set<std::pair<int, int>> right_nodes = {{move.first - 1, move.second + 1}, {move.first, move.second + 1}};
                    if (std::all_of(right_nodes.begin(), right_nodes.end(), [this](const std::pair<int, int> &node) {
                        return is_valid(node) && candidate_moves.find(node) != candidate_moves.end();
                    })) {
                        return true;
                    }
                }
            }
    return false;
}

bool Heuristic::creates_bridge_with_tile(const std::pair<int, int> &move) {
    // Check all move directions for a bridge and if that is valid and the tile is the same as the colour and both in between tiles are free
    // then that move creates a bridge
    return std::any_of(bridge_directions.begin(), bridge_directions.end(), [this, &move](const auto &direction) {
        const auto [dr, dc] = direction;
        if (const std::pair bridge_node = {move.first + dr, move.second + dc}; is_valid(bridge_node) && !is_tile_opponents(bridge_node) && !is_free(bridge_node)) {
            auto [intermediate_1, intermediate_2] = get_bridge_intermediates(move, bridge_node);
            return is_free(intermediate_1) && is_free(intermediate_2);
        }
        return false;
    });
}


bool Heuristic::does_move_create_bridge(const std::pair<int, int> &move) {
    // Check if the move creates a bridge
    if (creates_bridge_to_edge(move)) {
        return true;
    }
    // Check if creates a bridge with another tile
    if (creates_bridge_with_tile(move)) {
        return true;
    }
    return false;
}


double Heuristic::evaluate_move(const std::pair<int, int> &move) {
    // At the start of the game moves near the center are better
    // We want to decrease this value as the game goes on
    const auto move_count = static_cast<double>(red_moves.size() + blue_moves.size());
    const double center_value = (1.0 - std::sqrt(std::pow(move.first - (board_size - 1) / 2.0, 2) +
                                           std::pow(move.second - (board_size - 1) / 2.0, 2)) /
                           (std::sqrt(2) * (board_size - 1) / 2.0)) / (1.0 + move_count);

    // Moves which create bridges are good
    // So if the move creates a bridge we want to increase the value
    const double bridge_value = does_move_create_bridge(move) ? 1.0 : 0.0;

    // Add more heuristics here

    return center_value + bridge_value;
}

std::pair<int, int> Heuristic::get_best_move() {
    std::pair best_move = {-1, -1};
    double best_value = -std::numeric_limits<double>::infinity();
    for (const auto &[row, col] : candidate_moves) {
        if (const double value = evaluate_move({row, col}); value > best_value) {
            best_value = value;
            best_move = {row, col};
        }
    }
    return best_move;
}

