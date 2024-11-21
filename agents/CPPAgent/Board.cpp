//
// Created by chris on 20/11/24.
//

#include "Board.h"
#include "Tile.h"
#include <set>
#include <sstream>
#include <stack>

Board::Board(std::vector<std::vector<Tile>> state_vec, const int &size,
             std::string winner)
    : state(state_vec), size(size), winner(winner) {}

std::vector<std::vector<Tile>> &Board::get_state() { return this->state; }

void Board::make_move(std::pair<int, int> &move, std::string colour) {
  this->state.at(move.first).at(move.second).setColour(colour);
}

std::vector<std::pair<int, int>> Board::get_moves() const {
  std::vector<std::pair<int, int>> choices;
  for (int i = 0; i < this->size; ++i) {
    for (int j = 0; j < this->size; ++j) {
      if (this->state[i][j].getColour() == "0") {
        choices.emplace_back(i, j);
      }
    }
  }
  return choices;
}

Board Board::duplicate() {
  return Board(this->state, this->size, this->winner);
}

std::string Board::has_ended() {
  int size = 11;
  std::set<std::pair<int, int>> visited;

  std::vector<Tile> red_tiles;
  std::vector<Tile> blue_tiles;

  for (int idx = 0; idx < size; ++idx) {
    if (state[0][idx].getColour() == "R") {
      red_tiles.push_back(state[0][idx]);
    }
    if (state[idx][0].getColour() == "B") {
      blue_tiles.push_back(state[idx][0]);
    }
  }

  // Check red tiles
  for (const auto &tile : red_tiles) {
    if (visited.insert({tile.getX(), tile.getY()}).second) {
      if (DFSColour(0, tile.getY(), "R", visited) == "R") {
        return "R";
      }
    }
  }

  // Check blue tiles
  for (const auto &tile : blue_tiles) {
    if (visited.insert({tile.getX(), tile.getY()}).second) {
      if (DFSColour(tile.getX(), 0, "B", visited) == "B") {
        return "B";
      }
    }
  }

  return "";
}

std::string Board::DFSColour(int x, int y, const std::string &colour,
                             std::set<std::pair<int, int>> &visited) {
  int size = 11;

  std::stack<std::pair<int, int>> stack;
  stack.push({x, y});
  visited.insert({x, y});

  while (!stack.empty()) {
    std::pair<int, int> coordinate = stack.top();
    stack.pop();

    // Win conditions
    if (colour == "R" && coordinate.first == size - 1) {
      return "R";
    }
    if (colour == "B" && coordinate.second == size - 1) {
      return "B";
    }

    // Visit neighbours
    for (int i = 0; i < Tile::NEIGHBOUR_COUNT; ++i) {
      int x_n = coordinate.first + Tile::I_DISPLACEMENTS[i];
      int y_n = coordinate.second + Tile::J_DISPLACEMENTS[i];

      if (x_n >= 0 && x_n < size && y_n >= 0 && y_n < size) {
        if (visited.find({x_n, y_n}) == visited.end() &&
            state[x_n][y_n].getColour() == colour) {
          visited.insert({x_n, y_n});
          stack.push({x_n, y_n});
        }
      }
    }
  }
  return "";
}
