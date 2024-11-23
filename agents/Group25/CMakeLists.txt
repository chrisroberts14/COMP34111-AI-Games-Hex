cmake_minimum_required(VERSION 3.29.6)
project(CPPAgent)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY Group25)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(CPPAgent
        Board.cpp
        Board.h
        CPPAgent.cpp
        MCTSAgent.cpp
        MCTSAgent.h
        MCTSNode.cpp
        MCTSNode.h
        Tile.cpp
        Tile.h
        BestMove.cpp
        BestMove.h)

# Copy over all files in agents/CPP to the build directory as we submit the whole thing
file(COPY Board.cpp DESTINATION Group25)
file(COPY Board.h DESTINATION Group25)
file(COPY CPPAgent.cpp DESTINATION Group25)
file(COPY MCTSAgent.cpp DESTINATION Group25)
file(COPY MCTSAgent.h DESTINATION Group25)
file(COPY MCTSNode.cpp DESTINATION Group25)
file(COPY MCTSNode.h DESTINATION Group25)
file(COPY Tile.cpp DESTINATION Group25)
file(COPY Tile.h DESTINATION Group25)
file(COPY ExternalAgent.py DESTINATION Group25)
file(COPY Makefile DESTINATION Group25)
file(COPY cmd.txt DESTINATION Group25)

