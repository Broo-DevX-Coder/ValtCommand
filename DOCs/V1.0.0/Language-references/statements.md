# Statements

**Version:** 1.0.0

A program is parsed as a sequence of expressions, then its statements execute from top to bottom. There is no required newline or semicolon between ordinary statements: in the tested example, two `SET` statements share one line. The `FOR` header has its own semicolon syntax; see [Loops](loops.md). Whitespace, blank lines, and comments are handled by the lexer, as described in [Lexical Structure](lexical-structure.md#3-whitespace-and-line-tracking) and [its comment rules](lexical-structure.md#4-comments). The [sequence example](examples/statements-order-comments.vlt) ([actual output](examples/statements-order-comments.expected)) runs statements on one line and across blank/comment lines.

## Statement forms

The parser uses expressions as statements; it has no separate statement terminator. These are the statement-like forms accepted in statement position:

| Form | Concise syntax | Reference |
|---|---|---|
| Variable declaration | `SET name<type> = expression` | [Variables](variables.md#declare-read-and-assign) |
| Constant declaration | `SET_CONST name<type> = expression` | [Variables](variables.md#declare-read-and-assign) |
| Variable assignment | `SET name = expression` | [Variables](variables.md#declare-read-and-assign) |
| Variable read | `GET name` | [Variables](variables.md#declare-read-and-assign) |
| Function call | `CALL name arguments END` | [Functions](functions.md#calling-a-user-defined-function), [built-ins](built-in-functions.md) |
| Function definition | `FUNCTION name(...) -> type ... END` | [Functions](functions.md#defining-a-function) |
| Return | `RETURN expression` | [Functions](functions.md#returning-values) |
| Conditional | `IF ... THEN ... [ELSE THEN ...] END` | This page: [IF](#if) |
| Loop | `WHILE ... THEN ... END`; `FOR ... THEN ... END` | [Loops](loops.md#forms-and-syntax) |
| Loop control | `BREAK`; `CONTINUE` | [Loops](loops.md#break-and-continue) |
| Other expression | A literal or operator expression, such as `1 + 2` | [Expressions and Operators](expressions-and-operators.md) |

`CALL` and ordinary expressions may stand alone. Their result is discarded in statement position. The [call example](examples/statements-call-alone.vlt) calls a function returning `42` and then continues; the [expression example](examples/statements-expression-alone.vlt) evaluates `1 + 2` and continues. Their captured outputs show only the later marker text.

## IF

The current syntax is `IF condition THEN statements [ELSE THEN statements] END`. `ELSE` is optional. There is no `ELSE IF` or `ELIF` form; to test another condition, nest an `IF` inside the `ELSE THEN` body. The parser reports `SyntaxError: unexpected token 'IF'` and the comment “Forgot to add `THEN` after the condition” for the attempted [ELSE IF example](examples/statements-else-if.vlt) ([actual error](examples/statements-else-if.expected)).

The condition must have static type `bool`. For example, `IF 1 THEN` fails semantic analysis with the actual message `TypeError: expected 'bool', got 'int'`; see [the condition error](examples/statements-if-condition-error.vlt) ([actual output](examples/statements-if-condition-error.expected)). A true condition runs the `IF` body; a false condition runs the optional `ELSE` body. The [branch example](examples/statements-if-else.vlt) tests both outcomes. Nested conditionals work; see [the nested example](examples/statements-if-nested.vlt) ([output](examples/statements-if-nested.expected)).

Boolean literals use the case-sensitive spellings `True` and `False`; `True` is the source spelling of the true boolean value. See [Types and Values: literals](types-and-values.md#literals).

Each `IF` creates a child scope for its branch statements. Scope visibility and shadowing are described in [Variables](variables.md#scopes-shadowing-and-lifetime). A branch may be empty: `IF True THEN END` is accepted, as tested by [the empty-block example](examples/statements-if-empty.vlt) ([output](examples/statements-if-empty.expected)).

Inside an `IF`, `THEN` starts the branch body and the matching `END` closes the whole conditional, including its optional `ELSE THEN` branch. An `IF` nested in another block needs its own `END`. `THEN` is also used to start loop bodies; functions instead begin their body after the return type. Those forms are covered in [Loops](loops.md) and [Functions](functions.md).

`RETURN` inside an `IF` in a function propagates out of the conditional and returns from that function. The [return example](examples/statements-return-in-if.vlt) prints `7` from the selected branch.

## Context-specific behavior

`RETURN` outside a function currently produces no error: the module ignores its return state and continues to the next top-level statement. The [tested example](examples/statements-return-outside-function.vlt) prints `after top-level RETURN`. This is current behavior, not a guarantee; it is also recorded in [Known limitations](limitations.md#known-limitations).

The parser and analyzer currently accept a `FUNCTION` definition inside an `IF`, a loop, or another function when it is called in that scope. These cases were run in [an IF](examples/statements-function-in-if.vlt), [a loop](examples/statements-function-in-loop.vlt), and [another function](examples/statements-function-in-function.vlt). Function scope, return checking, and other definition-order behavior are described in [Functions](functions.md).

`MissingReturnError` occurs during semantic analysis when a non-`void` function has no `RETURN` statement in its body. [This example](examples/statements-missing-return.vlt) records the actual diagnostic. The analyzer checks for a return statement anywhere in the body, not that every path executes one: in [this tested case](examples/statements-return-not-on-all-paths.vlt), a return in the true branch satisfies the check, but a false condition falls through with no value and the next statement runs. Its output is a blank line followed by `continued`. The function return checks are described in [Functions](functions.md#returning-values).

## Parsing and execution errors

The runtime parses the complete source and then semantically analyzes the complete program before beginning execution. Thus a semantic error later in the file prevents earlier calls from running: [the example](examples/statements-semantic-before-execution.vlt) contains a print before an invalid `IF`, but only the type diagnostic appears. Parsing failures also occur before execution. Errors raised while evaluating a valid program happen during execution; [this runtime-error example](examples/statements-runtime-error.vlt) prints `before runtime error` before reporting `RuntimeError: division by zero`.

The parser requires `THEN` after an `IF` condition and reports `SyntaxError: unexpected token 'CALL'` with the comment “Forgot to add `THEN` after the condition” when it encounters the body without `THEN`; see [the missing-THEN example](examples/statements-if-missing-then.vlt) ([actual error](examples/statements-if-missing-then.expected)). It also requires the conditional to close with `END`; a missing terminator reports `SyntaxError: unexpected token '(end of code!)'` with the comment “Expected `END` to close the IF statement.” See [the missing-END example](examples/statements-if-missing-end.vlt) ([actual error](examples/statements-if-missing-end.expected)). The corresponding loop syntax errors are covered in [Loops](loops.md#conditions-and-errors).
