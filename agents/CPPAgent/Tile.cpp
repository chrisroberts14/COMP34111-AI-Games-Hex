//
// Created by chris on 20/11/24.
//

#include "Tile.h"

const int Tile::NEIGHBOUR_COUNT = 6;
const std::vector<int> Tile::I_DISPLACEMENTS = {-1, -1, 0, 1, 1, 0};
const std::vector<int> Tile::J_DISPLACEMENTS = {0, 1, 1, 0, -1, -1};

// Constructor
Tile::Tile(int x, int y, std::string colour) : x(x), y(y), colour(colour) {}

// Getters for x and y
int Tile::getX() const { return this->x; }

int Tile::getY() const { return this->y; }

// Getter and setter for colour
std::string Tile::getColour() const { return this->colour; }

void Tile::setColour(std::string colour) { this->colour = colour; }
