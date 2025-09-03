cmake_minimum_required(VERSION 3.16)
project(channelstrip-cli VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Ensure include directory exists
file(MAKE_DIRECTORY ${INCLUDE_DIR})

# Download dr_wav.h if not present
if(NOT EXISTS ${INCLUDE_DIR}/dr_wav.h)
    file(DOWNLOAD 
        https://raw.githubusercontent.com/mackron/dr_libs/master/dr_wav.h
        ${INCLUDE_DIR}/dr_wav.h
    )
endif()

# Source files
set(SOURCES
    ${SRC_DIR}/chst.cpp
    ${SRC_DIR}/clipper_module.cpp
    ${SRC_DIR}/eq_module.cpp
)

# Create executable
add_executable(chst ${SOURCES})

# Include directories
target_include_directories(chst PRIVATE ${INCLUDE_DIR})

# Set output directory
set_target_properties(chst PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# Documentation with Doxygen (optional)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
    )
endif()