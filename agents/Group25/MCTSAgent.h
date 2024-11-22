//
// Created by chris on 20/11/24.
//

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

#include <string>
#include "MCTSNode.h"

class MCTSAgent {
  std::string colour;
  int turn;
  int boardSize;

public:
  MCTSAgent(std::string agentColour, int gameBoardSize);

  static std::string getMessage();

  static void sendMessage(const std::string &msg);
  void run();
  bool interpretMessage(const std::string &s);
  void makeMove(const std::string &board) const;
  void multi_thread_move(MCTSNode &root) const;
  void single_thread_move(MCTSNode &root) const;
};

#endif // MCTSAGENT_H
