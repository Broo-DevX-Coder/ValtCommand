// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "utils.hpp"

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