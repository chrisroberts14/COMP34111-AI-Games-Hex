#include "Constants.h"

class BoardTile {
public:
    int x, y;
    int row;
    bool abandoned;
    BoardTile* relative_neighbours[6];
    BoardTile* fixed_neighbours[6];
    Player* occupied_by;
    bool bridgable;
    bool createsBridge;

    BoardTile(int x, int y);
    void add_neighbours(Board* board);
};
