//
// Created by chris on 27/11/24.
//

#include "../HexUnionFind.h"
#include <gtest/gtest.h>
#include <set>

TEST(TestUnionFind, test_1){
  std::set<std::pair<int, int>> red_moves = {{0, 0}, {1, 0}};
    std::set<std::pair<int, int>> blue_moves;
  HexUnionFind hex(11, red_moves, blue_moves);
  EXPECT_FALSE(hex.add_move(2, 0, "R"));
  EXPECT_FALSE(hex.add_move(3, 0, "R"));
  EXPECT_FALSE(hex.add_move(4, 0, "R"));
  EXPECT_FALSE(hex.add_move(5, 0, "R"));
  EXPECT_FALSE(hex.add_move(6, 0, "R"));
  EXPECT_FALSE(hex.add_move(7, 0, "R"));
  EXPECT_FALSE(hex.add_move(8, 0, "R"));
  EXPECT_FALSE(hex.add_move(9, 0, "R"));
  EXPECT_TRUE(hex.add_move(10, 0, "R"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
