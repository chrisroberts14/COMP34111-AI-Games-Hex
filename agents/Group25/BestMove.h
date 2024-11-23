//
// Created by chris on 23/11/2024.
//

#ifndef BESTMOVE_H
#define BESTMOVE_H
#include <vector>
#include "Tile.h"


class BestMove {
    const int move_count;
    const std::vector<std::vector<Tile>> board;
    const std::string colour;
    double potentials[6] = {};
    int neighbour_count[6] = {};
    double potential[11][11][4] = {};
    double bridge[11][11][4] = {};
    bool update[11][11] = {};

    [[nodiscard]] static bool is_top_right_region(int x, int y) ;
    [[nodiscard]] static bool is_bottom_left_region(int x, int y) ;
    [[nodiscard]] static bool is_bottom_right_region(int x, int y) ;
    static bool is_on_top_left_border(int x, int y);
    [[nodiscard]] static bool is_on_top_right_border(int x, int y) ;
    [[nodiscard]] static bool is_on_bottom_left_border(int x, int y) ;
    [[nodiscard]] static bool is_on_bottom_right_border(int x, int y) ;
    [[nodiscard]] bool is_north_east_bridge_position(int x, int y) const;
    [[nodiscard]] bool is_north_west_bridge_position(int x, int y) const;
    [[nodiscard]] bool is_south_east_bridge_position(int x, int y) const;
    [[nodiscard]] bool is_south_west_bridge_position(int x, int y) const;
    [[nodiscard]] bool is_north_east_bridge_with_clear_path(int x, int y) const;
    [[nodiscard]] bool is_north_west_bridge_with_clear_path(int x, int y) const;
    [[nodiscard]] bool is_south_east_bridge_with_clear_path(int x, int y) const;
    [[nodiscard]] bool is_south_west_bridge_with_clear_path(int x, int y) const;
    static int sign(int x);
    [[nodiscard]] std::pair<int, int> get_direction_sums() const;
    static bool is_top_left_region(int x, int y);
    double calculate_move_potential(int x, int y, int row_sum, int col_sum);
    [[nodiscard]] int can_connect_far_border(int x, int y, const std::string &local_col) const;
    [[nodiscard]] std::string get_tile_value(int x, int y) const;
    void calculate_potential();
    int set_potential(int x, int y, int direction, const std::string& col);
    [[nodiscard]] double get_potential_value(int x, int y, int direction) const;
    void set_update(int x, int y);
public:
    BestMove(const std::vector<std::vector<Tile>> &board, const std::string &colour, const int &move_count);
    std::pair<int, int> get_best_move();

};



#endif //BESTMOVE_H
