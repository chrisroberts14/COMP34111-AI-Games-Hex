from constants import Player, Neighbour
from src.Colour import Colour

class Tile:
    def __init__(self, x, y):
        self.x = x
        self.y = y

        self.row = None

        self.abandoned = False

        self.neighbours = [None, None, None, None, None, None]

        self.occupiedBy = None
        self.bridgable = False
        self.createsBridge = False

    def add_neighbours(self, board):
        if self.row > 0 and board.opponentColour == Colour.RED:
            self.neighbours[Neighbour.NORTH_WEST] = board.getNode(self.x, self.y-1)
            self.neighbours[Neighbour.NORTH_EAST] = board.getNode(self.x+1, self.y-1)
            self.neighbours[Neighbour.EAST] = board.getNode(self.x+1, self.y)
            self.neighbours[Neighbour.SOUTH_EAST] = board.getNode(self.x, self.y+1)
            self.neighbours[Neighbour.SOUTH_WEST] = board.getNode(self.x-1, self.y+1)
            self.neighbours[Neighbour.WEST] = board.getNode(self.x-1, self.y)
        elif self.row < 0 and board.opponentColour == Colour.RED:
            self.neighbours[Neighbour.NORTH_WEST]= board.getNode(self.x, self.y+1)
            self.neighbours[Neighbour.NORTH_EAST]= board.getNode(self.x-1, self.y+1)
            self.neighbours[Neighbour.EAST]= board.getNode(self.x-1, self.y)
            self.neighbours[Neighbour.SOUTH_EAST]= board.getNode(self.x, self.y-1)
            self.neighbours[Neighbour.SOUTH_WEST]= board.getNode(self.x+1, self.y-1)
            self.neighbours[Neighbour.WEST]= board.getNode(self.x+1, self.y)
        elif self.row > 0 and board.opponentColour == Colour.BLUE:
            self.neighbours[Neighbour.NORTH_WEST] = board.getNode(self.x-1, self.y+1)
            self.neighbours[Neighbour.NORTH_EAST] = board.getNode(self.x-1, self.y)
            self.neighbours[Neighbour.EAST] = board.getNode(self.x, self.y-1)
            self.neighbours[Neighbour.SOUTH_EAST] = board.getNode(self.x+1, self.y-1)
            self.neighbours[Neighbour.SOUTH_WEST] = board.getNode(self.x+1, self.y)
            self.neighbours[Neighbour.WEST] = board.getNode(self.x, self.y+1)
        elif self.row < 0 and board.opponentColour == Colour.BLUE:
            self.neighbours[Neighbour.NORTH_WEST] = board.getNode(self.x+1, self.y-1)
            self.neighbours[Neighbour.NORTH_EAST] = board.getNode(self.x+1, self.y)
            self.neighbours[Neighbour.EAST] = board.getNode(self.x, self.y+1)
            self.neighbours[Neighbour.SOUTH_EAST] = board.getNode(self.x-1, self.y+1)
            self.neighbours[Neighbour.SOUTH_WEST] = board.getNode(self.x-1, self.y)
            self.neighbours[Neighbour.WEST] = board.getNode(self.x, self.y-1)