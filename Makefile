# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Build all the files
all:
	$(CXX) $(CXXFLAGS) src/clipper_sample_data.cpp -o compiled/clipper_sample_data

clean:
	rm -f compiled
