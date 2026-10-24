#pragma once

// ==================================================================
// Marcos
// ==================================================================
#ifdef BUILDING_COMPILER_DLL
    #if defined(_WIN32)
        #define API __declspec(dllexport)
    #else
        #define API __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define API __declspec(dllimport)
    #else
        #define API 
    #endif
#endif

// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "API/Runtime.hpp"

// == Libs ==
#include <functional>

// ==================================================================
// Namespace
// ==================================================================
namespace Lib{
    
    // All mathimatical embeded functions
    namespace Math {
        
        void __init__(Runtime::RunTime& runtime); // Add all constants and functions in runtime

        // Basic ========
        ReturnResult<Value> abs_fn(ExternalFunInType input);   // abs(x)
        ReturnResult<Value> sqrt_fn(ExternalFunInType input);  // sqrt(x)
        ReturnResult<Value> cbrt_fn(ExternalFunInType input);  // cbrt(x)
        ReturnResult<Value> pow_fn(ExternalFunInType input);   // pow(base, exp)
        ReturnResult<Value> root_fn(ExternalFunInType input);  // root(x, n)

        // Rounding ========
        ReturnResult<Value> floor_fn(ExternalFunInType input); // floor(x)
        ReturnResult<Value> ceil_fn(ExternalFunInType input);  // ceil(x)
        ReturnResult<Value> round_fn(ExternalFunInType input); // round(x)
        ReturnResult<Value> trunc_fn(ExternalFunInType input); // trunc(x)

        // Remainder / Division ========
        ReturnResult<Value> fmod_fn(ExternalFunInType input);   // fmod(a, b)
        ReturnResult<Value> divmod_fn(ExternalFunInType input); // divmod(a, b)

        // Min / Max / Sign ========
        ReturnResult<Value> min_fn(ExternalFunInType input);   // min(a, b, ...)
        ReturnResult<Value> max_fn(ExternalFunInType input);   // max(a, b, ...)
        ReturnResult<Value> clamp_fn(ExternalFunInType input); // clamp(x, lo, hi)
        ReturnResult<Value> sign_fn(ExternalFunInType input);  // sign(x) -> -1, 0, 1

        // Power / Logarithmic ========
        ReturnResult<Value> exp_fn(ExternalFunInType input);   // exp(x)
        ReturnResult<Value> exp2_fn(ExternalFunInType input);  // exp2(x)
        ReturnResult<Value> log_fn(ExternalFunInType input);   // log(x)
        ReturnResult<Value> log2_fn(ExternalFunInType input);  // log2(x)
        ReturnResult<Value> log10_fn(ExternalFunInType input); // log10(x)
        ReturnResult<Value> log1p_fn(ExternalFunInType input); // log1p(x)

        // Trigonometric ========
        ReturnResult<Value> sin_fn(ExternalFunInType input);   // sin(x)
        ReturnResult<Value> cos_fn(ExternalFunInType input);   // cos(x)
        ReturnResult<Value> tan_fn(ExternalFunInType input);   // tan(x)
        ReturnResult<Value> asin_fn(ExternalFunInType input);  // asin(x)
        ReturnResult<Value> acos_fn(ExternalFunInType input);  // acos(x)
        ReturnResult<Value> atan_fn(ExternalFunInType input);  // atan(x)
        ReturnResult<Value> atan2_fn(ExternalFunInType input); // atan2(y, x)

        // Hyperbolic ========
        ReturnResult<Value> sinh_fn(ExternalFunInType input);  // sinh(x)
        ReturnResult<Value> cosh_fn(ExternalFunInType input);  // cosh(x)
        ReturnResult<Value> tanh_fn(ExternalFunInType input);  // tanh(x)

        // Angle conversion ========
        ReturnResult<Value> degrees_fn(ExternalFunInType input); // degrees(rad)
        ReturnResult<Value> radians_fn(ExternalFunInType input); // radians(deg)

        // Numeric utilities ========
        ReturnResult<Value> isnan_fn(ExternalFunInType input);    // isnan(x)
        ReturnResult<Value> isinf_fn(ExternalFunInType input);    // isinf(x)
        ReturnResult<Value> isfinite_fn(ExternalFunInType input); // isfinite(x)

        // Random ========
        ReturnResult<Value> random_fn(ExternalFunInType input);      // random() -> [0, 1)
        ReturnResult<Value> randint_fn(ExternalFunInType input);     // randint(lo, hi)
        ReturnResult<Value> seed_random_fn(ExternalFunInType input); // seed_random(seed)
 
    }
}