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
#include "Heuristic.h"
#include <iostream>
#include <utility>
#include <stack>

std::default_random_engine rng(time(nullptr));

inline std::string opp(const std::string &c) { return c == "R" ? "B" : "R"; }

MCTSNode::MCTSNode(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves, std::set<std::pair<int, int>> open_moves, std::string  colour,
                   MCTSNode *parent, std::pair<int, int> move)
    : colour_to_play(std::move(colour)), parent(parent), visits(0), payoff_sum(0),
      move(std::move(move)), blue_moves(std::move(blue_moves)), red_moves(std::move(red_moves)), open_moves(std::move(open_moves)) {}

void MCTSNode::setPayoffSum(const double value) {
  payoff_sum = value;
}

std::pair<int, int> MCTSNode::generate_all_children_nodes() {
  std::cerr << "Generating all children nodes" << std::endl;
  Heuristic heuristic(red_moves, blue_moves);
  std::cerr << "Heuristic created" << std::endl;
  heuristic.generate_values();
  std::cerr << "Heuristic generated" << std::endl;

  if (heuristic.can_complete_path_with_bridges(colour_to_play)) {
    std::cerr << "Board is winnable" << std::endl;
    // We can win the board we just need to connect the bridges
    // Find all bridges and connect them
    const std::set<std::pair<int, int>> bridge_nodes = heuristic.get_all_bridge_nodes(colour_to_play);
    std::cerr << "Got all bridge nodes" << std::endl;
    // Check if the opponent is trying to stop a bridge
    for (const std::pair<int, int> &move : open_moves) {
      std::cerr << "Checking move: " << move.first << ", " << move.second << std::endl;
      if (const double heuristic_value = heuristic.get_value(move.first, move.second, colour_to_play); heuristic_value > 95000) {
        std::cerr << "Heuristic short circuit to connect bridge" << std::endl;
        return move;
      }
    }
    // If the opponent is not trying to stop a bridge we can just connect any bridge
    std::pair<int, int> move = *bridge_nodes.begin();
    std::cerr << "Returning first bridge node: " << move.first << ", " << move.second << std::endl;
    return move;
  }

  for (const std::pair<int, int> &move : open_moves) {
    std::set<std::pair<int, int>> new_open_moves = open_moves;
    new_open_moves.erase(move);

    std::set<std::pair<int, int>> new_red_moves = red_moves;
    std::set<std::pair<int, int>> new_blue_moves = blue_moves;

    if (colour_to_play == "R") {
      new_red_moves.insert(move);
    } else if (colour_to_play == "B") {
      new_blue_moves.insert(move);
    }
    // Only add edge nodes if there is a connection already to them
    auto *new_node = new MCTSNode(new_red_moves, new_blue_moves, new_open_moves, opp(colour_to_play), this, move);
    // Test heuristic on new node

    if (Heuristic heuristic_new(new_red_moves, new_blue_moves); heuristic_new.can_complete_path_with_bridges(colour_to_play)) {
      // Placing here results in a winnable board
      std::cerr << "Heuristic short circuit to winnable board" << std::endl;
      return move;
    }
    const double heuristic_value = heuristic.get_value(move.first, move.second, colour_to_play);
    if (heuristic_value > 95000) {
      std::cerr << "Heuristic short circuit to connect bridge" << std::endl;
      return move;
    }
    if (heuristic_value < 0) {
      // If the heuristic value is low we don't want to add the node
      // This introduces a bug where if the none of the nodes have a heuristic value above 0
      // none are added, but I doubt this can actually happen
      continue;
    }
    new_node->setPayoffSum(heuristic_value);
    children.push_back(new_node);
  }
    return {-1, -1};
}

void MCTSNode::delete_children() {
  for (const MCTSNode *child : children) {
    delete child;
  }
  children.clear();
}

void MCTSNode::backpropagate(const double result) {
  auto node = this;
  while (node != nullptr) {
    node->visits++;
    node->payoff_sum += result;
    node = node->parent;
  }
}

std::pair<int, int> MCTSNode::get_best_move() {
  MCTSNode *best_node = best_child(0.0);
  std::cerr << "Best node visits: " << best_node->visits << std::endl;
  std::cerr << "Best node payoff sum: " << best_node->payoff_sum << std::endl;
  std::cerr << "Best node move: " << best_node->move.first << " " << best_node->move.second << std::endl;
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

double MCTSNode::simulate_from_node(const std::string& positive_colour) {
  // Copy the colour to play so we don't edit the class
  std::string current_colour = colour_to_play;
  // We want a positive reward if the opposite of colour to play wins
  HexUnionFind playout(11, red_moves, blue_moves);
  // Add all current moves
  for (const auto &[fst, snd] : blue_moves) {
    if (playout.add_move(fst, snd, "B") && positive_colour == "B") {
      return 2.0;
    }
  }
  for (const auto &[fst, snd] : red_moves) {
    if (playout.add_move(fst, snd, "R") && positive_colour == "R") {
      return 2.0;
    };
  }
  double move_count = 0;
  std::vector open_moves_vec(open_moves.begin(), open_moves.end());
  std::shuffle(open_moves_vec.begin(), open_moves_vec.end(), rng);
  for (const auto &[fst, snd] : open_moves_vec) {
    move_count++;
    if (playout.add_move(fst, snd, current_colour)) {
      return std::exp(-0.05 * move_count) * (positive_colour == current_colour ? 1 : -1);
    }
    current_colour = opp(current_colour);
  }
  return 0;
}

MCTSNode *MCTSNode::best_child(const float c) {
  MCTSNode *best_node = nullptr;
  double best_score = -std::numeric_limits<double>::infinity();
  const double log_parent_visits = std::log(visits);
  // Shuffle the children
  std::shuffle(children.begin(), children.end(), rng);

  for (MCTSNode *child : children) {
    // If the child has not been visited, return it immediately
    if (child->visits == 0 && c != 0.0) {
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
