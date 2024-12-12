from configparser import ConfigParser

import torch

from hexhex.logic.hexboard import Board
from hexhex.logic.hexgame import MultiHexGame
from hexhex.model.hexconvolution import RandomModel
from hexhex.utils.logger import logger


def create_puzzle(config):
    logger.info("")
    logger.info("=== creating puzzle data from random model ===")

    board_size = config.getint('board_size')
    model = RandomModel(board_size=board_size)

    all_boards_tensor = torch.Tensor()
    all_moves = torch.LongTensor()
    all_results = torch.Tensor()
    for _ in range(config.getint('num_samples', 1000)):
        multihexgame = MultiHexGame((Board(size=board_size),), (model,), temperature=1, 
            temperature_decay=1, noise=None, noise_parameters=None)
        board_states, moves, targets = multihexgame.play_moves()
        all_boards_tensor = torch.cat((all_boards_tensor, board_states[-2:]))
        all_moves = torch.cat((all_moves, moves[-2:]))
        all_results = torch.cat((all_results, targets[-2:]))

    filename = f'data/{board_size}_puzzle.pt'
    torch.save((all_boards_tensor, all_moves, all_results), filename)
    logger.info("Wrote " + filename)


def _main():
    config = ConfigParser()
    config.read('config.ini')
    create_puzzle(config['CREATE PUZZLE'])


if __name__ == '__main__':
    _main()