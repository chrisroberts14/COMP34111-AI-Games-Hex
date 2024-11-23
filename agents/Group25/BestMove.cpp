//
// Created by chris on 23/11/2024.
//

#define BOARDSIZE 11

#include "BestMove.h"

#include <iostream>
#include <utility>

#include "Board.h"

std::string opp(const std::string &colour) {
    if (colour == "R") {
        return "B";
    }
    return "R";
}

BestMove::BestMove(const std::vector<std::vector<Tile>> &board, const std::string& colour, const int &move_count)
    :move_count(move_count),
      board(board),
colour(colour){
}

int BestMove::sign(const int x) {
    if (x < 0) {
        return -1;
    }
    if (x > 0) {
        return 1;
    }
    return 0;
}

std::pair<int, int> BestMove::get_direction_sums() const {
    int row_sum = 0;
    int col_sum = 0;
    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++) {
            if (board[i][j].getColour() == colour) {
                row_sum += i;
                col_sum += j;
            }
        }
    }
    return std::make_pair(row_sum, col_sum);
}

bool BestMove::is_top_left_region(const int x, const int y) {
    return (x > 3) && (x < BOARDSIZE - 1) && (y > 0) && (y < 3);
}

bool BestMove::is_top_right_region(const int x, const int y) {
    return y > 3 && y < BOARDSIZE - 1 && x > 0 && x < 3;
}

bool BestMove::is_bottom_left_region(const int x, const int y) {
    return y > 0 && y < BOARDSIZE - 4 && x < BOARDSIZE - 1 && x > BOARDSIZE - 4;
}

bool BestMove::is_bottom_right_region(const int x, const int y) {
    return x > 0 && x < BOARDSIZE - 4 && y < BOARDSIZE - 1 && y > BOARDSIZE - 4;
}

bool BestMove::is_on_top_left_border(const int x, const int y) {
    return (x>0)&&(x<BOARDSIZE-1)&&(y==0);
}

bool BestMove::is_on_top_right_border(const int x, const int y) {
    return x == 0 && y > BOARDSIZE - 4 && y < BOARDSIZE - 1;
}

bool BestMove::is_on_bottom_left_border(const int x, const int y) {
    return x == BOARDSIZE - 1 && y > 0 && y < 3;
}

bool BestMove::is_on_bottom_right_border(const int x, const int y) {
    return x > 0 && x < BOARDSIZE - 1 && y == BOARDSIZE - 1;
}

bool BestMove::is_north_east_bridge_position(const int x, const int y) const {
    return board[x - 1][y + 2].getColour() == opp(colour);
}

bool BestMove::is_north_west_bridge_position(const int x, const int y) const {
    return board[x - 2][y + 1].getColour() == opp(colour);
}

bool BestMove::is_south_east_bridge_position(const int x, const int y) const {
    return board[x + 2][y - 1].getColour() == opp(colour);
}

bool BestMove::is_south_west_bridge_position(const int x, const int y) const {
    return board[x + 1][y - 2].getColour() == opp(colour);
}

bool BestMove::is_north_east_bridge_with_clear_path(const int x, const int y) const {
    return board[x - 1][y + 2].getColour() == opp(colour) && // Opponent's piece at NE bridge position
           board[x - 1][y].getColour() == "0" && // North cell is clear
           board[x - 1][y + 1].getColour() == "0" && // North-East adjacent cell is clear
           board[x][y + 1].getColour() == "0" && // East cell is clear
           board[x + 1][y].getColour() == "0" // South cell is clear
            ;
}

bool BestMove::is_north_west_bridge_with_clear_path(const int x, const int y) const {
    return board[x - 2][y + 1].getColour() == opp(colour) && // Opponent's piece at NW bridge position
           board[x][y + 1].getColour() == "0" && // North cell is clear
           board[x - 1][y + 1].getColour() == "0" && // North-West adjacent cell is clear
           board[x - 1][y].getColour() == "0" &&
           board[x][y - 1].getColour() == "0";
}

bool BestMove::is_south_east_bridge_with_clear_path(const int x, const int y) const {
    return board[x + 2][y - 1].getColour() == opp(colour) && // Opponent's piece at SE bridge position
           board[x][y - 1].getColour() == "0" && // South cell is clear
           board[x + 1][y - 1].getColour() == "0" && // South-East adjacent cell is clear
           board[x + 1][y].getColour() == "0" &&
           board[x][y + 1].getColour() == "0";
}

bool BestMove::is_south_west_bridge_with_clear_path(const int x, const int y) const {
    return board[x + 1][y - 2].getColour() == opp(colour) && // Opponent's piece at SW bridge position
           board[x + 1][y].getColour() == "0" && // South cell is clear
           board[x + 1][y - 1].getColour() == "0" && // South-West adjacent cell is clear
           board[x][y - 1].getColour() == "0" &&
           board[x - 1][y].getColour() == "0";
}

double BestMove::calculate_move_potential(const int x, const int y, const int row_sum, const int col_sum) {
    double move_potential = (std::abs(x - 5) + std::abs(y - 5)) * 190;
    move_potential += 8.0 * (row_sum * (x - 5) + col_sum * (y - 5)) / move_count;
    for (int direction = 0; direction < 4; direction++) {
        move_potential -= bridge[x][y][direction];
    }

    const double blue_potential = potential[x][y][0] + potential[x][y][1];
    const double red_potential = potential[x][y][2] + potential[x][y][3];
    move_potential += blue_potential + red_potential;

    if (blue_potential <= 268 || red_potential <= 268) {
        move_potential -= 400;
    }
    return move_potential;
}

int BestMove::can_connect_far_border(const int x, const int y, const std::string &local_col) const {
    if (local_col == "B") {
        // Check left half
        if (2 * y < BOARDSIZE - 1) {
            for (int i = 0; i < BOARDSIZE; i++) {
                for (int j = 0; j < BOARDSIZE; j++) {
                    // Blocked by a piece
                    if (j - i < y - x && i + j <= x + y && board[i][j].getColour() != "0") {
                        return 2;
                    }
                }
            }
            // Direct or indirect connections
            if (board[x - 1][y].getColour() == opp(local_col)) {
                return 0;
            }
            if (board[x - 1][y - 1].getColour() == opp(local_col)) {
                if (get_tile_value(x + 2, y - 1) == opp(local_col)) {
                    return 0;
                }
                return -1;
            }
            if (get_tile_value(x + 2, y - 1) == opp(local_col)) {
                return -2;
            }
        } else {
            // Right half
            for (int i = 0; i < BOARDSIZE; i++) {
                for (int j = BOARDSIZE - 1; j > y; j--) {
                    // Blocked by a piece
                    if (j - i > y - x && i + j >= x + y && board[i][j].getColour() != "0") {
                        return 2;
                    }
                }
            }
            // Direct or indirect connections
            if (board[x + 1][y].getColour() == opp(local_col)) {
                return 0;
            }
            if (board[x + 1][y + 1].getColour() == opp(local_col)) {
                if (get_tile_value(x - 2, y + 1) == opp(local_col)) {
                    return 0;
                }
                return -1;
            }
            if (get_tile_value(x - 2, y + 1) == opp(local_col)) {
                return -2;
            }
        }
    } else {
        // Red player
        // Check top half of the board
        if (2 * x < BOARDSIZE - 1) {
            for (int j = 0; j < BOARDSIZE; j++) {
                for (int i = 0; i < x; i++) {
                    // Blocked by a piece
                    if ((i - j < x - y) && (i + j <= x + y) && (board[i][j].getColour() != "0")) {
                        return 2;
                    }
                }
            }
            // Direct or indirect connections
            if (board[x][y - 1].getColour() == opp(local_col)) {
                return 0;
            }
            if (board[x - 1][y - 1].getColour() == opp(local_col)) {
                if (get_tile_value(x - 1, y + 2) == opp(local_col)) {
                    return 0;
                }
                return -1;
            }
            if (get_tile_value(x - 1, y + 2) == opp(local_col)) {
                return -2;
            }
        }
        // Check bottom half of the board
        else {
            for (int j = 0; j < BOARDSIZE; j++) {
                for (int i = BOARDSIZE - 1; i > x; i--) {
                    // Blocked by a piece
                    if ((i - j > x - y) && (i + j >= x + y) && (board[i][j].getColour() != "0")) {
                        return 2;
                    }
                }
            }
            // Direct or indirect connections
            if (board[x][y + 1].getColour() == opp(local_col)) {
                return 0;
            }
            if (board[x + 1][y + 1].getColour() == opp(local_col)) {
                if (get_tile_value(x + 1, y - 2) == opp(local_col)) {
                    return 0;
                }
                return -1;
            }
            if (get_tile_value(x + 1, y - 2) == opp(local_col)) {
                return -2;
            }
        }
    }

    // Default case: can connect
    return 1;
}

std::string BestMove::get_tile_value(const int x, const int y) const {
    if (x < 0) {
        return "R";
    }
    if (y < 0) {
        return "B";
    }
    if (x >= BOARDSIZE) {
        return "R";
    }
    if (y >= BOARDSIZE) {
        return "B";
    }
    return board[x][y].getColour();
}

double BestMove::get_potential_value(const int x, const int y, const int direction) const {
    if (x < 0) {
        return 30000;
    }
    if (y < 0) {
        return 30000;
    }
    if (x >= BOARDSIZE) {
        return 30000;
    }
    if (y >= BOARDSIZE) {
        return 30000;
    }
    if (board[x][y].getColour() == "0") {
        return potential[x][y][direction];
    }
    if (board[x][y].getColour() == opp(colour)) {
        return 30000;
    }
    return potential[x][y][direction] - 30000;
}

void BestMove::set_update(const int x, const int y) {
    if (x < 0 || y < 0 || x >= BOARDSIZE || y >= BOARDSIZE) {
        return;
    }
    update[x][y] = true;
}

int BestMove::set_potential(const int x, const int y, const int direction, const std::string &col) {
    int add_bonus = 0;
    double min_potential = 30000;
    const int default_bridge_value = col != colour ? 52 : 66;
    update[x][y] = false;
    bridge[x][y][direction] = 0;

    if (board[x][y].getColour() == opp(colour)) {
        return 0;
    }

    potentials[0] = get_potential_value(x + 1, y, direction);
    potentials[1] = get_potential_value(x, y + 1, direction);
    potentials[2] = get_potential_value(x - 1, y + 1, direction);
    potentials[3] = get_potential_value(x - 1, y, direction);
    potentials[4] = get_potential_value(x, y - 1, direction);
    potentials[5] = get_potential_value(x + 1, y - 1, direction);

    // Bonus for certain configurations of high potential neighbors
    for (int i = 0; i < 6; i++) {
        if ((potentials[i] >= 30000) && (potentials[(i + 2) % 6] >= 30000)) {
            if (potentials[(i + 1) % 6] < 0) {
                add_bonus = +32;
            } else {
                potentials[(i + 1) % 6] += 128; //512;
            }
        }
    }

    // Additional bonus for diagonal high potential neighbors
    for (int i = 0; i < 6; i++) {
        if ((potentials[i] >= 30000) && (potentials[(i + 3) % 6] >= 30000)) {
            add_bonus += 30;
        }
    }

    for (int i = 0; i < 6; i++) {
        if (potentials[i] < 0) {
            potentials[i] += 30000;
            neighbour_count[i] = 10;
        } else neighbour_count[i] = 1;
        min_potential = std::min(min_potential, potentials[i]);
    }

    // Count neighbors with minimum potential
    double min_potential_count = 0;
    for (int i = 0; i < 6; i++) {
        if (potentials[i] == min_potential) min_potential_count += neighbour_count[i];
    }

    bridge[x][y][direction] = min_potential_count / 5;
    if ((min_potential_count >= 2) && (min_potential_count < 10)) {
        bridge[x][y][direction] = default_bridge_value + min_potential_count - 2;
        min_potential -= 32;
    }
    if (min_potential_count < 2) {
        double next_min_potential = 30000;
        for (const double potential: potentials) {
            if ((potential > min_potential) && (next_min_potential > potential)) next_min_potential = potential;
        }
        if (next_min_potential <= min_potential + 104) {
            bridge[x][y][direction] = default_bridge_value - (next_min_potential - min_potential) / 4;
            min_potential -= 64;
        }
        min_potential += next_min_potential;
        min_potential /= 2;
    }

    if ((x > 0) && (x < BOARDSIZE - 1) && (y > 0) && (y < BOARDSIZE - 1)) {
        bridge[x][y][direction] += add_bonus;
    } else {
        bridge[x][y][direction] -= 2;
    }

    if (((x == 0) || (x == BOARDSIZE - 1)) && ((y == 0) || (y == BOARDSIZE - 1))) {
        bridge[x][y][direction] /= 2;
    } // /=4

    if (bridge[x][y][direction] > 68) {
        bridge[x][y][direction] = 68;
    } //66

    if (board[x][y].getColour() == colour) {
        if (min_potential < potential[x][y][direction]) {
            potential[x][y][direction] = min_potential;
            set_update(x + 1, y);
            set_update(x, y + 1);
            set_update(x - 1, y + 1);
            set_update(x - 1, y);
            set_update(x, y - 1);
            set_update(x + 1, y - 1);
            return (1);
        }
        return (0);
    }
    if (constexpr int default_potential_increment = 140;
        min_potential + default_potential_increment < potential[x][y][direction]) {
        potential[x][y][direction] = min_potential + default_potential_increment;
        set_update(x + 1, y);
        set_update(x, y + 1);
        set_update(x - 1, y + 1);
        set_update(x - 1, y);
        set_update(x, y - 1);
        set_update(x + 1, y - 1);
        return (1);
    }
    return (0);
}

void BestMove::calculate_potential() {
    // Initialize all potentials and bridges
    for (int row = 0; row < BOARDSIZE; row++) {
        for (int col = 0; col < BOARDSIZE; col++) {
            for (int direction = 0; direction < 4; direction++) {
                potential[row][col][direction] = 20000; // Set initial potential values to a large number
                bridge[row][col][direction] = 0; // Set bridge status to 0 (no bridge)
            }
        }
    }

    // Initialize border conditions for potential values
    for (int row = 0; row < BOARDSIZE; row++) {
        // Handle blue border (left column)
        if (board[row][0].getColour() == "0") {
            potential[row][0][0] = 128; // Blue border
        } else if (board[row][0].getColour() == "B") {
            potential[row][0][0] = 0; // Set potential to 0 if it's a blue piece
        }

        // Handle blue border (right column)
        if (board[row][BOARDSIZE - 1].getColour() == "0") {
            potential[row][BOARDSIZE - 1][1] = 128; // Blue border
        } else if (board[row][BOARDSIZE - 1].getColour() == "B") {
            potential[row][BOARDSIZE - 1][1] = 0; // Set potential to 0 if it's a blue piece
        }
    }

    for (int col = 0; col < BOARDSIZE; col++) {
        // Handle red border (top row)
        if (board[0][col].getColour() == "0") {
            potential[0][col][2] = 128; // Red border
        } else if (board[0][col].getColour() < "R") {
            potential[0][col][2] = 0; // Set potential to 0 if it's a red piece
        }

        // Handle red border (bottom row)
        if (board[BOARDSIZE - 1][col].getColour() == "0") {
            potential[BOARDSIZE - 1][col][3] = 128; // Red border
        } else if (board[BOARDSIZE - 1][col].getColour() == "R") {
            potential[BOARDSIZE - 1][col][3] = 0; // Set potential to 0 if it's a red piece
        }
    }

    int potential_change = 0;
    constexpr int max_iterations = 12;
    // Update blue potential values
    for (int direction = 0; direction < 2; direction++) {
        // Blue potential directions (0 and 1)
        // Mark all cells as needing an update
        for (auto & row : update) {
            for (bool & col : row) {
                col = true;
            }
        }

        int iteration = 0;
        do {
            iteration++;
            potential_change = 0;

            // Traverse the board and update potential values for blue
            for (int row = 0; row < BOARDSIZE; row++) {
                for (int col = 0; col < BOARDSIZE; col++) {
                    if (update[row][col]) {
                        potential_change += set_potential(row, col, direction, "B"); // Update blue potential
                    }
                }
            }

            // Traverse the board in reverse order to update blue potential
            for (int row = BOARDSIZE - 1; row >= 0; row--) {
                for (int col = BOARDSIZE - 1; col >= 0; col--) {
                    if (update[row][col]) {
                        potential_change += set_potential(row, col, direction, "B"); // Update blue potential
                    }
                }
            }
        } while (potential_change > 0 && iteration < max_iterations);
        // Stop if no more changes or iteration limit reached
    }

    // Update red potential values
    for (int direction = 2; direction < 4; direction++) {
        // Red potential directions (2 and 3)
        // Mark all cells as needing an update
        for (auto & row : update) {
            for (bool & col : row) {
                col = true;
            }
        }

        int iteration = 0;
        do {
            iteration++;
            potential_change = 0;

            // Traverse the board and update potential values for red
            for (int row = 0; row < BOARDSIZE; row++) {
                for (int col = 0; col < BOARDSIZE; col++) {
                    if (update[row][col]) {
                        potential_change += set_potential(row, col, direction, "R"); // Update red potential
                    }
                }
            }

            // Traverse the board in reverse order to update red potential
            for (int row = BOARDSIZE - 1; row >= 0; row--) {
                for (int col = BOARDSIZE - 1; col >= 0; col--) {
                    if (update[row][col]) {
                        potential_change += set_potential(row, col, direction, "R"); // Update red potential
                    }
                }
            }
        } while (potential_change > 0 && iteration < max_iterations);
        // Stop if no more changes or iteration limit reached
    }
}

std::pair<int, int> BestMove::get_best_move() {
    int best_row = 0;
    int best_col = 0;
    int connectivity;
    double min_potential = 30000;
    auto [row_sum, col_sum] = get_direction_sums();
    for (int row = 0; row < BOARDSIZE; row++) {
        for (int col = 0; col < BOARDSIZE; col++) {
            // Only consider empty cells
            if (board[row][col].getColour() == "0") {
                const double move_potential = calculate_move_potential(row, col, row_sum, col_sum);
                // Update the potentials
                potentials[row * BOARDSIZE + col] = move_potential;

                // If the move potential is less than the minimum potential, update the minimum potential and best row and column
                if (move_potential < min_potential) {
                    min_potential = move_potential;
                    best_row = row;
                    best_col = col;
                }
            }
        }
    }

    min_potential += 108;
    // Loop to check for bridge positions
    // If there is check if it has a potential less than the one found so far
    // If so take that
    for (int row = 0; row < BOARDSIZE; row++) {
        for (int col = 0; col < BOARDSIZE; col++) {
            // Check if the current position has a lower potential (better)
            if (potentials[row * BOARDSIZE + col] < min_potential) {
                if (colour == "R") //red
                {
                    if (is_top_left_region(row, col)) {
                        if (is_north_east_bridge_position(row, col)) {
                            // Decide if the position can connect to the far border
                            connectivity = can_connect_far_border(row - 1, col + 2, colour);
                            if (connectivity < 2) {
                                // If it can connect
                                best_row = row;
                                if (connectivity < -1) {
                                    // An indirect connection exists
                                    best_row--;
                                    connectivity++;
                                }
                                best_col = col - connectivity;
                                min_potential = potentials[row * BOARDSIZE + col];
                            }
                        }
                    }
                    if (is_on_top_left_border(row, col)) {
                        if (is_north_east_bridge_with_clear_path(row, col)) {
                            // Place a bridge in the North-East direction
                            best_row = row;
                            best_col = col;
                            min_potential = potentials[row * BOARDSIZE + col];
                        }
                    }
                    if (is_bottom_right_region(row, col)) {
                        if (is_south_west_bridge_position(row, col)) {
                            // Decide if the position can connect to the far border
                            connectivity = can_connect_far_border(row + 1, col - 2, opp(colour));
                            if (connectivity < 2) {
                                // If it can connect
                                best_row = row;
                                if (connectivity < -1) {
                                    // An indirect connection exists
                                    best_row++;
                                    connectivity++;
                                }
                                best_col = col + connectivity;
                                min_potential = potentials[row * BOARDSIZE + col];
                            }
                        }
                    }
                    if (is_on_bottom_right_border(row, col)) {
                        if (is_south_west_bridge_with_clear_path(row, col)) {
                            // Place a bridge in the South-West direction
                            best_row = row;
                            best_col = col;
                            min_potential = potentials[row * BOARDSIZE + col];
                        }
                    }
                } else {
                    if (is_top_right_region(row, col)) {
                        if (is_south_east_bridge_position(row, col)) {
                            // Decide if the position can connect to the far border
                            connectivity = can_connect_far_border(row + 2, col - 1, opp(colour));
                            if (connectivity < 2) {
                                // If it can connect
                                best_col = col;
                                if (connectivity < -1) {
                                    // An indirect connection exists
                                    best_col--;
                                    connectivity++;
                                }
                                best_row = row - connectivity;
                                min_potential = potentials[row * BOARDSIZE + col];
                            }
                        }
                    }
                    if (is_on_top_right_border(row, col)) {
                        if (is_south_east_bridge_with_clear_path(row, col)) {
                            // Place a bridge in the South-East direction
                            best_row = row;
                            best_col = col;
                            min_potential = potentials[row * BOARDSIZE + col];
                        }
                    }
                    if (is_bottom_left_region(row, col)) {
                        if (is_north_west_bridge_position(row, col)) {
                            // Decide if the position can connect to the far border
                            connectivity = can_connect_far_border(row - 2, col + 1, opp(colour));
                            if (connectivity < 2) {
                                // If it can connect
                                best_col = col;
                                if (connectivity < -1) {
                                    // An indirect connection exists
                                    best_col++;
                                    connectivity++;
                                }
                                best_row = row + connectivity;
                                min_potential = potentials[row * BOARDSIZE + col];
                            }
                        }
                    }
                    if (is_on_bottom_left_border(row, col)) {
                        if (is_north_west_bridge_with_clear_path(row, col)) {
                            // Place a bridge in the North-West direction
                            best_row = row;
                            best_col = col;
                            min_potential = potentials[row * BOARDSIZE + col];
                        }
                    }
                }
            }
        }
    }
    return {best_row, best_col};
}
