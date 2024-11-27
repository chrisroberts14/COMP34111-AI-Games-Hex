from constants import Player

class Template:
    def __init__(self, row, tile_of_origin: 'TemplateTile'):
        self.row = row
        self.tile_of_origin = tile_of_origin

class TemplateTile:
    def __init__(self, north_west_neighbour=None, north_east_neighbour=None, east_neighbour=None, south_east_neighbour=None, south_west_neighbour=None, west_neighbour=None):
        self.neighbours = [north_west_neighbour, north_east_neighbour, east_neighbour, south_east_neighbour, south_west_neighbour, west_neighbour]

class EmptyTile(TemplateTile):
    def __init__(self, north_west_neighbour=None, north_east_neighbour=None, east_neighbour=None, south_east_neighbour=None, south_west_neighbour=None, west_neighbour=None):
        super().__init__(north_west_neighbour, north_east_neighbour, east_neighbour, south_east_neighbour, south_west_neighbour, west_neighbour)
        self.occupied_by = None

class OpponentTile(TemplateTile):
    def __init__(self, north_west_neighbour=None, north_east_neighbour=None, east_neighbour=None, south_east_neighbour=None, south_west_neighbour=None, west_neighbour=None):
        super().__init__(north_west_neighbour, north_east_neighbour, east_neighbour, south_east_neighbour, south_west_neighbour, west_neighbour)
        self.occupied_by = Player.OPPONENT

class PlayerTile(TemplateTile):
    def __init__(self, north_west_neighbour=None, north_east_neighbour=None, east_neighbour=None, south_east_neighbour=None, south_west_neighbour=None, west_neighbour=None):
        super().__init__(north_west_neighbour, north_east_neighbour, east_neighbour, south_east_neighbour, south_west_neighbour, west_neighbour)
        self.occupied_by = Player.SELF

row2Template = Template(2, OpponentTile(south_west_neighbour=EmptyTile(), south_east_neighbour=EmptyTile()))

row3TemplateMirror = Template(3, OpponentTile(south_west_neighbour=EmptyTile(south_west_neighbour=EmptyTile(), south_east_neighbour=EmptyTile()), 
                                            south_east_neighbour=EmptyTile(south_east_neighbour=EmptyTile()),
                                            east_neighbour=EmptyTile(south_east_neighbour=EmptyTile(south_east_neighbour=EmptyTile()))
                                            )
                        )
row3Template = Template(3, OpponentTile(south_west_neighbour=EmptyTile(south_west_neighbour=EmptyTile(), south_east_neighbour=EmptyTile()), 
                                        south_east_neighbour=EmptyTile(south_east_neighbour=EmptyTile()),
                                        west_neighbour=EmptyTile(south_west_neighbour=EmptyTile(south_west_neighbour=EmptyTile()))
                                        )
                        )



