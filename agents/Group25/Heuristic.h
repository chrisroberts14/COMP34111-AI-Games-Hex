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
  double red_values[11][11] = {0};
  double blue_values[11][11] = {0};

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

public:
  Heuristic(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves);
  bool can_complete_path_with_bridges(const std::string& colour);
  void generate_values();
  [[nodiscard]] double get_value(int x, int y, const std::string &colour) const;
  void output_values() const;
  [[nodiscard]] std::set<std::pair<int, int>> get_all_bridge_nodes(const std::string &colour);

};



#endif //HEURISTIC_H
