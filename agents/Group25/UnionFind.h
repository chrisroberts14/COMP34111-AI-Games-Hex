//
// Created by chris on 25/11/24.
//

#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>

class UnionFind {
std::vector<int> parent;
std::vector<int> rank;

public:
  explicit UnionFind(int size);
  int find(int x);
  void union_sets(int x, int y);
  bool connected(int x, int y);
  void output_parents();
};



#endif //UNIONFIND_H
