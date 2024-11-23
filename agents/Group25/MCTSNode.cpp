//
// Created by chris on 20/11/24.
//

#include "MCTSNode.h"
#include "Board.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <stack>

#define DECAY_RATE (-0.05)
#include <iostream>
#include <utility>

auto rng = std::default_random_engine{};

inline std::string opp(const std::string &c) { return c == "R" ? "B" : "R"; }

std::vector<std::pair<int, int>> get_valid_moves(const std::string& state,
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

MCTSNode::MCTSNode(const Board &state, std::string  colour,
                   MCTSNode *parent, std::pair<int, int> move)
    : colour(std::move(colour)), state(state), parent(parent), visits(0), payoff_sum(0),
      move(std::move(move)), valid_moves(state.get_moves()) {}

void MCTSNode::generate_all_children_nodes() {
  for (std::pair<int, int> move : this->valid_moves) {
    // Make a new board and make the move
    Board brd = state.duplicate();
    brd.make_move(move, colour);
    auto *new_node = new MCTSNode(brd, colour, this, move);
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
  return best_node->move;
}

double distanceFromCenter(const int x, const int y) {
  constexpr double center = 5.5;
  return std::sqrt(std::pow(x - center, 2) + std::pow(y - center, 2));
}

int connectivity(Board& local_state, const int x, const int y, const std::string& colour) {
  // Example: Count adjacent cells owned by the same player
  int dirs[6][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {1, -1}};
  int count = 0;
  for (const auto& dir : dirs) {
    const int nx = x + dir[0];
    if (const int ny = y + dir[1]; nx >= 0 && nx < 11 && ny >= 0 && ny < 11 && local_state.get_state().at(nx).at(ny).getColour() == colour) {
      count++;
    }
  }
  return count;
}

double potential(Board& local_state, const int x, const int y, const std::string& colour) {
  // Potential is estimated by unoccupied neighbors and their connectivity to the player's path
  int dirs[6][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {1, -1}};
  double score = 0;
  for (const auto& dir : dirs) {
    const int nx = x + dir[0];
    if (int ny = y + dir[1]; nx >= 0 && nx < 11 && ny >= 0 && ny < 11 && local_state.get_state().at(nx).at(ny).getColour() == "0") {
      score += 1.0 + connectivity(local_state, nx, ny, colour) * 0.5;
    }
  }
  return score;
}

std::pair<int, int> MCTSNode::best_move(Board& local_state) const {
  double score = 0;
  std::pair<int, int> best_move = std::make_pair(0, 0);
  double best_score = -std::numeric_limits<double>::infinity();
  for (int x = 0; x < 11; x++) {
    for (int y = 0; y < 11; y++) {
      if (local_state.get_state()[x][y].getColour() == "0") {
        score -= distanceFromCenter(x, y);
        score += connectivity(local_state, x, y, colour) * 2;
        score += potential(local_state, x, y, colour);
        if (score > best_score) {
          best_score = score;
          best_move = std::make_pair(x, y);
        }
      }
    }
  }
  return best_move;
}

double MCTSNode::simulate_from_node(std::string current_colour) const {
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
    // std::pair<int, int> move = best_move(local_state);
    i++;
    std::cerr << "Move: " << move.first << ", " << move.second << std::endl;
    moves_taken.push_back(move);

    local_state.make_move(move, current_colour);
    winner = local_state.has_ended();
    if (!winner.empty()) {
      return (std::exp((DECAY_RATE * static_cast<double>(moves_taken.size()))) *
              (colour == current_colour ? 1 : -1));
    }
    current_colour = opp(current_colour);
  }
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
