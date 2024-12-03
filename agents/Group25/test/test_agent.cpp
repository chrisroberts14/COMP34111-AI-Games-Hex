//
// Created by chris on 27/11/24.
//

#include "../MCTSAgent.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <set>
#include <string>
#include <thread>

#include "../Heuristic.h"

std::string convert_sets_to_board_string(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves, int board_size) {
    std::string board;
    for (int i = 0; i < board_size; ++i) {
        for (int j = 0; j < board_size; ++j) {
            if (red_moves.find({i, j}) != red_moves.end()) {
                board += "R";
            } else if (blue_moves.find({i, j}) != blue_moves.end()) {
                board += "B";
            } else {
                board += "0";
            }
        }
        if (i != board_size - 1) {
            board += ",";
        }
    }
    return board;
}

void add_red_move(std::set<std::pair<int, int>> &red_moves, const std::string& move) {
    // move is in the form x,y
    std::stringstream ss(move);
    std::string item;
    std::getline(ss, item, ',');
    int x = std::stoi(item);
    std::getline(ss, item, ',');
    int y = std::stoi(item);
    red_moves.insert({x, y});
}

void add_blue_move(std::set<std::pair<int, int>> &blue_moves, const std::string& move) {
    // move is in the form x,y
    std::stringstream ss(move);
    std::string item;
    std::getline(ss, item, ',');
    int x = std::stoi(item);
    std::getline(ss, item, ',');
    int y = std::stoi(item);
    blue_moves.insert({x, y});
}

void swap_move_sets(std::set<std::pair<int, int>> &red_moves, std::set<std::pair<int, int>> &blue_moves) {
    const std::set<std::pair<int, int>> temp = red_moves;
    red_moves = blue_moves;
    blue_moves = temp;
}

std::string get_response(std::stringstream &oss) {
    std::string response;
    while (response.empty()) {
        std::cerr << oss.str() << std::endl;
        std::getline(oss, response);
    }
    return response;
}

std::string start_move(const std::string& board, MCTSAgent &agent, std::set<std::pair<int, int>> &red_moves) {
    testing::internal::CaptureStdout();
    const std::string message = "START;;" + board + ";1;\n";
    agent.interpretMessage(message);
    // Get the agents response
    std::string response = testing::internal::GetCapturedStdout();
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    add_red_move(red_moves, response);
    return response;
}

std::string swap_move(const std::string& board, MCTSAgent &agent, std::set<std::pair<int, int>> &blue_moves) {
    testing::internal::CaptureStdout();
    const std::string message = "SWAP;;" + board + ";3;\n";
    agent.interpretMessage(message);
    std::string response = testing::internal::GetCapturedStdout();
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    add_blue_move(blue_moves, response);
    return response;
}

std::string make_move(const std::string& board, MCTSAgent &agent, const std::string& opp_move, const int turn_number, const std::string& colour, std::set<std::pair<int, int>> &red_moves, std::set<std::pair<int, int>> &blue_moves) {
    testing::internal::CaptureStdout();
    const std::string message = "CHANGE;" + opp_move + ";" + board + ";" + std::to_string(turn_number) + ";\n";
    agent.interpretMessage(message);
    std::string response = testing::internal::GetCapturedStdout();
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    if (colour == "R") {
        add_red_move(red_moves, response);
    } else {
        add_blue_move(blue_moves, response);
    }
    return response;
}

void output_board(std::set<std::pair<int, int>> &red_moves, std::set<std::pair<int, int>> &blue_moves) {
    std::vector board(11, std::vector<std::string>(11, "."));
    for (const auto &[x, y]: red_moves) {
        board[x][y] = "R";
    }
    for (const auto &[x, y]: blue_moves) {
        board[x][y] = "B";
    }
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < i; ++j) {
            std::cerr << " ";
        }
        for (int j = 0; j < 11; ++j) {
            std::cerr << board[i][j] << " ";
        }
        std::cerr << std::endl;
    }
}

TEST(MCTSAgentTest, TestAgentSwap) {
    // Agent starts as red but after a swap turns to blue
    std::set<std::pair<int, int>> red_moves;
    std::set<std::pair<int, int>> blue_moves;
    std::string board = convert_sets_to_board_string(red_moves, blue_moves, 11);

    MCTSAgent agent("R", 11);

    EXPECT_EQ(start_move(board, agent, red_moves), "5,5");

    Heuristic heuristic(red_moves, blue_moves);
    // The agent is now blue
    board = convert_sets_to_board_string(red_moves, blue_moves, 11);
    swap_move(board, agent, blue_moves);
    std::cout << heuristic.get_value(6, 6, "B") << std::endl;
    std::cout << heuristic.get_value(6, 3, "B") << std::endl;

    // Setup the board for the next move
    blue_moves.insert({0, 0});
    blue_moves.insert({1, 1});
    red_moves.insert({0, 1});
    // The agent should return 1, 0
    output_board(red_moves, blue_moves);
    heuristic = Heuristic(red_moves, blue_moves);
    std::cout << heuristic.get_value(1, 0, "B") << std::endl;
    std::cout << heuristic.get_value(1, 0, "R") << std::endl;
    board = convert_sets_to_board_string(red_moves, blue_moves, 11);
    heuristic.output_values();
    EXPECT_EQ(make_move(board, agent, "0,1", 5, "B", red_moves, blue_moves), "1,0");
}

TEST(MCTSAgentTest, shouldSwap) {
    // Agent starts as red but after a swap turns to blue
    MCTSAgent agent("B", 11);
    std::set<std::pair<int, int>> red_moves;
    // Generate all places where the agent should swap
    for (int i = 2; i < 9; i++) {
        for (int j = 2; j < 9; j++) {
            const std::set<std::pair<int, int>> blue_moves;
            red_moves.insert({i, j});
            std::string board = convert_sets_to_board_string(red_moves, blue_moves, 11);
            testing::internal::CaptureStdout();
            const std::string message = "CHANGE;" + std::to_string(i) + "," + std::to_string(j) + ";" + board + ";2;\n";
            agent.interpretMessage(message);
            red_moves = {};
            std::string response = testing::internal::GetCapturedStdout();
            response.erase(response.find_last_not_of(" \n\r\t") + 1);
            EXPECT_EQ(response, "-1,-1");
        }
    }
}

TEST(MCTSAgentTest, shouldPickWinBlue) {
    // Agent starts as red but after a swap turns to blue
    MCTSAgent agent("B", 11);
    const std::set<std::pair<int, int>> red_moves = {{10, 10}};
    const std::set<std::pair<int, int>> blue_moves = {{0, 0}, {0, 1},
    {0,2}, {0,3}, {0,4}, {0,5}, {0,6},
    {0,7}, {0,8}, {0,9}};
    testing::internal::CaptureStdout();
    const std::string board = convert_sets_to_board_string(red_moves, blue_moves, 11);
    const std::string message = "CHANGE;10,10;" + board + ";5;\n";
    agent.interpretMessage(message);
    std::string response = testing::internal::GetCapturedStdout();
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    EXPECT_EQ(response, "0,10");
}

TEST(MCTSAgentTest, shouldPickWinRed) {
    // Agent starts as red but after a swap turns to blue
    MCTSAgent agent("R", 11);
    const std::set<std::pair<int, int>> blue_moves = {{10, 10}};
    const std::set<std::pair<int, int>> red_moves = {{0, 0}, {1, 0},
    {2,0}, {3,0}, {4,0}, {5,0}, {6,0},
    {7,0}, {8,0}, {9,0}};
    testing::internal::CaptureStdout();
    const std::string board = convert_sets_to_board_string(red_moves, blue_moves, 11);
    const std::string message = "CHANGE;10,10;" + board + ";5;\n";
    agent.interpretMessage(message);
    std::string response = testing::internal::GetCapturedStdout();
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    EXPECT_EQ(response, "10,0");
}

TEST(MCTSAgentTest, Edge) {
    MCTSAgent agent("B", 11);
    const std::set<std::pair<int, int>> red_moves = {{0, 8}, {0, 10},
 {3, 4}, {3, 9}, {4, 3}, {4, 6}, {6, 2}, {7, 2}, {8, 0}, {9, 1}};
    const std::set<std::pair<int, int>> blue_moves = {
        {4, 0}, {4, 1}, {4, 4}, {5,2}, {5,5}, {5, 8},
          {6, 3}, {6,5}, {6, 7}
    };
    EXPECT_EQ(red_moves.size(), 10);
    EXPECT_EQ(blue_moves.size(), 9);
    testing::internal::CaptureStdout();
    const std::string board = convert_sets_to_board_string(red_moves, blue_moves, 11);
    const std::string message = "CHANGE;10,10;" + board + ";5;\n";
    agent.interpretMessage(message);
    std::string response = testing::internal::GetCapturedStdout();
    response.erase(response.find_last_not_of(" \n\r\t") + 1);
    EXPECT_EQ(response, "5,3");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
