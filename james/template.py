from constants import Player, Neighbour

class Node:
    def __init__(self, northWestNeighbour=None, northEastNeighbour=None, eastNeighbour=None, southEastNeighbour=None, southWestNeighbour=None, westNeighbour=None):
        self.neighbours = [northWestNeighbour, northEastNeighbour, eastNeighbour, southEastNeighbour, southWestNeighbour, westNeighbour]

class EmptyNode(Node):
    def __init__(self, northWestNeighbour=None, northEastNeighbour=None, eastNeighbour=None, southEastNeighbour=None, southWestNeighbour=None, westNeighbour=None):
        super().__init__(northWestNeighbour, northEastNeighbour, eastNeighbour, southEastNeighbour, southWestNeighbour, westNeighbour)
        self.occupiedBy = None

class OpponentNode(Node):
    def __init__(self, northWestNeighbour=None, northEastNeighbour=None, eastNeighbour=None, southEastNeighbour=None, southWestNeighbour=None, westNeighbour=None):
        super().__init__(northWestNeighbour, northEastNeighbour, eastNeighbour, southEastNeighbour, southWestNeighbour, westNeighbour)
        self.occupiedBy = Player.OPPONENT

class SelfNode(Node):
    def __init__(self, northWestNeighbour=None, northEastNeighbour=None, eastNeighbour=None, southEastNeighbour=None, southWestNeighbour=None, westNeighbour=None):
        super().__init__(northWestNeighbour, northEastNeighbour, eastNeighbour, southEastNeighbour, southWestNeighbour, westNeighbour)
        self.occupiedBy = Player.SELF

class Template:
    def __init__(self, row, node):
        self.row = row
        self.node = node

row2Template = Template(2, OpponentNode(southWestNeighbour=EmptyNode(), southEastNeighbour=EmptyNode()))


