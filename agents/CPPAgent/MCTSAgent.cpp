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

std::string MCTSAgent::getMessage() {
  std::string message;
  std::getline(std::cin, message);
  return message;
}

void MCTSAgent::sendMessage(const std::string &msg) {
  std::cout << msg << std::endl;
  std::cout.flush();
}

void MCTSAgent::run() {
  while (true) {
    try {
      std::string msg;
      msg = getMessage();
      if (!interpretMessage(msg)) {
        return;
      }
    } catch (const std::exception &e) {
      return;
    }
  }
}

bool MCTSAgent::interpretMessage(const std::string &s) {
  turn += 2;

  std::string trimmed = s;
  trimmed.erase(trimmed.find_last_not_of(" \n\r\t") + 1);

  std::vector<std::string> msg;
  std::stringstream ss(trimmed);
  std::string item;
  while (std::getline(ss, item, ';')) {
    msg.push_back(item);
  }

  std::string board = msg[2];
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

void simulate_iterations(std::atomic<int> &iterations, MCTSNode *root,
                         std::string colour,
                         std::chrono::steady_clock::time_point end_time,
                         int thread_number) {
  int local_iterations = 0;
  MCTSNode *node = root->get_child(thread_number);
  while (std::chrono::steady_clock::now() < end_time) {
    local_iterations++;
    double result = node->simulate_from_node(colour);
    node->backpropagate(result);
  }
  iterations.fetch_add(local_iterations,
                       std::memory_order_relaxed); // Increment safely
}

void run_mcts_in_parallel(MCTSNode *root, const std::string &colour,
                          std::chrono::steady_clock::time_point end_time,
                          int num_threads) {
  std::atomic<int> iterations(0); // Atomic variable to safely track iterations

  // Create a vector of threads
  std::vector<std::thread> threads;

  // Launch the threads
  for (int i = 0; i < num_threads; ++i) {
    threads.push_back(std::thread(simulate_iterations, std::ref(iterations),
                                  root, colour, end_time, i));
  }

  // Wait for all threads to finish
  for (auto &t : threads) {
    t.join();
  }

  std::cerr << "Total iterations: " << iterations.load() << std::endl;
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
      std::chrono::steady_clock::now() + std::chrono::milliseconds(4500);
  run_mcts_in_parallel(&root, colour, end_time, root.get_children_size());

  std::pair<int, int> best_move = root.get_best_move();
  sendMessage(std::to_string(best_move.first) + "," +
              std::to_string(best_move.second));
}
