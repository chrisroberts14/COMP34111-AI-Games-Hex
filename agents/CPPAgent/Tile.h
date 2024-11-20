//
// Created by chris on 20/11/24.
//

#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>

class Tile {
public:
  // Constants for neighbour count and displacements
  static const int NEIGHBOUR_COUNT;
  static const std::vector<int> I_DISPLACEMENTS;
  static const std::vector<int> J_DISPLACEMENTS;

private:
  int x;
  int y;
  std::string colour;

public:
  // Constructor
  Tile(int x, int y, std::string colour = "");

  // Getters for x and y
  int getX() const;
  int getY() const;

  // Getter and setter for colour
  std::string getColour() const;
  void setColour(std::string colour);
};

#endif // TILE_H
