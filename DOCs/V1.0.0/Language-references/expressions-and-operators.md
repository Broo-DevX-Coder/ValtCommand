# Expressions and Operators

**Version:** 1.0.0

This reference describes literal values, expression parsing, operator precedence, and the behavior implemented by the current ValtCommand interpreter.

## Literal values

| Literal form | Type | Example |
|---|---|---|
| Whole-number literal | `int` | `42`, `0`, `9000` |
| Decimal numeric literal | `float` | `3.14`, `0.5` |
| String literal | `str` | `"hello"`, `'hello'` |
| Boolean literal | `bool` | `True`, `False` |

Numeric signs are unary operators, not part of the numeric token. Parentheses group an expression:

```text
-(2 + 3)
(1 + 2) * 3
```

String literals may use either quote style. The lexer maps `\n`, `\t`, `\\`, `\"`, and `\'` to their corresponding characters. Unknown escapes preserve the backslash and following byte as written; strings cannot contain a literal line feed. Numeric tokens start with a digit and may contain one decimal point; scientific notation is not recognized by the lexer.

### Names and calls as expressions

Names and calls can also be expression operands. `GET` consumes one variable name as a primary expression; for example, `GET x + GET y` is parsed as `(GET x) + (GET y)`.

```text
GET total
CALL sqrt x<float>:16 END
```

## Operator precedence

Operators in earlier rows bind more tightly than operators in later rows. Use parentheses to make grouping explicit.

| Precedence | Operators | Associativity / rule |
|---:|---|---|
| 1 (highest) | `**` | Right-associative; the right operand accepts a unary sign |
| 2 | Unary `+`, unary `-`, unary `!` | Right-associative; `**` binds more tightly than a leading sign |
| 3 | `*`, `/`, `%%`, `%` | Left-associative |
| 4 | `+`, `-` | Left-associative |
| 5 | `<`, `>`, `<=`, `>=` | Non-associative; chained comparisons are syntax errors |
| 6 | `==`, `!=` | Non-associative; chained equality comparisons are syntax errors |
| 7 | `&&` | Left-associative |
| 8 (lowest) | `\|\|` | Left-associative |

Examples:

```text
2 + 3 * 4       // parsed as 2 + (3 * 4)
2 ** 3 ** 2     // parsed as 2 ** (3 ** 2)
2 ** -1         // valid; evaluates to 0.5
-2 ** 2         // parsed as -(2 ** 2), evaluates to -4.0
(2 + 3) * 4     // parentheses change the grouping
```

## Arithmetic operators

Binary arithmetic operators require numeric (`int` or `float`) operands and return `float`, including `%%` and `%` even though their numeric results are integer-valued. Unary `+` and `-` preserve the operand type.

| Operator | Meaning | Behavior |
|---|---|---|
| `+` | Addition | Adds the operands. |
| `-` | Subtraction | Subtracts the right operand from the left. |
| `*` | Multiplication | Multiplies the operands. |
| `/` | Division | Floating-point division. A zero divisor reports `RuntimeError`. |
| `%%` | Integer division | Divides numerically, truncates the quotient toward zero, and returns it as `float`. A zero divisor reports `RuntimeError`. |
| `%` | Integer remainder | Converts both operands to signed 64-bit integers, truncating toward zero, and returns the remainder as `float`. If the converted divisor is zero, it reports `RuntimeError`. Out-of-range conversions report `OverflowError`. |
| `**` | Exponentiation | Computes the left operand raised to the right operand. Returns `float`; invalid results report `ValueError`, and infinite results report `OverflowError`. |

The `.0` suffix in result comments indicates the expression type is `float`; the interpreter may omit trailing zeroes when displaying a value. The `%` and `%%` operators are distinct from the `fmod` library function. `fmod` works as a standard-library call and reports `ZeroDivisionError` for a zero divisor; the operators report the core `RuntimeError`.

```text
SET quotient<float> = 7 / 2       // 3.5
SET whole<float> = 7 %% 2         // 3.0 (the expression type is float)
SET remainder<float> = 7 % 2      // 1.0 (the expression type is float)
SET squared<float> = 4 ** 2       // 16.0
```

Both `%` and `%%` expressions have the static type `float`; assigning either directly to an `<int>` variable is a type error. `%` truncates each operand toward zero before converting it to signed 64-bit integer, then calculates the remainder. For example, `7.9 % 2` behaves like `7 % 2` and produces `1.0`. A divisor such as `0.5` truncates to zero, so `7 % 0.5` reports `RuntimeError: division by zero`. `%` reports `OverflowError` if either truncated operand is outside the signed 64-bit range. `%%` divides the numeric operands, truncates the quotient toward zero, then returns that integer-valued result as `float`.

## Unary operators

| Operator | Meaning | Operand requirement | Result type |
|---|---|---|---|
| `+x` | Unary positive sign | `int` or `float` | Preserves operand type |
| `-x` | Unary negative sign | `int` or `float` | Preserves operand type |
| `!x` | Logical NOT | `bool` | `bool` |

```text
SET positive<int> = +5
SET negative<int> = -5
SET enabled<bool> = !False
```

## Comparison operators

Comparison expressions return `bool`.

| Operator | Meaning |
|---|---|
| `==` | Equal to |
| `!=` | Not equal to |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal to |
| `>=` | Greater than or equal to |

Equality supports operands of compatible types: matching types (including `str` with `str` and `bool` with `bool`), or a numeric `int`/`float` pair. Ordered comparisons (`<`, `>`, `<=`, `>=`) require numeric operands; strings cannot be ordered lexicographically. Chained comparisons such as `1 < 2 < 3` are rejected; combine comparisons with `&&` instead. `GET` consumes a variable name as a primary expression, so `GET score >= 0` parses as `(GET score) >= 0`. Use parentheses to make larger groupings explicit:

```text
(GET score >= 0) && (GET score <= 100)
```

## Logical operators

Logical operators require `bool` operands and return `bool`.

| Operator | Meaning |
|---|---|
| `&&` | Logical AND |
| `\|\|` | Logical OR |
| `!` | Logical NOT (see unary operators) |

```text
(True && True) || False
!False && True
```

`&&` and `||` short-circuit from left to right. `&&` does not evaluate its right operand when the left result is `False`; `||` does not evaluate its right operand when the left result is `True`. If the left value does not determine the result, the right operand is evaluated.

Logical expressions always return a `bool`; they do not return one of their operands. A skipped operand cannot raise a runtime error (for example, division by zero) and its function calls do not run, so their side effects do not occur. Type checking still analyzes the right operand, including when runtime short-circuiting would skip it.

The skipped divisions do not raise an error, and the skipped call prints no `SIDE_EFFECT`; the evaluated right operands run and print `RHS`. See the [skipped evaluation example](examples/logical-short-circuit-skipped.vlt) and [its output](examples/logical-short-circuit-skipped.expected), and the [evaluated example](examples/logical-short-circuit-evaluated.vlt) with [its output](examples/logical-short-circuit-evaluated.expected).

## Type checking and evaluation

The interpreter parses an expression, checks operand types during semantic analysis, then evaluates it. Arithmetic operators reject nonnumeric operands during semantic analysis. Logical operators and `!` require booleans. Equality checks type compatibility; ordered comparisons additionally require numbers.

Numeric operations convert operands to the runtime's numeric representation. Their precision and range follow the platform's `long double` implementation. Use parentheses when expression grouping should be explicit.

## Errors

Typical expression errors include:

- `SyntaxError` for malformed expressions or chained comparisons.
- `TypeError` when an arithmetic operator receives a nonnumeric value, or a logical operator receives a non-boolean value.
- `IncompatibleTypesError` when comparison operands have incompatible types or an ordered comparison uses nonnumeric operands.
- `RuntimeError: division by zero` for `/`, `%%`, or `%` with a zero divisor.
- `ValueError` when `**` produces an invalid result.
- `OverflowError` when `**` produces a non-finite result, or `%` cannot convert an operand to signed 64-bit integer.

See [Errors](errors.md) for error stages and formatted diagnostic examples.
