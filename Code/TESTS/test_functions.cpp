#include "test_helpers.hpp"
#include <sstream>
#include <tuple>

namespace tests {
void test_functions() {
    Suite& results = suite("functions");
    vltc::Runtime::RunTime runtime;

    // Cover a user-defined function and representative Math and Format calls.
    run_ok(results, runtime, "user function",
           "FUNCTION add(a<int> b<int>)->int RETURN GET a + GET b END SET r<int> = CALL add a<int>:2 b<int>:3 END",
           "r", "int", "5");
    run_ok(results, runtime, "math sqrt", "SET r<float> = CALL sqrt x<float>:9 END",
           "r", "float", "3");
    run_ok(results, runtime, "math sin", "SET r<float> = CALL sin x<float>:0 END",
           "r", "float", "0");
    run_ok(results, runtime, "format",
           "SET r<str> = CALL format text<str>:\"Hi {name}\" name<str>:\"Ada\" END",
           "r", "str", "Hi Ada");

    // One deterministic in-domain input per documented Math function.
    const std::vector<std::tuple<std::string, std::string, std::string, std::string>> math_cases = {
        {"abs", "x<float>:-2", "float", "2"},
        {"sqrt", "x<float>:4", "float", "2"},
        {"cbrt", "x<float>:-8", "float", "-2"},
        {"sign", "x<int>:-2", "int", "-1"},
        {"pow", "base<float>:2 exp<float>:3", "float", "8"},
        {"root", "x<float>:8 n<int>:3", "float", "2"},
        {"floor", "x<float>:2.8", "int", "2"},
        {"ceil", "x<float>:2.2", "int", "3"},
        {"round", "x<float>:-2.5", "int", "-3"},
        {"trunc", "x<float>:-2.8", "int", "-2"},
        {"fmod", "a<float>:7 b<float>:2", "float", "1"},
        {"min", "a<float>:2 b<float>:3", "float", "2"},
        {"max", "a<float>:2 b<float>:3", "float", "3"},
        {"clamp", "x<float>:15 lo<float>:0 hi<float>:10", "float", "10"},
        {"exp", "x<float>:0", "float", "1"},
        {"exp2", "x<float>:0", "float", "1"},
        {"log", "x<float>:1", "float", "0"},
        {"log2", "x<float>:1", "float", "0"},
        {"log10", "x<float>:1", "float", "0"},
        {"log1p", "x<float>:0", "float", "0"},
        {"sin", "x<float>:0", "float", "0"},
        {"cos", "x<float>:0", "float", "1"},
        {"tan", "x<float>:0", "float", "0"},
        {"asin", "x<float>:0", "float", "0"},
        {"acos", "x<float>:1", "float", "0"},
        {"atan", "x<float>:0", "float", "0"},
        {"atan2", "y<float>:0 x<float>:1", "float", "0"},
        {"sinh", "x<float>:0", "float", "0"},
        {"cosh", "x<float>:0", "float", "1"},
        {"tanh", "x<float>:0", "float", "0"},
        {"degrees", "x<float>:0", "float", "0"},
        {"radians", "x<float>:0", "float", "0"},
        {"isnan", "x<float>:0", "bool", "false"},
        {"isinf", "x<float>:0", "bool", "false"},
        {"isfinite", "x<float>:0", "bool", "true"},
        {"randint", "lo<int>:4 hi<int>:4", "int", "4"},
        {"seed_random", "seed<int>:42", "int", "42"},
    };
    for (const auto& [function, args, type, expected] : math_cases) {
        const auto code = "SET r<" + type + "> = CALL " + function + " " + args + " END";
        run_ok(results, runtime, "Math " + function, code, "r", type, expected);
    }

    // random() has no fixed value; assert its documented type and [0, 1) range.
    runtime.reset();
    const auto random_run = runtime.run("SET r<float> = CALL random END");
    const auto random_value = runtime.get_variable("r");
    bool random_in_range = random_run && random_value && random_value.type == "float";
    if (random_in_range) {
        try {
            const auto value = std::stold(random_value.value);
            random_in_range = value >= 0 && value < 1;
        } catch (...) {
            random_in_range = false;
        }
    }
    check(results, "Math random range", random_in_range, "float in [0, 1)",
          random_value ? describe(random_value) : "failed");

    // Exercise STDIO return behavior while discarding prompts and printed text.
    NullOutputBuffer null_output;
    auto* previous_output = std::cout.rdbuf(&null_output);
    runtime.reset();
    const auto print_run = runtime.run("CALL print value<str>:\"hidden\" END");
    const auto println_run = runtime.run("CALL println value<int>:7 END");

    // input() consumes the injected line and stores it as a runtime string.
    std::istringstream supplied_input("sample line\n");
    auto* previous_input = std::cin.rdbuf(supplied_input.rdbuf());
    runtime.reset();
    const auto input_run = runtime.run("SET r<str> = CALL input END");
    const auto input_value = runtime.get_variable("r");
    std::cin.rdbuf(previous_input);
    std::cout.rdbuf(previous_output);
    check(results, "STDIO print and println", print_run && println_run,
          "successful void calls", print_run && println_run ? "success" : "failure");
    check(results, "STDIO input return", input_run && input_value &&
          input_value.type == "str" && input_value.value == "sample line",
          "str:sample line", input_value ? describe(input_value) : "failed");

    // Constants are readable script values and retain their declared runtime types.
    const std::vector<std::tuple<std::string, std::string, std::string>> constants = {
        {"PI", "float", "3.14159265358979323846"}, {"E", "float", "2.71828182845904523536"},
        {"TAU", "float", "6.28318530717958647692"}, {"INT_MAX", "int", "9223372036854775807"},
        {"INT_MIN", "int", "-9223372036854775808"}, {"INF", "float", "inf"},
        {"NAN", "float", "nan"}, {"FLOAT_MAX", "float", "max"},
        {"FLOAT_MIN", "float", "min"}, {"FLOAT_EPSILON", "float", "epsilon"},
    };
    for (const auto& [constant, type, expected] : constants) {
        runtime.reset();
        const auto script = "SET r<" + type + "> = GET " + constant;
        const auto run = runtime.run(script);
        const auto value = runtime.get_variable("r");
        bool matches = run && value && value.type == type;
        if (matches && constant == "PI") matches = approx_equal(std::stold(value.value), 3.14159265358979323846L, 4);
        else if (matches && constant == "E") matches = approx_equal(std::stold(value.value), 2.71828182845904523536L, 4);
        else if (matches && constant == "TAU") matches = approx_equal(std::stold(value.value), 6.28318530717958647692L, 4);
        else if (matches && constant == "INT_MAX") matches = value.value == expected;
        else if (matches && constant == "INT_MIN") matches = value.value == expected;
        else if (matches && constant == "INF") matches = std::isinf(std::stold(value.value));
        else if (matches && constant == "NAN") matches = std::isnan(std::stold(value.value));
        else if (matches && (constant == "FLOAT_MAX" || constant == "FLOAT_MIN" || constant == "FLOAT_EPSILON")) {
            // The host API formats long double to six significant digits; that
            // text can overflow when parsed back at the extreme limits. Check
            // these constants through language operations, where the value is intact.
            runtime.reset();
            const auto predicate = constant == "FLOAT_MAX" ? "CALL isfinite x<float>:GET FLOAT_MAX END"
                : "GET " + constant + " > 0";
            const auto predicate_run = runtime.run("SET r<bool> = " + predicate);
            const auto predicate_value = runtime.get_variable("r");
            matches = predicate_run && predicate_value && predicate_value.type == "bool" &&
                predicate_value.value == (constant == "FLOAT_MAX" ? "true" : "true");
        }
        check(results, "constant " + constant, matches, type + " " + expected,
              value ? describe(value) : "failed");
    }

    // Developer inspectors are built-ins too; verify callable success and type
    // without asserting their presentation tables or row ordering.
    const std::vector<std::string> inspectors = {
        "__INSPECT_SCOPE__", "__INSPECT_VARS__", "__INSPECT_FUNCS__"
    };
    for (const auto& inspector : inspectors) {
        runtime.reset();
        const auto code = "SET marker<int> = 1 SET r<str> = CALL " + inspector + " END";
        const auto run = runtime.run(code);
        const auto value = runtime.get_variable("r");
        check(results, "inspector " + inspector,
              run && value && value.type == "str" && !value.value.empty(),
              "successful str result", value ? describe(value) : "failed");
    }

    // STDIO writes directly to stdout and exposes no output-capture hook.
    runtime.reset();
    const auto nan_run = runtime.run("SET r<bool> = CALL isnan x<float>:GET NAN END");
    const auto nan_value = runtime.get_variable("r");
    check(results, "Math NAN predicate", nan_run && nan_value &&
          nan_value.type == "bool" && nan_value.value == "true",
          "bool:true", nan_value ? describe(nan_value) : "failed");
}
}
