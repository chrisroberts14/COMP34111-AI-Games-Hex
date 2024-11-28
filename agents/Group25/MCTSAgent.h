//
// Created by chris on 20/11/24.
//

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

#include <iostream>
#include <string>
#include "MCTSNode.h"

class MCTSAgent {
  std::string colour;
  int turn;
  int boardSize;

public:
  MCTSAgent(std::string agentColour, int gameBoardSize)
      : colour(std::move(agentColour)), turn(0), boardSize(gameBoardSize) {};

  static std::string getMessage();

  static void sendMessage(const std::string &msg);
  void run();
  bool interpretMessage(const std::string &s);
  void makeMove(const std::string &board) const;
};

#endif // MCTSAGENT_H
