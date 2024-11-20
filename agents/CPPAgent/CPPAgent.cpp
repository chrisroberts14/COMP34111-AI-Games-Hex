#include "MCTSAgent.h"
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr
        << "ERROR: Missing arguments. Provide agent colour and board size."
        << std::endl;
    return 1;
  }

  std::string inputColour = argv[1];
  int parsedSize = std::stoi(argv[2]);

  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  MCTSAgent agent(inputColour, parsedSize);
  agent.run();
  return 0;
}
