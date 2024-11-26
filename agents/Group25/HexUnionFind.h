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

int position_to_index(int x, int y);
std::vector<std::pair<int, int>> get_neighbors(int x, int y);
  bool check_winner(std::string player);

public:
  explicit HexUnionFind(int board_size, std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves);
  bool add_move(int x, int y, std::string player);

  void output_board();
  void output_parents();

};



#endif //HEXUNIONFIND_H
