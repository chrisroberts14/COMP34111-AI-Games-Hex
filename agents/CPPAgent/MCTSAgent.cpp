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
#include <future>

#define DECAY_RATE -0.05
#define EXPLORATION_CONSTANT 1.41
#define THREAD_COUNT 32
#define TIME_LIMIT 290

int time_left_ms = TIME_LIMIT * 1000;

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
      //std::string msg = "CHANGE;0,0;RRRRBBRBRRR,RBRBRBRB000,RBRRR0RBB00,RRR00RB0000,RRBBRBBB000,RRBRB0R0000,0R0RBBBBB0B,BB0R0B000R0,00BRRB0BB00,BBBRBR0B000,0RBBR00B0R0;5;\n";
      if (!interpretMessage(msg)) {
        std::cerr << "PROBLEM INTERPRETING MESSAGE" << std::endl;
        return;
      }
      //return;
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
  int move_start_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch())
      .count();

  if (turn == 2) {
    sendMessage("-1,-1");
    return;
  }

  int turn_time = 5000;
  // Calculate time to run MCTS
  // Give 5 seconds of buffer time so total time is 290 seconds
  if (turn < 20) {
    // Use exponential decay to calculate time
    turn_time = time_left_ms * 0.15;
  } else {
    turn_time = time_left_ms / (turn - 15);
  }
  std::vector<std::vector<Tile>> boardState =
      convert_board_string(board, boardSize);

  Board brd(boardState, boardSize, "");
  MCTSNode root(brd, colour, nullptr, {-1, -1});
  root.generate_all_children_nodes();
  long loop_start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
  while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - loop_start_time < turn_time) {
    MCTSNode* node = root.best_child(EXPLORATION_CONSTANT);
    // Run this method N times and backpropagate the results
    std::vector<std::thread> threads;
    std::vector<std::future<double>> futures;
    for (int i = 0; i < THREAD_COUNT; i++) {
      std::promise<double> promise;
      futures.push_back(promise.get_future());
      threads.emplace_back([&node, promise = std::move(promise), this]() mutable {
        double result = node->simulate_from_node(colour);
        promise.set_value(result);
      });
    }
	// Wait for threads to finish
    for (std::thread &t : threads) {
      t.join();
    }
    // Get results from the threads and sum them
    double result = 0;
    for (auto &future : futures) {
        result += future.get();
    }
    node->backpropagate(result, THREAD_COUNT);
  }

  std::pair<int, int> best_move = root.get_best_move();
  root.delete_children();
  sendMessage(std::to_string(best_move.first) + "," +
              std::to_string(best_move.second));

  int move_end_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch())
      .count();

  time_left_ms -= (move_end_time_ms - move_start_time_ms);
  std::cerr << "Time left: " << time_left_ms << std::endl;
}
