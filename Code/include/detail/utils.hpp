/**
 * @file utils.hpp
 * @brief utils declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Libs ==
#include <random>
#include <string>

/// Namespace
namespace Utils {
    size_t random_num(size_t from = 0, size_t to=SIZE_MAX); ///< Create a random number
    std::vector<std::string> split_lines(const std::string& source); ///< Split string to vector of lines 
}
