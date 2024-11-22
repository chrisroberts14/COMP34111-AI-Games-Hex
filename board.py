import template
from tile import Tile
from src.Colour import Colour
from constants import Player, Neighbour

class Board:
    def __init__(self, opponentColour):
        self.array = []
        self.opponentColour = opponentColour

        # Declare 2D array of nodes
        for x in range(11):
            self.row = []
            for y in range(11):
                self.row.append(Tile(x, y))
            self.array.append(self.row)

        # Determine row
        if opponentColour == Colour.RED:
            for x in range(0,5):
                for y in range(11):
                    self.getNode(x, y).row = -x - 1

            for x in range(5,11):
                for y in range(11):
                    self.getNode(x, y).row = 11 - x
        
        elif opponentColour == Colour.BLUE:
            for x in range(11):
                for y in range(0,5):
                    self.getNode(x, y).row = -y - 1

            for x in range(11):
                for y in range(5,11):
                    self.getNode(x, y).row = 11 - y

        # Initialise nodes
        for x in range(11):
            for y in range(11):
                self.getNode(x, y).add_neighbours(self)

    def rowNumbers(self) -> str:

        output = ""
        leading_spaces = ""
        for row in self.array:
            output += leading_spaces
            leading_spaces += " "
            for node in row:
                output += f"{node.row} "
            output += "\n"

        return output                    

    def getNode(self,x,y):
        if x < 0 or x > 10 or y < 0 or y > 10:
            return None
        else:
            return self.array[x][y]

    def selfMove(self, x, y):
        self.getNode(x, y).occupiedBy = Player.SELF

    def opponentMove(self, x, y):
        self.getNode(x, y).occupiedBy = Player.OPPONENT
        self.evaluate(self.getNode(x, y))

    def evaluate(self, node): 
        if (abs(node.row) == 2):
            self.compare(template.row2Template.node, node)
        

    def compare(self, templateNode, node):
        for neighbour in range(Neighbour):
            if templateNode.neighbours[neighbour] == None:
                continue
            elif templateNode.neighbours[neighbour].occupiedBy != node.neighbours[neighbour].occupiedBy:
                return False
            elif templateNode.neighbours[neighbour].occupiedBy == node.neighbours[neighbour].occupiedBy:
                self.compare(templateNode.neighbours[neighbour], node.neighbours[neighbour])
        return True
                
    def print_board(self) -> str:
        """Returns the string representation of a board."""

        output = ""
        leading_spaces = ""
        for line in self._tiles:
            output += leading_spaces
            leading_spaces += " "
            for tile in line:
                output += Colour.get_char(tile.colour) + " "
            output += "\n"

        return output

    # def getValidMoves(self):
        
        
    # def isWinnable(self):

board = Board(Colour.BLUE)
print(board.rowNumbers())