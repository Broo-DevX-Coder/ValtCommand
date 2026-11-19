#include "test_helpers.hpp"
#include <sstream>
namespace tests {
void test_errors() {
    Suite& results = suite("errors");
    vltc::Runtime::RunTime runtime;

    // Current code reports one-based lines and zero-based byte-columns here;
    // errors.md documents zero-based lines, so this also guards that mismatch.
    error(results, runtime, "syntax location", "@", "SyntaxError",
          "unexpected token", 1, 0, vltc::Runtime::ErrorStage::Parse);
    error(results, runtime, "type location", "SET x<int> = \"s\"", "TypeError",
          "expected 'int'", 1, 6);
    error(results, runtime, "name location", "GET missing", "NameError",
          "'missing' is not defined", 1, 4, vltc::Runtime::ErrorStage::Semantic);
    error(results, runtime, "later-line location", "SET x<int> = 1\nGET missing",
          "NameError", "'missing' is not defined", 2, 4);

    // Some native library errors have no expression location in current code.
    error(results, runtime, "argument", "CALL format END", "ArgumentError", "missing");
    error(results, runtime, "incompatible comparison", "SET x<bool> = 1 < \"x\"",
          "IncompatibleTypesError", "", 1, 18);
    error(results, runtime, "redeclaration", "SET x<int> = 1 SET x<int> = 2",
          "VariableError", "already defined", 1, 19);
    error(results, runtime, "immutable", "SET_CONST x<int> = 1 SET x = 2",
          "ImmutableError", "constant", 1, 25);
    error(results, runtime, "return type",
          "FUNCTION f()->int RETURN \"x\" END CALL f END",
          "ReturnTypeError", "expected return type", 1, 9);
    error(results, runtime, "missing return",
          "FUNCTION f()->int CALL print value<str>:\"x\" END END",
          "MissingReturnError", "no RETURN", 1, 9);
    error(results, runtime, "math domain", "CALL sqrt x<float>:-1 END",
          "ValueError", "math domain");
    error(results, runtime, "zero divisor in fmod",
          "CALL fmod a<float>:1 b<float>:0 END", "ZeroDivisionError", "division by zero");
    error(results, runtime, "zero divisor in integer divide",
          "SET x<float> = 1 %% 0", "RuntimeError", "division by zero");
    error(results, runtime, "zero divisor in remainder",
          "SET x<float> = 1 % 0", "RuntimeError", "division by zero");
    error(results, runtime, "format braces", "CALL format text<str>:\"{\" END",
          "FormatError", "unmatched");
    error(results, runtime, "empty format placeholder",
          "CALL format text<str>:\"{}\" END", "FormatError", "empty placeholder");
    error(results, runtime, "missing format placeholder",
          "CALL format text<str>:\"{absent}\" END", "ArgumentError", "absent");
    error(results, runtime, "format text type",
          "CALL format text<int>:1 END", "TypeError", "must be str");
    error(results, runtime, "duplicate function",
          "FUNCTION f()->void END FUNCTION f()->void END", "FunctionError", "already defined");
    error(results, runtime, "missing user function argument",
          "FUNCTION f(x<int>)->int RETURN GET x END CALL f END",
          "ArgumentError", "missing required argument 'x'");
    error(results, runtime, "too many user function arguments",
          "FUNCTION f(x<int>)->int RETURN GET x END CALL f x<int>:1 y<int>:2 END",
          "ArgumentError", "Too many arguments");
    error(results, runtime, "wrong user function argument type",
          "FUNCTION f(x<int>)->int RETURN GET x END CALL f x<str>:\"s\" END",
          "ArgumentError", "expected 'int'");
    error(results, runtime, "overflow converting INF",
          "SET x<int> = GET INF", "OverflowError", "non-finite or out-of-range");
    error(results, runtime, "overflow integer power",
          "SET x<float> = CALL pow base<int>:2 exp<int>:63 END",
          "OverflowError", "does not fit in int");
    error(results, runtime, "overflow remainder conversion",
          "SET x<float> = GET FLOAT_MAX % 2", "OverflowError", "out of range");
    error(results, runtime, "power domain error",
          "SET x<float> = (-1) ** 0.5", "ValueError", "invalid");
    error(results, runtime, "invalid comparison types",
          "SET x<bool> = 1 < \"x\"", "IncompatibleTypesError", "can't compare");
    error(results, runtime, "invalid arithmetic type",
          "SET x<str> = \"a\" + \"b\"", "TypeError", "expected 'int | float'");
    error(results, runtime, "invalid logical type",
          "SET x<bool> = 1 && True", "TypeError", "expected 'bool'");
    error(results, runtime, "chained comparison syntax",
          "SET x<bool> = 1 < 2 < 3", "SyntaxError", "unexpected token");
    error(results, runtime, "unterminated string syntax",
          "SET x<str> = \"unfinished", "SyntaxError", "unterminated string literal");
    error(results, runtime, "unterminated comment syntax",
          "/* unfinished", "SyntaxError", "unterminated block comment");
    error(results, runtime, "UTF-8 unknown token",
          std::string("\xC3\xA9"), "SyntaxError", "unexpected token", 1, 0);
    error(results, runtime, "CRLF later-line location",
          "SET x<int> = 1\r\nGET missing", "NameError",
          "'missing' is not defined", 2, 4, vltc::Runtime::ErrorStage::Semantic);
    error(results, runtime, "division zero", "SET x<int> = 1 / 0",
          "RuntimeError", "division by zero", 1, 17, vltc::Runtime::ErrorStage::Runtime);

    // Inject an exhausted input stream to verify EOFError without stdout checks.
    NullOutputBuffer null_output;
    auto* previous_output = std::cout.rdbuf(&null_output);
    std::istringstream exhausted_input;
    auto* previous_input = std::cin.rdbuf(exhausted_input.rdbuf());
    const auto eof = runtime.run("SET r<str> = CALL input END", vltc::Runtime::ScopeMode::Fresh);
    std::cin.rdbuf(previous_input);
    std::cout.rdbuf(previous_output);
    check(results, "EOFError", !eof && eof.error.type == "EOFError" &&
          eof.error.info.type == "EOFError" && eof.error.message.find("end of input") != std::string::npos,
          "EOFError with end-of-input message", eof ? "success" : eof.error.type + ": " + eof.error.message);

    // Compare only the filename; its parent path depends on the host platform.
    const auto missing = runtime.run_file(
        std::filesystem::path("nonexistent-runtime-test.vlt").string(),
        vltc::Runtime::ScopeMode::Fresh);
    const auto marker = missing.error.message.find(": ");
    const auto file = marker == std::string::npos
        ? std::filesystem::path{}
        : std::filesystem::path(missing.error.message.substr(marker + 2));
    check(results, "file IO", !missing &&
          missing.error.stage == vltc::Runtime::ErrorStage::IO &&
          file.filename() == "nonexistent-runtime-test.vlt",
          "IO error naming file", missing ? "success" : missing.error.message);
}
}
