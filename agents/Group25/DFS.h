//
// Created by chris on 30/11/24.
//

#ifndef DFS_H
#define DFS_H

#include <vector>
#include <set>
#include <string>

class DFS {
    std::set<std::pair<int, int>> red_moves;
    std::set<std::pair<int, int>> blue_moves;
    std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 0}, {1, -1}, {0, -1}
    };
    std::set<std::pair<int, int>> visited;
    bool dfs(const std::pair<int, int> &move, const std::string &player);
   public:
   DFS(std::set<std::pair<int, int>> red_moves, std::set<std::pair<int, int>> blue_moves);
   std::string get_winner();
};


#endif //DFS_H
