// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Libs/STDIO.hpp"

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

    // Convert any runtime value to the text printed on screen
    std::string to_text(const Value& v) {
        if (auto* i = std::get_if<int64_t>(&v))     return std::to_string(*i);
        if (auto* f = std::get_if<long double>(&v)) return fmt::format("{}", *f);
        if (auto* s = std::get_if<std::string>(&v)) return *s;
        if (auto* b = std::get_if<bool>(&v))        return *b ? "true" : "false";
        return ""; // monostate = empty value
    }

    // Text of an optional argument (default if it is missing)
    std::string text_arg(
        const ExternalFunInType& in, const std::string& name, const std::string& fallback
    ) {
        auto it = in.arguments.find(name);
        return it == in.arguments.end() ? fallback : to_text(it->second);
    }
}


// ==================================================================
// Function: Initailyze runtime
// ==================================================================

void
Lib::STDIO::__init__(
    Runtime::RunTime& runtime
) {
    // print(value, end = "")
    runtime.add_external_function(
        print_fn,
        "print",
        "void",
        {
            {"value", {"any", false, false, true,  Value{}}},
            {"end",   {"str", false, false, false, Value{std::string("")}}}
        },
        false
    );

    // println(value)
    runtime.add_external_function(
        println_fn,
        "println",
        "void",
        {
            {"value", {"any", false, false, true, Value{}}}
        },
        false
    );

    // input(prompt = "")
    runtime.add_external_function(
        input_fn,
        "input",
        "str",
        {
            {"prompt", {"any", false, false, true, Value{}}}
        },
        false
    );
}


// ==================================================================
// Function: print
// ==================================================================

ReturnResult<Value>
Lib::STDIO::print_fn(
    ExternalFunInType input
) {
    std::cout << text_arg(input, "value", "")
              << text_arg(input, "end", "")
              << std::flush;
    return ok(Value{});
}

// ==================================================================
// Function: println
// ==================================================================

ReturnResult<Value>
Lib::STDIO::println_fn(
    ExternalFunInType input
) {
    std::cout << text_arg(input, "value", "") << '\n' << std::flush;
    return ok(Value{});
}

// ==================================================================
// Function: input
// ==================================================================

ReturnResult<Value>
Lib::STDIO::input_fn(
    ExternalFunInType input
) {
    std::cout << text_arg(input, "prompt", "") << std::flush;

    std::string line;
    if (!std::getline(std::cin, line))
        return fail("EOFError", "input(): end of input reached");

    if (!line.empty() && line.back() == '\r') line.pop_back(); // Windows line ending
    return ok(line);
}