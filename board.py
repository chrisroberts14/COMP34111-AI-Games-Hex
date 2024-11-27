from template import Templates
from tile import BoardTile
from src.Colour import Colour
from constants import Player, TileNeighbours
from rich.console import Console
from rich.text import Text

class Board:
    def __init__(self, player_colour: Colour):
        self.array_of_tiles = []
        self.player_colour = player_colour
        self.opponent_colour = player_colour.opposite()

        # Declare 2D array of nodes
        for row_index in range(11):
            row_of_tiles = []
            for column_index in range(11):
                row_of_tiles.append(BoardTile(x=column_index, y=row_index))
            self.array_of_tiles.append(row_of_tiles)

        # Determine row
        if self.opponent_colour == Colour.BLUE:
            for x in range(0,5):
                for y in range(11):
                    self.get_tile(x, y).row = -x - 1

            for x in range(5,11):
                for y in range(11):
                    self.get_tile(x, y).row = 11 - x

        elif self.opponent_colour == Colour.RED:
            for x in range(11):
                for y in range(0,5):
                    self.get_tile(x, y).row = -y - 1

            for x in range(11):
                for y in range(5,11):
                    self.get_tile(x, y).row = 11 - y

        # Initialise nodes
        for x in range(11):
            for y in range(11):
                self.get_tile(x, y).add_neighbours(self)

    def display_row_numbers(self) -> str:

        output = ""
        leading_spaces = ""
        for row in self.array_of_tiles:
            output += leading_spaces
            leading_spaces += " "
            for tile in row:
                output += f"{tile.row} "
            output += "\n"

        return output

    def display_abandoned_tiles(self) -> str:
        """
        Print the board with colours using rich to colour the characters
        """
        console = Console()
        output = ""
        leading_spaces = ""
        for row in self.array_of_tiles:
            text_line = Text(leading_spaces)  # Start with leading spaces
            leading_spaces += " "
            for tile in row:
                if tile.occupied_by == Player.SELF:
                    if self.opponent_colour == Colour.RED:
                        text_line.append("B" + " ", style="bold blue")
                    else:
                        text_line.append("R" + " ", style="bold red")
                elif tile.occupied_by == Player.OPPONENT:
                    if self.opponent_colour == Colour.RED:
                        text_line.append("R" + " ", style="bold red")
                    else:
                        text_line.append("B" + " ", style="bold blue")
                elif tile.abandoned:
                    text_line.append("A" + " ", style="bold orange1")
                else:
                    text_line.append("0" + " ", style="bold white")

            output += text_line.plain + "\n"  # Append line to output
            console.print(text_line)  # Print the line with colors

    def get_tile(self,x,y):
        if x < 0 or x > 10 or y < 0 or y > 10:
            return None
        else:
            return self.array_of_tiles[y][x]

    def player_makes_move(self, x, y):
        self.get_tile(x, y).occupied_by = Player.SELF

    def opponent_makes_move(self, x, y):
        self.get_tile(x, y).occupied_by = Player.OPPONENT
        look_for_guaranteed_edges_for_the_opponent(self.get_tile(x, y))
                
    def print_board(self) -> str:
        """Returns the string representation of a board."""

        output = ""
        leading_spaces = ""
        for row in self._tiles:
            output += leading_spaces
            leading_spaces += " "
            for tile in row:
                output += Colour.get_char(tile.colour) + " "
            output += "\n"

        return output

    def get_valid_moves(self):
        valid_moves = []
        for row in self.array_of_tiles:
            for tile in row:
                if tile.occupied_by == None and tile.abandoned == False:
                    valid_moves.append(tile)
        return valid_moves
        
    # def isWinnable(self):

def look_for_guaranteed_edges_for_the_opponent(played_tile): 
    if (abs(played_tile.row) == 2):
        if (guaranteed_edge_template_exists_on_board(played_tile, Templates.row_2.tile_of_origin)):
            abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(played_tile, Templates.row_2.tile_of_origin)
    elif (abs(played_tile.row) == 3):
        for row_3_template in Templates.row_3:
            if (guaranteed_edge_template_exists_on_board(played_tile, row_3_template.tile_of_origin)):
                abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(played_tile, row_3_template.tile_of_origin)
    

def guaranteed_edge_template_exists_on_board(board_tile, template_tile):
    for tile_neighbour in TileNeighbours:
        if template_tile.neighbours[tile_neighbour] == None:
            continue
        elif template_tile.neighbours[tile_neighbour].occupied_by != board_tile.relative_neighbours[tile_neighbour].occupied_by:
            return False
        elif template_tile.neighbours[tile_neighbour].occupied_by == board_tile.relative_neighbours[tile_neighbour].occupied_by:
            guaranteed_edge_template_exists_on_board(board_tile.relative_neighbours[tile_neighbour], template_tile.neighbours[tile_neighbour], )
    return True

def abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(board_tile, template_tile):
    for tile_neighbour in TileNeighbours:
        if template_tile.neighbours[tile_neighbour] == None:
            continue
        else:
            board_tile.relative_neighbours[tile_neighbour].abandoned = True
            abandon_tiles_that_are_empty_in_the_guaranteed_edge_template(board_tile.relative_neighbours[tile_neighbour], template_tile.neighbours[tile_neighbour])
    return True

board = Board(player_colour=Colour.BLUE)
print(board.display_row_numbers())
board.opponent_makes_move(4,8)
board.opponent_makes_move(8,9)
board.opponent_makes_move(2,1)
board.opponent_makes_move(7,2)
board.display_abandoned_tiles()

board.display_abandoned_tiles()