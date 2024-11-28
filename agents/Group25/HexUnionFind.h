//
// Created by chris on 25/11/24.
//

#ifndef HEXUNIONFIND_H
#define HEXUNIONFIND_H

#include <vector>
#include <string>
#include <set>
#include "UnionFind.h"

class HexUnionFind {
int board_size;
UnionFind uf;
int top;
int bottom;
int left;
int right;
  std::set<std::pair<int, int>> red_moves;
  std::set<std::pair<int, int>> blue_moves;

[[nodiscard]] int position_to_index(int x, int y) const;
[[nodiscard]] std::vector<std::pair<int, int>> get_neighbors(int x, int y) const;

public:
  bool check_winner(const std::string& player);
  HexUnionFind(int board_size, const std::set<std::pair<int, int>>& red_moves, const std::set<std::pair<int, int>>& blue_moves);
  bool add_move(int x, int y, const std::string& player);
};

#endif //HEXUNIONFIND_H
