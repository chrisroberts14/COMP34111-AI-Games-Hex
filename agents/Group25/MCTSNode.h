//
// Created by chris on 20/11/24.
//

#ifndef MCTSNODE_H
#define MCTSNODE_H

#include <random>
#include <set>

class MCTSNode {
  std::string colour_to_play;
  MCTSNode *parent;
  std::vector<MCTSNode *> children;
  int visits;
  double payoff_sum;
  std::pair<int, int> move;
  std::set<std::pair<int, int>> blue_moves;
  std::set<std::pair<int, int>> red_moves;
  std::set<std::pair<int, int>> open_moves;


public:
  MCTSNode(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves, std::set<std::pair<int, int>> open_moves, std::string colour, MCTSNode *parent,
           std::pair<int, int> move);

  void backpropagate(double result);
  MCTSNode *best_child(float c);
  [[nodiscard]] double simulate_from_node(const std::string& positive_colour);
  std::pair<int, int> generate_all_children_nodes();
  std::pair<int, int> get_best_move();
  [[nodiscard]] MCTSNode *get_child(int index) const;
  [[nodiscard]] unsigned long get_children_size() const;
  void delete_children();
  void setPayoffSum(double value);
};

#endif // MCTSNODE_H
