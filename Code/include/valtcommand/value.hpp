/**
 * @file value.hpp
 * @brief Public runtime value type used by ValtCommand hosts.
 */
#pragma once

#include <cstdint>
#include <string>
#include <variant>

/**
 * @brief A value supported by the ValtCommand runtime.
 *
 * The alternatives represent void, integer, floating-point, string, and
 * boolean values respectively.
 */
namespace vltc {

using Value = std::variant<std::monostate, int64_t, long double, std::string, bool>;

} // namespace vltc
