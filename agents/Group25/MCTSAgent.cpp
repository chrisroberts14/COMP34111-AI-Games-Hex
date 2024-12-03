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
#define THREAD_COUNT 64
#define TIME_LIMIT 295

double time_left_ms = TIME_LIMIT * 1000;

std::mt19937 rng(time(nullptr));

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
  //std::cerr << "Starting colour: " << colour << std::endl;
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
  std::pair prev_move = {-2, -2};
  if (!msg[1].empty()) {
    std::stringstream ss2(msg[1]);
    std::string item2;
    std::getline(ss2, item2, ',');
    prev_move.first = std::stoi(item2);
    std::getline(ss2, item2, ',');
    prev_move.second = std::stoi(item2);
  }
  const std::string board = msg[2];
  turn = std::stoi(msg[3]);
  if (turn == 2) {
    if (prev_move.first > 1 && prev_move.second > 1 && prev_move.first < 9 && prev_move.second < 9) {
      sendMessage("-1,-1");
      colour = colour == "R" ? "B" : "R";
    } else {

      // Make a random move near the center
      std::uniform_int_distribution<std::mt19937::result_type> coord_dist(3,7);
      unsigned long row = coord_dist(rng);
      unsigned long col = coord_dist(rng);
      //std::cerr << "Random move: " << row << " " << col << std::endl;
      sendMessage(std::to_string(row) + "," +
                  std::to_string(col));
    }
    return true;
  }
  if (msg[0] == "START") {
    if (colour == "R") {
      makeMove(board);
    }
  } else if (msg[0] == "CHANGE") {
    makeMove(board);
  } else if (msg[0] == "SWAP") {
    //std::cerr << "SWAP" << std::endl;
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
    // Make a random move near the center
    std::uniform_int_distribution<std::mt19937::result_type> coord_dist(3,7);
    unsigned long row = coord_dist(rng);
    unsigned long col = coord_dist(rng);
    //std::cerr << "Random move: " << row << " " << col << std::endl;
    sendMessage(std::to_string(row) + "," +
                std::to_string(col));
    return;
  }
  //std::cerr << "Our agent is: " << colour << std::endl;
  MCTSNode root(red_moves, blue_moves, open_moves, colour, nullptr, {-1, -1});

  //std::cerr << "Root node is made" << std::endl;
  // Short circuit moves
  if (auto [heuristic_check_fst, heuristic_check_snd] = root.generate_all_children_nodes(); heuristic_check_fst != -1) {
    //std::cerr << "Heuristic short circuit" << std::endl;
    sendMessage(std::to_string(heuristic_check_fst) + "," +
                std::to_string(heuristic_check_snd));
    root.delete_children();
    return;
  }
  //std::cerr << "Children generated" << std::endl;

  int count = 0;
  constexpr double turn_time = 10000;
  const long loop_start_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
  //std::cerr << "Starting loop" << std::endl;
  while (std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
                 .count() -
             loop_start_time <
         static_cast<long>(turn_time)) {
    count += THREAD_COUNT;
    MCTSNode *node = root.best_child(EXPLORATION_CONSTANT);

    std::vector<std::thread> threads;
    std::vector<std::future<double>> futures;
    for (int i = 0; i < THREAD_COUNT; ++i) {
      std::promise<double> promise;
      futures.emplace_back(promise.get_future());
      threads.emplace_back(
      [&node, promise = std::move(promise), this]() mutable {
        const double result = node->simulate_from_node();
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
  //std::cerr << "Time left: " << time_left_ms << std::endl;
}
