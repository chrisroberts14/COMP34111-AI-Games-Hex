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
#include <queue>

#include "DFS.h"

static std::vector<std::pair<int, int> > bridge_directions = {
    {-2, 1}, // North
    {-1, 2}, // North East
    {1, 1}, // South East
    {2, -1}, // South
    {1, -2}, // South West
    {-1, -1} // North West
};


Heuristic::Heuristic(std::set<std::pair<int, int> > red_moves, std::set<std::pair<int, int> > blue_moves)
    : red_moves(std::move(red_moves)), blue_moves(std::move(blue_moves)) {
    generate_values();
}

std::set<std::pair<int, int>> Heuristic::get_free_tiles(){
    std::set<std::pair<int, int>> free_tiles;
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            if (std::pair tile = {i, j}; is_tile_free(tile)) {
                free_tiles.insert(tile);
            }
        }
    }
    return free_tiles;
}


bool Heuristic::isPathClear(std::pair<int, int> start, std::pair<int, int> end) {
    if (red_moves.empty() && blue_moves.empty()) {
        return true;
    }

    for (const auto &direction: bridge_directions) {
        if (end == direction) {
            std::pair<int, int> first_node, second_node;
            if (direction == bridge_directions[0]) {
                first_node = {start.first - 1, start.second};
                second_node = {start.first - 1, start.second + 1};
            } else if (direction == bridge_directions[1]) {
                first_node = {start.first - 1, start.second + 1};
                second_node = {start.first, start.second + 1};
            } else if (direction == bridge_directions[2]) {
                first_node = {start.first, start.second + 1};
                second_node = {start.first + 1, start.second};
            } else if (direction == bridge_directions[3]) {
                first_node = {start.first + 1, start.second};
                second_node = {start.first + 1, start.second - 1};
            } else if (direction == bridge_directions[4]) {
                first_node = {start.first + 1, start.second - 1};
                second_node = {start.first, start.second - 1};
            } else if (direction == bridge_directions[5]) {
                first_node = {start.first, start.second - 1};
                second_node = {start.first - 1, start.second};
            }

            return !((!red_moves.empty() && (red_moves.find(first_node) != red_moves.end() || red_moves.
                                             find(second_node) != red_moves.end())) ||
                     (!blue_moves.empty() && (blue_moves.find(first_node) != blue_moves.end() || blue_moves.
                                              find(second_node) != blue_moves.end())));
        }
    }
    return false;
}

bool Heuristic::check_inbetweens(std::pair<int, int> start, std::pair<int, int> direction, const std::string &colour) {
    auto [fst, snd] = get_bridge_nodes(start, direction);

    const auto &opponent_moves = (colour == "R") ? blue_moves : red_moves;
    auto &values = (colour == "R") ? red_values : blue_values;

    if (opponent_moves.find(fst) != opponent_moves.end() || opponent_moves.find(snd) != opponent_moves.end()) {
        if (opponent_moves.find(fst) != opponent_moves.end() && opponent_moves.find(snd) == opponent_moves.end()) {
            if (is_coordinate_valid(snd)) {
                values[snd.first][snd.second] = 100000;
                return true;
            }
        }
        if (is_coordinate_valid(fst)) {
            values[fst.first][fst.second] = 100000;
            return true;
        }
    }
    return false;
}

double Heuristic::get_centre_value(const int x, const int y) {
    // Calculate how close the node is to the center of the board
    // The closer the node is to the center the higher the value
    // The further the node is from the center the lower the value
    return 0.05 / (1 + (std::abs(5 - x) + std::abs(5 - y)));
}

bool Heuristic::are_both_red(const std::pair<int, int> &start, const std::pair<int, int> &end) {
    if (red_moves.empty()) {
        return false;
    }
    return red_moves.find(start) != red_moves.end() && red_moves.find(end) != red_moves.end();
}

bool Heuristic::are_both_blue(const std::pair<int, int> &start, const std::pair<int, int> &end) {
    if (blue_moves.empty()) {
        return false;
    }
    return blue_moves.find(start) != blue_moves.end() && blue_moves.find(end) != blue_moves.end();
}

bool Heuristic::is_tile_free(const std::pair<int, int> &tile) {
    if (red_moves.empty() && blue_moves.empty()) {
        return true;
    }
    if (red_moves.empty()) {
        return blue_moves.find(tile) == blue_moves.end();
    }
    if (blue_moves.empty()) {
        return red_moves.find(tile) == red_moves.end();
    }
    return red_moves.find(tile) == red_moves.end() && blue_moves.find(tile) == blue_moves.end();
}

bool Heuristic::is_tile_red(const std::pair<int, int> &tile) {
    if (red_moves.empty()) {
        return false;
    }
    return red_moves.find(tile) != red_moves.end();
}

bool Heuristic::is_tile_blue(const std::pair<int, int> &tile) {
    if (blue_moves.empty()) {
        return false;
    }
    return blue_moves.find(tile) != blue_moves.end();
}

bool Heuristic::is_coordinate_valid(const std::pair<int, int> &coord) {
    return coord.first >= 0 && coord.first < 11 && coord.second >= 0 && coord.second < 11;
}

bool Heuristic::can_complete_path_with_bridges(const std::string &colour) {
    std::set<std::pair<int, int> > red_moves_with_bridges = red_moves;
    std::set<std::pair<int, int> > blue_moves_with_bridges = blue_moves;
    // Get all the nodes that are bridges
    for (int row = 0; row < 11; row++) {
        for (int col = 0; col < 11; col++) {
            std::pair start_tile = {row, col};
            // For each direction get the bridge nodes in every direction
            for (const auto &direction: bridge_directions) {
                if (std::pair bridge_tile = {row + direction.first, col + direction.second}; (
                    is_coordinate_valid(bridge_tile) || (
                        (bridge_tile.first == 11 || bridge_tile.first == -1) && colour == "R") || (
                        (bridge_tile.second == 11 || bridge_tile.second == -1) && colour == "B"))) {
                    if (auto [first_node, second_node] = get_bridge_nodes({row, col}, direction);
                        is_coordinate_valid(first_node) && is_coordinate_valid(second_node) && is_tile_free(first_node)
                        && is_tile_free(second_node)) {
                        if (are_both_red(start_tile, bridge_tile) ||
                            (is_tile_red(start_tile) && bridge_tile.first == 11) ||
                            (is_tile_red(start_tile) && bridge_tile.first == -1)) {
                            // If both are red and both tiles inbetween are free
                            red_moves_with_bridges.insert(first_node);
                            red_moves_with_bridges.insert(second_node);
                        }
                        if (are_both_blue(start_tile, bridge_tile) ||
                            (is_tile_blue(start_tile) && bridge_tile.second == 11) ||
                            (is_tile_blue(start_tile) && bridge_tile.second == -1)) {
                            // If both are blue and both tiles inbetween are free
                            blue_moves_with_bridges.insert(first_node);
                            blue_moves_with_bridges.insert(second_node);
                        }
                    }
                }
            }
        }
    }
    DFS dfs(red_moves_with_bridges, blue_moves_with_bridges);
    return dfs.get_winner() == colour;
}

std::pair<std::pair<int, int>, std::pair<int, int> > Heuristic::get_bridge_nodes(
    const std::pair<int, int> &start, const std::pair<int, int> &direction) {
    // Get the two nodes which are adjacent to the start node and end node
    std::pair<int, int> first_node, second_node;
    if (direction == bridge_directions[0]) {
        first_node = {start.first - 1, start.second};
        second_node = {start.first - 1, start.second + 1};
    } else if (direction == bridge_directions[1]) {
        first_node = {start.first - 1, start.second + 1};
        second_node = {start.first, start.second + 1};
    } else if (direction == bridge_directions[2]) {
        first_node = {start.first, start.second + 1};
        second_node = {start.first + 1, start.second};
    } else if (direction == bridge_directions[3]) {
        first_node = {start.first + 1, start.second};
        second_node = {start.first + 1, start.second - 1};
    } else if (direction == bridge_directions[4]) {
        first_node = {start.first + 1, start.second - 1};
        second_node = {start.first, start.second - 1};
    } else if (direction == bridge_directions[5]) {
        first_node = {start.first, start.second - 1};
        second_node = {start.first - 1, start.second};
    }
    return {first_node, second_node};
}

void Heuristic::add_bridge_values() {
  // Combine red and blue moves into one set
  // std::cerr << "Adding bridge values" << std::endl;
    std::set<std::pair<int, int>> all_moves = red_moves;
    all_moves.insert(blue_moves.begin(), blue_moves.end());
    //std::cerr << "All moves size: " << all_moves.size() << std::endl;
    for (const auto &start: all_moves) {
                for (const auto &direction: bridge_directions) {
                    // If the path is not clear and both tiles are the same colour TAKE THAT MOVE
                    const std::pair bridge_move = {start.first + direction.first, start.second + direction.second};
                    if (is_coordinate_valid(bridge_move) || (bridge_move.first == 11 && is_tile_red(start)) || (
                            bridge_move.second == 11 && is_tile_blue(start)) ||
                        (bridge_move.first == -1 && is_tile_red(start)) || (
                            bridge_move.second == -1 && is_tile_blue(start))) {
                        if (are_both_red(start, bridge_move) ||
                            (is_tile_red(start) && bridge_move.first == 11) ||
                            (is_tile_red(start) && bridge_move.first == -1)) {
                            if (!check_inbetweens(start, direction, "R")) {
                                // A bridge exists
                                // Only take that if is part of a path that can be completed with bridges
                                if (!can_complete_path_with_bridges("R")) {
                                    // Mark the inbetween nodes with a low score so they aren't taken
                                    auto [fst_tile, snd_tile] = get_bridge_nodes(start, direction);
                                    if (is_coordinate_valid(fst_tile) && is_coordinate_valid(snd_tile)) {
                                        red_values[fst_tile.first][fst_tile.second] = -100000;
                                        red_values[snd_tile.first][snd_tile.second] = -100000;
                                        // No point placing there if blue anyway so also set this really low
                                        blue_values[fst_tile.first][fst_tile.second] = -100000;
                                        blue_values[snd_tile.first][snd_tile.second] = -100000;
                                    }
                                } else {
                                    auto [fst_tile, snd_tile] = get_bridge_nodes(start, direction);
                                    if (is_coordinate_valid(fst_tile) && is_coordinate_valid(snd_tile)) {
                                        red_values[fst_tile.first][fst_tile.second] = 100000;
                                        red_values[snd_tile.first][snd_tile.second] = 100000;
                                    }
                                }
                            }
                        }

                        if (are_both_blue(start, bridge_move) ||
                            (is_tile_blue(start) && bridge_move.second == 11) ||
                            (is_tile_blue(start) && bridge_move.second == -1)) {
                            if (!check_inbetweens(start, direction, "B")) {
                                // A bridge exists
                                // Only take that if is part of a path that can be completed with bridges
                                if (!can_complete_path_with_bridges("B")) {
                                    // Mark the inbetween nodes with a low score so they aren't taken
                                    auto [fst_tile, snd_tile] = get_bridge_nodes(start, direction);
                                    if (is_coordinate_valid(fst_tile) && is_coordinate_valid(snd_tile)) {
                                        blue_values[fst_tile.first][fst_tile.second] = -100000;
                                        blue_values[snd_tile.first][snd_tile.second] = -100000;
                                        // No point placing there if red anyway so also set this really low
                                        red_values[fst_tile.first][fst_tile.second] = -100000;
                                        red_values[snd_tile.first][snd_tile.second] = -100000;
                                    }

                                } else {
                                    auto [fst_tile, snd_tile] = get_bridge_nodes(start, direction);
                                    if (is_coordinate_valid(fst_tile) && is_coordinate_valid(snd_tile)) {
                                        blue_values[fst_tile.first][fst_tile.second] = 100000;
                                        blue_values[snd_tile.first][snd_tile.second] = 100000;
                                    }
                                }
                            }
                        }
                        if (isPathClear(start, direction)) {
                            // We have already checked the start tile is occupied
                            if (is_tile_free(bridge_move)) {
                                if (is_coordinate_valid(bridge_move)) {
                                    // Positive for both red and blue so one can block the move and the other can take it
                                    red_values[bridge_move.first][bridge_move.second] += 150;
                                    blue_values[bridge_move.first][bridge_move.second] += 100;
                                } else if (is_tile_blue(start) && is_coordinate_valid(bridge_move)) {
                                    blue_values[bridge_move.first][bridge_move.second] += 150;
                                    red_values[bridge_move.first][bridge_move.second] += 100;
                                }
                            }
                        }
                    }
                }
    }
    // std::cerr << "Finished adding bridge values" << std::endl;
}

void Heuristic::add_center_values() {
    // Tiles closer to the center are more valuable
    // std::cerr << "Adding center values" << std::endl;
    for (const auto &start: free_moves) {
        red_values[start.first][start.second] += get_centre_value(start.first, start.second);
        blue_values[start.first][start.second] += get_centre_value(start.first, start.second);
    }
    // std::cerr << "Finished adding center values" << std::endl;
}

void Heuristic::generate_values() {
    // First component is if placing there creates a bridge
    // Go through each position on the board
    // If it is occupied go to all the places a bridge could be created
    // If it can be created add 5 to the value of the node
    // Clear the red and blue
    // std::cerr << "Generating values" << std::endl;
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            red_values[i][j] = 0;
            blue_values[i][j] = 0;
        }
    }
    add_bridge_values();
    add_center_values();
    // std::cerr << "Finished generating values" << std::endl;
}

double Heuristic::get_value(const int x, const int y, const std::string &colour) const {
    if (colour == "R") {
        return red_values[x][y];
    }
    return blue_values[x][y];
}

void Heuristic::output_values() const {
    std::cerr << "RED" << std::endl;
    for (int i = 0; i < 11; ++i) {
        // Display like a hex board with spaces before for each row
        for (int j = 0; j < i; ++j) {
            std::cerr << " ";
        }
        for (int j = 0; j < 11; ++j) {
            std::cerr << red_values[i][j] << " ";
        }
        std::cerr << std::endl;
    }
    std::cerr << "BLUE" << std::endl;
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < i; ++j) {
            std::cerr << " ";
        }
        for (int j = 0; j < 11; ++j) {
            std::cerr << blue_values[i][j] << " ";
        }
        std::cerr << std::endl;
    }
}

std::set<std::pair<int, int> > Heuristic::get_all_bridge_nodes(const std::string &colour) {
    std::set<std::pair<int, int> > red_bridge_nodes;
    std::set<std::pair<int, int> > blue_bridge_nodes;
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            for (const auto &direction: bridge_directions) {
                std::pair start = {i, j};
                if (std::pair bridge_tile = {i + direction.first, j + direction.second};
                    are_both_blue(start, bridge_tile) || are_both_red(start, bridge_tile) || (
                        colour == "R" && (bridge_tile.first == 11 || bridge_tile.first == -1) && is_tile_red(start)) ||
                    (colour == "B" && (bridge_tile.second == 11 || bridge_tile.second == -1) && is_tile_blue(start))) {
                    auto [tile_one, tile_two] = get_bridge_nodes(start, direction);
                    if (are_both_blue(start, bridge_tile)) {
                        blue_bridge_nodes.insert(tile_one);
                        blue_bridge_nodes.insert(tile_two);
                    } else {
                        red_bridge_nodes.insert(tile_one);
                        red_bridge_nodes.insert(tile_two);
                    }
                }
            }
        }
    }
    if (colour == "R") {
        return red_bridge_nodes;
    }
    return blue_bridge_nodes;
}

std::vector<std::pair<int, int>> Heuristic::get_neighbours(const std::pair<int, int> &tile, int board_size) {
    std::vector<std::pair<int, int>> neighbours;
    std::vector<std::pair<int, int>> deltas = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {1, -1}};
    for (const auto &[fst, snd]: bridge_directions) {
        if (std::pair neighbour = {tile.first + fst, tile.second + snd}; neighbour.first >= 0 && neighbour.first < board_size && neighbour.second >= 0 && neighbour.second < board_size) {
            neighbours.emplace_back(neighbour);
        }
    }
    return neighbours;
}