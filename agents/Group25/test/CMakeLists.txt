cmake_minimum_required(VERSION 3.10)
project(Tests)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Google Test
# Assuming you have Google Test installed via a package manager like vcpkg
find_package(GTest REQUIRED)

# Add the test source files
add_executable(test_heuristic test_heuristic.cpp ../../Group25-AZ/Heuristic.cpp ../../OldCPP/HexUnionFind.cpp ../../OldCPP/UnionFind.cpp)
add_executable(test_unionfind test_unionfind.cpp ../../OldCPP/HexUnionFind.cpp ../../OldCPP/UnionFind.cpp)
add_executable(test_agent test_agent.cpp ../MCTSAgent.cpp ../MCTSNode.cpp ../../Group25-AZ/Heuristic.cpp ../../OldCPP/HexUnionFind.cpp ../../OldCPP/UnionFind.cpp)

# Link Google Test libraries
target_link_libraries(test_unionfind PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_heuristic PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_agent PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_agent PRIVATE GTest::gmock GTest::gmock_main)

# Enable testing
enable_testing()

# Add tests to CTest
add_test(NAME HeuristicTest COMMAND test_heuristic)
add_test(NAME UnionFindTest COMMAND test_unionfind)
add_test(NAME AgentTest COMMAND test_agent)
