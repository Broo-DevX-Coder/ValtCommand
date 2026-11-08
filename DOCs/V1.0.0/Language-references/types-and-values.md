# Types and Values

**Version:** 1.0.0

The interpreter's source type spellings are `int`, `float`, `bool`, `str`, `void`, and `any`. Its runtime `Value` is a C++ variant containing `std::monostate`, `int64_t`, `long double`, `std::string`, or `bool`; `any` is a declared type marker, not a separate runtime variant alternative.

## Type names and runtime values

| Source type | Runtime representation | Meaning |
|---|---|---|
| `int` | `int64_t` | Signed 64-bit integer |
| `float` | `long double` | Floating-point number; range and precision depend on the host platform |
| `bool` | `bool` | Boolean value |
| `str` | `std::string` | Text |
| `void` | `std::monostate` | No returned value |
| `any` | The actual contained value | A type accepted by compatible declarations and generic function arguments |

`Get_ValueT` reports `int`, `float`, `bool`, `void`, and `str` for their corresponding runtime alternatives. The runtime validation example reports a string argument as `str`; see [the `any` forwarding example](examples/types-any-runtime-string.vlt) and its [captured error](examples/types-any-runtime-string.expected).

`void` has no value literal: a `void` function call has no value result. `any` is used as a declaration or parameter type, not as a literal. See [the no-value function example](examples/types-void.vlt) and [its output](examples/types-void.expected), and [the `any` example](examples/types-any.vlt) with [its output](examples/types-any.expected).

## Literals

The parser constructs literal nodes for integer, decimal, boolean, and string tokens. Boolean literals use the exact, case-sensitive spellings `True` and `False`; `True` is the source spelling for the true boolean value. Signs are unary operators. String quoting and escapes follow [Lexical Structure](lexical-structure.md): single or double quotes are accepted; `\n`, `\t`, `\\`, `\"`, and `\'` are decoded; an unknown escape keeps its backslash and following character. [The literal example](examples/types-literals.vlt) exercises each of these forms; its [expected output](examples/types-literals.expected) is the interpreter's captured output.

## Integer range and overflow

The runtime integer representation is signed 64-bit. The built-in `INT_MIN` and `INT_MAX` constants identify the limits: `-9223372036854775808` and `9223372036854775807`. Both constants and boundary literals were printed in [the range example](examples/types-int-limits.vlt) and [its output](examples/types-int-limits.expected). The minimum boundary spelling prints the expected value, but its positive digit token exceeds `INT_MAX`; the unchecked conversion described below means that output is not a portable guarantee.

The numeric-literal evaluator parses through `long double` and casts an `int` literal to `int64_t` without checking that it is in range. In [the out-of-range example](examples/types-int-out-of-range.vlt), the positive literal `9223372036854775808` prints `-9223372036854775808`; the negative expression `-9223372036854775809` raises `OverflowError: cannot convert non-finite or out-of-range float to int` when assigned to an integer. The positive literal conversion remains unchecked; see [Known limitations](limitations.md#known-limitations).

Binary arithmetic converts operands to `long double` and has static result type `float`, even for two integer operands. Thus `INT_MAX + 1` produced a floating-point result rather than an integer overflow error in [the arithmetic example](examples/types-arithmetic-conversions.vlt) and [its output](examples/types-arithmetic-conversions.expected). Multiplying `FLOAT_MAX` by two produced `inf` without an error in [the float overflow example](examples/types-float-overflow.vlt) and [its output](examples/types-float-overflow.expected). The power operator checks its result: an infinite result raises `OverflowError`, as shown by [the power overflow example](examples/types-power-overflow.vlt) and [its error output](examples/types-power-overflow.expected). The checked integer paths used by `%` and `%%` also raise `OverflowError` when the integer result or remainder operands do not fit; see [remainder overflow](examples/types-integer-mod-overflow.vlt) and [its error output](examples/types-integer-mod-overflow.expected), and [integer-division overflow](examples/types-integer-div-overflow.vlt) and [its error output](examples/types-integer-div-overflow.expected).

## Float range and precision

`float` is backed by C++ `long double`, so its limits are platform-specific. The [float limits example](examples/types-float-limits.vlt) prints `FLOAT_MAX` as `1.18973e+4932`, `FLOAT_MIN` as `3.3621e-4932`, and `FLOAT_EPSILON` as `1.0842e-19`; `FLOAT_MIN` is the smallest positive normal value from `std::numeric_limits<long double>`, not the most negative value. Its captured output is [here](examples/types-float-limits.expected).

The [precision example](examples/types-float-precision.vlt) distinguishes `2^63` from `2^63 + 1`, but treats `2^64` and `2^64 + 1` as equal. Other `long double` implementations can have different precision and range.

## Numeric conversion and arithmetic results

When `int` and `float` values are used together in arithmetic, the operation returns `float`. The tested outputs were `3.5` for `1 + 2.5`, `0.5` for `1 / 2`, `3` for `7 %% 2`, `1` for `7 % 2`, and `8` for `2 ** 3`; the `float` result type is also checked by the `<float>` argument tags in [the example](examples/types-arithmetic-conversions.vlt). `/`, `%%`, `%`, and `**` all produce `float` results.

For assignment to a declared numeric variable, an `int`/`float` value is compatible and is converted to the declared type. In the tested assignment example, `2.5` assigned to `int` printed `2`, while integer `2` assigned to `float` printed `2`; see [the example](examples/types-assignment-int-float.vlt) and [its output](examples/types-assignment-int-float.expected). A nonnumeric mismatch such as assigning `str` to `int` is rejected with `TypeError: expected 'int', got 'str'`; see [the failing example](examples/types-assignment-incompatible.vlt) and [its error output](examples/types-assignment-incompatible.expected). Float-to-int conversion in user-defined function parameters is described in [Known limitations](limitations.md#known-limitations).

Numeric argument tags also accept the other numeric expression type: `value<int>:2.6` is converted to `2`, and `value<float>:5` supplies `5.0` to the call. The [numeric tag example](examples/limitation-numeric-tag-conversion.vlt) records both conversions for a built-in and a user-defined function.

Division by zero raises `RuntimeError: division by zero` for both integer and floating-point operands; it does not return `INF` or `NAN`. The behavior was tested separately for [integer operands](examples/types-int-division-zero.vlt) and their [error output](examples/types-int-division-zero.expected), and [float operands](examples/types-float-division-zero.vlt) and their [error output](examples/types-float-division-zero.expected).

## Constants, `NAN`, and `INF`

The runtime registers `PI`, `E`, `TAU`, `INF`, and `NAN` as floating-point constants. Their tested display values were `3.14159`, `2.71828`, `6.28319`, `inf`, and `nan`. The same example also confirmed that `isnan(NAN)` and `isinf(INF)` return `true`: [constants example](examples/types-constants.vlt), [captured output](examples/types-constants.expected).

The [comparison example](examples/types-nan-inf-comparisons.vlt) records `NAN == NAN` as false, `NAN != NAN` as true, `NAN < 1` as false, and `INF == INF` and `INF > 1` as true. Ordinary arithmetic can preserve or produce non-finite values: `INF + 1` prints `inf`, `INF - INF` and `INF / INF` print `-nan`, and `NAN + 1` prints `nan`. These outputs are recorded in the [comparison output](examples/types-nan-inf-comparisons.expected) and the [operation example](examples/types-nan-inf-operations.vlt) with [its output](examples/types-nan-inf-operations.expected). Power treats these results specially: `NAN ** 2` raises `ValueError` in [this example](examples/types-nan-power.vlt) ([output](examples/types-nan-power.expected)), while `INF ** 2` raises `OverflowError` in [this example](examples/types-inf-power.vlt) ([output](examples/types-inf-power.expected)).

Assigning `NAN`, `INF`, or `FLOAT_MAX` to an `int` variable raises `OverflowError: cannot convert non-finite or out-of-range float to int`. See the separate examples for [NAN](examples/types-nonfinite-to-int.vlt), [INF](examples/types-infinity-to-int.vlt), and [FLOAT_MAX](examples/types-float-max-to-int.vlt), with their captured outputs ([NAN](examples/types-nonfinite-to-int.expected), [INF](examples/types-infinity-to-int.expected), [FLOAT_MAX](examples/types-float-max-to-int.expected)).

## Comparisons, strings, and `any`

Equality works for matching types and numeric `int`/`float` pairs. The comparison example returns `true` for `1 == 1.0` and equal strings, and `false` for `True == False`; see [the comparison example](examples/types-comparisons.vlt) and [its output](examples/types-comparisons.expected). Comparing `str` with `int`, or `bool` with `int`, produces `IncompatibleTypesError: can't compare <type> with <type>`. Ordered string comparison produces `IncompatibleTypesError: Invalid operand types for ordered comparison between `str` and `str`` and the comment `Ordered comparisons require numeric operands`. The examples and outputs are [string/int equality](examples/types-string-int-equality.vlt) ([output](examples/types-string-int-equality.expected)), [bool/int equality](examples/types-bool-int-equality.vlt) ([output](examples/types-bool-int-equality.expected)), and [ordered string comparison](examples/types-string-order.vlt) ([output](examples/types-string-order.expected)).

Strings support equality but not ordered comparison. The `+` arithmetic operator does not concatenate strings; it rejects them with `TypeError: expected 'int | float', got 'str'`. See [the string addition example](examples/types-string-add.vlt) and [its captured error](examples/types-string-add.expected).

`any` is accepted by generic arguments such as `println` and can be assigned to a concrete numeric variable when its runtime value is numeric; [the tested example](examples/types-any.vlt) and [its output](examples/types-any.expected) show `5` twice. It cannot be used directly in arithmetic because the analyzer requires a statically known `int` or `float`; see [the arithmetic error example](examples/types-any-arithmetic.vlt) and [its error output](examples/types-any-arithmetic.expected). A generic value forwarded to a numeric built-in is checked at runtime: a string value produces `TypeError` with actual type `str` in [this example](examples/types-any-runtime-string.vlt) and [its error output](examples/types-any-runtime-string.expected). Assigning an `any` value with a nonnumeric payload to a concrete numeric variable raises `TypeError: cannot convert '<actual>' to '<target>'`. Examples cover [string to int](examples/types-any-string-to-int.vlt), [string to float](examples/types-any-string-to-float.vlt), [bool to int](examples/types-any-bool-to-int.vlt), and [void to int](examples/types-any-void-to-int.vlt), with corresponding captured outputs.

Logical expressions always return `bool`; see [Expressions and Operators](expressions-and-operators.md#logical-operators). Function-parameter conversion behavior is described in [Known limitations](limitations.md#known-limitations).
