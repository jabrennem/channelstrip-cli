# Root Makefile for channelstrip-pipe-cli

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Directories
BIN_DIR := bin
BUILD_DIR := build
INCLUDE_DIR := include
SRC_DIR := src
DOCS_DIR := docs

# Create directories
$(shell mkdir -p $(BIN_DIR) $(BUILD_DIR) $(INCLUDE_DIR))

# Source files
SRCS := chst.cpp clipper_module.cpp eq_module.cpp
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
TARGET := $(BIN_DIR)/chst

# Build all
all: deps $(TARGET)

# Download dependencies
deps:
	@if [ ! -f $(INCLUDE_DIR)/dr_wav.h ]; then \
		curl -L -o $(INCLUDE_DIR)/dr_wav.h https://raw.githubusercontent.com/mackron/dr_libs/master/dr_wav.h; \
	fi

# Build chst binary
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Generate documentation
docs:
	doxygen Doxyfile

# Clean all
clean:
	rm -rf $(BUILD_DIR)/* $(BIN_DIR)/*

# Clean documentation
clean-docs:
	rm -rf $(DOCS_DIR)

# Clean everything including docs
clean-all: clean clean-docs

.PHONY: all deps clean docs clean-docs clean-all