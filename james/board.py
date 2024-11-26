import template
from tile import Tile
from src.Colour import Colour
from constants import Player, Neighbour
from rich.console import Console
from rich.text import Text

class Board:
    def __init__(self, opponentColour):
        self.array = []
        self.opponentColour = opponentColour

        # Declare 2D array of nodes
        for row in range(11):
            self.row = []
            for column in range(11):
                self.row.append(Tile(x=column, y=row))
            self.array.append(self.row)

        # Determine row
        if opponentColour == Colour.RED:
            for x in range(11):
                for y in range(0,5):
                    self.getNode(x, y).row = -y - 1

            for x in range(11):
                for y in range(5,11):
                    self.getNode(x, y).row = 11 - y

        elif opponentColour == Colour.BLUE:
            for x in range(0,5):
                for y in range(11):
                    self.getNode(x, y).row = -x - 1

            for x in range(5,11):
                for y in range(11):
                    self.getNode(x, y).row = 11 - x

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

    def showAbandoned(self) -> str:
        """
        Print the board with colours using rich to colour the characters
        """
        console = Console()
        output = ""
        leading_spaces = ""
        for row in self.array:
            text_line = Text(leading_spaces)  # Start with leading spaces
            leading_spaces += " "
            for node in row:
                if node.occupiedBy == Player.SELF:
                    if self.opponentColour == Colour.RED:
                        text_line.append("B" + " ", style="bold blue")
                    else:
                        text_line.append("R" + " ", style="bold red")
                elif node.occupiedBy == Player.OPPONENT:
                    if self.opponentColour == Colour.RED:
                        text_line.append("R" + " ", style="bold red")
                    else:
                        text_line.append("B" + " ", style="bold blue")
                elif node.abandoned:
                    text_line.append("A" + " ", style="bold orange1")
                else:
                    text_line.append("0" + " ", style="bold white")

            output += text_line.plain + "\n"  # Append line to output
            console.print(text_line)  # Print the line with colors

    def getNode(self,x,y):
        if x < 0 or x > 10 or y < 0 or y > 10:
            return None
        else:
            return self.array[y][x]

    def selfMove(self, x, y):
        self.getNode(x, y).occupiedBy = Player.SELF

    def opponentMove(self, x, y):
        self.getNode(x, y).occupiedBy = Player.OPPONENT
        self.evaluate(self.getNode(x, y))

    def evaluate(self, node): 
        if (abs(node.row) == 2):
            if (self.compare(template.row2Template.node, node)):
                self.abandon(node, template.row2Template.node)
        

    def compare(self, templateNode, node):
        for neighbour in Neighbour:
            if templateNode.neighbours[neighbour] == None:
                continue
            elif templateNode.neighbours[neighbour].occupiedBy != node.neighbours[neighbour].occupiedBy:
                return False
            elif templateNode.neighbours[neighbour].occupiedBy == node.neighbours[neighbour].occupiedBy:
                self.compare(templateNode.neighbours[neighbour], node.neighbours[neighbour])
        return True
    
    def abandon(self, node, templateNode):
        for neighbour in Neighbour:
            if templateNode.neighbours[neighbour] == None:
                continue
            else:
                node.neighbours[neighbour].abandoned = True
                self.abandon(node.neighbours[neighbour], templateNode.neighbours[neighbour])
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

board = Board(Colour.RED)
print(board.rowNumbers())
board.opponentMove(6,9)
board.opponentMove(4,9)
board.opponentMove(4,1)

board = Board(Colour.BLUE)
print(board.rowNumbers())
board.opponentMove(9,6)
board.opponentMove(9,4)
board.opponentMove(1,4)


board.showAbandoned()