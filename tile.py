from constants import Player, TileNeighbours
from src.Colour import Colour

class BoardTile():
    def __init__(self, x, y):
        self.x = x
        self.y = y

        self.row = None

        self.abandoned = False

        self.relative_neighbours = [None, None, None, None, None, None]
        self.fixed_neighbours = [None, None, None, None, None, None]

        self.occupied_by = None
        self.bridgable = False
        self.createsBridge = False

    def add_neighbours(self, board):
        # Fixed neighbours
        self.fixed_neighbours[TileNeighbours.NORTH_WEST] = board.get_tile(self.x, self.y-1)
        self.fixed_neighbours[TileNeighbours.NORTH_EAST] = board.get_tile(self.x+1, self.y-1)
        self.fixed_neighbours[TileNeighbours.EAST] = board.get_tile(self.x+1, self.y)
        self.fixed_neighbours[TileNeighbours.SOUTH_EAST] = board.get_tile(self.x, self.y+1)
        self.fixed_neighbours[TileNeighbours.SOUTH_WEST] = board.get_tile(self.x-1, self.y+1)
        self.fixed_neighbours[TileNeighbours.WEST] = board.get_tile(self.x-1, self.y)
        # Relative neighbours
        if self.row > 0 and board.opponent_colour == Colour.RED:
            self.relative_neighbours[TileNeighbours.NORTH_WEST] = board.get_tile(self.x, self.y-1)
            self.relative_neighbours[TileNeighbours.NORTH_EAST] = board.get_tile(self.x+1, self.y-1)
            self.relative_neighbours[TileNeighbours.EAST] = board.get_tile(self.x+1, self.y)
            self.relative_neighbours[TileNeighbours.SOUTH_EAST] = board.get_tile(self.x, self.y+1)
            self.relative_neighbours[TileNeighbours.SOUTH_WEST] = board.get_tile(self.x-1, self.y+1)
            self.relative_neighbours[TileNeighbours.WEST] = board.get_tile(self.x-1, self.y)
        elif self.row < 0 and board.opponent_colour == Colour.RED:
            self.relative_neighbours[TileNeighbours.NORTH_WEST]= board.get_tile(self.x, self.y+1)
            self.relative_neighbours[TileNeighbours.NORTH_EAST]= board.get_tile(self.x-1, self.y+1)
            self.relative_neighbours[TileNeighbours.EAST]= board.get_tile(self.x-1, self.y)
            self.relative_neighbours[TileNeighbours.SOUTH_EAST]= board.get_tile(self.x, self.y-1)
            self.relative_neighbours[TileNeighbours.SOUTH_WEST]= board.get_tile(self.x+1, self.y-1)
            self.relative_neighbours[TileNeighbours.WEST]= board.get_tile(self.x+1, self.y)
        elif self.row > 0 and board.opponent_colour == Colour.BLUE:
            self.relative_neighbours[TileNeighbours.NORTH_WEST] = board.get_tile(self.x-1, self.y+1)
            self.relative_neighbours[TileNeighbours.NORTH_EAST] = board.get_tile(self.x-1, self.y)
            self.relative_neighbours[TileNeighbours.EAST] = board.get_tile(self.x, self.y-1)
            self.relative_neighbours[TileNeighbours.SOUTH_EAST] = board.get_tile(self.x+1, self.y-1)
            self.relative_neighbours[TileNeighbours.SOUTH_WEST] = board.get_tile(self.x+1, self.y)
            self.relative_neighbours[TileNeighbours.WEST] = board.get_tile(self.x, self.y+1)
        elif self.row < 0 and board.opponent_colour == Colour.BLUE:
            self.relative_neighbours[TileNeighbours.NORTH_WEST] = board.get_tile(self.x+1, self.y-1)
            self.relative_neighbours[TileNeighbours.NORTH_EAST] = board.get_tile(self.x+1, self.y)
            self.relative_neighbours[TileNeighbours.EAST] = board.get_tile(self.x, self.y+1)
            self.relative_neighbours[TileNeighbours.SOUTH_EAST] = board.get_tile(self.x-1, self.y+1)
            self.relative_neighbours[TileNeighbours.SOUTH_WEST] = board.get_tile(self.x-1, self.y)
            self.relative_neighbours[TileNeighbours.WEST] = board.get_tile(self.x, self.y-1)