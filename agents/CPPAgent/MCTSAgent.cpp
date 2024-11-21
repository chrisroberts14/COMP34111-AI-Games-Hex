//
// Created by chris on 20/11/24.
//

#include "MCTSAgent.h"
#include "Board.h"
#include "MCTSNode.h"
#include <atomic>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#define DECAY_RATE -0.05
#define EXPLORATION_CONSTANT 1.41

std::vector<std::vector<Tile>>
convert_board_string(const std::string &board_string, const int boardSize) {
  std::vector<std::vector<Tile>> board;
  std::vector<std::string> lines;
  std::stringstream ss(board_string);
  std::string line;
  while (std::getline(ss, line, ',')) {
    lines.push_back(line);
  }

  for (int i = 0; i < boardSize; ++i) {
    std::vector<Tile> row;
    for (int j = 0; j < boardSize; ++j) {
      Tile tile(i, j, std::string(1, lines[i][j]));
      row.push_back(tile);
    }
    board.push_back(row);
  }
  return board;
}

MCTSAgent::MCTSAgent(const std::string &agentColour, int gameBoardSize)
    : colour(agentColour), turn(0), boardSize(gameBoardSize) {}

inline std::string MCTSAgent::getMessage() {
  std::string message;
  std::getline(std::cin, message);
  return message;
}

inline void MCTSAgent::sendMessage(const std::string &msg) {
  std::cout << msg << std::endl;
  std::cout.flush();
}

void MCTSAgent::run() {
  while (true) {
    try {
      std::string msg = getMessage();
      if (!interpretMessage(msg)) {
        return;
      }
    } catch (const std::exception &e) {
      return;
    }
  }
}

bool MCTSAgent::interpretMessage(const std::string &s) {
  std::string trimmed = s;
  trimmed.erase(trimmed.find_last_not_of(" \n\r\t") + 1);

  std::vector<std::string> msg;
  std::stringstream ss(trimmed);
  std::string item;
  while (std::getline(ss, item, ';')) {
    msg.push_back(item);
  }

  std::string board = msg[2];
  turn = std::stoi(msg[3]);
  if (msg[0] == "START") {
    if (colour == "R") {
        makeMove(board);
    }
  } else if (msg[0] == "CHANGE") {
    makeMove(board);
  } else if (msg[0] == "SWAP") {
    colour = colour == "R" ? "B" : "R";
    makeMove(board);
  } else {
    return false;
  }

  return true;
}

void MCTSAgent::makeMove(const std::string &board) {
  std::cerr << turn << std::endl;
  if (turn == 2) {
    sendMessage("-1,-1");
    return;
  }
  std::vector<std::vector<Tile>> boardState =
      convert_board_string(board, boardSize);

  Board brd(boardState, boardSize, "");
  MCTSNode root(brd, colour, nullptr, {-1, -1});
  root.generate_all_children_nodes();
  std::chrono::steady_clock::time_point end_time =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(10000);
  while (std::chrono::steady_clock::now() < end_time) {
    MCTSNode* node = root.best_child(EXPLORATION_CONSTANT);
    double result = node->simulate_from_node(colour);
    node->backpropagate(result);
  }

  std::pair<int, int> best_move = root.get_best_move();
  sendMessage(std::to_string(best_move.first) + "," +
              std::to_string(best_move.second));
}
