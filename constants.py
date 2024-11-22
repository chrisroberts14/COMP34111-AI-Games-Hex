from enum import Enum, IntEnum

class Player(Enum):
    SELF = 1
    OPPONENT = 2

class Neighbour(IntEnum):
    NORTH_WEST = 0
    NORTH_EAST = 1
    EAST = 2
    SOUTH_EAST = 3
    SOUTH_WEST = 4
    WEST = 5