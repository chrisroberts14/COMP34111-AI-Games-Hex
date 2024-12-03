#include "Tile.h"
#include "constants.h"
#include "Colour.h"
#include "Board.h"

BoardTile::BoardTile(int x, int y) : x(x), y(y), row(0), abandoned(false), occupied_by(nullptr), bridgable(false), createsBridge(false) {
    for (int i = 0; i < 6; ++i) {
        relative_neighbours[i] = nullptr;
        fixed_neighbours[i] = nullptr;
    }
}

void BoardTile::add_neighbours(Board* board) {
    // Fixed neighbours
    fixed_neighbours[TileNeighbours::NORTH_WEST] = board->get_tile(x, y-1);
    fixed_neighbours[TileNeighbours::NORTH_EAST] = board->get_tile(x+1, y-1);
    fixed_neighbours[TileNeighbours::EAST] = board->get_tile(x+1, y);
    fixed_neighbours[TileNeighbours::SOUTH_EAST] = board->get_tile(x, y+1);
    fixed_neighbours[TileNeighbours::SOUTH_WEST] = board->get_tile(x-1, y+1);
    fixed_neighbours[TileNeighbours::WEST] = board->get_tile(x-1, y);

    // Relative neighbours
    if (row > 0 && board->opponent_colour == Colour::RED) {
        relative_neighbours[TileNeighbours::NORTH_WEST] = board->get_tile(x, y-1);
        relative_neighbours[TileNeighbours::NORTH_EAST] = board->get_tile(x+1, y-1);
        relative_neighbours[TileNeighbours::EAST] = board->get_tile(x+1, y);
        relative_neighbours[TileNeighbours::SOUTH_EAST] = board->get_tile(x, y+1);
        relative_neighbours[TileNeighbours::SOUTH_WEST] = board->get_tile(x-1, y+1);
        relative_neighbours[TileNeighbours::WEST] = board->get_tile(x-1, y);
    } else if (row < 0 && board->opponent_colour == Colour::RED) {
        relative_neighbours[TileNeighbours::NORTH_WEST] = board->get_tile(x, y+1);
        relative_neighbours[TileNeighbours::NORTH_EAST] = board->get_tile(x-1, y+1);
        relative_neighbours[TileNeighbours::EAST] = board->get_tile(x-1, y);
        relative_neighbours[TileNeighbours::SOUTH_EAST] = board->get_tile(x, y-1);
        relative_neighbours[TileNeighbours::SOUTH_WEST] = board->get_tile(x+1, y-1);
        relative_neighbours[TileNeighbours::WEST] = board->get_tile(x+1, y);
    } else if (row > 0 && board->opponent_colour == Colour::BLUE) {
        relative_neighbours[TileNeighbours::NORTH_WEST] = board->get_tile(x-1, y+1);
        relative_neighbours[TileNeighbours::NORTH_EAST] = board->get_tile(x-1, y);
        relative_neighbours[TileNeighbours::EAST] = board->get_tile(x, y-1);
        relative_neighbours[TileNeighbours::SOUTH_EAST] = board->get_tile(x+1, y-1);
        relative_neighbours[TileNeighbours::SOUTH_WEST] = board->get_tile(x+1, y);
        relative_neighbours[TileNeighbours::WEST] = board->get_tile(x, y+1);
    } else if (row < 0 && board->opponent_colour == Colour::BLUE) {
        relative_neighbours[TileNeighbours::NORTH_WEST] = board->get_tile(x+1, y-1);
        relative_neighbours[TileNeighbours::NORTH_EAST] = board->get_tile(x+1, y);
        relative_neighbours[TileNeighbours::EAST] = board->get_tile(x, y+1);
        relative_neighbours[TileNeighbours::SOUTH_EAST] = board->get_tile(x-1, y+1);
        relative_neighbours[TileNeighbours::SOUTH_WEST] = board->get_tile(x-1, y);
        relative_neighbours[TileNeighbours::WEST] = board->get_tile(x, y-1);
    }
}