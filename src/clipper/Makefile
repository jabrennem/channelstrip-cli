# Clipper Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Directories
ROOT_DIR := ../..
INCLUDE_DIR := $(ROOT_DIR)/include
BUILD_DIR := $(ROOT_DIR)/build
BIN_DIR := $(ROOT_DIR)/bin

# Source files
SRCS := clipper.cpp
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
TARGET := $(BIN_DIR)/clipper

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/clipper.o $(TARGET)

.PHONY: all clean