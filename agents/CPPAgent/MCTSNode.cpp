//
// Created by chris on 20/11/24.
//

#include "MCTSNode.h"
#include "Board.h"
#include "Tile.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <set>
#include <sstream>
#include <stack>

#define DECAY_RATE -0.05
#include <iostream>

inline std::string opp(const std::string &c) { return c == "R" ? "B" : "R"; }

std::vector<std::pair<int, int>> get_valid_moves(const std::string state,
                                                 const int boardSize) {
  std::vector<std::string> lines;
  std::stringstream ss(state);
  std::string line;
  while (std::getline(ss, line, ',')) {
    lines.push_back(line);
  }

  std::vector<std::pair<int, int>> choices;
  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      if (lines[i][j] == '0') {
        choices.emplace_back(i, j);
      }
    }
  }
  return choices;
}

MCTSNode::MCTSNode(const Board &state, const std::string colour,
                   MCTSNode *parent, std::pair<int, int> move)
    : colour(colour), state(state), parent(parent), visits(0), payoff_sum(0),
      move(move), valid_moves(state.get_moves()) {}

void MCTSNode::generate_all_children_nodes() {
  for (std::pair<int, int> move : this->valid_moves) {
    // Make a new board and make the move
    Board brd(this->state);
    brd.make_move(move, this->colour);
    MCTSNode *new_node = new MCTSNode(brd, this->colour, this, move);
    this->children.push_back(new_node);
  }
}

void MCTSNode::delete_children() {
  for (MCTSNode *child : this->children) {
    delete child;
  }
  this->children.clear();
}

void MCTSNode::backpropagate(double result, int visits) {
  MCTSNode *node = this;
  while (node != nullptr) {
    node->visits += visits;
    node->payoff_sum += result;
    node = node->parent;
  }
}

std::pair<int, int> MCTSNode::get_best_move() {
  MCTSNode *best_node = this->best_child(0.0);
  return best_node->move;
}

std::string MCTSNode::has_ended() {
  std::vector<std::vector<Tile>> &tiles = this->state.get_state();
  int size = 11;
  std::set<std::pair<int, int>> visited;

  std::vector<Tile> red_tiles;
  std::vector<Tile> blue_tiles;

  for (int idx = 0; idx < size; ++idx) {
    if (tiles[0][idx].getColour() == "R") {
      red_tiles.push_back(tiles[0][idx]);
    }
    if (tiles[idx][0].getColour() == "B") {
      blue_tiles.push_back(tiles[idx][0]);
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

std::string MCTSNode::DFSColour(int x, int y, const std::string &colour,
                                std::set<std::pair<int, int>> &visited) {
  int size = 11;
  std::vector<std::vector<Tile>> &tiles = this->state.get_state();

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
            tiles[x_n][y_n].getColour() == colour) {
          visited.insert({x_n, y_n});
          stack.push({x_n, y_n});
        }
      }
    }
  }
  return "";
}

double MCTSNode::simulate_from_node(std::string current_colour) {
  std::vector<std::pair<int, int>> moves_taken;

  // Check if the game has ended at this node if so there is a winning move
  // The reward here is higher than is possible for other moves which guarantees
  // that the winning move is chosen
  std::string winner = this->has_ended();
  if (winner == this->colour) {
    return 2;
  }

  std::default_random_engine rng(time(0));

  // Shuffle the vector
  // Faster than picking a random element from the vector every iteration
  std::shuffle(valid_moves.begin(), valid_moves.end(), rng);
  int i = 0;
  while (true) {
    std::pair<int, int> move = valid_moves[i];
    i++;
    moves_taken.push_back(move);

    this->state.make_move(move, current_colour);
    winner = this->has_ended();
    if (winner != "") {
      for (auto &move : moves_taken) {
        this->state.make_move(move, "0");
      }
      return (std::exp((long double)(DECAY_RATE * moves_taken.size())) *
              (this->colour == current_colour ? 1 : -1));
    }
    current_colour = opp(current_colour);
  }
}

MCTSNode *MCTSNode::best_child(float c) {
  MCTSNode *best_node = nullptr;
  double best_score = -std::numeric_limits<double>::infinity();
  double log_parent_visits = std::log(this->visits);

  for (MCTSNode *child : this->children) {
    // If the child has not been visited, return it immediately
    if (child->visits == 0) {
      return child;
    }

    double score = child->payoff_sum / child->visits +
                   c * std::sqrt(2 * log_parent_visits / child->visits);
    if (score > best_score) {
      best_score = score;
      best_node = child;
    }
  }
  return best_node;
}

MCTSNode *MCTSNode::get_child(int index) { return this->children[index]; }

int MCTSNode::get_children_size() { return this->children.size(); }
