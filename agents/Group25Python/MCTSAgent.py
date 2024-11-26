import math
from random import randint, choice, shuffle
import time

from src.Board import Board
from src.Colour import Colour
from src.Move import Move
from src.AgentBase import AgentBase
import copy
import numpy as np

from src.Tile import Tile

OPPONENT_COLOUR = {Colour.RED: Colour.BLUE, Colour.BLUE: Colour.RED}
decay_factor = 0.05

class BestMove:
    MAX_ITERATIONS = 12
    DEFAULT_POTENTIAL = 20000
    INITIAL_POTENTIAL = 128
    ZERO_POTENTIAL = 0
    SIZE = 11

    def __init__(self, board, move_count=0, colour=1):
        self.colour = colour
        self.potentials = [0] * 121
        self.neighbour_count = [0, 0, 0, 0, 0, 0]
        self.move_count = move_count
        self.active_colour = 0
        self.size = BestMove.SIZE
        self.board = board
        self.potential = [[[0] * 4 for _ in range(BestMove.SIZE)] for _ in range(BestMove.SIZE)]
        self.bridge = [[[0] * 4 for _ in range(BestMove.SIZE)] for _ in range(BestMove.SIZE)]
        self.update = [[False for _ in range(11)] for _ in range(BestMove.SIZE)]
        self.calculate_potential()

    @staticmethod
    def sign(x):
        if x < 0:
            return -1
        if x > 0:
            return 1
        return 0

    def make_random_move(self):
        random_row = randint(0, 4)
        random_col = randint(0, 4 - random_row)
        if randint(0, 2) < 1:
            random_row = self.size - 1 - random_row
            random_col = self.size - 1 - random_col
        return random_row, random_col

    def swap(self):
        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] != 0:
                    if (row + col < 2) or row + col > 2 * self.size - 4:
                        return -1, -1
                    if row + col == 2 or row + col == 2 * self.size - 4:
                        if randint(0, 2) < 1:
                            return -1, -1
                    return row, col
        return -1, -1

    def ai_move(self):
        if self.move_count == 0:
            return self.make_random_move()
        if self.move_count == 1:
            row, col = self.swap()
            if row != -1 and col != -1:
                return row, col

        self.calculate_potential()
        row, col = self.get_best_move(self.colour)
        return row, col

    def make_move(self, row, col, update_potentials, colour):
        swap_row = row
        swap_col = col

        if self.move_count == 1:
            if self.board[row][col] != 0:
                self.board[row][col] = 0
                swap_row = col
                swap_col = row

        self.board[swap_row][swap_col] = colour

        self.move_count += 1

        if not update_potentials:
            return

        self.calculate_potential()

        if colour < 0:
            if self.potential[row][col][2] > 0 or self.potential[row][col][3] > 0:
                return
        else:
            if self.potential[row][col][0] > 0 or self.potential[row][col][1] > 0:
                return

    def get_direction_sums(self):
        row_sum = 0
        col_sum = 0
        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] != 0:
                    row_sum += 2 * row + 1 - self.size
                    col_sum += 2 * row + 1 - self.size
        return BestMove.sign(row_sum), BestMove.sign(col_sum)

    def calculate_move_potential(self, row, col, factor, row_sum, col_sum):
        move_potential = (abs(row - 5) + abs(col - 5)) * factor
        move_potential += 8 * (row_sum * (row - 5) + col_sum * (col - 5)) / (self.move_count + 1)

        for direction in range(4):
            move_potential -= self.bridge[row][col][direction]

        blue_potential = self.potential[row][col][0] + self.potential[row][col][1]
        red_potential = self.potential[row][col][2] + self.potential[row][col][3]
        move_potential += blue_potential + red_potential

        if blue_potential <= 268 or red_potential <= 268:
            move_potential -= 400

        return move_potential

    def get_best_move(self, colour):
        best_row = -1
        best_col = -1
        factor = 0
        if self.move_count > 0:
            factor = 190 / (self.move_count ** 2)
        min_potential = 30000
        row_sum, col_sum = self.get_direction_sums()

        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] == 0:
                    move_potential = self.calculate_move_potential(row, col, factor, row_sum, col_sum)
                    self.potentials[row * self.size + col] = move_potential
                    if move_potential < min_potential:
                        min_potential = move_potential
                        best_row = row
                        best_col = col

        min_potential += 108
        for row in range(self.size):
            for col in range(self.size):
                if self.potentials[row * self.size + col] < min_potential:
                    if colour < 0:
                        if self.size - 1 > row > 3 > col > 0:
                            if self.board[row - 1][col + 2] == -colour:
                                connectivity = self.can_connect_far_border(row - 1, col + 2, -colour)
                                if connectivity < 2:
                                    best_row = row
                                    if connectivity < -1:
                                        best_row -= 1
                                        connectivity += 1
                                    best_col = col - connectivity
                                    min_potential = self.potentials[row * self.size + col]

                        if self.size - 1 > row > 0 == col:
                            if self.board[row - 1][col + 2] == -colour and self.board[row - 1][col] == 0 and self.board[row - 1][
                                col + 1] == 0 and self.board[row + 1][col] == 0:
                                best_row = row
                                best_col = col
                                min_potential = self.potentials[row * self.size + col]

                        if 0 < row < self.size - 4 and col < self.size - 1 and col < self.size - 4:
                            if self.board[row + 1][col - 2] == -colour:
                                connectivity = self.can_connect_far_border(row + 1, col - 2, -colour)
                                if connectivity < 2:
                                    best_row = row
                                    if connectivity < -1:
                                        best_row += 1
                                        connectivity += 1
                                    best_col = col + connectivity
                                    min_potential = self.potentials[row * self.size + col]

                        if 0 < row < self.size - 1 and col == self.size - 1:
                            if self.board[row + 1][col - 2] == -colour and self.board[row + 1][col] == 0 and self.board[row + 1][
                                col - 1] == 0 and self.board[row][col - 1] == 0 and self.board[row - 1][col] == 0:
                                best_row = row
                                best_col = col
                                min_potential = self.potentials[row * self.size + col]
                    else:
                        if self.size - 1 > col > 3 > row > 0:
                            if self.board[row + 2][col - 1] == -colour:
                                connectivity = self.can_connect_far_border(row + 2, col - 1, -colour)
                                if connectivity < 2:
                                    best_col = col
                                    if connectivity < -1:
                                        best_col -= 1
                                        connectivity += 1
                                    best_row = row - connectivity
                                    min_potential = self.potentials[row * self.size + col]

                        if self.size - 1 > col > 0 == row:
                            if self.board[row + 2][col - 1] == -colour and self.board[row][col - 1] == 0 and self.board[row + 1][
                                col - 1] == 0 and self.board[row + 1][col] == 0 and self.board[row][col + 1] == 0:
                                best_row = row
                                best_col = col
                                min_potential = self.potentials[row * self.size + col]

                        if 0 < col < self.size - 4 < row < self.size - 1:
                            if self.board[row - 2][col + 1] == -colour:
                                connectivity = self.can_connect_far_border(row - 2, col + 1, -colour)
                                if connectivity < 2:
                                    best_col = col
                                    if connectivity < -1:
                                        best_col += 1
                                        connectivity += 1
                                    best_row = row + connectivity
                                    min_potential = self.potentials[row * self.size + col]

                        if 0 < col < self.size - 1 and row == self.size - 1:
                            if self.board[row - 2][col + 1] == -colour and self.board[row][col + 1] == 0 and self.board[row - 1][
                                col + 1] == 0 and self.board[row - 1][col] == 0 and self.board[row][col - 1] == 0:
                                best_row = row
                                best_col = col
                                min_potential = self.potentials[row * self.size + col]
        return best_row, best_col

    def can_connect_far_border(self, row, col, colour):
        if colour > 0:
            if 2 * col < self.size - 1:
                for i in range(self.size):
                    for j in range(col):
                        if j - i < col - row and i + j <= row + col and self.board[i][j] != 0:
                            return 2
                if self.board[row - 1][col] == -colour:
                    return 0
                if self.board[row - 1][col - 1] == -colour:
                    if self.get_board(row + 2, col - 2) == -colour:
                        return 0
                    return -1
                if self.get_board(row + 2, col - 1) == -colour:
                    return -2
            else:
                for i in range(self.size):
                    for j in range(self.size - 1, col, -1):
                        if j - i > col - row and i + j >= row + col and self.board[i][j] != 0:
                            return 2
                    if self.board[row + 1][col] == -colour:
                        return 0
                    if self.board[row + 1][col + 1] == -colour:
                        if self.get_board(row - 2, col + 1) == -colour:
                            return 0
                        return -1
                    if self.get_board(row - 2, col + 1) == -colour:
                        return -2
        else:
            if 2 * row < self.size - 1:
                for i in range(row):
                    for j in range(self.size):
                        if i - j < row - col and i + j <= row + col and self.board[i][j] != 0:
                            return 2
                if self.board[row][col - 1] == -colour:
                    return 0
                if self.board[row - 1][col - 1] == -colour:
                    if self.get_board(row - 1, col + 2) == -colour:
                        return 0
                    return -1
                if self.get_board(row - 1, col + 2) == -colour:
                    return -2
            else:
                for j in range(self.size):
                    for i in range(self.size - 1, row, -1):
                        if i - j > row - col and i + j >= row + col and self.board[i][j] != 0:
                            return 2
                    if self.board[row][col + 1] == -colour:
                        return 0
                    if self.board[row + 1][col + 1] == -colour:
                        if self.get_board(row + 1, col - 2) == -colour:
                            return 0
                        return -1
                    if self.get_board(row + 1, col - 2) == -colour:
                        return -2
        return 1

    def get_board(self, row, col):
        if row < 0:
            return -1
        if col < 0:
            return 1
        if row >= self.size:
            return -1
        if col >= self.size:
            return 1
        return self.board[row][col]

    def initialise_potential_and_bridge(self):
        for row in range(self.size):
            for col in range(self.size):
                for direction in range(4):
                    self.potential[row][col][direction] = BestMove.DEFAULT_POTENTIAL
                    self.bridge[row][col][direction] = 0

        for row in range(self.size):
            if self.board[row][0] == 0:
                self.potential[row][0][0] = BestMove.INITIAL_POTENTIAL
            elif self.board[row][0] > 0:
                self.potential[row][0][0] = 0

            if self.board[row][self.size - 1] == 0:
                self.potential[row][self.size - 1][1] = BestMove.INITIAL_POTENTIAL
            elif self.board[row][self.size - 1] > 0:
                self.potential[row][self.size - 1][1] = 0

        for col in range(self.size):
            if self.board[0][col] == 0:
                self.potential[0][col][2] = BestMove.INITIAL_POTENTIAL
            elif self.board[0][col] < 0:
                self.potential[0][col][2] = 0

            if self.board[self.size - 1][col] == 0:
                self.potential[self.size - 1][col][3] = BestMove.INITIAL_POTENTIAL
            elif self.board[self.size - 1][col] < 0:
                self.potential[self.size - 1][col][3] = 0


    def update_potentials(self, direction, colour):
        for row in range(self.size):
            for col in range(self.size):
                self.update[row][col] = True

        iteration = 0
        potential_change = 1
        while potential_change > 0 and iteration < self.MAX_ITERATIONS:
            iteration += 1
            potential_change = 0
            for row in range(self.size):
                for col in range(self.size):
                    if self.update[row][col]:
                        potential_change += self.set_potential(row, col, direction, colour)
            for row in range(self.size - 1, -1, -1):
                for col in range(self.size - 1, -1, -1):
                    if self.update[row][col]:
                        potential_change += self.set_potential(row, col, direction, colour)

    def calculate_potential(self):
        self.active_colour = ((self.move_count + 1) % 2) * 2 - 1
        self.initialise_potential_and_bridge()

        for direction in range(2):
            self.update_potentials(direction, 1)

        for direction in range(2, 4):
            self.update_potentials(direction, -1)

    def set_potential(self, row, col, direction: int, colour):
        self.update[row][col] = False
        self.bridge[row][col][direction] = 0
        if self.board[row][col] == -colour:
            return 0
        add_bonus = 0
        default_potential_increment = 140
        min_potential = BestMove.DEFAULT_POTENTIAL
        default_bridge_value = 66
        if self.active_colour != colour:
            default_bridge_value = 52

        self.potentials[0] = self.potential_value(row + 1, col, direction, colour)
        self.potentials[1] = self.potential_value(row, col + 1, direction, colour)
        self.potentials[2] = self.potential_value(row - 1, col + 1, direction, colour)
        self.potentials[3] = self.potential_value(row - 1, col, direction, colour)
        self.potentials[4] = self.potential_value(row, col - 1, direction, colour)
        self.potentials[5] = self.potential_value(row + 1, col - 1, direction, colour)

        for i in range(6):
            if self.potentials[i] >= 30000 and self.potentials[(i + 2) % 6] >= 30000:
                if self.potentials[(i + 1) % 6] < 0:
                    add_bonus += 32
                else:
                    self.potentials[(i + 1) % 6] += BestMove.INITIAL_POTENTIAL

        for i in range(6):
            if self.potentials[i] >= 30000 and self.potentials[(i + 3) % 6] >= 30000:
                add_bonus += 30

        for i in range(6):
            if self.potentials[i] < 0:
                self.potentials[i] += 30000
                self.neighbour_count[i] = 10
            else:
                self.neighbour_count[i] = 1
            min_potential = min(min_potential, self.potentials[i])

        min_potential_count = 0
        for i in range(6):
            if self.potentials[i] == min_potential:
                min_potential_count += self.neighbour_count[i]

        self.bridge[row][col][direction] = min_potential_count / 5
        if 2 <= min_potential_count < 10:
            self.bridge[row][col][direction] = default_bridge_value + min_potential_count - 2
            min_potential -= 32

        if min_potential_count < 2:
            next_min_potential = 30000
            for i in range(6):
                if min_potential < self.potentials[i] < next_min_potential:
                    next_min_potential = self.potentials[i]
            if next_min_potential <= min_potential + 104:
                self.bridge[row][col][direction] = default_bridge_value - (next_min_potential - min_potential) / 4
                min_potential -= 64
            min_potential += next_min_potential
            min_potential /= 2

        if 0 < row < self.size - 1 and 0 < col < self.size - 1:
            self.bridge[row][col][direction] += add_bonus
        else:
            self.bridge[row][col][direction] -= 2

        if (row == 0 or row == self.size - 1) and (col == 0 or col == self.size - 1):
            self.bridge[row][col][direction] /= 2

        if self.bridge[row][col][direction] > 68:
            self.bridge[row][col][direction] = 68

        if self.board[row][col] == colour:
            if min_potential < self.potential[row][col][direction]:
                self.potential[row][col][direction] = min_potential
                self.set_update(row + 1, col)
                self.set_update(row, col + 1)
                self.set_update(row - 1, col + 1)
                self.set_update(row - 1, col)
                self.set_update(row, col - 1)
                self.set_update(row + 1, col - 1)
                return 1
            return 0
        if min_potential + default_potential_increment < self.potential[row][col][direction]:
            self.potential[row][col][direction] = min_potential + default_potential_increment
            self.set_update(row + 1, col)
            self.set_update(row, col + 1)
            self.set_update(row - 1, col + 1)
            self.set_update(row - 1, col)
            self.set_update(row, col - 1)
            self.set_update(row + 1, col - 1)
            return 1
        return 0

    def potential_value(self, row, col, direction, colour):
        if row < 0 or col < 0 or row >= self.size or col >= self.size:
            return 30000
        if self.board[row][col] == 0:
            return self.potential[row][col][direction]
        if self.board[row][col] == -colour:
            return 30000
        return self.potential[row][col][direction] - 30000

    def set_update(self, row, col):
        if row < 0 or col < 0 or row >= self.size or col >= self.size:
            return
        self.update[row][col] = True

    def print_board(self):
        """
        print the board like a hex board
        """
        for i in range(11):
            print(" " * i, end="")
            for j in range(11):
                print(f"{self.board[i][j]}", end=" ")
            print()

    def get_random_move(self):
        moves = [(x, y) for x in range(11) for y in range(11) if self.board[x][y] == 0]
        return choice(moves)

def get_valid_moves(state: Board) -> list[tuple]:
    """
    Get all valid moves for the given state
    """

    return [(i, j) for i in range(state.size) for j in range(state.size) if state.tiles[i][j].colour is None]

def get_opponent_colour(colour: Colour) -> Colour:
    """
    Get the opponent colour
    """
    return OPPONENT_COLOUR[colour]

class MCTSNode:
    """
    Node for MCTS
    """
    def __init__(self, state: Board, agent_colour: Colour, parent=None, move=None):
        """
        Initialize the node
        Arguments:
            state: State of the board in this node
            parent: Parent node
            move: The move that was made to reach this node
        """
        self.agent_colour = agent_colour
        self.state: Board = state
        self.parent: MCTSNode = parent
        self.children: list[MCTSNode] = []
        self.visits: int = 0
        self.payoff_sum: int = 0
        # Use tuples for the move to avoid overhead of using Move objects
        # In the form (x, y)
        self.move: tuple = move
        self.valid_moves: list[tuple] = get_valid_moves(state)

    def has_ended(self):
        """Checks if the game has ended. It will attempt to find a red chain
        from top to bottom or a blue chain from left to right of the board.
        """
        tiles = self.state.tiles
        size = self.state.size
        visited = set()

        red_tiles = [tiles[0][idx] for idx in range(size) if tiles[0][idx]._colour == Colour.RED]
        blue_tiles = [tiles[idx][0] for idx in range(size) if tiles[idx][0]._colour == Colour.BLUE]

        def DFS_colour(x, y, colour):
            """A recursive DFS method that iterates through connected same-colour
            tiles until it finds a bottom tile (Red) or a right tile (Blue).
            """
            stack = [(x, y)]
            visited.add((x, y))

            while stack:
                cx, cy = stack.pop()

                # win conditions
                if colour == Colour.RED and cx == size - 1:
                    return True
                elif colour == Colour.BLUE and cy == size - 1:
                    return True

                # visit neighbours
                for dx, dy in zip(Tile.I_DISPLACEMENTS, Tile.J_DISPLACEMENTS):
                    x_n, y_n = cx + dx, cy + dy
                    if 0 <= x_n < size and 0 <= y_n < size:
                        if (x_n, y_n) not in visited and tiles[x_n][y_n]._colour == colour:
                            visited.add((x_n, y_n))
                            stack.append((x_n, y_n))

            return False

        for tile in red_tiles:
            if (tile._x, tile._y) not in visited:
                if DFS_colour(0, tile._y, Colour.RED):
                    return True
        for tile in blue_tiles:
            if (tile._x, tile._y) not in visited:
                if DFS_colour(tile._x, 0, Colour.BLUE):
                    return True

        return False

    def generate_all_children_nodes(self, player_colour: Colour):
        """
        Generate all children nodes
        Input:
            player_colour: Colour of the player
        """
        # Create all children nodes with the same state as the parent
        def conv_colour(col):
            if col is None:
                return 0
            elif col == Colour.RED:
                return -1
            else:
                return 1

        # number of tiles already placed
        self.children = [
            MCTSNode(
                state=copy.deepcopy(self.state),  # Deepcopy here is unavoidable
                agent_colour=self.agent_colour,
                parent=self,
                move=move,
            ) for move in self.valid_moves
        ]

        # Apply the move to each of the children nodes
        for child, move in zip(self.children, self.valid_moves):
            child.state.tiles[move[0]][move[1]].colour = player_colour

    def backpropagate(self, result: float):
        """
        Update the nodes in the tree with the result of the simulation
        Arguments:
            result: 1 if the player won, -1 if the opponent won
        """
        node = self
        while node is not None:
            node.visits += 1
            node.payoff_sum += result
            node = node.parent

    def simulate_from_node(self, current_colour: Colour) -> float:
        """Simulate the game until the end with random moves

        Arguments:
            current_colour: Colour of the current player
        Returns:
            Reward for the current player
        """
        moves_taken = []
        valid_moves = get_valid_moves(self.state)

        if self.has_ended():
            if get_opponent_colour(current_colour) == self.agent_colour:
                return 1
            return -1
        # Decide all the moves before the loop
        shuffle(valid_moves)
        i = 0
        while True:
            # Faster to remove the move from the list than to generate a new list every move
            move = valid_moves[i]
            i += 1
            moves_taken.append(move)

            # Do the move
            self.state.tiles[move[0]][move[1]].colour = current_colour
            # Check if the game has ended every other move and if it is check the move before too
            if self.has_ended():
                for move in moves_taken:
                    self.state.tiles[move[0]][move[1]].colour = None
                return (math.exp(-decay_factor * len(moves_taken))) if self.agent_colour == current_colour else -1 * math.exp(-decay_factor * len(moves_taken))

            # Switch the player for the next move
            current_colour = get_opponent_colour(current_colour)


    def best_child(self, c: float):
        """
        Select the best child node based on UCT
        Arguments:
            c: Exploration parameter
        """
        values = np.array([child.payoff_sum for child in self.children])
        visits = np.array([child.visits for child in self.children])

        # If there are unexplored nodes we will always choose one of them so don't bother with UCT
        unexplored = np.where(visits == 0)[0]
        if unexplored.size > 0:
            best_index = unexplored[np.random.randint(0, unexplored.size)]
            return self.children[best_index]

        # Calculate the UCT value for each child and select the best one
        exploitation = values / visits
        exploration = c * np.sqrt(np.log(self.visits) / visits)
        ucb1_values = exploitation + exploration
        return self.children[np.argmax(ucb1_values)]


class MCTSAgent(AgentBase):
    """
    Monte Carlo Tree Search Agent
    """

    _choices: list[Move]
    _board_size: int = 11

    def __init__(self, colour: Colour):
        super().__init__(colour)
        self._choices = [
            (i, j) for i in range(self._board_size) for j in range(self._board_size)
        ]


    def make_move(self, turn: int, board: Board, opp_move: Move | None) -> Move:
        """
        Make move based on MCTS
        Arguments:
            turn: Current turn number
            board: Current state of the board
            opp_move: Move made by the opponent
        """
        # At the moment always switch on the second turn
        # TODO: Implement a better way to decide if we should swap
        if turn == 2:
            return Move(-1, -1)

        root = MCTSNode(state=board, agent_colour=self.colour)
        root.generate_all_children_nodes(self.colour)



        # Should use some time limit here based on how much time we have left
        start = time.time()
        iteration_count = 0

        while time.time() - start < 10:
            iteration_count += 1
            # Use the tree policy to select the best node
            # Uses UCT to select the best node
            child_to_expand = root.best_child(c=1.41)
            # Simulate the game until the end
            reward = child_to_expand.simulate_from_node(get_opponent_colour(self.colour))
            # Backpropagate the result
            child_to_expand.backpropagate(reward)

        print(f"Iteration count new: {iteration_count}")
        best_child = root.best_child(c=0)
        print(f"Best child payoff: {best_child.payoff_sum}")
        print(f"Best child visits: {best_child.visits}")
        return Move(best_child.move[0], best_child.move[1])
