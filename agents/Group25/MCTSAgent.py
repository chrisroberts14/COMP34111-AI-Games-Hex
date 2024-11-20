import math
import random
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
        random.shuffle(valid_moves)
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
        while time.time() - start < 5:
            iteration_count += 1
            # Use the tree policy to select the best node
            # Uses UCT to select the best node
            child_to_expand = root.best_child(c=1)
            # Simulate the game until the end
            reward = child_to_expand.simulate_from_node(get_opponent_colour(self.colour))
            # Backpropagate the result
            child_to_expand.backpropagate(reward)

        print(f"Iteration count new: {iteration_count}")
        best_child = root.best_child(c=0)
        return Move(best_child.move[0], best_child.move[1])
