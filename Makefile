# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
DR_WAV_URL := https://raw.githubusercontent.com/mackron/dr_libs/master/dr_wav.h
DR_WAV := dr_wav.h

# Source files
SRC_DIR := src
OBJ_DIR := obj
COMPILED_DIR := compiled

# Create directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(COMPILED_DIR))

# Object files
OBJS := $(OBJ_DIR)/clipper.o

# Build all the files
all: $(DR_WAV) clipper

$(DR_WAV):
	curl -L -o $(SRC_DIR)/$(DR_WAV) $(DR_WAV_URL)

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the final executable
clipper: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $(COMPILED_DIR)/clipper

# Clean up
clean:
	rm -rf $(OBJ_DIR)/* $(COMPILED_DIR)/*