# Root Makefile for channelstrip-pipe-cli

# Tools to build
TOOLS := clipper

# Directories
BIN_DIR := bin
BUILD_DIR := build
INCLUDE_DIR := include

# Create directories
$(shell mkdir -p $(BIN_DIR) $(BUILD_DIR))

# Build all tools
all: deps $(TOOLS)

# Download dependencies
deps:
	@if [ ! -f $(INCLUDE_DIR)/dr_wav.h ]; then \
		curl -L -o $(INCLUDE_DIR)/dr_wav.h https://raw.githubusercontent.com/mackron/dr_libs/master/dr_wav.h; \
	fi

# Build individual tools
clipper:
	$(MAKE) -C src/clipper

# Clean all
clean:
	rm -rf $(BUILD_DIR)/* $(BIN_DIR)/*
	$(MAKE) -C src/clipper clean

.PHONY: all deps clean clipper