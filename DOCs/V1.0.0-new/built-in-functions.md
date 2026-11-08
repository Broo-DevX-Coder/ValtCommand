# ValtCommand Built-in Functions Reference

**Version:** 1.0.0

This document combines the reference for the embedded `Format`, `Math`, and `STDIO` libraries. These functions are available to ValtCommand scripts through `CALL`; constants are read with `GET`.

## Contents

- [Format](#format-function)
- [Math and STDIO](#valtcommand-standard-library-math--stdio)
  - [Quick start](#1-quick-start)
  - [How calls work](#2-how-calls-work)
  - [Math quick reference](#3-quick-reference-table)
  - [Math constants](#4-constants)
  - [STDIO functions](#14-stdio-input-and-output)
  - [Complete examples](#15-complete-examples)
  - [Errors](#16-errors-and-how-to-avoid-them)
  - [Common mistakes](#17-common-mistakes)
  - [Using the libraries from C++](#18-using-the-libraries-from-c)
  - [Known limitations](#19-known-limitations)

## Format Function

The `format()` function is part of the ValtCommand embedded standard library. It creates formatted strings by replacing named placeholders with the values of the corresponding arguments.

### Syntax

```
CALL format
    text<str>:"Hello {name}!"
    name<str>:"World"
END
```

**Output value:**

```text
Hello World!
```

The `text` argument contains the format string. All other arguments are named values that can be referenced inside the string using placeholders.

### Arguments

| Argument | Type | Required | Description |
|---|---|---|---|
| `text` | `str` | Yes | The template string containing placeholders. |
| Named arguments | Any supported value type | When referenced | Values used to replace placeholders in the template. |

The `format()` function accepts dynamically named arguments. You do not need to declare each possible placeholder name in advance.

#### The `text` argument

The `text` argument defines the string to format and must be a string value.

```
CALL format
    text<str>:"Welcome to ValtCommand!"
END
```

Result:

```text
Welcome to ValtCommand!
```

Templates without placeholders are valid.

#### Named arguments

Named arguments provide values for the placeholders used in `text`.

```
CALL format
    text<str>:"Hello {name}, welcome to {language}!"
    name<str>:"Developer"
    language<str>:"ValtCommand"
END
```

Result:

```text
Hello Developer, welcome to ValtCommand!
```

Each placeholder must match the name of a supplied argument exactly.

### Supported Value Types

The function converts supported runtime values to strings before inserting them into the result.

| Type | Example value | Formatted text |
|---|---|---|
| `str` | `"ValtCommand"` | `ValtCommand` |
| `int` | `18` | `18` |
| `float` | `3.14` | `3.14` |
| `bool` | `true` | `true` |
| `bool` | `false` | `false` |

Example:

```
CALL format
    text<str>:"Name: {name}, Age: {age}, Active: {active}"
    name<str>:"Developer"
    age<int>:18
    active<bool>:true
END
```

Result:

```text
Name: Developer, Age: 18, Active: true
```

Floating-point output follows the runtime's numeric-to-string conversion.

### Using `format()` with Other Functions

The formatted string can be passed directly to another function, such as `print`.

```
CALL print
    value<str>: CALL format
        text<str>:"Hello {name}!"
        name<str>:"World"
    END
END
```

Output:

```text
Hello World!
```

This allows you to construct messages dynamically without concatenating strings manually.

### Multiple Placeholders

A template can contain multiple placeholders, including repeated references to the same argument.

```
CALL format
    text<str>:"{name} is using {language}. Welcome, {name}!"
    name<str>:"Developer"
    language<str>:"ValtCommand"
END
```

Result:

```text
Developer is using ValtCommand. Welcome, Developer!
```

Repeated placeholders reuse the same named argument value.

### Error Handling

The function returns a runtime error when the template or its arguments are invalid.

#### Missing `text`

The `text` argument is mandatory.

```
CALL format
    name<str>:"World"
END
```

Error type: `ArgumentError`

The error indicates that the required `text` argument was not supplied.

#### Invalid `text` type

The `text` argument must be a string.

```
CALL format
    text<int>:123
END
```

Error type: `TypeError`

#### Missing placeholder argument

Every placeholder must reference an argument supplied to the function.

```
CALL format
    text<str>:"Hello {name}!"
END
```

Error type: `ArgumentError`

The error identifies the missing argument `name`.

#### Unmatched opening brace

Every opening `{` used to start a placeholder must have a corresponding closing `}`.

```
CALL format
    text<str>:"Hello {name"
    name<str>:"World"
END
```

Error type: `FormatError`

#### Empty placeholder

Placeholder names cannot be empty.

```
CALL format
    text<str>:"Hello {}!"
END
```

Error type: `FormatError`

### Placeholder Rules

- Placeholders use the `{name}` syntax.
- The name must match a supplied named argument exactly.
- Every referenced argument must exist.
- Empty placeholders are invalid.
- An opening brace without a closing brace is invalid.
- Arguments that are not referenced by the template are ignored.
- Braces are reserved for placeholder syntax; literal brace escaping is not supported by the current implementation.

### Limitations

The current implementation provides basic named-placeholder substitution. It does not support:

- Positional placeholders.
- Numeric formatting options, such as fixed decimal precision.
- Alignment, padding, or width specifiers.
- Conditional formatting.
- Escape sequences for literal braces.

For example, `{price:.2f}` is **not** a supported formatting specifier. The implementation treats the text inside the braces as an argument name.

### Summary

Use `format()` when you need to build a string from named values.

```
CALL format
    text<str>:"{greeting}, {name}! Version: {version}"
    greeting<str>:"Hello"
    name<str>:"Developer"
    version<str>:"1.0.0"
END
```

Result:

```text
Hello, Developer! Version: 1.0.0
```

Remember: **`text` defines the template, and named arguments provide the values used to replace its placeholders.**

## ValtCommand Standard Library: Math & STDIO

*Reference for the embedded Math and STDIO libraries.*

This section documents the embedded **Math** and **STDIO** libraries: their arguments, return types, errors, and examples. All examples use ValtCommand syntax. Values marked with `≈` are rounded for readability.

---

### Contents

1. Quick start
2. How calls work (rules for every function)
3. Quick reference table
4. Constants
5. Basic functions: `abs`, `sqrt`, `cbrt`, `sign`
6. Power functions: `pow`, `root`
7. Rounding functions: `floor`, `ceil`, `round`, `trunc`
8. Remainder and comparison: `fmod`, `min`, `max`, `clamp`
9. Exponential and logarithmic functions
10. Trigonometric and hyperbolic functions
11. Angle conversion
12. Numeric utilities: `isnan`, `isinf`, `isfinite`
13. Random numbers
14. STDIO: `print`, `println`, `input`
15. Complete example programs
16. Errors and how to avoid them
17. Common mistakes
18. Using the libraries from C++
19. Known limitations

---

### 1. Quick start

The libraries are registered once, when the runtime is created (see section 18). After that, every function and constant is available in every script, with no import needed.

```
// Print text
CALL println value<str>:"Hello, ValtCommand!" END

// Call a math function and print the result
CALL println value<float>:CALL sqrt x<float>:16 END END

// Read a constant with GET
CALL println value<float>:GET PI END
```

Output:

```
Hello, ValtCommand!
4
3.14159265358979...
```

Three things to notice:

- A function call starts with `CALL`, then the function name, then its arguments, then `END`.
- Each argument has a **name**, a **type tag** in `< >`, a colon, and a value: `x<float>:16`.
- A call can be an argument of another call. Each call needs its own `END`.

---

### 2. How calls work

These rules apply to **every** function in this guide.

#### 2.1 Named arguments

Arguments are passed by name, so their order does not matter. These two calls are identical:

```
CALL clamp x<float>:15 lo<float>:0 hi<float>:10 END
CALL clamp hi<float>:10 x<float>:15 lo<float>:0 END
```

The argument names for each function are listed in the reference table and in each function's section.

#### 2.2 The type tag decides what the function receives

The tag in `x<float>:5` is not decoration. ValtCommand converts the value to the tagged type before calling the function. So the same number can take a different path through the same function:

```
CALL pow base<int>:2 exp<int>:10 END      // exact integer arithmetic
CALL pow base<float>:2 exp<int>:10 END    // floating-point arithmetic
```

Two conversion rules:

- An **int can be used where a float is expected**: `x<float>:5` is fine, and the function receives `5.0`.
- A **float cannot be tagged as int**: `x<int>:2.5` is rejected before the program runs.

Functions that require whole numbers (`root` for `n`, and `randint`, `seed_random`) need the `<int>` tag.

#### 2.3 Return types are fixed

Every function always returns the same type, whatever its input. This is required by the semantic analyzer, which checks types before the program runs.

| Group | Return type |
| --- | --- |
| `abs`, `sqrt`, `cbrt`, `pow`, `root`, `fmod`, `min`, `max`, `clamp` | `float` |
| `exp`, `exp2`, `log`, `log2`, `log10`, `log1p` | `float` |
| All trigonometric, hyperbolic, `degrees`, `radians` | `float` |
| `floor`, `ceil`, `round`, `trunc`, `sign` | `int` |
| `isnan`, `isinf`, `isfinite` | `bool` |
| `random` | `float` |
| `randint`, `seed_random` | `int` |
| `print`, `println` | nothing (empty value) |
| `input` | `str` |

The declared return type is checked by the analyzer, but assigning a numeric result to the other numeric variable type converts it. A `float` assigned to `int` is converted with `static_cast<int64_t>`; on the tested build this discards the fractional part toward zero. The example below prints the declared return values and the converted values.

See the runnable [return-type and conversion example](examples/builtin-return-types.vlt) and its [captured output](examples/builtin-return-types.expected): `abs(-5)` is stored as `float`, `abs(-5.7)` is stored as `int` and prints `5`, and `floor(2.7)` is stored as either `int` or `float`.

#### 2.4 Mistakes caught before the program runs

The semantic analyzer checks every call **before execution**. These are reported as errors without running any code:

- A required argument is missing: `CALL sin END`.
- An argument name that the function does not have: `CALL sqrt y<float>:4 END`.
- Too many arguments: `CALL sqrt x<float>:4 y<float>:1 END`.
- A value that does not match its own type tag: `x<int>:2.5`.

#### 2.5 Mistakes caught while running

Some problems depend on the values and can only be found at run time: `sqrt` of a negative number, `log(0)`, a result too large to represent, and so on. The runtime reports an error with a **type** and a **message**:

| Error type | Meaning |
| --- | --- |
| `TypeError` | An argument is missing or is not a number where a number is required |
| `ValueError` | The value is outside the function's domain (`sqrt(-1)`, `log(0)`) |
| `OverflowError` | The result is too large to represent |
| `ZeroDivisionError` | Division by zero, or a power that would divide by zero |
| `EOFError` | `input` reached the end of the input stream |

Messages always start with the function name, for example `sqrt(): math domain error`. Errors are reported to the host runtime; scripts do not currently provide an error-catching construct. Validate inputs with `IF` where practical (see [Errors and how to avoid them](#16-errors-and-how-to-avoid-them)).

#### 2.6 Numbers

- `int` is a 64-bit signed integer: from `INT_MIN` to `INT_MAX`.
- `float` is a `long double`. On Linux and macOS (GCC/Clang on x86) it has 64 bits of precision, so every `int` value converts to `float` exactly. On Windows with MSVC, `long double` is the same as `double`, and integers above 2^53 lose precision when converted to `float`.
- Math functions never return `NaN` or `INF` silently for ordinary inputs. They return an error instead (see section 2.5). `NaN` and `INF` only appear when you create them yourself with the constants `NAN` and `INF`.

---

### 3. Quick reference table

| Function | Arguments | Returns | Description |
| --- | --- | --- | --- |
| `abs` | `x` | float | Absolute value |
| `sqrt` | `x` | float | Square root |
| `cbrt` | `x` | float | Cube root (negatives allowed) |
| `sign` | `x` | int | -1, 0, or 1 |
| `pow` | `base`, `exp` | float | Power |
| `root` | `x`, `n` (int) | float | n-th root |
| `floor` | `x` | int | Round down |
| `ceil` | `x` | int | Round up |
| `round` | `x` | int | Round to nearest (halves away from zero) |
| `trunc` | `x` | int | Drop the fractional part |
| `fmod` | `a`, `b` | float | Floating-point remainder |
| `min` | `a`, `b` | float | Smaller of two |
| `max` | `a`, `b` | float | Larger of two |
| `clamp` | `x`, `lo`, `hi` | float | Limit to a range |
| `exp` | `x` | float | e to the power x |
| `exp2` | `x` | float | 2 to the power x |
| `log` | `x` | float | Natural logarithm |
| `log2` | `x` | float | Base-2 logarithm |
| `log10` | `x` | float | Base-10 logarithm |
| `log1p` | `x` | float | log(1 + x), accurate for small x |
| `sin`, `cos`, `tan` | `x` | float | Trigonometry (radians) |
| `asin`, `acos`, `atan` | `x` | float | Inverse trigonometry (radians) |
| `atan2` | `y`, `x` | float | Angle of the point (x, y) |
| `sinh`, `cosh`, `tanh` | `x` | float | Hyperbolic functions |
| `degrees` | `x` | float | Radians to degrees |
| `radians` | `x` | float | Degrees to radians |
| `isnan` | `x` | bool | Is the value NaN? |
| `isinf` | `x` | bool | Is the value infinite? |
| `isfinite` | `x` | bool | Is the value finite (not NaN or infinite)? |
| `random` | none | float | Random number in \[0, 1) |
| `randint` | `lo`, `hi` (int) | int | Random integer in \[lo, hi\] |
| `seed_random` | `seed` (int) | int | Set the random seed |
| `print` | `value`, `end` (optional) | none | Print without a newline |
| `println` | `value` (optional) | none | Print with a newline |
| `input` | `prompt` (optional) | str | Read one line |

---

### 4. Constants

Constants are read with `GET`, like variables. They cannot be changed.

| Name | Type | Value |
| --- | --- | --- |
| `PI` | float | 3.14159265358979323846... |
| `E` | float | 2.71828182845904523536... |
| `TAU` | float | 2 × PI = 6.28318530717958647692... |
| `INF` | float | Positive infinity |
| `NAN` | float | Not a number |
| `INT_MAX` | int | 9223372036854775807 |
| `INT_MIN` | int | -9223372036854775808 |
| `FLOAT_MAX` | float | Largest finite float |
| `FLOAT_MIN` | float | Smallest positive normal float |
| `FLOAT_EPSILON` | float | Difference between 1.0 and the next larger float |

```
// Circumference of a circle with radius 3
SET r<float> = 3
CALL println value<float>:TAU * (GET r) END

// Negative infinity
SET minus_inf<float> = 0 - (GET INF)

// Check that a number fits the int range
IF (GET INT_MAX) > 0 THEN
    CALL println value<str>:"INT_MAX is positive" END
END
```

Notes:

- `NAN` is never equal to anything, **including itself**. Use `isnan` to test for it, not `==`.
- `FLOAT_EPSILON` is useful for comparing floats: instead of `a == b`, check that the difference between them is smaller than a small tolerance.

---

### 5. Basic functions

#### abs

Absolute value.

- **Arguments:** `x` (int or float)
- **Returns:** float
- **Errors:** `TypeError` if `x` is not a number

```
CALL println value<float>:CALL abs x<int>:-5 END END       // Output: 5
CALL println value<float>:CALL abs x<float>:-2.5 END END   // Output: 2.5
```

`abs` always returns a float, even for an int argument, so store it in a `float` variable.

#### sqrt

Square root.

- **Arguments:** `x` (int or float, must be 0 or greater)
- **Returns:** float
- **Errors:** `ValueError` if `x` is negative

```
CALL println value<float>:CALL sqrt x<float>:16 END END    // Output: 4
CALL println value<float>:CALL sqrt x<float>:2 END END     // Output: ≈ 1.41421356
```

#### cbrt

Cube root. Unlike `sqrt`, negative numbers are allowed.

- **Arguments:** `x` (int or float)
- **Returns:** float
- **Errors:** `TypeError` only

```
CALL println value<float>:CALL cbrt x<float>:27 END END    // Output: 3
CALL println value<float>:CALL cbrt x<float>:-8 END END    // Output: -2
```

#### sign

The sign of a number: `-1` for negative, `0` for zero, `1` for positive.

- **Arguments:** `x` (int or float)
- **Returns:** int
- **Errors:** `ValueError` if `x` is `NAN`

```
CALL println value<int>:CALL sign x<int>:-9 END END        // Output: -1
CALL println value<int>:CALL sign x<int>:0 END END         // Output: 0
CALL println value<int>:CALL sign x<float>:3.5 END END     // Output: 1
```

A common use is choosing a direction:

```
SET speed<float> = 4
SET velocity<float> = (CALL sign x<float>:GET target END) * (GET speed)
```

---

### 6. Power functions

#### pow

Raises `base` to the power `exp`.

- **Arguments:** `base` (int or float), `exp` (int or float)
- **Returns:** float
- **Errors:**
  - `ZeroDivisionError` if `base` is 0 and `exp` is negative
  - `ValueError` if the result is not a real number (for example `pow(-8, 0.5)`)
  - `OverflowError` if the result is too large. With two `<int>` arguments and a non-negative exponent, the implementation uses checked signed 64-bit arithmetic and fails if the result is outside the int range, even though the declared return type is `float`. Other cases use platform `long double` arithmetic.

When both arguments are tagged `<int>` and `exp` is non-negative, the result is computed with checked signed 64-bit integer arithmetic and then returned as `float`. Values outside the int range raise `OverflowError`. Other inputs use floating-point arithmetic, with precision and range determined by the platform’s `long double`.

```
CALL println value<float>:CALL pow base<int>:2 exp<int>:10 END END        // Output: 1024
CALL println value<float>:CALL pow base<int>:-3 exp<int>:3 END END       // Output: -27
CALL println value<float>:CALL pow base<int>:2 exp<int>:-1 END END       // Output: 0.5
CALL println value<float>:CALL pow base<float>:2 exp<float>:0.5 END END  // Output: ≈ 1.41421356
```

Compound interest: 1000 invested at 5% for 10 years.

```
SET factor<float> = CALL pow base<float>:1.05 exp<int>:10 END
CALL println value<float>:1000 * (GET factor) END   // Output: ≈ 1628.89
```

Tip: `pow` returns a `float` even for int inputs. With two `<int>` arguments and a non-negative exponent, `pow` computes using checked integer multiplication and rejects results outside the signed 64-bit integer range, then exposes the result as `float`. Other inputs use floating-point `pow`; precision and range depend on the platform’s `long double` implementation.

#### root

The n-th root of a number.

- **Arguments:** `x` (int or float), `n` (**int**, not 0)
- **Returns:** float
- **Errors:**
  - `ValueError` if `n` is 0
  - `ValueError` if `x` is negative and `n` is even (no real answer)
  - `ZeroDivisionError` if `x` is 0 and `n` is negative
  - `TypeError` if `n` is not tagged `<int>`

Odd roots of negative numbers are allowed.

```
CALL println value<float>:CALL root x<float>:27 n<int>:3 END END     // Output: 3
CALL println value<float>:CALL root x<float>:-8 n<int>:3 END END     // Output: -2
CALL println value<float>:CALL root x<float>:16 n<int>:4 END END     // Output: 2
```

---

### 7. Rounding functions

All four functions take one number and return an **int**. They differ in how they treat the fractional part.

| Input | `floor` | `ceil` | `round` | `trunc` |
| --- | --- | --- | --- | --- |
| 2.7 | 2 | 3 | 3 | 2 |
| 2.5 | 2 | 3 | 3 | 2 |
| 2.4 | 2 | 3 | 2 | 2 |
| -2.1 | -3 | -2 | -2 | -2 |
| -2.5 | -3 | -2 | -3 | -2 |
| -2.9 | -3 | -2 | -3 | -2 |

- **Arguments for all four:** `x` (int or float)
- **Returns:** int
- **Errors for all four:**
  - `ValueError` if `x` is `NAN` or `INF` (cannot become an int)
  - `OverflowError` if the result does not fit in an int

If the argument is already an int, it is returned unchanged.

#### floor

Rounds toward negative infinity (the largest int not greater than `x`).

```
CALL println value<int>:CALL floor x<float>:2.7 END END    // Output: 2
CALL println value<int>:CALL floor x<float>:-2.1 END END   // Output: -3
```

#### ceil

Rounds toward positive infinity (the smallest int not less than `x`).

```
CALL println value<int>:CALL ceil x<float>:2.1 END END     // Output: 3
CALL println value<int>:CALL ceil x<float>:-2.9 END END    // Output: -2
```

#### round

Rounds to the nearest int. A value exactly halfway rounds **away from zero**, so `round(2.5)` is 3 and `round(-2.5)` is -3. (This differs from Python, which rounds halves to the nearest even number.)

```
CALL println value<int>:CALL round x<float>:2.5 END END    // Output: 3
CALL println value<int>:CALL round x<float>:-2.5 END END   // Output: -3
CALL println value<int>:CALL round x<float>:2.4 END END    // Output: 2
```

Rounding to a number of decimal places:

```
SET x<float> = 3.14159
SET scaled<float> = (GET x) * 100
SET whole<int> = CALL round x<float>:GET scaled END
CALL println value<float>:(GET whole) / 100.0 END          // Output: 3.14
```

#### trunc

Removes the fractional part (rounds toward zero).

```
CALL println value<int>:CALL trunc x<float>:2.9 END END    // Output: 2
CALL println value<int>:CALL trunc x<float>:-2.9 END END   // Output: -2
```

Use `floor` when you need "the integer part" of positive numbers only, and `trunc` when negative numbers should behave symmetrically.

---

### 8. Remainder and comparison functions

#### fmod

The floating-point remainder of `a` divided by `b`. The result has the **same sign as `a`**.

- **Arguments:** `a` (int or float), `b` (int or float)
- **Returns:** float
- **Errors:** `ZeroDivisionError` if `b` is 0

```
CALL println value<float>:CALL fmod a<float>:7.5 b<float>:2 END END     // Output: 1.5
CALL println value<float>:CALL fmod a<float>:-7.5 b<float>:2 END END    // Output: -1.5
```

The language also has the operators `%` (modulo) and `%%` (integer division). Use `fmod` when you need a remainder of floating-point numbers.

#### min and max

The smaller or larger of **two** numbers.

- **Arguments:** `a`, `b` (int or float)
- **Returns:** float
- **Errors:** `TypeError` for non-numbers

```
CALL println value<float>:CALL min a<int>:3 b<int>:8 END END         // Output: 3
CALL println value<float>:CALL max a<float>:2.5 b<int>:1 END END     // Output: 2.5
```

To find the smallest of three values, nest the calls:

```
SET smallest<float> = CALL min a<float>:GET x b<float>:CALL min a<float>:GET y b<float>:GET z END END
```

#### clamp

Limits a value to the range from `lo` to `hi`. If `x` is below `lo` the result is `lo`; if above `hi` the result is `hi`; otherwise it is `x`.

- **Arguments:** `x`, `lo`, `hi` (int or float)
- **Returns:** float
- **Errors:** `ValueError` if `lo` is greater than `hi`

```
CALL println value<float>:CALL clamp x<int>:15 lo<int>:0 hi<int>:10 END END    // Output: 10
CALL println value<float>:CALL clamp x<int>:-5 lo<int>:0 hi<int>:10 END END    // Output: 0
CALL println value<float>:CALL clamp x<int>:5 lo<int>:0 hi<int>:10 END END     // Output: 5
```

Keeping a health value between 0 and 100:

```
SET health<float> = 140
SET health = CALL clamp x<float>:GET health lo<float>:0 hi<float>:100 END
```

---

### 9. Exponential and logarithmic functions

All of these take one number `x` and return a float.

| Function | Computes | Domain | Errors |
| --- | --- | --- | --- |
| `exp` | e^x | any number | `OverflowError` if the result is too large (when the result exceeds the platform’s representable `long double` range) |
| `exp2` | 2^x | any number | `OverflowError` if the result is too large |
| `log` | ln(x) | x > 0 | `ValueError` if x is 0 or negative |
| `log2` | log base 2 | x > 0 | `ValueError` if x is 0 or negative |
| `log10` | log base 10 | x > 0 | `ValueError` if x is 0 or negative |
| `log1p` | ln(1 + x) | x > -1 | `ValueError` if x is -1 or less |

```
CALL println value<float>:CALL exp x<float>:1 END END         // Output: ≈ 2.71828183
CALL println value<float>:CALL exp2 x<float>:10 END END       // Output: 1024
CALL println value<float>:CALL log x<float>:GET E END END     // Output: 1
CALL println value<float>:CALL log2 x<float>:8 END END        // Output: 3
CALL println value<float>:CALL log10 x<float>:1000 END END    // Output: 3
```

**Why `log1p`?** For a very small `x`, `1 + x` loses almost all of its digits, so `log(1 + x)` is inaccurate. `log1p(x)` computes the same value without that loss.

**Logarithm of any base:** divide two logarithms.

```
// log base 5 of 125
SET result<float> = (CALL log x<float>:125 END) / (CALL log x<float>:5 END)   // ≈ 3
```

**How many digits does an integer have?**

```
SET n<float> = 12345
SET digits<int> = (CALL floor x<float>:CALL log10 x<float>:GET n END END) + 1
CALL println value<int>:GET digits END     // Output: 5
```

---

### 10. Trigonometric and hyperbolic functions

Angles are always in **radians**. To work with degrees, convert with `radians` and `degrees` (section 11).

#### sin, cos, tan

- **Arguments:** `x` (int or float, in radians)
- **Returns:** float
- **Errors:** `TypeError` for non-numbers

```
CALL println value<float>:CALL sin x<float>:0 END END                   // Output: 0
CALL println value<float>:CALL sin x<float>:(GET PI)/2 END END          // Output: 1
CALL println value<float>:CALL cos x<float>:GET PI END END              // Output: -1
CALL println value<float>:CALL tan x<float>:(GET PI)/4 END END          // Output: ≈ 1
```

Results near zero may show tiny rounding noise (for example `sin(PI)` is about `1e-19`, not exactly 0). Compare with a tolerance, not with `==`.

#### asin, acos, atan

The inverse functions. They return an angle in radians.

| Function | Domain | Result range |
| --- | --- | --- |
| `asin` | x from -1 to 1 | -PI/2 to PI/2 |
| `acos` | x from -1 to 1 | 0 to PI |
| `atan` | any number | -PI/2 to PI/2 |

- **Errors:** `ValueError` if `x` is outside \[-1, 1\] for `asin` and `acos`

```
CALL println value<float>:CALL asin x<float>:1 END END     // Output: ≈ 1.5707963 (PI/2)
CALL println value<float>:CALL acos x<float>:1 END END     // Output: 0
CALL println value<float>:CALL atan x<float>:1 END END     // Output: ≈ 0.7853982 (PI/4)
```

#### atan2

The angle, in radians, of the point (`x`, `y`) measured from the positive x-axis. Unlike `atan(y / x)`, it knows which quadrant the point is in and it works when `x` is 0.

- **Arguments:** `y`, `x` (int or float). Note the order: **`y` is the first argument.**
- **Returns:** float, from -PI to PI
- **Errors:** `TypeError` for non-numbers

```
CALL println value<float>:CALL atan2 y<float>:1 x<float>:1 END END      // Output: ≈ 0.7853982 (45 degrees)
CALL println value<float>:CALL atan2 y<float>:1 x<float>:-1 END END     // Output: ≈ 2.3561945 (135 degrees)
```

Because arguments are named, the order you write them in does not matter.

#### sinh, cosh, tanh

Hyperbolic sine, cosine, and tangent.

- **Arguments:** `x`
- **Returns:** float
- **Errors:** `OverflowError` for `sinh` and `cosh` if `x` is very large (when the result exceeds the platform’s representable `long double` range)

```
CALL println value<float>:CALL sinh x<float>:0 END END     // Output: 0
CALL println value<float>:CALL cosh x<float>:0 END END     // Output: 1
CALL println value<float>:CALL tanh x<float>:0 END END     // Output: 0
```

`tanh` squeezes any number into the range from -1 to 1, which makes it a handy "activation" function.

---

### 11. Angle conversion

#### radians

Converts degrees to radians.

- **Arguments:** `x` (degrees)
- **Returns:** float

#### degrees

Converts radians to degrees.

- **Arguments:** `x` (radians)
- **Returns:** float

```
CALL println value<float>:CALL radians x<float>:180 END END          // Output: ≈ 3.14159265
CALL println value<float>:CALL degrees x<float>:GET PI END END       // Output: 180

// Sine of 30 degrees
CALL println value<float>:CALL sin x<float>:CALL radians x<float>:30 END END END    // Output: ≈ 0.5
```

---

### 12. Numeric utilities

Three functions classify a number. All take one argument `x`, accept int or float, and return a **bool**.

| Function | True when |
| --- | --- |
| `isnan` | `x` is not a number |
| `isinf` | `x` is positive or negative infinity |
| `isfinite` | `x` is finite: neither NaN nor positive or negative infinity |

An int is never NaN and never infinite, so `isnan` and `isinf` return false for ints and `isfinite` returns true.

```
CALL println value<bool>:CALL isnan x<float>:GET NAN END END       // Output: true
CALL println value<bool>:CALL isinf x<float>:GET INF END END       // Output: true
CALL println value<bool>:CALL isfinite x<int>:10 END END           // Output: true
CALL println value<bool>:CALL isfinite x<float>:GET INF END END    // Output: false
```

Because `NAN == NAN` is false, `isnan` is the only reliable way to test for it.

---

### 13. Random numbers

All random functions share one generator (a 64-bit Mersenne Twister). If you never call `seed_random`, it starts from a different seed every run.

#### random

A random float from 0 up to, but not including, 1.

- **Arguments:** none
- **Returns:** float

```
SET r<float> = CALL random END
CALL println value<float>:GET r END      // for example 0.4837...
```

A random float in a range \[a, b): `a + (b - a) * random()`.

```
SET temperature<float> = 15 + (30 - 15) * (CALL random END)    // between 15 and 30
```

#### randint

A random integer from `lo` to `hi`, **both included**.

- **Arguments:** `lo` (int), `hi` (int)
- **Returns:** int
- **Errors:** `ValueError` if `lo` is greater than `hi`; `TypeError` if either is not an int

```
CALL println value<int>:CALL randint lo<int>:1 hi<int>:6 END END     // a die roll: 1 to 6
CALL println value<int>:CALL randint lo<int>:7 hi<int>:7 END END     // always 7
```

#### seed\_random

Sets the seed of the generator, so the same sequence of numbers is produced every time. Useful for tests and reproducible simulations.

- **Arguments:** `seed` (int)
- **Returns:** int (the seed you passed)
- **Errors:** `TypeError` if `seed` is not an int

```
CALL seed_random seed<int>:42 END
SET a<float> = CALL random END

CALL seed_random seed<int>:42 END
SET b<float> = CALL random END

// a and b are equal
```

**Rolling five dice with a fixed seed:**

```
CALL seed_random seed<int>:2026 END

FOR
    SET i<int> = 0;
    GET i < 5;
    SET i = GET i +1
THEN
    CALL print value<str>:"Roll: " END
    CALL println value<int>:CALL randint lo<int>:1 hi<int>:6 END END
END
```

Run it twice: you get the same five rolls both times. Remove the `seed_random` line and the rolls change on every run.

### 14. STDIO: input and output

The STDIO library provides `print`, `println`, and `input`. The runtime registers these functions when it is created; scripts do not need an import statement.

#### `print`

Writes `value` followed by `end` without adding a newline. `end` defaults to the empty string.

```
CALL print value<str>:"Hello" end<str>:" " END
CALL print value<str>:"world" END
```

Output: `Hello world` (no newline)

#### `println`

Writes `value` followed by a newline. The `value` argument accepts `any`, so it can receive `str`, `int`, `float`, or `bool`; strings are printed without quotation marks.

```
CALL println value<str>:"Hello, ValtCommand!" END
```

#### `input`

Writes the optional `prompt` (empty by default), reads one line from standard input, and returns that line as `str`. If input ends before a line is read, the runtime reports `EOFError`. The example uses `format`; see the [Format Function](#format-function) section above.

```
SET name<str> = CALL input prompt<str>:"Name: " END
CALL println value<str>:CALL format text<str>:"Hello {name}!" name<str>:GET name END END
```

### 15. Complete examples

#### Report a calculation

```
SET radius<float> = 4
SET area<float> = (GET PI) * (GET radius) ** 2
CALL println value<str>:CALL format text<str>:"Circle area: {area}" area<float>:GET area END END
```

Output: `Circle area: ≈ 50.2655`

#### Read and format input

```
SET name<str> = CALL input prompt<str>:"Your name: " END
CALL println value<str>:CALL format text<str>:"Welcome, {name}." name<str>:GET name END END
```

### 16. Errors and how to avoid them

Errors may be detected while parsing, during semantic analysis, or while a built-in function runs. The host receives a failed runtime result; the runtime API exposes the error stage, message, and source location where available.

Common built-in error types include:

| Error type | Typical cause |
|---|---|
| `TypeError` | A value is not an accepted type for an argument. |
| `ValueError` | An argument is outside a function’s mathematical domain. |
| `OverflowError` | A computed result cannot be represented. |
| `ZeroDivisionError` | A function operation divides by zero, including `pow(0, negative_exponent)`. |
| `ArgumentError` | `format` is missing `text` or a referenced placeholder argument. |
| `FormatError` | `format` receives an unmatched `{` or an empty placeholder. |
| `EOFError` | `input` cannot read a line because the input stream has ended. |

Check values before calling functions with restricted domains. For example, ensure a square-root input is non-negative and a logarithm input is positive. Exact floating-point limits vary by platform because `float` is backed by C++ `long double`.

### 17. Common mistakes

- Use `<int>` for integer-only parameters such as `randint` bounds, `seed_random` seeds, and `root`'s `n`.
- Use the documented return type when assigning a function result. `abs`, `min`, `max`, and `clamp` return `float`, including for integer inputs; see the type mismatch example in [2.3 Return types are fixed](#23-return-types-are-fixed).
- `pow` declares a `float` return type. For two integer arguments with a non-negative exponent, the implementation still rejects intermediate or final results outside the signed 64-bit integer range.
- Trigonometric functions take radians. Convert degrees with `radians` first.
- `isfinite` means “not NaN and not infinite”; it also returns true for zero and subnormal finite values.
- `format` recognizes `{name}` placeholders only. Literal brace escaping and format specifiers such as `{price:.2f}` are not supported.
- Use `isnan` to test NaN; equality comparisons with NaN are always false.

### 18. Using the libraries from C++

The standard libraries are registered by `vltc::Runtime::RunTime` during initialization. A host program can run ValtCommand source through the public runtime API:

```cpp
#include "valtcommand/runtime.hpp"
#include <iostream>

int main() {
    vltc::Runtime::RunTime runtime;
    auto result = runtime.run("CALL println value<float>:GET PI END");
    if (!result) {
        std::cerr << result.error.message << '\n';
        return 1;
    }
}
```

The runtime API and result type are documented in `include/valtcommand/runtime.hpp` and `include/valtcommand/result.hpp`.

### 19. Known limitations

- `format` supports named `{placeholder}` substitution only. It has no escaping for literal braces and no numeric precision, alignment, or padding specifiers.
- Math functions use the platform’s C++ `long double` implementation. Precision, representable range, and overflow thresholds can differ across compilers and platforms; examples marked `≈` are approximate.
- `abs`, `min`, `max`, and `clamp` have fixed `float` return types; they do not preserve integer input types.
- `pow` has a fixed `float` return type. Its non-negative integer path uses checked signed 64-bit arithmetic before converting the result to `long double`; floating-point cases follow platform precision and range.
- Script-level exception handling is not currently part of the language.
