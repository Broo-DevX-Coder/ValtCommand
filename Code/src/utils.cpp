// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "utils.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Functions
// ==================================================================

// Create a random number
size_t
Utils::random_num(
    size_t from,
    size_t to 
) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<std::size_t> dist(
        from,
        to
    );
    return dist(gen);
}

// Split string to vector of lines
std::vector<std::string> 
Utils::split_lines(
    const std::string& source
) {
    std::vector<std::string> lines;
    std::istringstream stream(source);
    std::string line;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    return lines;
}