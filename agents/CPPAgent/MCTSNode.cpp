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

auto rng = std::default_random_engine {};

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
    Board brd = state.duplicate();
    brd.make_move(move, colour);
    MCTSNode *new_node = new MCTSNode(brd, colour, this, move);
    children.push_back(new_node);
  }
}

void MCTSNode::delete_children() {
  for (MCTSNode *child : children) {
    delete child;
  }
  children.clear();
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
  MCTSNode *best_node = best_child(0.0);
  return best_node->move;
}

double MCTSNode::simulate_from_node(std::string current_colour) {
  std::vector<std::pair<int, int>> moves_taken;

  // Check if the game has ended at this node if so there is a winning move
  // The reward here is higher than is possible for other moves which guarantees
  // that the winning move is chosen
  Board local_state = state.duplicate();
  std::vector<std::pair<int, int>> local_moves = local_state.get_moves();

  std::string winner = local_state.has_ended();
  if (winner == colour) {
    return 2;
  }

  // Shuffle the vector
  // Faster than picking a random element from the vector every iteration
  std::shuffle(local_moves.begin(), local_moves.end(), rng);
  int i = 0;
  while (true) {
    std::pair<int, int> move = local_moves[i];
    i++;
    moves_taken.push_back(move);

    local_state.make_move(move, current_colour);
    winner = local_state.has_ended();
    if (winner != "") {
      return (std::exp((long double)(DECAY_RATE * moves_taken.size())) *
              (colour == current_colour ? 1 : -1));
    }
    current_colour = opp(current_colour);
  }
}

MCTSNode *MCTSNode::best_child(float c) {
  MCTSNode *best_node = nullptr;
  double best_score = -std::numeric_limits<double>::infinity();
  double log_parent_visits = std::log(visits);

  for (MCTSNode *child : children) {
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
