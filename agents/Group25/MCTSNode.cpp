//
// Created by chris on 20/11/24.
//

#include "MCTSNode.h"
#include "HexUnionFind.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>

#define DECAY_RATE (-0.05)
#include <iostream>
#include <utility>
#include <stack>

std::default_random_engine rng(time(nullptr));

inline std::string opp(const std::string &c) { return c == "R" ? "B" : "R"; }

MCTSNode::MCTSNode(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves, std::set<std::pair<int, int>> open_moves, std::string  colour,
                   MCTSNode *parent, std::pair<int, int> move)
    : colour(std::move(colour)), parent(parent), visits(0), payoff_sum(0),
      move(std::move(move)), blue_moves(std::move(blue_moves)), red_moves(std::move(red_moves)), open_moves(std::move(open_moves)) {}

void MCTSNode::generate_all_children_nodes() {
  for (const std::pair<int, int> &move : open_moves) {
    std::set<std::pair<int, int>> new_open_moves = open_moves;
    new_open_moves.erase(move);

    std::set<std::pair<int, int>> new_red_moves = red_moves;
    std::set<std::pair<int, int>> new_blue_moves = blue_moves;

    if (colour == "R") {
      new_red_moves.insert(move);
    } else if (colour == "B") {
      new_blue_moves.insert(move);
    }

    auto *new_node = new MCTSNode(new_red_moves, new_blue_moves, new_open_moves, opp(colour), this, move);
    children.push_back(new_node);
  }
}

void MCTSNode::delete_children() {
  for (const MCTSNode *child : children) {
    delete child;
  }
  children.clear();
}

void MCTSNode::backpropagate(const double result, const int visits) {
  auto node = this;
  while (node != nullptr) {
    node->visits += visits;
    node->payoff_sum += result;
    node = node->parent;
  }
}

std::pair<int, int> MCTSNode::get_best_move() {
  MCTSNode *best_node = best_child(0.0);
  std::cerr << "Best node visits: " << best_node->visits << std::endl;
  std::cerr << "Best node payoff sum: " << best_node->payoff_sum << std::endl;
  return best_node->move;
}

void output_board(const std::set<std::pair<int, int>> &red_moves, const std::set<std::pair<int, int>> &blue_moves) {
  std::vector<std::vector<std::string>> board(11, std::vector<std::string>(11, "."));
  for (const auto &[fst, snd] : red_moves) {
    board[fst][snd] = "R";
  }
  for (const auto &[fst, snd] : blue_moves) {
    board[fst][snd] = "B";
  }
  for (int i = 0; i < 11; ++i) {
    for (int j = 0; j < 11; ++j) {
      std::cerr << board[i][j];
    }
    std::cerr << std::endl;
  }
}

double MCTSNode::simulate_from_node() {
  std::string current_colour = colour;
  // Get all possible moves
  // Shuffle them
  // Split into blue and red moves
  HexUnionFind playout(11, red_moves, blue_moves);
  // Add all current moves
  for (const auto &[fst, snd] : blue_moves) {
    if (playout.add_move(fst, snd, "B") && current_colour == "B") {
      return 1;
    }
  }
  for (const auto &[fst, snd] : red_moves) {
    if (playout.add_move(fst, snd, "R") && current_colour == "R") {
      return 1;
    };
  }
  double move_count = 0;
  std::vector open_moves_vec(open_moves.begin(), open_moves.end());
  std::shuffle(open_moves_vec.begin(), open_moves_vec.end(), rng);
  for (const auto &[fst, snd] : open_moves_vec) {
    move_count++;
    if (playout.add_move(fst, snd, current_colour)) {
      return std::exp(-0.05 * move_count) * (colour == current_colour ? -1 : 1);
    }
    current_colour = opp(current_colour);
  }
  return 0;
}

MCTSNode *MCTSNode::best_child(const float c) {
  MCTSNode *best_node = nullptr;
  double best_score = -std::numeric_limits<double>::infinity();
  const double log_parent_visits = std::log(visits);

  for (MCTSNode *child : children) {
    // If the child has not been visited, return it immediately
    if (child->visits == 0) {
      return child;
    }

    const double score = child->payoff_sum / child->visits +
                         c * std::sqrt(2 * log_parent_visits / child->visits);
    if (score > best_score) {
      best_score = score;
      best_node = child;
    }
  }
  return best_node;
}

MCTSNode *MCTSNode::get_child(const int index) const { return this->children[index]; }

unsigned long MCTSNode::get_children_size() const { return this->children.size(); }
