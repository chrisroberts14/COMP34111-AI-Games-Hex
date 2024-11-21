//
// Created by chris on 20/11/24.
//

#ifndef MCTSNODE_H
#define MCTSNODE_H

#include "Board.h"
#include <chrono>
#include <cmath>
#include <random>
#include <set>
#include <stack>

class MCTSNode {
private:
  std::string colour;
  Board state;
  MCTSNode *parent;
  std::vector<MCTSNode *> children;
  int visits;
  double payoff_sum;
  std::pair<int, int> move;
  std::vector<std::pair<int, int>> valid_moves;

public:
  MCTSNode(const Board &state, const std::string colour, MCTSNode *parent,
           std::pair<int, int> move);

  void backpropagate(double result, int visits=1);
  std::string has_ended();
  std::string DFSColour(int i, int j, const std::string &colour,
                        std::set<std::pair<int, int>> &visited);
  MCTSNode *best_child(float c);
  double simulate_from_node(std::string current_colour);
  void generate_all_children_nodes();
  std::pair<int, int> get_best_move();
  MCTSNode *get_child(int index);
  int get_children_size();
  void delete_children();
};

#endif // MCTSNODE_H
