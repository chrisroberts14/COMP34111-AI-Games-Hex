CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -g -lc -fstack-protector

SRCS := $(wildcard *.cpp)

OBJS := $(SRCS:.cpp=.o)

TARGET := CPPAgent

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)
.PHONY: all clean

