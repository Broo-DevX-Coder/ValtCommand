# Errors

**Version:** 1.0.0

This page describes the `SyntaxError` and core error classes declared in `include/detail/errors.hpp` and implemented in `src/Errors.cpp`. Other categories such as `IncompatibleTypesError`, `ValueError`, and `OverflowError` are produced directly by AST nodes or library functions and returned through runtime error results; they are not classes in `Errors.cpp`. Standard libraries can produce additional categories; see [Built-in Functions](built-in-functions.md).

## Error stages

A script run passes through parsing, semantic analysis, and execution. The host API reports the stage in `vltc::Runtime::RunResult.error.stage`:

| Stage | Meaning |
|---|---|
| `Parse` | The lexer or parser could not produce a valid program. |
| `Semantic` | Names, types, declarations, function signatures, or other program rules failed validation before execution. |
| `Runtime` | Execution began and an operation failed. |
| `IO` | A file could not be opened by `run_file`. |
| `Host` | A host-side API operation failed. |

Core language diagnostics below list their usual stage. Some built-in functions use the same category names for errors they return during execution; the category alone does not identify the stage.

## Error information and source positions

Core diagnostics carry a `vltc::Errors::ErrorInfo` record:

| Field | Meaning |
|---|---|
| `type` | Category name, such as `SyntaxError` or `NameError`. |
| `message` | Main diagnostic message. |
| `comment` | Optional additional explanation. |
| `line` | Zero-based source line index. |
| `column` | Zero-based byte column within that line. |
| `length` | Number of source bytes to mark with the caret. |

The formatter prints the type and message, source lines around the error, and a caret marker. It displays source lines starting at one; `vltc::Errors::ErrorInfo` and `vltc::Runtime::RunResult` keep zero-based line indexes and byte columns. An optional comment appears after the excerpt. The public runtime result also exposes the stage, message, type, line, and column; see `include/valtcommand/result.hpp`.

## Core error types

| Type | Usual stage | Cause and message form |
|---|---|---|
| `SyntaxError` | Parse | Unexpected token: `unexpected token '<token>'`. Parser comments may add a specific hint. |
| `NameError` | Semantic | An undefined variable or function is referenced: `'<name>' is not defined`. Unknown function and variable lookups attach a comment identifying which kind of name was unknown. |
| `TypeError` | Semantic | A value is incompatible with an expected type. Variable assignment reports `Cannot assign value of type '<actual>' to variable '<name>' of type '<declared>'`. Built-ins may also return `TypeError` at runtime. |
| `ArgumentError` | Semantic | A function call has too many, unknown, missing, or wrongly typed arguments. |
| `IncompatibleTypesError` | Semantic | Comparison operands have incompatible types, or an ordered comparison uses nonnumeric operands. |
| `VariableError` | Semantic | A variable is declared twice in the same scope: `Variable '<name>' already defined in this scope`. |
| `ImmutableError` | Semantic | A constant variable is modified: `Cannot modify constant variable '<name>'`. |
| `ReturnTypeError` | Semantic | A function returns an incompatible type: `function '<function>' expected return type '<expected>', got '<actual>'`. |
| `MissingReturnError` | Semantic | A non-`void` function contains no `RETURN` statement: `Function '<function>' has no RETURN statement`. |
| `FunctionError` | Semantic | A function is defined twice in the same scope. |
| `RuntimeError` | Runtime | The `/` or `%%` operator has a zero divisor, or the divisor of `%` truncates to zero: `division by zero`. |
| `ValueError` | Runtime | The `**` operator produces an invalid numeric result. |
| `OverflowError` | Runtime | The `**` operator produces a non-finite result, or `%` cannot convert an operand to signed 64-bit integer. |

### Function argument diagnostics

The core semantic analyzer reports these `ArgumentError` cases before executing the script:

| Cause | Message form |
|---|---|
| Too many arguments | `Too many arguments to function '<function>'. Expected <expected>, but got <actual>` |
| Unknown argument name | `Unexpected argument '<argument>' in function '<function>' arguments` |
| Argument type mismatch | `Argument '<argument>' in function '<function>' has type '<actual>'; expected '<expected>'` |
| Required argument missing | `Function '<function>' is missing required argument '<argument>'` |

## Examples

These excerpts are from the current command-line interpreter. The formatter numbers displayed source lines from one and may show the preceding line as context.

### Syntax error

Source:

```text
CALL println value<str>:"Hi" END
END
```

Formatted diagnostic:

```text

SyntaxError: unexpected token 'END'
------------------------------------------
1-|  CALL println value<str>:"Hi" END
2-|  END
--|  ^^^
```

The extra `END` is encountered by the parser, so the stage is `Parse`.

### Undefined function

Source:

```text
CALL foo END
```

The call produces `NameError` with the comment `Unknown function`, during `Semantic` analysis. The interpreter formats it as:

```text

NameError: 'foo' is not defined
------------------------------------------
1-|  CALL foo END
--|       ^^^
------------------------------------------
Unknown function
```

Undefined variables also use `NameError`, with the comment `Unknown variable`.

### Incompatible variable assignment

Source:

```text
SET value<int> = 1
SET value = "x"
```

Formatted diagnostic:

```text

TypeError: Cannot assign value of type 'str' to variable 'value' of type 'int'
------------------------------------------
1-|  SET value<int> = 1
2-|  SET value = "x"
--|      ^^^^^
```

This is rejected during `Semantic` analysis, before the assignment executes.

### Division by zero

Source:

```text
SET quotient<float> = 1 / 0
```

The `/` operator reports `RuntimeError: division by zero` at the `Runtime` stage. The interpreter formats it as:

```text

RuntimeError: division by zero
------------------------------------------
1-|  SET quotient<float> = 1 / 0
--|                            ^
```

## Lexer behavior

The lexer does not define a separate public error class. Unsupported characters use `UNKNOWN`; unterminated strings and block comments use dedicated token types. All three are reported by the parser as `SyntaxError`, with dedicated messages for unterminated strings/comments. String literals cannot contain a line feed.

## Same category name, different source

Core diagnostics and standard-library functions share category names such as `TypeError` and `ArgumentError`. Read the stage and message together: the source of the error determines its precise meaning. For example, core `ArgumentError` reports invalid call arguments during semantic analysis, while `format` uses `ArgumentError` for a missing `text` parameter or a missing placeholder value during runtime. Likewise, core assignment `TypeError` is semantic, while a library function can return `TypeError` during execution.

Arithmetic operators also report `ValueError` and `OverflowError`: `**` reports them for invalid or non-finite results, and `%` can report `OverflowError` when an operand exceeds signed 64-bit range. The `/`, `%%`, and `%` zero-divisor error is the core `RuntimeError`. The `fmod` library function reports `ZeroDivisionError` for a zero divisor. They are separate implementations with different error categories; other math and formatting errors are listed in [Built-in Functions](built-in-functions.md).

## Host API

Errors from `vltc::Runtime::RunTime::run()` and `vltc::Runtime::RunTime::run_file()` are returned in `vltc::Runtime::RunResult`; a failed result does not throw an exception. Check the result, then inspect `result.error.stage`, `type`, `message`, and source position. Host API calls may report `Host` or `IO` stages rather than a language-level error.

## Scope

Other components can construct diagnostics directly, and standard library functions can return their own error categories. This page covers the core error classes and formatter, not every failure possible in the interpreter.
