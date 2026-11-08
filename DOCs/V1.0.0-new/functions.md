# Functions

**Version:** 1.0.0

This page describes user-defined functions in ValtCommand. Built-in functions such as `print`, `sqrt`, and `format` are documented in [Built-in Functions](built-in-functions.md).

## Defining a function

A function declaration starts with `FUNCTION`, gives a name and a parenthesized parameter list, then `->` and a return type. The body ends with `END`. Parameters are separated by whitespace; commas are not part of the current syntax.

```text
FUNCTION add(
    x<int>
    y<int>
)->int
    RETURN GET x + GET y
END
```

A parameter has a name and declared type, written as `name<type>`. The return type follows `->`.

- Use a concrete type (`int`, `float`, `str`, or `bool`) when a parameter expects that type.
- Use `any` for a parameter that accepts any value type. Its value is passed through without numeric conversion. Because its static type remains `any`, it cannot be used directly in arithmetic or ordered comparisons, which require known numeric types. Its practical use is currently limited to forwarding values to generic functions, such as `println`, or equality checks; ValtCommand has no built-in cast that narrows `any` to a concrete type.
- Use `void` as the function return type when the function returns no value. A `void` parameter is not useful as an input type.

Example: a generic display helper can accept and forward different value types:

```text
FUNCTION show(value<any>)->void
    CALL println value<any>:GET value END
END

CALL show value<int>:7 END
CALL show value<str>:"hello" END
```

Output:

```text
7
hello
```

This demonstrates `any` as a forwarding type; the function cannot perform arithmetic on `value` without a concrete static type.

## Parameters and default values

Parameters without a default are required. A default expression after `=` makes the parameter optional:

```text
FUNCTION greet(name<str> = "World")->str
    RETURN CALL format text<str>:"Hello {name}!" name<str>:GET name END
END
```

Defaults are evaluated once, when the function declaration executes, and the resulting value is reused by calls that omit the argument. For example, a random default is sampled once at declaration time, not once per call:

```text
FUNCTION sample(value<float> = CALL random END)->float
    RETURN GET value
END

CALL println value<float>:CALL sample END END
CALL println value<float>:CALL sample END END
```

Both calls return the same stored default value (the exact number changes on each program run). To generate a new random value each time, call `random` in the function body instead. Default expressions must have a type compatible with the parameter type.

Call arguments are named, so they may be supplied in any order. Optional arguments can be omitted by name:

```text
CALL greet END
CALL greet name<str>:"Sam" END
```

## Calling a user-defined function

A call uses `CALL`, the function name, zero or more named arguments, and `END`. Each argument is written `name<type>:expression`:

```text
CALL add x<int>:2 y<int>:3 END
```

The argument's type tag must match the expression's actual type, then the tagged type is checked for compatibility with the parameter type. In the current implementation, `int` and `float` are compatible with each other for user-defined parameters, and the value is converted to the parameter's declared type. The current float-to-int conversion truncates toward zero silently: `2.9` becomes `2`, and `-2.9` becomes `-2`. This behavior may change; see [Known limitations](limitations.md#known-limitations). This differs from a mismatched literal type tag:

```text
FUNCTION to_int(value<int>)->int
    RETURN GET value
END

SET result<int> = CALL to_int value<float>:2.9 END  // result is 2
```

By contrast, tagging a float literal as an integer argument (`value<int>:2.9`) is rejected because the `<int>` tag does not match the literal's actual type. This tag-based conversion rule differs from built-in function validation; see [Built-in Functions](built-in-functions.md#2-how-calls-work). For expression type rules, see [Expressions and Operators](expressions-and-operators.md#type-checking-and-evaluation).

> **Current behavior:** Passing a float to an `int` user-defined parameter silently truncates it toward zero. This behavior may change; see [Known limitations](limitations.md#known-limitations).

Function calls can be used inside expressions, including as values passed to other calls:

```text
SET total<int> = CALL add x<int>:2 y<int>:3 END
CALL println value<int>:GET total END
```

## Returning values

Use `RETURN` followed by an expression to return a value:

```text
FUNCTION square(value<int>)->int
    RETURN GET value * GET value
END
```

`GET` consumes a variable name as a primary expression, so `RETURN GET x + GET y` is parsed as `RETURN (GET x) + (GET y)`. See [Expressions and Operators](expressions-and-operators.md#names-and-calls-as-expressions).

The returned expression must be compatible with the declared return type. Compatible `int` and `float` values are converted to the declared type. A non-`void` function must contain at least one `RETURN` statement; the current analyzer does not verify that every control-flow path reaches a return.

A `void` function has no value result and can finish by reaching its `END`:

```text
FUNCTION announce(message<str>)->void
    CALL println value<str>:GET message END
END
```

Omit `RETURN` in a `void` function. A `void` function call can be used as a statement, not as a value expression.

## Scope and definition order

Function parameters are added to a call-local scope and are immutable inside the body. ValtCommand currently uses **dynamic scoping**: name lookup walks from the active call scope through its parent (caller) scopes. A function can therefore resolve a name from an enclosing caller's scope, not only from the scope where the function was defined.

New variables are local to the call. Assignment to an existing mutable name found in a parent scope can change that outer variable. For example:

```text
SET count<int> = 0

FUNCTION increment()->int
    SET count = GET count + 1
    RETURN GET count
END

SET updated<int> = CALL increment END
CALL println value<int>:GET updated END  // 1
CALL println value<int>:GET count END    // 1
```

This is a side effect on the caller's scope. Use a local variable or pass state through parameters and return values when you want to avoid changing an outer binding.

A function name may be declared only once in the same scope. Current restrictions on recursion and calls before definition are listed in [Known limitations](limitations.md#known-limitations) and may change in a later version.

## Type checking and errors

The interpreter parses the declaration, checks parameter defaults, analyzes the body and return types, then registers the function. Calls are validated during semantic analysis:

| Situation | Error type |
|---|---|
| Too many arguments, unknown argument name, missing required argument, or incompatible argument type | `ArgumentError` |
| Function or variable name is undefined | `NameError` |
| Function name is already defined in the same scope | `FunctionError` |
| Returned value is incompatible with the declared return type | `ReturnTypeError` |
| A non-`void` function contains no `RETURN` statement | `MissingReturnError` |

See [Errors](errors.md) for messages, stages, and formatted diagnostics.

### Example: calling a function before its definition

```text
CALL later END

FUNCTION later()->int
    RETURN 1
END
```

This fails during semantic analysis because `later` has not yet been registered. The interpreter reports:

```text
NameError: 'later' is not defined
------------------------------------------
1-|  CALL later END
--|       ^^^^^
------------------------------------------
Unknown function
```

## Current limitations

- Parameter declarations use whitespace separators; commas are not accepted.
- Calls use named arguments; there is no positional-argument form.
- Default expressions are evaluated once at declaration time and reused by omitted arguments.
- Current float-to-int parameter conversion behavior is described in [Known limitations](limitations.md#known-limitations); it may change in a later version.
- A non-`void` function is checked for the presence of at least one `RETURN`, but the analyzer does not prove that every control-flow path returns a value.
- Dynamic scoping permits functions to read names from caller scopes, and assignment may mutate an existing outer variable.
- Current recursion and forward-call restrictions are described in [Known limitations](limitations.md#known-limitations); they may change in a later version.
