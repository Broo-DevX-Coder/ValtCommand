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
| `file` | `std::filesystem::path` for the source file, or empty for source passed to `run()`. |
| `message` | Main diagnostic message. |
| `comment` | Optional additional explanation. |
| `line` | Zero-based source line index. |
| `column` | Zero-based byte column within that line. |
| `length` | Number of source bytes to mark with the caret. |

The CLI diagnostic formatter is an internal helper (declared in `include/detail/globals.hpp`, not in the installed public API). It prints the type and message, a separator line of 42 hyphens, then source context when available. For an error at zero-based line `n`, it may print the preceding line as `n-|  ...`, then the failing line as `n+1-|  ...`; a caret line is printed only when that failing line exists. An optional `comment` follows another separator. The public runtime result carries this record as `result.error.info`; see `include/valtcommand/result.hpp`. CLI examples below capture the actual formatter output, including cases where zero-length locations produce no caret.

## Core error types

| Type | Usual stage | Cause and message form |
|---|---|---|
| `SyntaxError` | Parse | Unexpected token: `unexpected token '<token>'`. Parser comments may add a specific hint. |
| `NameError` | Semantic | An undefined variable or function is referenced: `'<name>' is not defined`. Unknown function and variable lookups attach a comment identifying which kind of name was unknown. |
| `TypeError` | Semantic or Runtime | A value is incompatible with an expected type. Variable assignment reports `Cannot assign value of type '<actual>' to variable '<name>' of type '<declared>'`. Converting a nonnumeric `any` payload to a numeric variable reports `cannot convert '<actual>' to '<target>'` at runtime. Built-ins may also return `TypeError` at runtime. |
| `ArgumentError` | Semantic | A function call has too many, unknown, missing, or wrongly typed arguments. |
| `IncompatibleTypesError` | Semantic | Comparison operands have incompatible types, or an ordered comparison uses nonnumeric operands. |
| `VariableError` | Semantic | A variable is declared twice in the same scope: `Variable '<name>' already defined in this scope`. |
| `ImmutableError` | Semantic | A constant variable is modified: `Cannot modify constant variable '<name>'`. |
| `ReturnTypeError` | Semantic | A function returns an incompatible type: `function '<function>' expected return type '<expected>', got '<actual>'`. |
| `MissingReturnError` | Semantic | A non-`void` function contains no `RETURN` statement: `Function '<function>' has no RETURN statement`. |
| `FunctionError` | Semantic | A function is defined twice in the same scope. |
| `RuntimeError` | Runtime | The `/` or `%%` operator has a zero divisor, or the divisor of `%` truncates to zero: `division by zero`. |
| `ValueError` | Runtime | The `**` operator produces an invalid numeric result. |
| `OverflowError` | Runtime | The `**` operator produces a non-finite result, `%` cannot convert an operand to signed 64-bit integer, or converting a non-finite/out-of-range float to `int` reports `cannot convert non-finite or out-of-range float to int`. Whether a borderline float is considered out of range depends on the platform's `long double` precision and range. In particular, the documented `-9223372036854775809` integer-literal example may raise this error on platforms with extended-precision `long double`, but can round to `INT64_MIN` and be accepted where `long double` has only double precision; see [Known limitations](limitations.md#known-limitations). |

### Function argument diagnostics

The core semantic analyzer reports these `ArgumentError` cases before executing the script:

| Cause | Message form |
|---|---|
| Too many arguments | `Too many arguments to function '<function>'. Expected <expected>, but got <actual>` |
| Unknown argument name | `Unexpected argument '<argument>' in function '<function>' arguments` |
| Argument type mismatch | `Argument '<argument>' in function '<function>' has type '<actual>'; expected '<expected>'` |
| Required argument missing | `Function '<function>' is missing required argument '<argument>'` |

## Examples

The linked `.vlt` examples are runnable programs. The `.expected` outputs are generated by running the current CLI against those files. In CLI file mode, an error is preceded by a blank line; after the formatted diagnostic, `main.cpp` prints `File: "<absolute normalized path>"` and a blank line. In embedded `runtime.run(source)`, `ErrorInfo.file` is empty; the runtime itself returns a result and does not print a CLI diagnostic or add a file line. Eval errors likewise have no `File:` line.

### Syntax error

Run the [syntax error example](examples/errors-syntax-error.vlt); its [expected output](exceptions/errors-syntax-error.expected) records the formatted diagnostic.

The extra `END` is encountered by the parser, so the stage is `Parse`.

### Undefined function

The call produces `NameError` with the comment `Unknown function`, during `Semantic` analysis. See the [undefined function example](examples/errors-undefined-function.vlt) and [its expected output](exceptions/errors-undefined-function.expected).

Undefined variables also use `NameError`, with the comment `Unknown variable`.

### Incompatible variable assignment

See the [incompatible assignment example](examples/errors-incompatible-assignment.vlt) and [its expected output](exceptions/errors-incompatible-assignment.expected).

This is rejected during `Semantic` analysis, before the assignment executes.

### Division by zero

The `/` operator reports `RuntimeError: division by zero` at the `Runtime` stage. See the [division-by-zero example](examples/errors-division-by-zero.vlt) and [its expected output](exceptions/errors-division-by-zero.expected).

## Lexer behavior

The lexer does not define a separate public error class. Unsupported characters use `UNKNOWN`; unterminated strings and block comments use dedicated token types. All three are reported by the parser as `SyntaxError`, with dedicated messages for unterminated strings/comments. String literals cannot contain a line feed.

## Same category name, different source

Core diagnostics and standard-library functions share category names such as `TypeError` and `ArgumentError`. Read the stage and message together: the source of the error determines its precise meaning. For example, core `ArgumentError` reports invalid call arguments during semantic analysis, while `format` uses `ArgumentError` for a missing `text` parameter or a missing placeholder value during runtime. Likewise, core assignment `TypeError` is semantic, while a library function can return `TypeError` during execution.

Arithmetic operators also report `ValueError` and `OverflowError`: `**` reports them for invalid or non-finite results, and `%` can report `OverflowError` when an operand exceeds signed 64-bit range. The `/`, `%%`, and `%` zero-divisor error is the core `RuntimeError`. The `fmod` library function reports `ZeroDivisionError` for a zero divisor. They are separate implementations with different error categories; other math and formatting errors are listed in [Built-in Functions](built-in-functions.md).

## Host API

Errors from `vltc::Runtime::RunTime::run()` and `vltc::Runtime::RunTime::run_file()` are returned in `vltc::Runtime::RunResult`; a failed result does not throw an exception. Check the result, then inspect `result.error.stage`, `type`, `message`, and `info.file` along with the source position. `run()` has no filename parameter, so `result.error.info.file` is empty; `run_file()` associates the source path with tokens and errors. If the file cannot be opened, `run_file()` returns `ErrorStage::IO` with `Cannot open file: <path>`. The CLI checks that `--file` names an existing regular file before calling `run_file()`; if not, it writes `Cannot open file: <absolute normalized path>` to stderr and exits with code `3`. A script parse or runtime failure is formatted to stderr and exits with code `2`.

## Scope

Other components can construct diagnostics directly, and standard library functions can return their own error categories. This page describes the core error classes and the CLI formatter output, not a host-callable formatter function or every failure possible in the interpreter.
