#include <vector>
#include <string>
#include "Templates.h"
#include "BoardTile.h"
#include "Colour.h"
#include "Constants.h"

class Board {
public:
    Board(Colour player_colour);
    std::string display_row_numbers();
    void display_abandoned_tiles();
    BoardTile* get_tile(int x, int y);
    void player_makes_move(int x, int y);
    void opponent_makes_move(int x, int y);
    std::string print_board();
    std::vector<BoardTile*> get_valid_moves();

private:
    std::vector<std::vector<BoardTile>> array_of_tiles;
    Colour player_colour;
    Colour opponent_colour;
};

void look_for_guaranteed_edges_for_the_opponent(BoardTile* played_tile);
bool guaranteed_edge_template_exists_on_board(BoardTile* board_tile, BoardTile* template_tile);
void abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(BoardTile* board_tile, BoardTile* template_tile);
