//
// Created by chris on 20/11/24.
//

#include "MCTSAgent.h"
#include "Board.h"
#include "MCTSNode.h"
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#define DECAY_RATE (-0.05)
#define EXPLORATION_CONSTANT 1.41
#define THREAD_COUNT 4
#define TIME_LIMIT 295

double time_left_ms = TIME_LIMIT * 1000;

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

  /*
   *Dead code for getting the previous move leaving here incase it is useful later
  // msg[1] is the previous move in the form "x,y"
  // Convert this to a pair
  std::pair prev_move = {-2, -2};
  if (!msg[1].empty()) {
    std::stringstream ss2(msg[1]);
    std::string item2;
    std::getline(ss2, item2, ',');
    prev_move.first = std::stoi(item2);
    std::getline(ss2, item2, ',');
    prev_move.second = std::stoi(item2);
  }
  */
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

void MCTSAgent::multi_thread_move(MCTSNode &root) const {
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
    MCTSNode *node = root.best_child(EXPLORATION_CONSTANT);
    // Run this method N times and backpropagate the results
    std::vector<std::thread> threads;
    std::vector<std::future<double>> futures;
    for (int i = 0; i < THREAD_COUNT; i++) {
      std::promise<double> promise;
      futures.push_back(promise.get_future());
      threads.emplace_back(
          [&node, promise = std::move(promise), this]() mutable {
            const double result = node->simulate_from_node(colour);
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
}

void MCTSAgent::single_thread_move(MCTSNode &root) const {
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
    MCTSNode *node = root.best_child(EXPLORATION_CONSTANT);
    const double result = node->simulate_from_node(colour);
    node->backpropagate(result);
  }
}


void MCTSAgent::makeMove(const std::string &board) const {
  const long move_start_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();

  const std::vector<std::vector<Tile>> boardState =
      convert_board_string(board, boardSize);

  const Board brd(boardState, boardSize, "");
  if (turn == 1) {
    auto [fst_open, snd_open] = Board::opening_move();
    sendMessage(std::to_string(fst_open) + "," +
                std::to_string(snd_open));
    return;
  }

  if (turn == 2) {
    if (brd.should_swap()) {
      sendMessage("-1,-1");
      return;
    }
  }

  MCTSNode root(brd, colour, nullptr, {-1, -1});
  root.generate_all_children_nodes();

  if (turn < 20) {
    multi_thread_move(root);
  } else {
    single_thread_move(root);
  }

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
