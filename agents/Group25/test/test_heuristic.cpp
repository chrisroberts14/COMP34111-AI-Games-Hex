//
// Created by chris on 27/11/24.
//

#include "../Heuristic.h"
#include <gtest/gtest.h>



class HeuristicTest : public ::testing::Test {
protected:
    Heuristic *heuristic = nullptr;
    std::set<std::pair<int, int>> red_moves = {{1, 1}, {2, 2}};
    std::set<std::pair<int, int>> blue_moves = {{3, 3}, {4, 4}};
    void SetUp() override {
        heuristic = new Heuristic(red_moves, blue_moves, "R");
    }

    void TearDown() override {
        // Output the board here
        heuristic->output_board();
        delete heuristic;
    }
};

TEST_F(HeuristicTest, createsBridgeWithTile_ValidBridge_ReturnsTrue) {
    constexpr std::pair move = {1, 3};
    heuristic->insert_red_move({1, 3});
    EXPECT_TRUE(heuristic->creates_bridge_with_tile(move));
}

TEST_F(HeuristicTest, createsBridgeWithTile_NoValidBridge_ReturnsFalse) {
    constexpr std::pair move = {0, 0};
    EXPECT_FALSE(heuristic->creates_bridge_with_tile(move));
}

TEST_F(HeuristicTest, createsBridgeToEdge_ValidEdgeBridge_ReturnsTrue) {
    std::pair<int, int> move = {1, 2};
    EXPECT_TRUE(heuristic->creates_bridge_to_edge(move));
}

TEST_F(HeuristicTest, createsBridgeToEdge_NoValidEdgeBridge_ReturnsFalse) {
    constexpr std::pair move = {0, 0};
    EXPECT_FALSE(heuristic->creates_bridge_to_edge(move));
}

TEST_F(HeuristicTest, doesMoveCreateBridge_ValidBridge_ReturnsTrue) {
    constexpr std::pair move = {1, 3};
    heuristic->insert_red_move({1,3});
    EXPECT_TRUE(heuristic->does_move_create_bridge(move));
}

TEST_F(HeuristicTest, doesMoveCreateBridge_NoValidBridge_ReturnsFalse) {
    constexpr std::pair move = {0, 0};
    EXPECT_FALSE(heuristic->does_move_create_bridge(move));
}

TEST_F(HeuristicTest, evaluateMove_CreatesBridge_ReturnsHigherValue) {
    constexpr std::pair move = {1, 3};
    heuristic->insert_red_move({1,3});
    const double value = heuristic->evaluate_move(move);
    EXPECT_GT(value, 0.5);
}

TEST_F(HeuristicTest, evaluateMove_NoBridge_ReturnsLowerValue) {
    constexpr std::pair move = {0, 0};
    const double value = heuristic->evaluate_move(move);
    EXPECT_LT(value, 0.5);
}

TEST_F(HeuristicTest, getBestMove) {
    for (int i = 0; i < 10; i++) {
        heuristic->insert_red_move(heuristic->get_best_move());
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}