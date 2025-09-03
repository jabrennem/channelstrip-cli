/**
 * @file chst.cpp
 * @brief Main entry point for ChannelStrip CLI audio processing tools
 * @author ChannelStrip CLI
 */

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Forward declaration for clipper module main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status code
 */
int clipper_main(int argc, char** argv);

/**
 * @brief Forward declaration for equalizer module main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status code
 */
int eq_main(int argc, char** argv);

/**
 * @brief Prints usage information for the main program
 */
void print_usage() {
    std::cout << "Usage: chst <subcommand> [options]\n";
    std::cout << "\nSubcommands:\n";
    std::cout << "  clipper    Audio clipping/saturation\n";
    std::cout << "  eq         Equalizer (coming soon)\n";
    std::cout << "\nUse 'chst <subcommand> --help' for subcommand-specific help\n";
}

/**
 * @brief Main entry point for ChannelStrip CLI
 * 
 * Parses the subcommand and delegates to the appropriate module.
 * Supports 'clipper' for audio clipping/saturation and 'eq' for equalization.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit status code (0 for success, 1 for error)
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string subcommand = argv[1];
    
    // Create new argv for subcommand (skip program name and subcommand)
    std::vector<char*> sub_argv;
    sub_argv.push_back(argv[0]); // Keep program name for error messages
    for (int i = 2; i < argc; ++i) {
        sub_argv.push_back(argv[i]);
    }
    sub_argv.push_back(nullptr);
    
    int sub_argc = sub_argv.size() - 1;
    
    if (subcommand == "clipper") {
        return clipper_main(sub_argc, sub_argv.data());
    } else if (subcommand == "eq") {
        return eq_main(sub_argc, sub_argv.data());
    } else {
        std::cerr << "Unknown subcommand: " << subcommand << std::endl;
        print_usage();
        return 1;
    }
}