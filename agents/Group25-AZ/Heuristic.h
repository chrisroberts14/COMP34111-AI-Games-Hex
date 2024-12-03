//
// Created by chris on 26/11/24.
//

#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <set>
#include <utility>
#include <string>

class Heuristic {
    std::set<std::pair<int, int>> red_moves;
    std::set<std::pair<int, int>> blue_moves;
    const std::string colour;
    int board_size;
    std::set<std::pair<int, int>> candidate_moves = get_candidate_moves();

public:
    Heuristic(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves, std::string colour, const int board_size = 11) :
        red_moves(std::move(red_moves)), blue_moves(std::move(blue_moves)), colour(std::move(colour)), board_size(board_size) {};

    [[nodiscard]] bool is_valid(int row, int col) const;
    [[nodiscard]] bool is_valid(const std::pair<int, int> &node) const;
    [[nodiscard]] bool is_free(int row, int col) const;
    [[nodiscard]] bool is_free(const std::pair<int, int> &node) const;
    [[nodiscard]] bool is_tile_opponents(const std::pair<int, int> &node) const;
    static std::pair<std::pair<int, int>, std::pair<int, int>> get_bridge_intermediates(const std::pair<int, int> &start_tile, const std::pair<int, int> &bridge_tile) ;
    double evaluate_move(const std::pair<int, int> &move);
    std::set<std::pair<int, int>> get_candidate_moves();
    std::set<std::pair<int, int>> get_abandoned_moves() const;
    bool does_move_create_bridge(const std::pair<int, int> &move);
    bool creates_bridge_to_edge(const std::pair<int, int> &move);
    bool creates_bridge_with_tile(const std::pair<int, int> &move);
    double reduces_distance_to_edge(const std::pair<int, int> &move);
    std::pair<int, int> get_best_move();
    void insert_red_move(const std::pair<int, int> &move);
    void insert_blue_move(const std::pair<int, int> &move);
    void output_board();
};



#endif //HEURISTIC_H
