//
// Created by chris on 27/11/24.
//

#include "../Heuristic.h"
#include <gtest/gtest.h>

TEST(HeuristicTest, TestAlwaysPickBlocked) {
    const std::set<std::pair<int, int>> red_moves = {{0, 1}, {2, 0}};
    const std::set<std::pair<int, int>> blue_moves = {{1, 0}};
    Heuristic heuristic(red_moves, blue_moves);
    heuristic.generate_values();
    EXPECT_EQ(heuristic.get_value(1, 1, "R"), 100000);
}

TEST(HeuristicTest, TestAlwaysPickBlockedEdge) {
    /* Test this board
    0 0 0 0 0 0 0 0 0 B R
     0 0 0 0 0 0 0 B B 0 0
      0 R B 0 0 0 0 0 0 R 0
       0 0 R 0 0 B 0 0 0 0 0
        B 0 0 0 0 R 0 0 R 0 0
         0 R 0 0 0 0 B 0 B 0 0
          0 0 0 0 R 0 0 R 0 0 0
           0 B 0 0 0 R 0 0 0 0 0
            0 0 0 0 0 0 0 0 0 0 0
             0 0 0 0 0 0 0 0 0 0 0
              0 0 0 0 0 0 0 0 0 0 0
    */
 const std::set<std::pair<int, int>> red_moves = {{0, 10},
  {2, 1}, {2, 9}, {3, 2}, {4, 5}, {4, 8},
  {5, 1}, {6, 4}, {6, 7}, {7, 5}};
 const std::set<std::pair<int, int>> blue_moves = {{0, 9}, {1, 7}, {1, 8},
  {2, 2}, {3,5}, {4, 0}, {5, 6}, {5, 8}, {6, 1}};
 Heuristic heuristic(red_moves, blue_moves);
 heuristic.generate_values();
 EXPECT_EQ(heuristic.get_value(3, 1, "R"), 100000);
 EXPECT_EQ(heuristic.get_value(5, 7, "R"), 100000);
}

TEST(HeuristcTest, Edge1) {
 const std::set<std::pair<int, int>> red_moves = {{8,4}, {0, 10}, {1, 10}, {3, 10}, {4, 5}, {5, 4}, {6, 4}, {6, 5}};
 const std::set<std::pair<int, int>> blue_moves = {{8,5}, {9, 3}};
 Heuristic heuristic(red_moves, blue_moves);
 heuristic.generate_values();
 EXPECT_NEAR(heuristic.get_value(9, 4, "B"), 100000, 1);
}

TEST(HeuristcTest, Edge2) {
 const std::set<std::pair<int, int>> red_moves = {{6,4}, {2, 7}, {4, 6}, {6, 6}, {10, 0}};
 const std::set<std::pair<int, int>> blue_moves = {{6,3}, {5, 5}, {4, 7}, {5, 5}, {5, 6}};
 Heuristic heuristic(red_moves, blue_moves);
 heuristic.generate_values();
 EXPECT_NEAR(heuristic.get_value(5, 4, "B"), 100000, 1);
}

TEST(HeuristicTest, ShouldNotCompleteBridgeIfNoBridgePathWin) {
 const std::set<std::pair<int, int>> red_moves = {{0, 1}, {2, 0}};
 const std::set<std::pair<int, int>> blue_moves = {{10, 10}};
 Heuristic heuristic(red_moves, blue_moves);
 heuristic.generate_values();
 EXPECT_NEAR(heuristic.get_value(1, 0, "R"), -100000, 1);
 EXPECT_NEAR(heuristic.get_value(1, 1, "R"), -100000, 1);
}

TEST(HeuristicTest, ShouldCompleteBridgeIfBridgePathWin) {
 const std::set<std::pair<int, int>> red_moves = {{9,2}, {7,3},{5,4},{3,5},{1,6}};
 const std::set<std::pair<int, int>> blue_moves;
 Heuristic heuristic(red_moves, blue_moves);
 heuristic.generate_values();
 heuristic.output_values();
 EXPECT_NEAR(heuristic.get_value(10, 1, "R"), 100000, 1);
 EXPECT_NEAR(heuristic.get_value(10, 2, "R"), 100000, 1);
}

TEST(HeuristicTest, TestGetAllBridgeTiles) {
 const std::set<std::pair<int, int>> red_moves = {{9,2}, {7,3},{5,4},{3,5},{1,6}};
 const std::set<std::pair<int, int>> blue_moves;
 Heuristic heuristic(red_moves, blue_moves);
 heuristic.generate_values();
 const std::set<std::pair<int, int>> bridge_nodes = heuristic.get_all_bridge_nodes("R");
 for (const auto &[x, y] : bridge_nodes) {
     std::cerr << x << ", " << y << std::endl;
 }
 EXPECT_EQ(bridge_nodes.size(), 12);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}