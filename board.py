import template
from tile import Tile
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
                row_of_tiles.append(Tile(x=column_index, y=row_index))
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
        look_for_guaranteed_edge_templates(self.get_tile(x, y))
                
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

    # def getValidMoves(self):
        
        
    # def isWinnable(self):

def look_for_guaranteed_edge_templates(played_tile): 
    if (abs(played_tile.row) == 2):
        if (template_exists_on_board(template.row2Template.tile_of_origin, played_tile)):
            abandon_tiles_that_are_empty_in_the_template(played_tile, template.row2Template.tile_of_origin)
    

def template_exists_on_board(template_tile, board_tile):
    for tile_neighbour in TileNeighbours:
        if template_tile.neighbours[tile_neighbour] == None:
            continue
        elif template_tile.neighbours[tile_neighbour].occupied_by != board_tile.neighbours[tile_neighbour].occupied_by:
            return False
        elif template_tile.neighbours[tile_neighbour].occupied_by == board_tile.neighbours[tile_neighbour].occupied_by:
            template_exists_on_board(template_tile.neighbours[tile_neighbour], board_tile.neighbours[tile_neighbour])
    return True

def abandon_tiles_that_are_empty_in_the_template(board_tile, template_tile):
    for tile_neighbour in TileNeighbours:
        if template_tile.neighbours[tile_neighbour] == None:
            continue
        else:
            board_tile.neighbours[tile_neighbour].abandoned = True
            abandon_tiles_that_are_empty_in_the_template(board_tile.neighbours[tile_neighbour], template_tile.neighbours[tile_neighbour])
    return True

board = Board(player_colour=Colour.BLUE)
print(board.display_row_numbers())
board.opponent_makes_move(6,9)
board.opponent_makes_move(4,9)
board.opponent_makes_move(4,1)
board.display_abandoned_tiles()

board = Board(player_colour=Colour.RED)
print(board.display_row_numbers())
board.opponent_makes_move(9,6)
board.opponent_makes_move(9,4)
board.opponent_makes_move(1,4)

board.display_abandoned_tiles()