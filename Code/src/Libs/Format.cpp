// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Libs/Format.hpp"

#include <fmt/format.h>

#include <cstdint>
#include <string>
#include <utility>

// ==================================================================
// Private helpers (visible only in this file)
// ==================================================================
namespace {

    using Fn = ReturnResult<Value>;

    Fn ok(Value v) {
        return {"", true, std::move(v)};
    }

    Fn fail(const std::string& type, const std::string& message) {
        Fn r{message, false, Value{}};
        r.error_info.type = type;
        r.error_info.message = message;
        return r;
    }

    // Convert any runtime value to its string representation
    std::string to_text(const Value& v) {
        if (auto* i = std::get_if<int64_t>(&v))     return std::to_string(*i);
        if (auto* f = std::get_if<long double>(&v)) return fmt::format("{}", *f);
        if (auto* s = std::get_if<std::string>(&v)) return *s;
        if (auto* b = std::get_if<bool>(&v))        return *b ? "true" : "false";
        return ""; // monostate = empty value
    }

}


// ==================================================================
// Function: Initialize runtime
// ==================================================================

void
Lib::Format::__init__(
    Runtime::RunTime& runtime
) {
    // format(text, **named_arguments)
    // All arguments are validated inside format_fn.
    runtime.add_external_function(
        format_fn,
        "format",
        "str",
        {},
        true
    );
}


// ==================================================================
// Function: format
// ==================================================================

ReturnResult<Value>
Lib::Format::format_fn(
    ExternalFunInType input
) {
    // Validate required argument: text
    auto text_it = input.arguments.find("text");

    if (text_it == input.arguments.end()) {
        return fail(
            "ArgumentError",
            "format(): missing required argument 'text'"
        );
    }

    // Validate text argument type
    auto* text_value = std::get_if<std::string>(&text_it->second);

    if (text_value == nullptr) {
        return fail(
            "TypeError",
            "format(): argument 'text' must be str"
        );
    }

    const std::string& text = *text_value;

    std::string result;
    result.reserve(text.size());

    std::size_t position = 0;

    while (position < text.size()) {

        // Copy regular characters
        if (text[position] != '{') {
            result += text[position];
            ++position;
            continue;
        }

        // Find the closing brace
        const std::size_t close = text.find('}', position + 1);

        if (close == std::string::npos) {
            return fail(
                "FormatError",
                "format(): unmatched '{' at position " +
                std::to_string(position)
            );
        }

        // Extract placeholder name
        const std::string name = text.substr(
            position + 1,
            close - position - 1
        );

        if (name.empty()) {
            return fail(
                "FormatError",
                "format(): empty placeholder at position " +
                std::to_string(position)
            );
        }

        // Find the corresponding named argument
        auto argument_it = input.arguments.find(name);

        if (argument_it == input.arguments.end()) {
            return fail(
                "ArgumentError",
                "format(): missing argument '" + name +
                "' referenced by placeholder '{" + name + "}'"
            );
        }

        // Replace placeholder with the argument value
        result += to_text(argument_it->second);

        // Continue after the closing brace
        position = close + 1;
    }

    return ok(Value{std::move(result)});
}