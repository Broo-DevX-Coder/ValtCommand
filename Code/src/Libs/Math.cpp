// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Libs/Math.hpp"

// == Libs ==
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>


// ==================================================================
// Private helpers (visible only in this file)
// ==================================================================
namespace {

    using Fn = ReturnResult<Value>;

    constexpr long double PI_VALUE = 3.141592653589793238462643383279502884L;
    constexpr long double E_VALUE  = 2.718281828459045235360287471352662498L;

    // Success result
    Fn ok(Value v) {
        return {"", true, std::move(v)};
    }

    // Error result (returned through ReturnResult only)
    Fn fail(const std::string& type, const std::string& message) {
        Fn r{message, false, Value{}};
        r.error_info.type = type;
        r.error_info.message = message;
        return r;
    }

    // Find an argument by name (nullptr if missing)
    const Value* find_arg(const ExternalFunInType& in, const std::string& name) {
        auto it = in.arguments.find(name);
        return it == in.arguments.end() ? nullptr : &it->second;
    }

    bool is_number(const Value& v) {
        return std::holds_alternative<int64_t>(v)
            || std::holds_alternative<long double>(v);
    }

    long double to_float(const Value& v) {
        if (auto* i = std::get_if<int64_t>(&v)) return static_cast<long double>(*i);
        return std::get<long double>(v);
    }

    // Read a numeric argument (int or float). On failure fills `err`.
    bool get_num(
        const ExternalFunInType& in, const std::string& fn,
        const std::string& name, Value& out, Fn& err
    ) {
        const Value* v = find_arg(in, name);
        if (!v) {
            err = fail("TypeError", fn + "(): missing argument '" + name + "'");
            return false;
        }
        if (!is_number(*v)) {
            err = fail("TypeError", fn + "(): argument '" + name
                + "' must be int or float, got " + Get_ValueT(*v));
            return false;
        }
        out = *v;
        return true;
    }

    // Read an integer-only argument
    bool get_int(
        const ExternalFunInType& in, const std::string& fn,
        const std::string& name, int64_t& out, Fn& err
    ) {
        const Value* v = find_arg(in, name);
        if (!v) {
            err = fail("TypeError", fn + "(): missing argument '" + name + "'");
            return false;
        }
        auto* i = std::get_if<int64_t>(v);
        if (!i) {
            err = fail("TypeError", fn + "(): argument '" + name
                + "' must be int, got " + Get_ValueT(*v));
            return false;
        }
        out = *i;
        return true;
    }

    // Read two numeric arguments
    bool get_two(
        const ExternalFunInType& in, const std::string& fn,
        const std::string& n1, const std::string& n2,
        Value& a, Value& b, Fn& err
    ) {
        return get_num(in, fn, n1, a, err) && get_num(in, fn, n2, b, err);
    }

    // Final check of a float result: domain error or overflow
    Fn finish(const std::string& fn, bool inputs_finite, long double r) {
        if (inputs_finite && std::isnan(r))
            return fail("ValueError", fn + "(): math domain error");
        if (inputs_finite && std::isinf(r))
            return fail("OverflowError", fn + "(): result out of range");
        return ok(r);
    }

    // One number in -> one float out (with a domain check)
    template <typename Valid, typename F>
    Fn unary_checked(
        const std::string& fn, const ExternalFunInType& in, Valid valid, F f
    ) {
        Value x; Fn err;
        if (!get_num(in, fn, "x", x, err)) return err;
        long double a = to_float(x);
        if (!valid(a)) return fail("ValueError", fn + "(): math domain error");
        return finish(fn, std::isfinite(a), f(a));
    }

    // Same, but with no domain check
    template <typename F>
    Fn unary_float(const std::string& fn, const ExternalFunInType& in, F f) {
        return unary_checked(fn, in, [](long double) { return true; }, f);
    }

    // floor/ceil/round/trunc: one number in -> one INT out
    template <typename F>
    Fn rounding(const std::string& fn, const ExternalFunInType& in, F f) {
        Value x; Fn err;
        if (!get_num(in, fn, "x", x, err)) return err;
        if (std::holds_alternative<int64_t>(x)) return ok(x); // already int
        long double a = std::get<long double>(x);
        if (!std::isfinite(a))
            return fail("ValueError", fn + "(): cannot convert NaN or INF to int");
        long double r = f(a);
        if (r >= 9.2233720368547758e18L || r < -9.2233720368547758e18L)
            return fail("OverflowError", fn + "(): result does not fit in int");
        return ok(static_cast<int64_t>(r));
    }

    // a * b with overflow detection (used by pow)
    bool checked_mul(int64_t a, int64_t b, int64_t& out) {
        if (a == 0 || b == 0) { out = 0; return true; }
        if ((a == -1 && b == std::numeric_limits<int64_t>::min()) ||
            (b == -1 && a == std::numeric_limits<int64_t>::min())) return false;
        int64_t r = static_cast<int64_t>(
            static_cast<uint64_t>(a) * static_cast<uint64_t>(b));
        if (r / b != a) return false;
        out = r;
        return true;
    }

    // a < b for two numbers (int vs int exactly, otherwise as float)
    bool less_than(const Value& a, const Value& b) {
        auto* ia = std::get_if<int64_t>(&a);
        auto* ib = std::get_if<int64_t>(&b);
        if (ia && ib) return *ia < *ib;
        return to_float(a) < to_float(b);
    }

    // One shared random engine (seed_random reseeds it)
    std::mt19937_64& rng() {
        static std::mt19937_64 engine{std::random_device{}()};
        return engine;
    }

    // Register a function: all arguments are "any" and required
    void reg(
        Runtime::RunTime& rt, ExternalFuncType fn, const std::string& name,
        const std::string& return_type, const std::vector<std::string>& params
    ) {
        std::unordered_map<std::string, Runtime::ExternalFunctionArgument> args;
        for (const auto& p : params)
            args[p] = {"any", false, true, true, Value{}};
        rt.add_external_function(fn, name, return_type, args, false);
    }
}

// ==================================================================
// Basic functions
// ==================================================================

// abs(x) -> float
ReturnResult<Value>
Lib::Math::abs_fn(ExternalFunInType input) {
    Value x; Fn err;
    if (!get_num(input, "abs", "x", x, err)) return err;
    return ok(std::fabs(to_float(x)));
}

ReturnResult<Value>
Lib::Math::sqrt_fn(ExternalFunInType input) {
    return unary_float("sqrt", input, [](long double x) { return std::sqrt(x); });
}

ReturnResult<Value>
Lib::Math::cbrt_fn(ExternalFunInType input) {
    return unary_float("cbrt", input, [](long double x) { return std::cbrt(x); });
}

ReturnResult<Value>
Lib::Math::sign_fn(ExternalFunInType input) {
    Value x; Fn err;
    if (!get_num(input, "sign", "x", x, err)) return err;
    if (auto* i = std::get_if<int64_t>(&x))
        return ok(static_cast<int64_t>((*i > 0) - (*i < 0)));
    long double f = std::get<long double>(x);
    if (std::isnan(f)) return fail("ValueError", "sign(): argument is NaN");
    return ok(static_cast<int64_t>((f > 0) - (f < 0)));
}

// ==================================================================
// Power functions
// ==================================================================

// pow(base, exp) -> float
ReturnResult<Value>
Lib::Math::pow_fn(ExternalFunInType input) {
    Value b, e; Fn err;
    if (!get_two(input, "pow", "base", "exp", b, e, err)) return err;

    auto* bi = std::get_if<int64_t>(&b);
    auto* ei = std::get_if<int64_t>(&e);

    // int ** non-negative int -> exact result (square-and-multiply)
    if (bi && ei && *ei >= 0) {
        int64_t result = 1, base = *bi, n = *ei;
        while (n > 0) {
            if ((n & 1) && !checked_mul(result, base, result))
                return fail("OverflowError", "pow(): result does not fit in int");
            n >>= 1;
            if (n > 0 && !checked_mul(base, base, base))
                return fail("OverflowError", "pow(): result does not fit in int");
        }
        return ok(static_cast<long double>(result));
    }

    // Everything else -> float
    long double x = to_float(b), y = to_float(e);
    if (x == 0 && y < 0)
        return fail("ZeroDivisionError", "pow(): 0 cannot be raised to a negative power");
    return finish("pow", std::isfinite(x) && std::isfinite(y), std::pow(x, y));
}

ReturnResult<Value>
Lib::Math::root_fn(ExternalFunInType input) {
    Value x; int64_t n; Fn err;
    if (!get_num(input, "root", "x", x, err)) return err;
    if (!get_int(input, "root", "n", n, err)) return err;
    if (n == 0) return fail("ValueError", "root(): n must not be 0");

    long double a = to_float(x);
    if (a == 0 && n < 0)
        return fail("ZeroDivisionError", "root(): zero has no negative root");
    if (a < 0) {
        if (n % 2 == 0)
            return fail("ValueError", "root(): even root of a negative number");
        return finish("root", std::isfinite(a), -std::pow(-a, 1.0L / n));
    }
    return finish("root", std::isfinite(a), std::pow(a, 1.0L / n));
}

// ==================================================================
// Rounding functions (always return int)
// ==================================================================

ReturnResult<Value>
Lib::Math::floor_fn(ExternalFunInType input) {
    return rounding("floor", input, [](long double x) { return std::floor(x); });
}

ReturnResult<Value>
Lib::Math::ceil_fn(ExternalFunInType input) {
    return rounding("ceil", input, [](long double x) { return std::ceil(x); });
}

ReturnResult<Value>
Lib::Math::round_fn(ExternalFunInType input) {
    // Halves round away from zero: round(2.5) = 3, round(-2.5) = -3
    return rounding("round", input, [](long double x) { return std::round(x); });
}

ReturnResult<Value>
Lib::Math::trunc_fn(ExternalFunInType input) {
    return rounding("trunc", input, [](long double x) { return std::trunc(x); });
}

// ==================================================================
// Remainder function
// ==================================================================

ReturnResult<Value>
Lib::Math::fmod_fn(ExternalFunInType input) {
    Value a, b; Fn err;
    if (!get_two(input, "fmod", "a", "b", a, b, err)) return err;
    long double x = to_float(a), y = to_float(b);
    if (y == 0) return fail("ZeroDivisionError", "fmod(): division by zero");
    return finish("fmod", std::isfinite(x) && std::isfinite(y), std::fmod(x, y));
}

// ==================================================================
// Min / Max / Clamp (all return float)
// ==================================================================

ReturnResult<Value>
Lib::Math::min_fn(ExternalFunInType input) {
    Value a, b; Fn err;
    if (!get_two(input, "min", "a", "b", a, b, err)) return err;
    return ok(to_float(less_than(b, a) ? b : a));
}

ReturnResult<Value>
Lib::Math::max_fn(ExternalFunInType input) {
    Value a, b; Fn err;
    if (!get_two(input, "max", "a", "b", a, b, err)) return err;
    return ok(to_float(less_than(a, b) ? b : a));
}

ReturnResult<Value>
Lib::Math::clamp_fn(ExternalFunInType input) {
    Value x, lo, hi; Fn err;
    if (!get_num(input, "clamp", "x", x, err)) return err;
    if (!get_two(input, "clamp", "lo", "hi", lo, hi, err)) return err;
    if (less_than(hi, lo))
        return fail("ValueError", "clamp(): lo must not be greater than hi");
    if (less_than(x, lo)) return ok(to_float(lo));
    if (less_than(hi, x)) return ok(to_float(hi));
    return ok(to_float(x));
}

// ==================================================================
// Exponential and logarithmic functions
// ==================================================================

ReturnResult<Value>
Lib::Math::exp_fn(ExternalFunInType input) {
    return unary_float("exp", input, [](long double x) { return std::exp(x); });
}

ReturnResult<Value>
Lib::Math::exp2_fn(ExternalFunInType input) {
    return unary_float("exp2", input, [](long double x) { return std::exp2(x); });
}

ReturnResult<Value>
Lib::Math::log_fn(ExternalFunInType input) {
    return unary_checked("log", input,
        [](long double x) { return x > 0; },
        [](long double x) { return std::log(x); });
}

ReturnResult<Value>
Lib::Math::log2_fn(ExternalFunInType input) {
    return unary_checked("log2", input,
        [](long double x) { return x > 0; },
        [](long double x) { return std::log2(x); });
}

ReturnResult<Value>
Lib::Math::log10_fn(ExternalFunInType input) {
    return unary_checked("log10", input,
        [](long double x) { return x > 0; },
        [](long double x) { return std::log10(x); });
}

ReturnResult<Value>
Lib::Math::log1p_fn(ExternalFunInType input) {
    return unary_checked("log1p", input,
        [](long double x) { return x > -1; },
        [](long double x) { return std::log1p(x); });
}

// ==================================================================
// Trigonometric functions (radians)
// ==================================================================

ReturnResult<Value>
Lib::Math::sin_fn(ExternalFunInType input) {
    return unary_float("sin", input, [](long double x) { return std::sin(x); });
}

ReturnResult<Value>
Lib::Math::cos_fn(ExternalFunInType input) {
    return unary_float("cos", input, [](long double x) { return std::cos(x); });
}

ReturnResult<Value>
Lib::Math::tan_fn(ExternalFunInType input) {
    return unary_float("tan", input, [](long double x) { return std::tan(x); });
}

ReturnResult<Value>
Lib::Math::asin_fn(ExternalFunInType input) {
    return unary_float("asin", input, [](long double x) { return std::asin(x); });
}

ReturnResult<Value>
Lib::Math::acos_fn(ExternalFunInType input) {
    return unary_float("acos", input, [](long double x) { return std::acos(x); });
}

ReturnResult<Value>
Lib::Math::atan_fn(ExternalFunInType input) {
    return unary_float("atan", input, [](long double x) { return std::atan(x); });
}

ReturnResult<Value>
Lib::Math::atan2_fn(ExternalFunInType input) {
    Value y, x; Fn err;
    if (!get_two(input, "atan2", "y", "x", y, x, err)) return err;
    return ok(std::atan2(to_float(y), to_float(x)));
}

// ==================================================================
// Hyperbolic functions
// ==================================================================

ReturnResult<Value>
Lib::Math::sinh_fn(ExternalFunInType input) {
    return unary_float("sinh", input, [](long double x) { return std::sinh(x); });
}

ReturnResult<Value>
Lib::Math::cosh_fn(ExternalFunInType input) {
    return unary_float("cosh", input, [](long double x) { return std::cosh(x); });
}

ReturnResult<Value>
Lib::Math::tanh_fn(ExternalFunInType input) {
    return unary_float("tanh", input, [](long double x) { return std::tanh(x); });
}

// ==================================================================
// Angle conversion functions
// ==================================================================

ReturnResult<Value>
Lib::Math::degrees_fn(ExternalFunInType input) {
    return unary_float("degrees", input,
        [](long double x) { return x * 180.0L / PI_VALUE; });
}

ReturnResult<Value>
Lib::Math::radians_fn(ExternalFunInType input) {
    return unary_float("radians", input,
        [](long double x) { return x * PI_VALUE / 180.0L; });
}

// ==================================================================
// Numeric utilities
// ==================================================================

ReturnResult<Value>
Lib::Math::isnan_fn(ExternalFunInType input) {
    Value x; Fn err;
    if (!get_num(input, "isnan", "x", x, err)) return err;
    if (std::holds_alternative<int64_t>(x)) return ok(false);
    return ok(std::isnan(std::get<long double>(x)));
}

ReturnResult<Value>
Lib::Math::isinf_fn(ExternalFunInType input) {
    Value x; Fn err;
    if (!get_num(input, "isinf", "x", x, err)) return err;
    if (std::holds_alternative<int64_t>(x)) return ok(false);
    return ok(std::isinf(std::get<long double>(x)));
}

ReturnResult<Value>
Lib::Math::isfinite_fn(ExternalFunInType input) {
    Value x; Fn err;
    if (!get_num(input, "isfinite", "x", x, err)) return err;
    if (std::holds_alternative<int64_t>(x)) return ok(true);
    return ok(std::isfinite(std::get<long double>(x)));
}

// ==================================================================
// Random functions
// ==================================================================

ReturnResult<Value>
Lib::Math::random_fn(ExternalFunInType input) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return ok(static_cast<long double>(dist(rng())));
}

ReturnResult<Value>
Lib::Math::randint_fn(ExternalFunInType input) {
    int64_t lo, hi; Fn err;
    if (!get_int(input, "randint", "lo", lo, err)) return err;
    if (!get_int(input, "randint", "hi", hi, err)) return err;
    if (lo > hi) return fail("ValueError", "randint(): lo must not be greater than hi");
    std::uniform_int_distribution<int64_t> dist(lo, hi); // both ends included
    return ok(dist(rng()));
}

ReturnResult<Value>
Lib::Math::seed_random_fn(ExternalFunInType input) {
    int64_t seed; Fn err;
    if (!get_int(input, "seed_random", "seed", seed, err)) return err;
    rng().seed(static_cast<uint64_t>(seed));
    return ok(seed);
}


// ==================================================================
// Function: Initailyze runtime
// ==================================================================

void
Lib::Math::__init__(
    Runtime::RunTime& runtime
) {
    // ---- Mathematical constants ----
    runtime.add_external_constant("PI",  PI_VALUE);
    runtime.add_external_constant("E",   E_VALUE);
    runtime.add_external_constant("TAU", 2 * PI_VALUE);
    runtime.add_external_constant("INF", std::numeric_limits<long double>::infinity());
    runtime.add_external_constant("NAN", std::numeric_limits<long double>::quiet_NaN());

    // ---- Numeric limits ----
    runtime.add_external_constant("INT_MAX",       std::numeric_limits<int64_t>::max());
    runtime.add_external_constant("INT_MIN",       std::numeric_limits<int64_t>::min());
    runtime.add_external_constant("FLOAT_MAX",     std::numeric_limits<long double>::max());
    runtime.add_external_constant("FLOAT_MIN",     std::numeric_limits<long double>::min());
    runtime.add_external_constant("FLOAT_EPSILON", std::numeric_limits<long double>::epsilon());

    // ---- Basic ----
    reg(runtime, abs_fn,  "abs",  "float", {"x"});
    reg(runtime, sqrt_fn, "sqrt", "float", {"x"});
    reg(runtime, cbrt_fn, "cbrt", "float", {"x"});
    reg(runtime, sign_fn, "sign", "int",   {"x"});

    // ---- Power ----
    reg(runtime, pow_fn,  "pow",  "float", {"base", "exp"});
    reg(runtime, root_fn, "root", "float", {"x", "n"});

    // ---- Rounding ----
    reg(runtime, floor_fn, "floor", "int", {"x"});
    reg(runtime, ceil_fn,  "ceil",  "int", {"x"});
    reg(runtime, round_fn, "round", "int", {"x"});
    reg(runtime, trunc_fn, "trunc", "int", {"x"});

    // ---- Remainder / Min / Max / Clamp ----
    reg(runtime, fmod_fn,  "fmod",  "float", {"a", "b"});
    reg(runtime, min_fn,   "min",   "float", {"a", "b"});
    reg(runtime, max_fn,   "max",   "float", {"a", "b"});
    reg(runtime, clamp_fn, "clamp", "float", {"x", "lo", "hi"});

    // ---- Exponential / Logarithmic ----
    reg(runtime, exp_fn,   "exp",   "float", {"x"});
    reg(runtime, exp2_fn,  "exp2",  "float", {"x"});
    reg(runtime, log_fn,   "log",   "float", {"x"});
    reg(runtime, log2_fn,  "log2",  "float", {"x"});
    reg(runtime, log10_fn, "log10", "float", {"x"});
    reg(runtime, log1p_fn, "log1p", "float", {"x"});

    // ---- Trigonometric ----
    reg(runtime, sin_fn,   "sin",   "float", {"x"});
    reg(runtime, cos_fn,   "cos",   "float", {"x"});
    reg(runtime, tan_fn,   "tan",   "float", {"x"});
    reg(runtime, asin_fn,  "asin",  "float", {"x"});
    reg(runtime, acos_fn,  "acos",  "float", {"x"});
    reg(runtime, atan_fn,  "atan",  "float", {"x"});
    reg(runtime, atan2_fn, "atan2", "float", {"y", "x"});

    // ---- Hyperbolic ----
    reg(runtime, sinh_fn, "sinh", "float", {"x"});
    reg(runtime, cosh_fn, "cosh", "float", {"x"});
    reg(runtime, tanh_fn, "tanh", "float", {"x"});

    // ---- Angle conversion ----
    reg(runtime, degrees_fn, "degrees", "float", {"x"});
    reg(runtime, radians_fn, "radians", "float", {"x"});

    // ---- Numeric utilities ----
    reg(runtime, isnan_fn,    "isnan",    "bool", {"x"});
    reg(runtime, isinf_fn,    "isinf",    "bool", {"x"});
    reg(runtime, isfinite_fn, "isfinite", "bool", {"x"});

    // ---- Random ----
    reg(runtime, random_fn,      "random",      "float", {});
    reg(runtime, randint_fn,     "randint",     "int",   {"lo", "hi"});
    reg(runtime, seed_random_fn, "seed_random", "int",   {"seed"});
}

