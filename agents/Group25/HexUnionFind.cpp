//
// Created by chris on 25/11/24.
//

#include "HexUnionFind.h"

#include "HexUnionFind.h"
#include <stdexcept>
#include <set>
#include <iostream>

HexUnionFind::HexUnionFind(int board_size, std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves)
    : board_size(board_size), uf(board_size * board_size + 4), red_moves(std::move(red_moves)), blue_moves(std::move(blue_moves)) {
  int total_nodes = board_size * board_size + 4;
  bottom = total_nodes - 1;
  top = total_nodes - 2;
  right = total_nodes - 3;
  left = total_nodes - 4;
}

int HexUnionFind::position_to_index(int x, int y) {
  return x * board_size + y;
}

std::vector<std::pair<int, int>> HexUnionFind::get_neighbors(int x, int y) {
  static const std::vector<std::pair<int, int>> directions = {
    {-1, 0}, {-1, 1}, {0, 1}, {1, 0}, {1, -1}, {0, -1}
  };

  std::vector<std::pair<int, int>> neighbors;
  for (auto [dr, dc] : directions) {
    int new_x = x + dr, new_y = y + dc;
    if (new_x >= 0 && new_x < board_size && new_y >= 0 && new_y < board_size) {
      neighbors.emplace_back(new_x, new_y);
    }
  }
  return neighbors;
}

bool HexUnionFind::add_move(int x, int y, std::string player) {
  int index = position_to_index(x, y);
	if (player == "R") {
                  red_moves.insert({x, y});
                  }
                  else {
                    blue_moves.insert({x, y});
                    }

  if (player == "R") {
    if (x == 0) uf.union_sets(index, top);
    if (x == board_size - 1) uf.union_sets(index, bottom);
  } else if (player == "B") {
    if (y == 0) uf.union_sets(index, left);
    if (y == board_size - 1) uf.union_sets(index, right);
  }

  auto neighbors = get_neighbors(x, y);
  for (auto [nr, nc] : neighbors) {
    int neighborIndex = position_to_index(nr, nc);
     // Only union sets if the neighbor is the same player
    if (player == "R" && red_moves.count({nr, nc})) {
      uf.union_sets(index, neighborIndex);
    } else if (player == "B" && blue_moves.count({nr, nc})) {
      uf.union_sets(index, neighborIndex);
    }
  }
  return check_winner(player);
}

bool HexUnionFind::check_winner(std::string player) {
  if (player == "R") return uf.connected(top, bottom);
  if (player == "B") return uf.connected(left, right);
  return false;
}

void HexUnionFind::output_board() {
  for (int i = 0; i < board_size; ++i) {
    // output spaces for each row
    for (int j = 0; j < i; ++j) {
      std::cerr << " ";
    }
    for (int j = 0; j < board_size; ++j) {
      if (red_moves.count({i, j})) {
        std::cerr << "R ";
      } else if (blue_moves.count({i, j})) {
        std::cerr << "B ";
      } else {
        std::cerr << ". ";
        }
    }
    std::cerr << std::endl;
  }
}

void HexUnionFind::output_parents() {
  uf.output_parents();
}

