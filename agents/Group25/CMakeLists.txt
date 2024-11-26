cmake_minimum_required(VERSION 3.28.3)
project(CPPAgent)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY Group25)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(CPPAgent CPPAgent.cpp MCTSAgent.cpp MCTSNode.cpp HexUnionFind.cpp UnionFind.cpp)

add_executable(test_union_find
        test_union_find.cpp
        HexUnionFind.cpp
        UnionFind.cpp
        )
find_package(GTest REQUIRED)
target_link_libraries(test_union_find GTest::gtest GTest::gtest_main pthread)

# Copy over all files in agents/CPP to the build directory as we submit the whole thing
file(COPY CPPAgent.cpp DESTINATION Group25)
file(COPY MCTSAgent.cpp DESTINATION Group25)
file(COPY MCTSAgent.h DESTINATION Group25)
file(COPY MCTSNode.cpp DESTINATION Group25)
file(COPY MCTSNode.h DESTINATION Group25)
file(COPY ExternalAgent.py DESTINATION Group25)
file(COPY Makefile DESTINATION Group25)
file(COPY cmd.txt DESTINATION Group25)

