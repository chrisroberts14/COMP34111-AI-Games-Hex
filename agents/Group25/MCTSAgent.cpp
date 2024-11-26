//
// Created by chris on 20/11/24.
//

#include "MCTSAgent.h"
#include "MCTSNode.h"
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <set>

#define DECAY_RATE (-0.05)
#define EXPLORATION_CONSTANT 1.41
#define THREAD_COUNT 32
#define TIME_LIMIT 295

double time_left_ms = TIME_LIMIT * 1000;

std::tuple<std::set<std::pair<int, int>>, std::set<std::pair<int, int>>, std::set<std::pair<int, int>>> convert_board_string(const std::string &board_string, const int boardSize) {
  std::vector<std::string> lines;
  std::stringstream ss(board_string);
  std::string line;
  while (std::getline(ss, line, ',')) {
    lines.push_back(line);
  }

  std::set<std::pair<int, int>> red_moves;
  std::set<std::pair<int, int>> blue_moves;
  std::set<std::pair<int, int>> open_moves;

  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      if (std::string(1, lines[i][j]) == "R") {
        red_moves.insert({i, j});
      } else if (std::string(1, lines[i][j]) == "B") {
        blue_moves.insert({i, j});
      } else {
        open_moves.insert({i, j});
      }
    }
  }
  return {red_moves, blue_moves, open_moves};
}

MCTSAgent::MCTSAgent(std::string agentColour, int gameBoardSize)
  : colour(std::move(agentColour)), turn(0), boardSize(gameBoardSize) {}

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
      if (std::string msg = getMessage(); !interpretMessage(msg)) {
        std::cerr << "PROBLEM INTERPRETING MESSAGE" << std::endl;
        return;
      }
    } catch (const std::exception &_) {
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

  // msg[1] is the previous move in the form "x,y"
  // Convert this to a pair
  /*std::pair prev_move = {-2, -2};
  if (!msg[1].empty()) {
    std::stringstream ss2(msg[1]);
    std::string item2;
    std::getline(ss2, item2, ',');
    prev_move.first = std::stoi(item2);
    std::getline(ss2, item2, ',');
    prev_move.second = std::stoi(item2);
  }*/
  const std::string board = msg[2];
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


void MCTSAgent::makeMove(const std::string &board) const {
  const long move_start_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();

  auto [red_moves, blue_moves, open_moves] = convert_board_string(board, boardSize);

  if (turn == 1) {
    // Make a random move
    sendMessage(std::to_string(1) + "," +
                std::to_string(2));
    return;
  }

  if (turn == 2) {
      sendMessage("-1,-1");
      return;
  }

  MCTSNode root(red_moves, blue_moves, open_moves, colour, nullptr, {-1, -1});
  root.generate_all_children_nodes();

  int count = 0;
  constexpr double turn_time = 10000;
  const long loop_start_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
  while (std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
                 .count() -
             loop_start_time <
         static_cast<long>(turn_time)) {
    count++;
    MCTSNode *node = root.best_child(EXPLORATION_CONSTANT);
    double result = node->simulate_from_node();
    if (result > 1) {
      std::cerr << result << std::endl;
    }
    node->backpropagate(result, 1);
         }
  std::cerr << "iteration count: " << count << std::endl;

  auto [fst, snd] = root.get_best_move();
  root.delete_children();
  sendMessage(std::to_string(fst) + "," +
              std::to_string(snd));

  const long move_end_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();

  time_left_ms -= static_cast<double>(move_end_time_ms - move_start_time_ms);
  std::cerr << "Time left: " << time_left_ms << std::endl;
}
