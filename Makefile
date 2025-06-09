# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
DR_WAV_URL := https://raw.githubusercontent.com/mackron/dr_libs/master/dr_wav.h
DR_WAV := dr_wav.h

# Build all the files

all: $(DR_WAV) clipper

$(DR_WAV):
	curl -L -o src/$(DR_WAV) $(DR_WAV_URL)

clipper:
	$(CXX) $(CXXFLAGS) src/clipper.cpp -o compiled/clipper

clean:
	rm -f compiled
