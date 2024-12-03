#include "Board.h"
#include "Tile.h"
#include <iostream>

Board::Board(Colour player_colour) : player_colour(player_colour) {
    opponent_colour = player_colour.opposite();

    // Declare 2D array of nodes
    for (int row_index = 0; row_index < 11; ++row_index) {
        std::vector<BoardTile> row_of_tiles;
        for (int column_index = 0; column_index < 11; ++column_index) {
            row_of_tiles.emplace_back(column_index, row_index);
        }
        array_of_tiles.push_back(row_of_tiles);
    }

    // Determine row
    if (opponent_colour == Colour::BLUE) {
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 11; ++y) {
                get_tile(x, y)->row = -x - 1;
            }
        }
        for (int x = 5; x < 11; ++x) {
            for (int y = 0; y < 11; ++y) {
                get_tile(x, y)->row = 11 - x;
            }
        }
    } else if (opponent_colour == Colour::RED) {
        for (int x = 0; x < 11; ++x) {
            for (int y = 0; y < 5; ++y) {
                get_tile(x, y)->row = -y - 1;
            }
        }
        for (int x = 0; x < 11; ++x) {
            for (int y = 5; y < 11; ++y) {
                get_tile(x, y)->row = 11 - y;
            }
        }
    }

    // Initialise nodes
    for (int x = 0; x < 11; ++x) {
        for (int y = 0; y < 11; ++y) {
            get_tile(x, y)->add_neighbours(this);
        }
    }
}

std::string Board::display_row_numbers() {
    std::string output;
    std::string leading_spaces;
    for (const auto& row : array_of_tiles) {
        output += leading_spaces;
        leading_spaces += " ";
        for (const auto& tile : row) {
            output += std::to_string(tile.row) + " ";
        }
        output += "\n";
    }
    return output;
}

void Board::display_abandoned_tiles() {
    std::string output;
    std::string leading_spaces;
    for (const auto& row : array_of_tiles) {
        std::string text_line = leading_spaces;
        leading_spaces += " ";
        for (const auto& tile : row) {
            if (tile.occupied_by == Player::SELF) {
                if (opponent_colour == Colour::RED) {
                    text_line += "B ";
                } else {
                    text_line += "R ";
                }
            } else if (tile.occupied_by == Player::OPPONENT) {
                if (opponent_colour == Colour::RED) {
                    text_line += "R ";
                } else {
                    text_line += "B ";
                }
            } else if (tile.abandoned) {
                text_line += "A ";
            } else {
                text_line += "0 ";
            }
        }
        output += text_line + "\n";
        std::cout << text_line << std::endl;
    }
}

BoardTile* Board::get_tile(int x, int y) {
    if (x < 0 || x > 10 || y < 0 || y > 10) {
        return nullptr;
    } else {
        return &array_of_tiles[y][x];
    }
}

void Board::player_makes_move(int x, int y) {
    get_tile(x, y)->occupied_by = Player::SELF;
}

void Board::opponent_makes_move(int x, int y) {
    get_tile(x, y)->occupied_by = Player::OPPONENT;
    look_for_guaranteed_edges_for_the_opponent(get_tile(x, y));
}

std::string Board::print_board() {
    std::string output;
    std::string leading_spaces;
    for (const auto& row : array_of_tiles) {
        output += leading_spaces;
        leading_spaces += " ";
        for (const auto& tile : row) {
            output += Colour::get_char(tile.colour) + " ";
        }
        output += "\n";
    }
    return output;
}

std::vector<BoardTile*> Board::get_valid_moves() {
    std::vector<BoardTile*> valid_moves;
    for (auto& row : array_of_tiles) {
        for (auto& tile : row) {
            if (tile.occupied_by == nullptr && !tile.abandoned) {
                valid_moves.push_back(&tile);
            }
        }
    }
    return valid_moves;
}

void look_for_guaranteed_edges_for_the_opponent(BoardTile* played_tile) {
    if (abs(played_tile->row) == 2) {
        if (guaranteed_edge_template_exists_on_board(played_tile, Templates::row_2.tile_of_origin)) {
            abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(played_tile, Templates::row_2.tile_of_origin);
        }
    } else if (abs(played_tile->row) == 3) {
        for (const auto& row_3_template : Templates::row_3) {
            if (guaranteed_edge_template_exists_on_board(played_tile, row_3_template.tile_of_origin)) {
                abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(played_tile, row_3_template.tile_of_origin);
            }
        }
    }
}

bool guaranteed_edge_template_exists_on_board(BoardTile* board_tile, BoardTile* template_tile) {
    for (const auto& tile_neighbour : TileNeighbours) {
        if (template_tile->neighbours[tile_neighbour] == nullptr) {
            continue;
        } else if (template_tile->neighbours[tile_neighbour]->occupied_by != board_tile->relative_neighbours[tile_neighbour]->occupied_by) {
            return false;
        } else if (template_tile->neighbours[tile_neighbour]->occupied_by == board_tile->relative_neighbours[tile_neighbour]->occupied_by) {
            guaranteed_edge_template_exists_on_board(board_tile->relative_neighbours[tile_neighbour], template_tile->neighbours[tile_neighbour]);
        }
    }
    return true;
}

void abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(BoardTile* board_tile, BoardTile* template_tile) {
    for (const auto& tile_neighbour : TileNeighbours) {
        if (template_tile->neighbours[tile_neighbour] == nullptr) {
            continue;
        } else {
            board_tile->relative_neighbours[tile_neighbour]->abandoned = true;
            abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(board_tile->relative_neighbours[tile_neighbour], template_tile->neighbours[tile_neighbour]);
        }
    }
}

int main() {
    Board board(Colour::BLUE);
    std::cout << board.display_row_numbers();
    board.opponent_makes_move(4, 8);
    board.opponent_makes_move(8, 9);
    board.opponent_makes_move(2, 1);
    board.opponent_makes_move(7, 2);
    board.display_abandoned_tiles();
    board.display_abandoned_tiles();
    return 0;
}