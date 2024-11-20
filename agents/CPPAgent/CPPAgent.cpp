#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <memory>
#include <random>
#include <set>
#include <stack>
#include <cmath>
#include <chrono>
#include "Tile.h"
#include "Board.h"
#include "MCTSNode.h"
#include "MCTSAgent.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "ERROR: Missing arguments. Provide agent colour and board size." << std::endl;
        return 1;
    }

    std::string inputColour = argv[1];
    int parsedSize = std::stoi(argv[2]);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    MCTSAgent agent(inputColour, parsedSize);
    agent.run();
    return 0;
}
