//
// Created by chris on 25/11/24.
//

#include "UnionFind.h"
#include <stdexcept>
#include <vector>
#include <iostream>

UnionFind::UnionFind(int size)
    : parent(size), rank(size, 0) {
  for (int i = 0; i < size; i++) {
    parent[i] = i;
  }
}

int UnionFind::find(int x) {
  if (parent[x] != x) {
    parent[x] = find(parent[x]); // Path compression
  }
  return parent[x];
}

void UnionFind::union_sets(int x, int y) {
  x = find(x);
  y = find(y);
  if (x != y) {
    if (rank[x] > rank[y]) {
      parent[y] = x;
    } else if (rank[x] < rank[y]) {
      parent[x] = y;
    } else {
      parent[y] = x;
      rank[x]++;
    }
  }
}

bool UnionFind::connected(int x, int y) {
  return find(x) == find(y);
}
