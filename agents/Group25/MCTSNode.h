//
// Created by chris on 20/11/24.
//

#ifndef MCTSNODE_H
#define MCTSNODE_H

#include "Board.h"
#include <random>
#include <stack>

class MCTSNode {
  std::string colour;
  Board state;
  MCTSNode *parent;
  std::vector<MCTSNode *> children;
  int visits;
  double payoff_sum;
  std::pair<int, int> move;
  std::vector<std::pair<int, int>> valid_moves;

public:
  MCTSNode(const Board &state, std::string colour, MCTSNode *parent,
           std::pair<int, int> move);

  void backpropagate(double result, int visits = 1);
  MCTSNode *best_child(float c);
  [[nodiscard]] double simulate_from_node(std::string current_colour) const;
  void generate_all_children_nodes();
  std::pair<int, int> get_best_move();
  [[nodiscard]] MCTSNode *get_child(int index) const;
  [[nodiscard]] unsigned long get_children_size() const;
  void delete_children();
};

#endif // MCTSNODE_H
