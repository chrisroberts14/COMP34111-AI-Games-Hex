//
// Created by chris on 26/11/24.
//

#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <set>
#include <utility>
#include <string>
#include <vector>

class Heuristic {
  std::set<std::pair<int, int>> red_moves;
  std::set<std::pair<int, int>> blue_moves;
  std::set<std::pair<int, int>> free_moves = get_free_tiles();
  double red_values[11][11] = {1};
  double blue_values[11][11] = {1};

  bool isPathClear(std::pair<int, int> start, std::pair<int, int> end);
  bool check_inbetweens(std::pair<int, int> start, std::pair<int, int> end, const std::string& colour);
  static double get_centre_value(int x, int y) ;
    bool are_both_red(const std::pair<int, int> &start, const std::pair<int, int> &end);
  bool are_both_blue(const std::pair<int, int> &start, const std::pair<int, int> &end);
  bool is_tile_free(const std::pair<int, int> &tile);
  bool is_tile_red(const std::pair<int, int> &tile);
  bool is_tile_blue(const std::pair<int, int> &tile);
  static bool is_coordinate_valid(const std::pair<int, int> &coord);
  static std::pair<std::pair<int, int>, std::pair<int, int>> get_bridge_nodes(const std::pair<int, int> &start, const std::pair<int, int> &direction);
  static std::vector<std::pair<int, int>> get_neighbours(const std::pair<int, int> &tile, int board_size = 11);

  [[nodiscard]] std::vector<std::set<std::pair<int, int>>> get_red_groups() const;
  [[nodiscard]] std::vector<std::set<std::pair<int, int>>> get_blue_groups() const;

  void add_defensive_moves();
    void add_bridge_values();
  void add_center_values();
  void add_connectivity_values();
  std::set<std::pair<int, int>> get_free_tiles();

public:
  Heuristic(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves);
  bool can_complete_path_with_bridges(const std::string& colour);
  void generate_values();
  [[nodiscard]] double get_value(int x, int y, const std::string &colour) const;
  void output_values() const;
  [[nodiscard]] std::set<std::pair<int, int>> get_all_bridge_nodes(const std::string &colour);

};



#endif //HEURISTIC_H
