//
// Created by chris on 20/11/24.
//

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

#include <string>

class MCTSAgent {
private:
    std::string colour;
    int turn;
    int boardSize;
public:
    MCTSAgent(const std::string& agentColour, int gameBoardSize);

    std::string getMessage();
    void sendMessage(const std::string& msg);
    void run();
    bool interpretMessage(const std::string& s);
    void makeMove(const std::string& board);
};

#endif //MCTSAGENT_H
