//
// Created by chris on 20/11/24.
//

#ifndef BOARD_H
#define BOARD_H

#include "Tile.h"
#include <set>
#include <string>
#include <vector>

class Board {
private:
  std::vector<std::vector<Tile>> state;
  int size;
  std::string winner;

public:
  explicit Board(std::vector<std::vector<Tile>> state_vec, const int &size = 11,
        std::string winner = "");
  void make_move(const std::pair<int, int> &move, const std::string &colour);
  [[nodiscard]] std::vector<std::pair<int, int>> get_moves() const;
  std::vector<std::vector<Tile>> &get_state();
  [[nodiscard]] Board duplicate() const;
  [[nodiscard]] std::string has_ended() const;
  std::string DFSColour(int x, int y, const std::string &colour,
                        std::set<std::pair<int, int>> &visited) const;
};

#endif // BOARD_H
